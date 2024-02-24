#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#include "sample-common.h"

#define TAG "sample-common-venc"

typedef struct {
	IMP_BOOL bThreadStart;
	IMP_S32 VencChn[MAX_VENC_CHN];
	IMP_S32 s32Cnt;
} SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct {
	IMP_BOOL bThreadStart;
	IMP_S32 VencChn[MAX_VENC_CHN];
	IMP_S32 s32Cnt;
	IMP_S32 s32FrmCnt;
	FILE *inFile;
} SAMPLE_VENC_SENDFRAME_PARA_S;

pthread_t g_VencGetPid, g_VencSendPid;
static SAMPLE_VENC_GETSTREAM_PARA_S g_VencGetPara;
static SAMPLE_VENC_SENDFRAME_PARA_S g_VencSendPara;

IMP_S32 calc_blksize(IMP_U32 u32Width, IMP_U32 u32Height)
{
	return u32Width * u32Height * 3 / 2;
}

IMP_S32 sample_venc_savestream(FILE *pFile, VENC_STREAM_S *pstStream)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 i = 0;

	for (i = 0; i < pstStream->u32PackCount; i++) {
		s32Ret = fwrite((void *)pstStream->pack[i].u32VirAddr, 1, pstStream->pack[i].u32Len, pFile);
		if (s32Ret != pstStream->pack[i].u32Len) {
			IMP_LOG_ERR(TAG, "fwrite stream failed!\n");
			return IMP_FAILURE;
		}
	}

	return IMP_SUCCESS;
}

IMP_VOID *send_user_frame(IMP_VOID *p)
{
	SAMPLE_VENC_SENDFRAME_PARA_S *pstVencPara = (SAMPLE_VENC_SENDFRAME_PARA_S *)p;
	IMP_S32 i;
	IMP_S32 s32Ret;
	IMP_S32 s32ChnTotal;
	IMP_S32 VencChn;
	VENC_CHN_ATTR_S VencChnAttr;
	FRAME_INFO_S frame;
	VENC_STREAM_S Stream;
	IMP_U32 u32PoolId = 0;
	IMP_U32 u32BlkCnt = 1;
	IMP_U32 u32BlkSize = 0;
	IMP_S32 frmcnt[MAX_VENC_CHN] = {0};
	IMP_CHAR FileName[MAX_VENC_CHN][30];
	FILE *pFile[MAX_VENC_CHN];

	s32ChnTotal = pstVencPara->s32Cnt;

	for (i = 0; i < s32ChnTotal; i++) {
		VencChn = pstVencPara->VencChn[i];
		IMP_VENC_GetChnAttr(VencChn, &VencChnAttr);

		u32BlkSize = calc_blksize(VencChnAttr.u32PicWidth, VencChnAttr.u32PicHeight);
		IMP_System_CreatPool(&u32PoolId, u32BlkSize, u32BlkCnt, "VencFramePool");
		frame.u32VirAddr = (IMP_U32)IMP_System_GetBlock(u32PoolId, "VencFramePool", u32BlkSize, "VencFrame");
		frame.u32PhyAddr = IMP_System_Block2PhyAddr((IMP_VOID *)frame.u32VirAddr);
		frame.u32Width = VencChnAttr.u32PicWidth;
		frame.u32Height = VencChnAttr.u32PicHeight;
		frame.u32Stride = VencChnAttr.u32PicWidth;
		frame.stPixfmt = PIX_FMT_NV12;

		while(pstVencPara->bThreadStart && frmcnt[VencChn] < pstVencPara->s32FrmCnt) {
			IMP_S32 try_cnt = 3;
			IMP_S32 s32Size = calc_blksize(frame.u32Width, frame.u32Height);
			if (fread((IMP_VOID *)frame.u32VirAddr, 1, s32Size, pstVencPara->inFile) == 0) {
				fseek(pstVencPara->inFile, 0, SEEK_SET);
				continue;
			}

			s32Ret = IMP_VENC_SendFrame(VencChn, &frame, -1);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VENC_SendFrame %d failed\n", VencChn);
				continue;
			}

			while (try_cnt--) {
				s32Ret = IMP_VENC_GetStream(VencChn, &Stream, -1);
				if (s32Ret != IMP_SUCCESS) {
					IMP_LOG_ERR(TAG, "IMP_VENC_GetStream(%d) cnt:%d failed! ret = 0x%08x\n", VencChn, frmcnt[VencChn], s32Ret);
					continue;
				}

				sprintf(FileName[VencChn], "chn%d_%d.jpg", VencChn, frmcnt[VencChn]);
				pFile[VencChn] = fopen(FileName[VencChn], "wb");

				s32Ret = sample_venc_savestream(pFile[VencChn], &Stream);
				if (s32Ret != IMP_SUCCESS) {
					IMP_LOG_ERR(TAG, "save_stream failed\n");
					continue;
				}
				fclose(pFile[VencChn]);

				s32Ret = IMP_VENC_ReleaseStream(VencChn, &Stream);
				if (s32Ret != IMP_SUCCESS) {
					IMP_LOG_ERR(TAG, "IMP_VENC_ReleaseStream %d failed! ret = 0x%08x\n", VencChn, s32Ret);
					continue;
				}
				break;
			}
			frmcnt[VencChn]++;
		}

		IMP_System_ReleaseBlock(u32PoolId, (IMP_VOID *)frame.u32VirAddr);
		IMP_System_DestroyPool(u32PoolId, "VencFramePool");
	}
	return NULL;
}

IMP_VOID *get_video_stream(IMP_VOID *p)
{
	SAMPLE_VENC_GETSTREAM_PARA_S *pstVencPara = (SAMPLE_VENC_GETSTREAM_PARA_S *)p;
	IMP_S32 i;
	IMP_S32 s32ChnTotal;
	VENC_CHN_ATTR_S VencChnAttr;
	fd_set read_fds;
	IMP_S32 maxfd = 0;
	IMP_S32 VencFd[MAX_VENC_CHN];
	IMP_S32 s32PicCnt[MAX_VENC_CHN];
	IMP_CHAR FileName[MAX_VENC_CHN][30];
	FILE *pFile[MAX_VENC_CHN];
	VENC_STREAM_S Stream;
	VENC_CHN_STATUS_S Stat;
	IMP_S32 s32Ret;
	IMP_S32 VencChn;
	IMP_U32 u32NumFrame = NUM_SAVE_FRAME;

	s32ChnTotal = pstVencPara->s32Cnt;
	for (i = 0; i < s32ChnTotal; i++) {
		VencChn = pstVencPara->VencChn[i];
		VencFd[i] = IMP_VENC_GetFd(VencChn);
		if (VencFd[i] < 0) {
			IMP_LOG_ERR(TAG, "IMP_VENC_GetFd %d failed! ret = %08x\n", i, s32Ret);
			return IMP_NULL;
		}
		if (maxfd < VencFd[i]) {
			maxfd = VencFd[i];
		}
		s32Ret = IMP_VENC_GetChnAttr(VencChn, &VencChnAttr);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "IMP_VENC_GetChnAttr %d failed! ret = 0x%08x\n", VencChn, s32Ret);
			return IMP_NULL;
		}
	}

	while(u32NumFrame--) {
		if (!pstVencPara->bThreadStart) {
			break;
		}
		FD_ZERO(&read_fds);
		for (i = 0; i < s32ChnTotal; i++) {
			FD_SET(VencFd[i], &read_fds);
		}

		s32Ret = select(maxfd+1, &read_fds, NULL, NULL, NULL);
		if (s32Ret < 0) {
			IMP_LOG_ERR(TAG, "select failed\n");
			break;
		} else if (s32Ret == 0) {
			IMP_LOG_DBG(TAG, "get venc stream time out\n");
			continue;
		} else {
			for (i = 0; i < s32ChnTotal; i++) {
				if (FD_ISSET(VencFd[i], &read_fds)) {
					memset(&Stream, 0, sizeof(VENC_STREAM_S));

					VencChn = pstVencPara->VencChn[i];
					s32Ret = IMP_VENC_Query(VencChn, &Stat);
					if (s32Ret != IMP_SUCCESS) {
						IMP_LOG_ERR(TAG, "IMP_VENC_Query %d failed! ret = 0x%08x\n", VencChn, s32Ret);
						continue;
					}

					if (Stat.u32CurPacks == 0) {
						IMP_LOG_ERR(TAG, "%d Current frame is NULL\n", VencChn);
						continue;
					}

					sprintf(FileName[i], "chn%d_%d.jpg", VencChn, s32PicCnt[i]);
					pFile[i] = fopen(FileName[i], "wb");
					if (pFile[i] == NULL) {
						IMP_LOG_ERR(TAG, "fopen file(%s) failed\n", FileName[i]);
						break;
					}

					s32Ret = IMP_VENC_GetStream(VencChn, &Stream, 10);
					if (s32Ret != IMP_SUCCESS) {
						IMP_LOG_ERR(TAG, "IMP_VENC_GetStream %d failed! ret = 0x%08x\n", VencChn, s32Ret);
						break;
					}

					s32Ret = sample_venc_savestream(pFile[i], &Stream);
					if (s32Ret != IMP_SUCCESS) {
						IMP_LOG_ERR(TAG, "save_stream failed\n");
						break;
					}

					s32Ret = IMP_VENC_ReleaseStream(VencChn, &Stream);
					if (s32Ret != IMP_SUCCESS) {
						IMP_LOG_ERR(TAG, "IMP_VENC_ReleaseStream %d failed! ret = 0x%08x\n", VencChn, s32Ret);
						break;
					}
					fclose(pFile[i]);
					s32PicCnt[i]++;
				}
			}
		}
	}

	return IMP_NULL;
}

IMP_S32 sample_venc_start_chn(IMP_S32 s32Chn, VENC_CHN_ATTR_S *chnAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VENC_CreateChn(s32Chn, chnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VENC_CreateChn %d failed! ret = 0x%08x\n", s32Chn, s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_VENC_StartRecvStream(s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VENC_StartRecvStream %d failed! ret = 0x%08x\n", s32Chn, s32Ret);
		return s32Ret;
	}

	return IMP_SUCCESS;
}

IMP_S32 sample_venc_stop_chn(IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VENC_StopRecvStream(s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VENC_StopRecvStream %d failed! ret = 0x%08x\n", s32Chn, s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_VENC_DestroyChn(s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VENC_DestroyChn %d failed! ret = 0x%08x\n", s32Chn, s32Ret);
		return s32Ret;
	}

	return IMP_SUCCESS;
}

IMP_S32 sample_venc_start_send_frame(IMP_S32 s32Chn[], IMP_S32 s32Cnt, IMP_S32 s32FrmCnt, FILE *inFile)
{
	IMP_S32 i;
	g_VencSendPara.bThreadStart = IMP_TRUE;
	g_VencSendPara.s32Cnt = s32Cnt;
	g_VencSendPara.s32FrmCnt = s32FrmCnt;
	g_VencSendPara.inFile = inFile;
	for (i = 0; i < s32Cnt; i++) {
		g_VencSendPara.VencChn[i] = s32Chn[i];
	}

	return pthread_create(&g_VencSendPid, NULL, send_user_frame, (IMP_VOID *)&g_VencSendPara);

}

IMP_S32 sample_venc_stop_send_frame(IMP_VOID)
{
	if (g_VencSendPara.bThreadStart == IMP_TRUE) {
		g_VencSendPara.bThreadStart = IMP_FALSE;
		pthread_cancel(g_VencSendPid);
		pthread_join(g_VencSendPid, NULL);
	}

	return IMP_SUCCESS;
}

IMP_S32 sample_venc_start_get_stream(IMP_S32 s32Chn[], IMP_S32 s32Cnt)
{
	IMP_S32 i = 0;

	g_VencGetPara.bThreadStart = IMP_TRUE;
	g_VencGetPara.s32Cnt = s32Cnt;
	for (i = 0; i < s32Cnt; i++) {
		g_VencGetPara.VencChn[i] = s32Chn[i];
	}

	return pthread_create(&g_VencGetPid, NULL, get_video_stream, (IMP_VOID *)&g_VencGetPara);
}

IMP_S32 sample_venc_stop_get_stream(IMP_VOID)
{
	if (g_VencGetPara.bThreadStart == IMP_TRUE)
    {
		g_VencGetPara.bThreadStart = IMP_FALSE;
		pthread_cancel(g_VencGetPid);
		pthread_join(g_VencGetPid, NULL);
    }
    return IMP_SUCCESS;
}
