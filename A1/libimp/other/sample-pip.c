#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#include "../common/sample-common.h"

#define TAG "Sample-Pip"

void test_pip_usage(void)
{
	printf("\n\n/************************************/\n");
	printf("please choose the Cmd which you want to run:\n");
    printf("\tzoomin:  zoom in on the screen\n");
    printf("\tzoomout: zoom out on the screen\n");
	printf("\tquit : quit the program\n");
}

/*
   画中画功能，开启VO layer0和layer1层，分别绑定解码Device0下的
   Channel0和Channel1；layer0全屏显示，layer1左下角显示小窗口。
 */
int main(int argc,char *argv[])
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32VoMod;
	IMP_S32 s32VoChn;
	IMP_S32 s32VdecDev;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	SAMPLE_VDEC_THREAD_S stThreadParam;
	VDEC_DEV_ATTR_S stVdecDevAttr;
	VDEC_CHN_ATTR_S stVdecChnAttr;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	VO_CHN_ATTR_S stVoChnAttr;
	pthread_t sendTid;
	IMP_CHAR Cmd[64] = {0};
	VO_ZOOM_ATTR_S stZoomAttr;

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
	s32VdecDev = 0;
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = s32VdecDev;
	stVdecCell.u32ChnId = 0;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER0;
	stVoCell.u32ChnId = 0;
	s32Ret = IMP_System_Bind(&stVdecCell, &stVoCell);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
		goto END0;
	}
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = s32VdecDev;
	stVdecCell.u32ChnId = 1;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER1;
	stVoCell.u32ChnId = 0;
	s32Ret = IMP_System_Bind(&stVdecCell, &stVoCell);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
		goto END1;
	}

    /************************************************
    step3:  start vo
    *************************************************/
	s32VoMod = SAMPLE_VO_MODULE;
	IMP_VO_Disable(s32VoMod);
	stVoPubAttr.u32BgColor = 0x00000000;
	stVoPubAttr.stCanvasSize.u32Width = 1920;
	stVoPubAttr.stCanvasSize.u32Height = 1080;
	stVoPubAttr.enIntfType = VO_INTF_VGA|VO_INTF_HDMI;
	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	s32Ret = sample_vo_start_module(s32VoMod,&stVoPubAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_module failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	if(stVoPubAttr.enIntfType & VO_INTF_HDMI){
		s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
		if(s32Ret != IMP_SUCCESS){
			IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
			goto END3;
		}
	}

	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END4;
	}

	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(SAMPLE_VO_LATER0, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END5;
	}
	s32Ret = sample_vo_start_layer(SAMPLE_VO_LATER1, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END6;
	}

	s32VoChn = SAMPLE_VO_CHN0;
	stVoChnAttr.u32Priority = 0;
	stVoChnAttr.bDeflicker = IMP_FALSE;
	stVoChnAttr.stRect.s32X = ORIGINAL_X;
	stVoChnAttr.stRect.s32Y = ORIGINAL_Y;
	stVoChnAttr.stRect.u32Width = 1920;
	stVoChnAttr.stRect.u32Height = 1080;
	s32Ret = sample_vo_start_chn(SAMPLE_VO_LATER0,s32VoChn,&stVoChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END7;
	}

	stVoChnAttr.stRect.u32Width = 640;
	stVoChnAttr.stRect.u32Height = 360;
	stVoChnAttr.stRect.s32X = ORIGINAL_X;
	stVoChnAttr.stRect.s32Y = stVoLayerAttr.stImageSize.u32Height - stVoChnAttr.stRect.u32Height;
	s32Ret = sample_vo_start_chn(SAMPLE_VO_LATER1,s32VoChn,&stVoChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END8;
	}

    /************************************************
    step4:  start vdec
    *************************************************/
	stVdecDevAttr.enType = PT_H265;
	stVdecDevAttr.enMode = VIDEO_MODE_FRAME;
	stVdecDevAttr.u32PicWidth = 1920;
	stVdecDevAttr.u32PicHeight = 1080;
	stVdecDevAttr.u32StreamBufSize = 1920*1080*3/4;
	stVdecDevAttr.u32RefFrameNum = 2;
	s32Ret = sample_vdec_start_dev(s32VdecDev, &stVdecDevAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END9;
	}

	stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
	stVdecChnAttr.u32OutputWidth = 1920;
	stVdecChnAttr.u32OutputHeight = 1080;
	stVdecChnAttr.bEnableCrop = IMP_FALSE;
	stVdecChnAttr.u32FrameBufCnt = 3;
	s32Ret = sample_vdec_start_chn(s32VdecDev,0,&stVdecChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END10;
	}

	stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
	stVdecChnAttr.u32OutputWidth = 640;
	stVdecChnAttr.u32OutputHeight = 360;
	stVdecChnAttr.bEnableCrop = IMP_FALSE;
	stVdecChnAttr.u32FrameBufCnt = 3;
	s32Ret = sample_vdec_start_chn(s32VdecDev,1,&stVdecChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		goto END11;
	}

	s32Ret = sample_vdec_start_recv_stream(s32VdecDev);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_recv_stream failed! ret = 0x%08x\n", s32Ret);
		goto END12;
	}

    /************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(1,&stThreadParam,&stVdecDevAttr,"1920x1080.hevc");
	stThreadParam.bLoopPlayback = IMP_TRUE;
	s32Ret = sample_vdec_start_send_stream(1,&stThreadParam, &sendTid);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END13;
	}

	stZoomAttr.stZoomRect.s32X = 0;
	stZoomAttr.stZoomRect.s32Y = 0;
	stZoomAttr.stZoomRect.u32Width = stVoLayerAttr.stImageSize.u32Width;
	stZoomAttr.stZoomRect.u32Height = stVoLayerAttr.stImageSize.u32Height;

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
			test_pip_usage();
		}else if (strncmp("zoomin", Cmd, strlen("zoomin")) == 0) {
			stZoomAttr.stZoomRect.s32X += 16;
			stZoomAttr.stZoomRect.s32Y += 8;
			stZoomAttr.stZoomRect.u32Width -= 32;
			stZoomAttr.stZoomRect.u32Height -= 16;
			IMP_VO_SetZoomInWindow(SAMPLE_VO_LATER0,0,&stZoomAttr);
		}else if (strncmp("zoomout", Cmd, strlen("zoomout")) == 0) {
			stZoomAttr.stZoomRect.s32X -= 16;
			stZoomAttr.stZoomRect.s32Y -= 8;
			stZoomAttr.stZoomRect.u32Width +=32;
			stZoomAttr.stZoomRect.u32Height += 16;
			IMP_VO_SetZoomInWindow(SAMPLE_VO_LATER0,0,&stZoomAttr);
		}else if(strncmp("quit", Cmd, strlen("quit")) == 0){
			break;
		}else{
			printf("Unkown command ,enter 'help' to see usage\n");
		}
	}
	sample_vdec_stop_send_stream(1,&stThreadParam, &sendTid);
END13:
	sample_vdec_stop_recv_stream(0);
END12:
	sample_vdec_stop_chn(0,1);
END11:
	sample_vdec_stop_chn(0,0);
END10:
	sample_vdec_stop_dev(0);
END9:
	sample_vo_stop_chn(SAMPLE_VO_LATER1,s32VoChn);
END8:
	sample_vo_stop_chn(SAMPLE_VO_LATER0,s32VoChn);
END7:
	sample_vo_stop_layer(SAMPLE_VO_LATER1);
END6:
	sample_vo_stop_layer(SAMPLE_VO_LATER0);
END5:
END4:
	if(stVoPubAttr.enIntfType & VO_INTF_HDMI){
		sample_vo_hdmi_stop();
	}
END3:
	sample_vo_stop_module(s32VoMod);
END2:
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = 0;
	stVdecCell.u32ChnId = 1;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER1;
	stVoCell.u32ChnId = 0;
	IMP_System_UnBind(&stVdecCell, &stVoCell);
END1:
	stVdecCell.enModId = MOD_ID_VDEC;
	stVdecCell.u32DevId = 0;
	stVdecCell.u32ChnId = 0;
	stVoCell.enModId = MOD_ID_VO;
	stVoCell.u32DevId = SAMPLE_VO_LATER0;
	stVoCell.u32ChnId = 0;
	IMP_System_UnBind(&stVdecCell, &stVoCell);
END0:
	sample_system_exit();
	return IMP_SUCCESS;
}

