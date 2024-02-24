#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#include "../common/sample-common.h"

#define TAG "Sample-Snap"

void test_snap_usage(void)
{
	printf("\n\n/************************************/\n");
	printf("please choose the Cmd which you want to run:\n");
	printf("\tsnap : snap a picture\n");
	printf("\tquit : quit the program\n");
}

IMP_S32 sample_tde_quickcopy(TDE_SURFACE_S *pstSrcTdeData,TDE_RECT_S *pstSrcRect,
		TDE_SURFACE_S *pstDstTdeData,TDE_RECT_S *pstDstRect)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	TDE_HANDLE Handle;
	IMP_BOOL block;
	IMP_U32 timeout;

	Handle = IMP_TDE_BeginJob(1);
	if(Handle < 0) {
		IMP_LOG_ERR(TAG,"IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
		return Handle;
	}

	s32Ret = IMP_TDE_QuickCopy(Handle, pstSrcTdeData, pstSrcRect,
			pstDstTdeData, pstDstRect);
	if(s32Ret < 0) {
		IMP_LOG_ERR(TAG,"IMP_TDE_QuickCopy error (%d)\n", s32Ret);
		IMP_TDE_CancelJob(Handle);
		return s32Ret;
	}

	block = IMP_TRUE;
	timeout = 1000;
	s32Ret = IMP_TDE_EndJob(Handle, 0, block, timeout);
	if(s32Ret < 0) {
		printf("IMP_TDE_EndJob error (%d)\n",s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

/*
   sample-snap是从VO模块抓取一张NV12图片，再送给venc模块编码
   成jpg图片，存放到当前路径下。
 */
int main(int argc,char *argv[])
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32VoMod;
	IMP_S32 s32VoLayer;
	IMP_S32 s32VoChn;
	IMP_S32 s32VdecDev;
	IMP_S32 s32VdecChn;
	IMP_S32 s32VencChn;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	SAMPLE_VDEC_THREAD_S stThreadParam;
	VDEC_DEV_ATTR_S stVdecDevAttr;
	VDEC_CHN_ATTR_S stVdecChnAttr;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	VO_CHN_ATTR_S stVoChnAttr;
	VENC_CHN_ATTR_S stVencChnAttr;
	pthread_t sendTid;
	IMP_CHAR Cmd[64] = {0};
	FILE *voSnapfd = NULL;
	IMP_CHAR access_name[64] = {0};
	FRAME_INFO_S *pstFrame = NULL;
	VENC_STREAM_S stStream;

    /************************************************
    step1:  init system
    *************************************************/
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_system_init failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

    /************************************************
    step2:  bind vdec and vo
    *************************************************/
	s32VoLayer = SAMPLE_VO_LATER0;
	s32VoChn = SAMPLE_VO_CHN0;
	s32VdecDev = SAMPLE_VDEC_DEV0;
	s32VdecChn = SAMPLE_VDEC_CHN0;
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = s32VdecDev;
	stVdecCell.u32ChnId = s32VdecChn;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = s32VoLayer;
	stVoCell.u32ChnId = s32VoChn;
	s32Ret = IMP_System_Bind(&stVdecCell, &stVoCell);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
		goto END0;
	}

    /************************************************
    step3:  start vo
    *************************************************/
	s32VoMod = SAMPLE_VO_MODULE;
	IMP_VO_Disable(s32VoMod);
	stVoPubAttr.u32BgColor = 0x00000000;
	stVoPubAttr.stCanvasSize.u32Width = 1920;
	stVoPubAttr.stCanvasSize.u32Height = 1080;
	stVoPubAttr.enIntfType = VO_INTF_HDMI | VO_INTF_VGA;
	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	s32Ret = sample_vo_start_module(s32VoMod,&stVoPubAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_module failed! ret = 0x%08x\n", s32Ret);
		goto END1;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END3;
	}
	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(s32VoLayer, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END4;
	}

	stVoChnAttr.u32Priority = 0;
	stVoChnAttr.bDeflicker = IMP_FALSE;
	stVoChnAttr.stRect.s32X = ORIGINAL_X;
	stVoChnAttr.stRect.s32Y = ORIGINAL_Y;
	stVoChnAttr.stRect.u32Width = 1920;
	stVoChnAttr.stRect.u32Height = 1080;
	s32Ret = sample_vo_start_chn(s32VoLayer,s32VoChn,&stVoChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END5;
	}

    /************************************************
    step4:  start vdec
    *************************************************/
	stVdecDevAttr.enType = PT_H265;
	stVdecDevAttr.enMode = VIDEO_MODE_FRAME;
	stVdecDevAttr.u32PicWidth = 1920;
	stVdecDevAttr.u32PicHeight = 1080;
	stVdecDevAttr.u32StreamBufSize = 1920*1080*3/4;
	stVdecDevAttr.u32RefFrameNum = 3;
	s32Ret = sample_vdec_start_dev(s32VdecDev, &stVdecDevAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END6;
	}

	stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
	stVdecChnAttr.u32OutputWidth = 1920;
	stVdecChnAttr.u32OutputHeight = 1080;
	stVdecChnAttr.bEnableCrop = IMP_FALSE;
	stVdecChnAttr.u32FrameBufCnt = 3;
	s32Ret = sample_vdec_start_chn(s32VdecDev,s32VdecChn,&stVdecChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END7;
	}

	s32Ret = sample_vdec_start_recv_stream(s32VdecDev);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_recv_stream failed! ret = 0x%08x\n", s32Ret);
		goto END8;
	}

    /************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(1,&stThreadParam,&stVdecDevAttr,"1920x1080.hevc");
	stThreadParam.bLoopPlayback = IMP_TRUE;
	s32Ret = sample_vdec_start_send_stream(1,&stThreadParam, &sendTid);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END9;
	}

	while(1){
		printf("> ");
		fflush(stdin);
		if (fgets(Cmd, 64, stdin) == NULL) {
			printf("fgets error\n");
			return -1;
		}
		if (Cmd[strlen(Cmd) - 1] == '\n')
			Cmd[strlen(Cmd) - 1] = '\0';
		if (strncmp("help", Cmd, strlen("help")) == 0) {
			test_snap_usage();
		}else if(strncmp("snap", Cmd, strlen("snap")) == 0){
			IMP_VO_GetChnFrame(s32VoLayer,s32VoChn,&pstFrame);
			s32VencChn = 0;
			memset(&stVencChnAttr, 0x0, sizeof(VENC_CHN_ATTR_S));
			stVencChnAttr.enType = PT_JPEG;
			stVencChnAttr.u32BufSize = 0;
			stVencChnAttr.u32ProFile = 0;
			stVencChnAttr.u32PicWidth = pstFrame->u32Width;
			stVencChnAttr.u32PicHeight = pstFrame->u32Height;
			stVencChnAttr.u32OutPicWidth = pstFrame->u32Width;
			stVencChnAttr.u32OutPicHeight = pstFrame->u32Height;
			stVencChnAttr.stCrop.enCrop = 1;
			//snap area
			stVencChnAttr.stCrop.u32X = 250;
			stVencChnAttr.stCrop.u32Y = 250;
			stVencChnAttr.stCrop.u32W = pstFrame->u32Width/2;
			stVencChnAttr.stCrop.u32H = pstFrame->u32Height/2;
			s32Ret = IMP_VENC_CreateChn(s32VencChn, &stVencChnAttr);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VENC_CreateChn %d failed! ret = 0x%08x\n", s32VencChn, s32Ret);
				IMP_VO_ReleaseChnFrame(s32VoLayer,s32VoChn,pstFrame);
				continue;
			}

			s32Ret = IMP_VENC_StartRecvStream(s32VencChn);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VENC_StartRecvStream %d failed! ret = 0x%08x\n", s32VencChn, s32Ret);
				IMP_VO_ReleaseChnFrame(s32VoLayer,s32VoChn,pstFrame);
				continue;
			}

			s32Ret = IMP_VENC_SendFrame(s32VencChn, pstFrame, -1);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VENC_SendFrame %d failed\n", s32VencChn);
				IMP_VO_ReleaseChnFrame(s32VoLayer,s32VoChn,pstFrame);
				continue;
			}
			s32Ret = IMP_VENC_GetStream(s32VencChn, &stStream, -1);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VENC_GetStream %d failed! ret = 0x%08x\n", s32VencChn, s32Ret);
				IMP_VO_ReleaseChnFrame(s32VoLayer,s32VoChn,pstFrame);
				continue;
			}

			sprintf(access_name, "layer%d-chn%d.jpeg", s32VoLayer, s32VoChn);
			voSnapfd = fopen(access_name, "w+");
			fwrite((void *)stStream.pack[0].u32VirAddr, 1, stStream.pack[0].u32Len, voSnapfd);
			fclose(voSnapfd);

			s32Ret = IMP_VENC_ReleaseStream(s32VencChn, &stStream);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VENC_ReleaseStream %d failed! ret = 0x%08x\n", s32VencChn, s32Ret);
				IMP_VO_ReleaseChnFrame(s32VoLayer,s32VoChn,pstFrame);
				continue;
			}
			IMP_VO_ReleaseChnFrame(s32VoLayer,s32VoChn,pstFrame);
			IMP_VENC_StopRecvStream(s32VencChn);
			IMP_VENC_DestroyChn(s32VencChn);
		}else if(strncmp("quit", Cmd, strlen("quit")) == 0){
			break;
		}else{
			printf("Unkown command ,enter 'help' to see usage\n");
		}
	}
	sample_vdec_stop_send_stream(1,&stThreadParam, &sendTid);
END9:
	sample_vdec_stop_recv_stream(0);
END8:
	sample_vdec_stop_chn(0,0);
END7:
	sample_vdec_stop_dev(0);
END6:
	sample_vo_stop_chn(SAMPLE_VO_LATER0,s32VoChn);
END5:
	sample_vo_stop_layer(SAMPLE_VO_LATER0);
END4:
END3:
	sample_vo_hdmi_stop();
END2:
	sample_vo_stop_module(s32VoMod);
END1:
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = s32VdecDev;
	stVdecCell.u32ChnId = s32VdecChn;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = s32VoLayer;
	stVoCell.u32ChnId = s32VoChn;
	IMP_System_UnBind(&stVdecCell, &stVoCell);
END0:
	sample_system_exit();
	return IMP_SUCCESS;
}

