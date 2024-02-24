#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <pthread.h>

#include "imp/imp_vdec.h"
#include "sample-common.h"
#define TAG "sample-vdec"

IMP_S32 sample_vdec_start_dev(IMP_S32 s32DevNum, VDEC_DEV_ATTR_S *pstDevAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VDEC_SetDisplayMode(s32DevNum, VIDEO_DISPLAY_MODE_PREVIEW);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_SetDisplayMode failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_VDEC_EnablePtsCtrl(s32DevNum);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_EnablePtsCtrl failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_VDEC_CreateDev(s32DevNum, pstDevAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_CreateDev failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	return IMP_SUCCESS;
}

IMP_S32 sample_vdec_stop_dev(IMP_S32 s32DevNum)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VDEC_DestroyDev(s32DevNum);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_DestroyDev failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}
	return IMP_SUCCESS;
}

IMP_S32 sample_vdec_start_chn(IMP_S32 s32DevNum,IMP_S32 s32ChnNum,VDEC_CHN_ATTR_S *pstChnAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VDEC_SetChnAttr(s32DevNum, s32ChnNum, pstChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_SetChnAttr failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_VDEC_EnableChn(s32DevNum, s32ChnNum);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_EnableChn failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	return IMP_SUCCESS;
}

IMP_S32 sample_vdec_stop_chn(IMP_S32 s32DevNum,IMP_S32 s32ChnNum)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VDEC_DisableChn(s32DevNum, s32ChnNum);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_DisableChn failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	return IMP_SUCCESS;
}

IMP_S32 sample_vdec_start_recv_stream(IMP_S32 s32DevNum)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	s32Ret = IMP_VDEC_StartRecvStream(s32DevNum);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_StartRecvStream failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}
	return IMP_SUCCESS;
}

IMP_S32 sample_vdec_stop_recv_stream(IMP_S32 s32DevNum)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	s32Ret = IMP_VDEC_StopRecvStream(s32DevNum);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_VDEC_StopRecvStream failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}
	return IMP_SUCCESS;
}

IMP_VOID *sample_vdec_get_frame_thread(IMP_VOID *parg)
{
	SAMPLE_VDEC_THREAD_S *pstThreadParam = (SAMPLE_VDEC_THREAD_S *)parg;
	IMP_S32 s32Ret = 0;
	FILE *pFile = NULL;
	pFile = fopen(pstThreadParam->cFileName, "w+");
	if (pFile == NULL) {
		IMP_LOG_ERR(TAG, "sample-vdecyuv file open failed!\n");
		return NULL;
	}
	FRAME_INFO_S *pstFrameInfo = NULL;
	IMP_U32 u32NumFrame = NUM_SAVE_FRAME;
	while (u32NumFrame--) {
		if(pstThreadParam->enCtrlSig == VDEC_CTRL_STOP){
			break;
		}
		s32Ret = IMP_VDEC_GetFrame(pstThreadParam->s32DevId, pstThreadParam->s32ChnId, &pstFrameInfo);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "IMP_VDEC_GetFrame failed! dev = %d, ret = 0x%08x\n", pstThreadParam->s32DevId, s32Ret);
			continue;
		}

		s32Ret = fwrite((IMP_U8 *)pstFrameInfo->u32VirAddr, 1, pstFrameInfo->u32Size, pFile);
		if (s32Ret != pstFrameInfo->u32Size) {
			IMP_LOG_ERR(TAG, "sample-vdecyuv fwrite len %d not equal to picSize %d\n", s32Ret, pstFrameInfo->u32Size);
			IMP_VDEC_ReleaseFrame(pstThreadParam->s32DevId, pstThreadParam->s32ChnId, pstFrameInfo);
			return NULL;
		}

		s32Ret = IMP_VDEC_ReleaseFrame(pstThreadParam->s32DevId, pstThreadParam->s32ChnId, pstFrameInfo);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "IMP_VDEC_ReleaseFrame failed! dev = %d, ret = 0x%08x\n", pstThreadParam->s32DevId, s32Ret);
			return NULL;
		}
		usleep(40000);
	}
	fclose(pFile);
	IMP_LOG_INFO(TAG, "[%d]sample_vdec_get_frame_thread exit!\n",pstThreadParam->s32DevId);
	return NULL;
}

IMP_VOID sample_vdec_stop_get_frame(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara, pthread_t *pVdecThread)
{
	IMP_S32 i;

	for(i = 0; i < s32DevCnt; i++){
		pstVdecThreadPara[i].enCtrlSig = VDEC_CTRL_STOP;
		if (pVdecThread[i]){
			pthread_join(pVdecThread[i], IMP_NULL);
			pVdecThread[i] = 0;
		}
	}
}


IMP_VOID *sample_vdec_send_stream_thread(IMP_VOID *pArg)
{
	SAMPLE_VDEC_THREAD_S *pstVdecThread = (SAMPLE_VDEC_THREAD_S *)pArg;
	VDEC_STREAM_S stStream = {0};
	FILE *fpStrm = NULL;;
	IMP_U8 *pu8Buf = NULL;
	IMP_BOOL bNewPic = 0, bFindStart = 0, bFindEnd = 0;
	IMP_S32 s32ReadLen = 0, s32UsedBytes = 0;
	IMP_S32 s32Ret = 0, i = 0;
	IMP_S32 times = 0;
	IMP_S32 len = 0;
	IMP_S32 start = 0;

	fpStrm = fopen(pstVdecThread->cFileName, "rb");
	if (fpStrm == NULL) {
		IMP_LOG_ERR(TAG, "Can't open file: %s\n", pstVdecThread->cFileName);
		return NULL;
	}

	IMP_LOG_INFO(TAG, "open file: %s\n", pstVdecThread->cFileName);
	pu8Buf = (IMP_U8 *)malloc(pstVdecThread->u32BufSize);
	while (1) {
		if(pstVdecThread->enCtrlSig == VDEC_CTRL_STOP){
			break;
		}
		bNewPic = 0;
		bFindStart = 0;
		bFindEnd = 0;
		fseek(fpStrm, s32UsedBytes, SEEK_SET);
		s32ReadLen = fread(pu8Buf, 1, pstVdecThread->u32BufSize, fpStrm);
		if (s32ReadLen == 0) {
			s32UsedBytes = 0;
			if (pstVdecThread->bLoopPlayback)
				continue;
			else
				break;
		}
		if (pstVdecThread->enType == PT_H265 && pstVdecThread->enVideoMode == VIDEO_MODE_FRAME) {
			for (i = 0; i < s32ReadLen - 6; i ++) {
				bNewPic = (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
						&&(((pu8Buf[i+3]&0x7D) >= 0x0 && (pu8Buf[i+3]&0x7D) <= 0x2A) || (pu8Buf[i+3]&0x1F) == 0x1)
						&&((pu8Buf[i+5]&0x80) == 0x80));//first slice segment

				if (bNewPic) {
					bFindStart = 1;
					i += 4;
					break;
				}
			}

			for (; i < s32ReadLen - 6; i ++) {
				bNewPic = (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
						&&(((pu8Buf[i+3]&0x7D) >= 0x0 && (pu8Buf[i+3]&0x7D) <= 0x2A) || (pu8Buf[i+3]&0x1F) == 0x1)
						&&((pu8Buf[i+5]&0x80) == 0x80));//first slice segment

				if (  pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
						&&(((pu8Buf[i+3]&0x7D) == 0x40) || ((pu8Buf[i+3]&0x7D) == 0x42) || ((pu8Buf[i+3]&0x7D) == 0x44)|| bNewPic)
				   ) {
					bFindEnd = 1;
					break;
				}
			}
		} else if (pstVdecThread->enType == PT_H264 && pstVdecThread->enVideoMode == VIDEO_MODE_FRAME) {
			for (i=0; i<s32ReadLen-8; i++) {
				int tmp = pu8Buf[i+3] & 0x1F;
				if (pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
						(((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80)) || (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80))
				   ) {
					bFindStart = 1;
					i += 8;
					break;
				}
			}

			for (; i<s32ReadLen-8; i++) {
				int tmp = pu8Buf[i+3] & 0x1F;
				if (pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
						(tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 || ((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80))
						 || (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80))
				   ) {
					bFindEnd = 1;
					if (pu8Buf[i-1] == 0)
						i -= 1;
					break;
				}
			}
		} else if (pstVdecThread->enType == PT_JPEG && pstVdecThread->enVideoMode == VIDEO_MODE_FRAME) {
			for (i=0; i<s32ReadLen-2; i++) {
				if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8) {
					start = i;
					bFindStart = IMP_TRUE;
					i = i + 2;
					break;
				}
			}

			for (; i<s32ReadLen-4; i++) {
				if ((pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0) {
					len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
					i += 1 + len;
				} else
					break;
			}

			for (; i<s32ReadLen-2; i++) {
				if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8) {
					bFindEnd = IMP_TRUE;
					break;
				}
			}
		}

		if (i > 0) s32ReadLen = i;

		if (bFindStart == 0) {
			printf("Stream can not find start code!\n");
			return NULL;
		} else if (bFindEnd == 0) {
			if (pstVdecThread->enType == PT_H265)
				s32ReadLen = i+6;
			else if (pstVdecThread->enType == PT_H264)
				s32ReadLen = i+8;
			else if (pstVdecThread->enType == PT_JPEG)
				s32ReadLen = i+2;
		}

		stStream.pu8Addr = pu8Buf + start;
		stStream.u32Len  = s32ReadLen;
		stStream.bDisplay = IMP_TRUE;

		if(pstVdecThread->s32FpsCtrl > 0){
			stStream.u64PTS += 1000000/pstVdecThread->s32FpsCtrl;
		}else if(pstVdecThread->s32FpsCtrl == 0){
			stStream.u64PTS = 0;
		}else{
			stStream.u64PTS = -1;
		}
		s32Ret = IMP_VDEC_SendStream(pstVdecThread->s32DevId, &stStream, -1);
		if (s32Ret != IMP_SUCCESS) {
			/* usleep(50*1000); */
			/* s32UsedBytes += s32ReadLen; */
			/* continue; */
			IMP_LOG_INFO(TAG, "IMP_VDEC_SendStream failed! ret = 0x%08x\n", s32Ret);
			break;
		}
		s32UsedBytes += s32ReadLen;
		times ++;
		//printf("This is %d times send stream! send %d byte\n", times, s32ReadLen);
	}
	if(pu8Buf) {
		free(pu8Buf);
		pu8Buf = NULL;
	}
	fclose(fpStrm);
	IMP_LOG_INFO(TAG, "[%d]sample_vdec_send_stream_thread exit!\n",pstVdecThread->s32DevId);
	return IMP_NULL;
}

IMP_VOID sample_vdec_thread_param(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara,
		VDEC_DEV_ATTR_S *pstDevAttr,char *pStreamFileName)
{
	IMP_S32 i;
	for(i = 0; i < s32DevCnt; i++){
		pstVdecThreadPara[i].s32DevId = i;
		sprintf(pstVdecThreadPara[i].cFileName, pStreamFileName);
		pstVdecThreadPara[i].enType = pstDevAttr[i].enType;
		pstVdecThreadPara[i].enVideoMode = VIDEO_MODE_FRAME;
		pstVdecThreadPara[i].u32BufSize = pstDevAttr[i].u32StreamBufSize;
		pstVdecThreadPara[i].enCtrlSig = VDEC_CTRL_START;
		pstVdecThreadPara[i].s32FpsCtrl = 25;
		pstVdecThreadPara[i].bLoopPlayback = IMP_FALSE;
	}
}

IMP_S32 sample_vdec_start_send_stream(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara, pthread_t *pVdecThread)
{
	IMP_S32 i;
	for(i = 0; i < s32DevCnt; i++){
		pthread_create(&pVdecThread[i], 0, sample_vdec_send_stream_thread, (void *)&pstVdecThreadPara[i]);
	}
	return IMP_SUCCESS;
}

IMP_VOID sample_vdec_stop_send_stream(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara, pthread_t *pVdecThread)
{
	IMP_S32  i;

	for(i = 0; i < s32DevCnt; i++){
		pstVdecThreadPara[i].enCtrlSig = VDEC_CTRL_STOP;
		if (pVdecThread[i]){
			pVdecThread[i] = 0;
		}
		IMP_VDEC_StopRecvStream(i);
	}
}
