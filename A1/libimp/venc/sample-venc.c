#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>

#include "../common/sample-common.h"

#define TAG "sample_venc"

extern pthread_t g_VencGetPid, g_VencSendPid;
IMP_VOID SAMPLE_VENC_Usage(IMP_VOID)
{
    printf("\n\n/************************************/\n");
    printf("please choose the case which you want to run:\n");
    printf("\t0:  JPEG encoding(Bind Mode)\n");
    printf("\t1:  JPEG encoding(Unbind Mode)\n");
    printf("\tq:  quit the whole sample\n");
    printf("sample command:");
}

/*
   SAMPLE_VENC_JPEG_Bind是绑定vdec和venc模块，使用vdec解码
   出的NV12图片，送给venc模块编码出JPEG图片，并保存到当前
   目录下。默认开启三路解码和编码，每路编码输出
   NUM_SAVE_FRAME张jpg图片。
 */
IMP_S32 SAMPLE_VENC_JPEG_Bind(IMP_VOID)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 i = 0;
	IMP_S32 bUserPool = IMP_FALSE;
	IMP_S32 s32VdecChn = 0;
	IMP_S32 s32VencDev = 0;
	IMP_S32 s32VencChn[MAX_VENC_CHN] = {0, 1, 2};
	IMP_U32 u32VePoolId[MAX_VENC_CHN] = {0};
	IMP_U32 u32VdPoolId[MAX_VENC_CHN] = {0};
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVencCell = {0};
	IMP_CHAR PoolName[32] = {0};
	VENC_CHN_ATTR_S stVencChnAttr[MAX_VENC_CHN] = {
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,
			.u32ProFile = 1,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 640,
			.u32OutPicHeight = 360
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,
			.u32ProFile = 1,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 1920,
			.u32OutPicHeight = 1080
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 3840,
			.u32OutPicHeight = 2160
		}
	};
	SAMPLE_VDEC_THREAD_S stThreadParam[MAX_VDEC_DEVS];
	pthread_t sendTid[MAX_VDEC_DEVS];
	VDEC_DEV_ATTR_S stVdecDevAttr[MAX_VDEC_DEVS];
	VDEC_CHN_ATTR_S stVdecChnAttr[MAX_VDEC_DEVS];
	VDEC_BUFFER_PARAM_S stVdecBufferParam[MAX_VDEC_DEVS];
	IMP_U32 u32FrameBufferCnt = 2;
	IMP_S32 s32ChnCnt = 3;

    /************************************************
    step1:  init system
    *************************************************/
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_system_init failed! ret = 0x%08x\n", s32Ret);
		goto END0;
	}

    /************************************************
    step2:  bind vdec and venc
    *************************************************/
	for (i = 0; i < s32ChnCnt; i++) {
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = i;
		stVdecCell.u32ChnId = s32VdecChn;
		stVencCell.enModId = MOD_ID_VENC;
		stVencCell.u32DevId = s32VencDev;
		stVencCell.u32ChnId = i;
		s32Ret = IMP_System_Bind(&stVdecCell, &stVencCell);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
			goto END1;
		}
	}

    /************************************************
    step3:  start venc
    *************************************************/
	for (i = 0; i < s32ChnCnt; i++) {
		if (bUserPool) {
			sprintf(PoolName, "Chn%d-pool", s32VencChn[i]);
			s32Ret = IMP_System_CreatPool(u32VePoolId + s32VencChn[i], IMP_VENC_GetBufferSize(&stVencChnAttr[i]), 1, PoolName);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
				goto END2;
			}

			s32Ret = IMP_VENC_AttachVbPool(s32VencChn[i], u32VePoolId[i]);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VDEC_AttachVbPool failed! ret = 0x%08x\n", s32Ret);
				goto END2;
			}
		}
		s32Ret = sample_venc_start_chn(s32VencChn[i], &stVencChnAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_venc_start %d failed! ret = 0x%08x\n", i, s32Ret);
			goto END3;
		}
	}

    /************************************************
    step4:  start vdec
    *************************************************/
	for (i = 0; i < s32ChnCnt; i++) {
		stVdecDevAttr[i].enType = PT_H265;
		stVdecDevAttr[i].enMode = VIDEO_MODE_FRAME;
		stVdecDevAttr[i].u32PicWidth = 1920;
		stVdecDevAttr[i].u32PicHeight = 1080;
		stVdecDevAttr[i].u32StreamBufSize = 0x100000;
		stVdecDevAttr[i].u32RefFrameNum = 2;

		if (bUserPool) {
			memcpy(&stVdecBufferParam[i].stDevAttr, stVdecDevAttr + i, sizeof(VDEC_DEV_ATTR_S));
			stVdecBufferParam[i].bEnableChn0 = IMP_TRUE;
			stVdecBufferParam[i].stChn0Attr.enOutputFormat = PIX_FMT_NV12;
			stVdecBufferParam[i].stChn0Attr.u32OutputWidth = stVencChnAttr[i].u32PicWidth;
			stVdecBufferParam[i].stChn0Attr.u32OutputHeight = stVencChnAttr[i].u32PicHeight;
			stVdecBufferParam[i].stChn0Attr.u32FrameBufCnt = u32FrameBufferCnt;
			stVdecBufferParam[i].bEnableChn1 = IMP_FALSE;

			sprintf(PoolName, "Dev%d-pool", i);
			s32Ret = IMP_System_CreatPool(u32VdPoolId + i, IMP_VDEC_GetBufferSize(stVdecBufferParam + i), 1, PoolName);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
				goto END4;
			}

			s32Ret = IMP_VDEC_AttachVbPool(i, u32VdPoolId[i]);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VDEC_AttachVbPool failed! ret = 0x%08x\n", s32Ret);
				goto END4;
			}
		}

		s32Ret = sample_vdec_start_dev(i, &stVdecDevAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END5;
		}

		stVdecChnAttr[i].enOutputFormat = PIX_FMT_NV12;
		stVdecChnAttr[i].u32OutputWidth = stVencChnAttr[i].u32PicWidth;
		stVdecChnAttr[i].u32OutputHeight = stVencChnAttr[i].u32PicHeight;
		stVdecChnAttr[i].bEnableCrop = IMP_FALSE;
		stVdecChnAttr[i].u32FrameBufCnt = u32FrameBufferCnt;
		s32Ret = sample_vdec_start_chn(i, s32VdecChn, &stVdecChnAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END6;
		}
		sample_vdec_start_recv_stream(i);
	}

    /************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(s32ChnCnt, &stThreadParam[0], &stVdecDevAttr[0], "1920x1080.hevc");
	s32Ret = sample_vdec_start_send_stream(s32ChnCnt, &stThreadParam[0], &sendTid[0]);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END7;
	}

    /************************************************
    step6:  save jpg picture
    *************************************************/
	s32Ret = sample_venc_start_get_stream(s32VencChn, s32ChnCnt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_venc_startgetstream failed! ret = 0x%08x\n", s32Ret);
		goto END8;
	}

	pthread_join(g_VencGetPid, IMP_NULL);

	for(i = 0; i < s32ChnCnt; i++){
		pthread_join(sendTid[i],IMP_NULL);
	}

	sample_venc_stop_get_stream();
END8:
	sample_vdec_stop_send_stream(s32ChnCnt, &stThreadParam[0], &sendTid[0]);
END7:
	for(i = 0; i < s32ChnCnt; i++){
		sample_vdec_stop_chn(i,s32VdecChn);
	}
END6:
	for(i = 0; i < s32ChnCnt; i++){
		sample_vdec_stop_dev(i);
	}
END5:
	for (i = 0; i < s32ChnCnt; i++) {
		if (bUserPool) {
			IMP_VDEC_DetachVbPool(i);
			s32Ret = IMP_System_DestroyPool(u32VdPoolId[i], NULL);
		}
	}
END4:
	for (i = 0; i < s32ChnCnt; i++) {
		sample_venc_stop_chn(s32VencChn[i]);
	}
END3:
	for (i = 0; i < s32ChnCnt; i++) {
		if (bUserPool) {
			IMP_VENC_DetachVbPool(i);
			IMP_System_DestroyPool(u32VePoolId[i], NULL);
		}
	}
END2:
	for (i = 0; i < s32ChnCnt; i++) {
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = i;
		stVdecCell.u32ChnId = s32VdecChn;
		stVencCell.enModId = MOD_ID_VENC;
		stVencCell.u32DevId = s32VencDev;
		stVencCell.u32ChnId = i;
		IMP_System_UnBind(&stVdecCell, &stVencCell);
	}
END1:
	sample_system_exit();
END0:
	return s32Ret;
}

/*
   SAMPLE_VENC_JPEG_Unbind是用户自行送给venc模块NV12图片，然后
   编码出JPEG图片，并保存到当前目录下；默认开启8个编码通道，每
   个通道都会编码输出10张jpg图片。
 */
IMP_S32 SAMPLE_VENC_JPEG_Unbind(IMP_VOID)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 i;
	IMP_S32 bUserPool = IMP_FALSE;
	IMP_S32 s32VencChn[MAX_VENC_CHN] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	IMP_U32 u32VePoolId[MAX_VENC_CHN] = {0};
	IMP_CHAR PoolName[32] = {0};
	VENC_CHN_ATTR_S chnAttr[MAX_VENC_CHN] = {
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 3840,
			.u32OutPicHeight = 2160
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 1920,
			.u32OutPicHeight = 1080
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 640,
			.u32OutPicHeight = 360
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 3840,
			.u32OutPicHeight = 2160
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 1920,
			.u32OutPicHeight = 1080
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 640,
			.u32OutPicHeight = 360
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 3840,
			.u32OutPicHeight = 2160
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 1920,
			.u32OutPicHeight = 1080
		},
		{
			.enType = PT_JPEG,
			.u32BufSize = 0,//设置为0, 会自动计算bufsize
			.u32ProFile = 0,
			.u32PicWidth = 1920,
			.u32PicHeight = 1080,
			.u32OutPicWidth = 640,
			.u32OutPicHeight = 360
		},
	};

	FILE *inFile = fopen("1920x1080.nv12", "rb");
	if (!inFile) {
		IMP_LOG_ERR(TAG, "fopen inFile %s failed", "1920x1080.nv12");
		goto END0;
	}
	IMP_S32 s32FrmCnt = 10;
	IMP_S32 s32ChnCnt = 8;

	/************************************************
    step1:  init system
    *************************************************/
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_system_init failed! ret = 0x%08x\n", s32Ret);
		goto END1;
	}

	/************************************************
    step2:  start venc
    *************************************************/
	for (i = 0; i < s32ChnCnt; i++) {
		if (bUserPool) {
			sprintf(PoolName, "Chn%d-pool", i);
			s32Ret = IMP_System_CreatPool(u32VePoolId + i, IMP_VENC_GetBufferSize(&chnAttr[i]), 1, PoolName);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
				goto END2;
			}

			s32Ret = IMP_VENC_AttachVbPool(i, u32VePoolId[i]);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VDEC_AttachVbPool failed! ret = 0x%08x\n", s32Ret);
				goto END2;
			}
		}
		s32Ret = sample_venc_start_chn(s32VencChn[i], &chnAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_venc_start %d failed! ret = 0x%08x\n", i, s32Ret);
			goto END3;
		}
	}

	/************************************************
    step3:  send frame to venc
    *************************************************/
	s32Ret = sample_venc_start_send_frame(s32VencChn, s32ChnCnt, s32FrmCnt, inFile);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_venc_start_send_frame %d failed! ret = 0x%08x\n", i, s32Ret);
		goto END4;
	}

	pthread_join(g_VencSendPid, IMP_NULL);

	sample_venc_stop_send_frame();
END4:
	for (i = 0; i < s32ChnCnt; i++) {
		sample_venc_stop_chn(s32VencChn[i]);
	}
END3:
	for (i = 0; i < s32ChnCnt; i++) {
		if (bUserPool) {
			IMP_VDEC_DetachVbPool(i);
			s32Ret = IMP_System_DestroyPool(u32VePoolId[i], NULL);
		}
	}
END2:
	sample_system_exit();
END1:
	fclose(inFile);
END0:
	return s32Ret;
}

int main(int argc,char *argv[])
{
    IMP_S32 s32Ret = IMP_SUCCESS;
    IMP_CHAR ch;
    IMP_BOOL bExit = IMP_FALSE;

	while(1){
		SAMPLE_VENC_Usage();
        ch = getchar();
		if (10 == ch){
			continue;
        }
        getchar();
        switch (ch){
            case '0':{
				printf("\nBind JPEG encoding,Please wait...");
    	        SAMPLE_VENC_JPEG_Bind();
    	        break;
            }
            case '1':{
				printf("\nUnbind JPEG encoding,Please wait...");
    	        SAMPLE_VENC_JPEG_Unbind();
    	        break;
            }
            case 'q':
            case 'Q':{
    	        bExit = IMP_TRUE;
    	        break;
    	    }
			default:{
    	        printf("input invaild! please try again.\n");
    	        break;
    	    }
		}
	    if (bExit){
            break;
        }
	}
	return s32Ret;
}
