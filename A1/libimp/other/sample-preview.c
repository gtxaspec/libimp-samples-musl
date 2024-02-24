#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#include "../common/sample-common.h"

#define TAG "sample-preview"
#define DECODE_FILE "640x360.hevc"

static IMP_S32 s32MaxVoChn;
static IMP_S32 s32MaxVdecChn;
static IMP_S32 s32PreviewMode;
static IMP_S32 s32PreviewFirstChn;
static IMP_BOOL bVdecDevOn[MAX_VDEC_DEVS];

void test_preview_usage(void)
{
	printf("\n\n/************************************/\n");
	printf("please choose the Cmd which you want to run:\n");
	printf("\t1mux : 1 window mode\n");
	printf("\t4mux : 4 window mode\n");
	printf("\t9mux : 9 window mode\n");
	printf("\t16mux : 16 window mode\n");
	printf("\t25mux : 25 window mode\n");
	printf("\t36mux : 36 window mode\n");
	printf("\tpause : pause the channel\n");
	printf("\tresume : resume the channel\n");
	printf("\tstep : play frame by frame\n");
	printf("\tbrightness : set brightness\n");
	printf("\tcontrast : set contrast\n");
	printf("\tsaturation : set saturation\n");
	printf("\thue : set hue\n");
	printf("\tquit : quit the program\n");
}

IMP_S32 sample_preview_switch(SAMPLE_VO_MODE_E enVoMode,IMP_S32 s32FirstChn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 i;
	IMP_S32 s32VoLayer = SAMPLE_VO_LATER0;
	IMP_S32 s32VdecChn = 0;
	IMP_U32 u32WndNum = 0;
	IMP_U32 u32Square = 0;
	IMP_U32 u32Width = 0;
	IMP_U32 u32Height = 0;
	VO_VIDEO_LAYER_ATTR_S stLayerAttr;
	VO_CHN_ATTR_S stOldVoChnAttr;
	VO_CHN_ATTR_S stNewVoChnAttr;
	VDEC_CHN_ATTR_S stVdecChnAttr;
	IMP_S32 s32EndChn = s32FirstChn + enVoMode;
	memset(&stVdecChnAttr,0,sizeof(VDEC_CHN_ATTR_S));

	if(s32EndChn > s32MaxVoChn){
		return IMP_FAILURE;
	}

	for (i = s32PreviewFirstChn; i < s32PreviewFirstChn + s32PreviewMode; i++){
		IMP_VO_HideChn(s32VoLayer, i);
		sample_vdec_stop_chn(i,s32VdecChn);
	}

	switch (enVoMode){
		case VO_MODE_1MUX:
			u32WndNum = 1;
			u32Square = 1;
			break;
		case VO_MODE_4MUX:
			u32WndNum = 4;
			u32Square = 2;
			break;
		case VO_MODE_9MUX:
			u32WndNum = 9;
			u32Square = 3;
			break;
		case VO_MODE_16MUX:
			u32WndNum = 16;
			u32Square = 4;
			break;
		case VO_MODE_25MUX:
			u32WndNum = 25;
			u32Square = 5;
			break;
		case VO_MODE_36MUX:
			u32WndNum = 36;
			u32Square = 6;
			break;
		default:
			IMP_LOG_ERR(TAG,"vo display mode not support!\n");
			return IMP_FAILURE;
	}

	s32Ret = IMP_VO_GetVideoLayerAttr(s32VoLayer, &stLayerAttr);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"%s(%d):failed with %#x!\n",__func__,__LINE__,s32Ret);
		return IMP_FAILURE;
	}
	u32Width = stLayerAttr.stImageSize.u32Width;
	u32Height = stLayerAttr.stImageSize.u32Height;

	for (i = s32FirstChn; i < s32EndChn; i++){
		//获取VO channel属性，以判断接下来切屏是否需要重建VO channel
		s32Ret = IMP_VO_GetChnAttr(s32VoLayer, i, &stOldVoChnAttr);
		if(s32Ret != IMP_SUCCESS){
			IMP_LOG_ERR(TAG,"%s(%d):IMP_VO_GetChnAttr failed with %#x!\n",__func__,__LINE__,s32Ret);
			return s32Ret;
		}
		stNewVoChnAttr.stRect.s32X       = ALIGN_BACK(ALIGN_BACK(u32Width/u32Square,2) * ((i-s32FirstChn)%u32Square), 2);
		stNewVoChnAttr.stRect.s32Y       = ALIGN_BACK((u32Height/u32Square) * ((i-s32FirstChn)/u32Square), 2);
		stNewVoChnAttr.stRect.u32Width   = ALIGN_BACK(u32Width/u32Square, 2);
		stNewVoChnAttr.stRect.u32Height  = ALIGN_BACK(u32Height/u32Square, 2);
		stNewVoChnAttr.u32Priority       = 0;
		stNewVoChnAttr.bDeflicker        = IMP_FALSE;
		if(	stOldVoChnAttr.stRect.s32X != stNewVoChnAttr.stRect.s32X ||
			stOldVoChnAttr.stRect.s32Y != stNewVoChnAttr.stRect.s32Y ||
			stOldVoChnAttr.stRect.u32Width != stNewVoChnAttr.stRect.u32Width ||
			stOldVoChnAttr.stRect.u32Height != stNewVoChnAttr.stRect.u32Height){
			s32Ret = IMP_VO_SetChnAttr(s32VoLayer, i, &stNewVoChnAttr);
			if (s32Ret != IMP_SUCCESS){
				IMP_LOG_ERR(TAG,"%s(%d):IMP_VO_SetChnAttr failed with %#x!\n",__func__,__LINE__,s32Ret);
				return s32Ret;
			}
		}
		if(i < s32MaxVdecChn && bVdecDevOn[i] == IMP_TRUE){
			stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
			stVdecChnAttr.u32OutputWidth = ALIGN_UP(stNewVoChnAttr.stRect.u32Width,8);
			stVdecChnAttr.u32OutputHeight = stNewVoChnAttr.stRect.u32Height;
			stVdecChnAttr.bEnableCrop = IMP_FALSE;
			stVdecChnAttr.u32FrameBufCnt = 3;
			s32Ret = sample_vdec_start_chn(i,s32VdecChn,&stVdecChnAttr);
			if (s32Ret != IMP_SUCCESS){
				IMP_LOG_ERR(TAG,"%s(%d):sample_vdec_start_chn failed with %#x!\n",__func__,__LINE__,s32Ret);
				return s32Ret;
			}
			IMP_VO_ShowChn(s32VoLayer, i);
		}
	}
	s32PreviewFirstChn = s32FirstChn;
	s32PreviewMode = enVoMode;
	IMP_LOG_INFO(TAG,"preview mode =%d first channel = %d u32WndNum=%d\n", s32PreviewMode,s32PreviewFirstChn, u32WndNum);
	return IMP_SUCCESS;
}

IMP_S32 sample_enable_vo_all(SAMPLE_VO_MODE_E enVoMode)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32VoMod;
	IMP_S32 s32VoLayer;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;

	memset(&stVoPubAttr,0,sizeof(VO_PUB_ATTR_S));
	memset(&stVoLayerAttr,0,sizeof(VO_VIDEO_LAYER_ATTR_S));

	s32VoMod = SAMPLE_VO_MODULE;
	stVoPubAttr.u32BgColor = 0x00000000;
#if 1
	stVoPubAttr.stCanvasSize.u32Width = 1920;
	stVoPubAttr.stCanvasSize.u32Height = 1080;
	stVoPubAttr.enIntfType = VO_INTF_VGA | VO_INTF_HDMI;
	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
#else
	stVoPubAttr.stCanvasSize.u32Width = 3840;
	stVoPubAttr.stCanvasSize.u32Height = 2160;
	stVoPubAttr.enIntfType = VO_INTF_HDMI;
	stVoPubAttr.enIntfSync = VO_OUTPUT_3840x2160_30;
#endif
	s32Ret = sample_vo_start_module(s32VoMod,&stVoPubAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_module failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto END0;
	}
	s32VoLayer = SAMPLE_VO_LATER0;
	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END1;
	}
	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(s32VoLayer, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	s32Ret = sample_vo_start_chns(s32VoLayer,enVoMode);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END3;
	}
	IMP_VO_EnableBcsh(s32VoLayer);
	s32MaxVoChn = enVoMode;
	s32PreviewMode = enVoMode;
	s32PreviewFirstChn = 0;

	return IMP_SUCCESS;
END3:
	sample_vo_stop_layer(s32VoLayer);
END2:
END1:
	sample_vo_hdmi_stop();
END0:
	sample_vo_stop_module(s32VoMod);
	return s32Ret;
}

IMP_VOID sample_disable_vo_all(SAMPLE_VO_MODE_E enVoMode)
{
	IMP_S32 s32VoMod = SAMPLE_VO_MODULE;
	IMP_S32 s32VoLayer = SAMPLE_VO_LATER0;
	IMP_VO_DisableBcsh(s32VoLayer);
	sample_vo_stop_chns(s32VoLayer, enVoMode);
	sample_vo_stop_layer(s32VoLayer);
	sample_vo_hdmi_stop();
	sample_vo_stop_module(s32VoMod);
}

IMP_S32 sample_preview_vdec_open(IMP_S32 s32PrevWin,PAYLOAD_TYPE_E enType,IMP_U32 u32Width,IMP_U32 u32Height)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	VDEC_DEV_ATTR_S stVdecDevAttr;
	VDEC_CHN_ATTR_S stVdecChnAttr;
	VO_CHN_ATTR_S stVoChnAttr;
	IMP_S32 s32VdecChn = 0;
	IMP_S32 s32VoLayer = SAMPLE_VO_LATER0;

	memset(&stVdecDevAttr,0,sizeof(VDEC_DEV_ATTR_S));
	memset(&stVdecChnAttr,0,sizeof(VDEC_CHN_ATTR_S));

	stVdecDevAttr.enType = enType;
	stVdecDevAttr.enMode = VIDEO_MODE_FRAME;
	stVdecDevAttr.u32PicWidth = u32Width;
	stVdecDevAttr.u32PicHeight = u32Height;
	stVdecDevAttr.u32StreamBufSize = u32Width * u32Height * 3 / 4;
	switch(enType){
		case PT_H264:
			stVdecDevAttr.u32RefFrameNum = 2;
			break;
		case PT_H265:
			stVdecDevAttr.u32RefFrameNum = 2;
			break;
		case PT_JPEG:
			stVdecDevAttr.u32RefFrameNum = 0;
			break;
		default:
			IMP_LOG_ERR(TAG, "stream type error\n");
			return IMP_FAILURE;
	}

	s32Ret = sample_vdec_start_dev(s32PrevWin, &stVdecDevAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	IMP_VO_GetChnAttr(s32VoLayer,s32PrevWin,&stVoChnAttr);
	stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
	stVdecChnAttr.u32OutputWidth = ALIGN_UP(stVoChnAttr.stRect.u32Width,8);
	stVdecChnAttr.u32OutputHeight = stVoChnAttr.stRect.u32Height;
	stVdecChnAttr.bEnableCrop = IMP_FALSE;
	stVdecChnAttr.u32FrameBufCnt = 3;
	s32Ret = sample_vdec_start_chn(s32PrevWin,s32VdecChn,&stVdecChnAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END0;
	}
	bVdecDevOn[s32PrevWin] = IMP_TRUE;
	s32Ret = sample_vdec_start_recv_stream(s32PrevWin);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END1;
	}

	return IMP_SUCCESS;
END1:
	sample_vdec_stop_chn(s32PrevWin,s32VdecChn);
END0:
	sample_vdec_stop_dev(s32PrevWin);
	return IMP_SUCCESS;
}

IMP_VOID sample_preview_vdec_close(IMP_S32 s32PrevWin)
{
	IMP_S32 s32VdecChn = 0;
	sample_vdec_stop_recv_stream(s32PrevWin);
	sample_vdec_stop_chn(s32PrevWin,s32VdecChn);
	sample_vdec_stop_dev(s32PrevWin);
	bVdecDevOn[s32PrevWin] = IMP_FALSE;
}

/*
   sample-preview是一个综合性sample，可以实现切屏、暂停、恢复、
   逐帧播放、修改图像效果、VO抓图功能。
 */
int main(int argc,char *argv[])
{
	IMP_S32 i = 0;
	IMP_S32 s32Ret = IMP_SUCCESS;
	pthread_t sendTid[MAX_VDEC_DEVS];
	SAMPLE_VDEC_THREAD_S stVdecThreadPara[MAX_VDEC_DEVS];
	IMP_CHAR Cmd[64] = {0};
	IMP_S32 s32VoLayer = SAMPLE_VO_LATER0;
	IMP_U32 u32VoMux = VO_MODE_16MUX;
	IMP_U32 u32VdecMux = VO_MODE_16MUX;
	IMP_S32 s32VdecChn = 0;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	IMP_U32 u32FirstChn = 0;
	IMP_U32 u32PauseChn = 0;
	IMP_U32 u32SnapChn = 0;
	FILE *voSnapfd = NULL;
	IMP_CHAR access_name[64] = {0};
	FRAME_INFO_S *pstFrame = NULL;
	IMP_U8 u8Brightness = 128;
	IMP_U8 u8Saturation = 128;
	IMP_U8 u8Hue = 128;
	IMP_U8 u8Contrast = 128;
	IMP_BOOL bcshChange = IMP_FALSE;
	VO_BCSH_S stBcsh;

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
	//如果绑定关系不变，在切屏过程中可以不用Unbind
	for(i = 0; i < u32VdecMux; i++){
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = i;
		stVdecCell.u32ChnId = s32VdecChn;
		stVoCell.enModId = MOD_ID_VO;
		stVoCell.u32DevId = s32VoLayer;
		stVoCell.u32ChnId = i;
		s32Ret = IMP_System_Bind(&stVdecCell, &stVoCell);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
			goto END0;
		}
	}

    /************************************************
    step3:  start vo
    *************************************************/
	s32Ret = sample_enable_vo_all(u32VoMux);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_enable_vo_all failed! ret = 0x%08x\n", s32Ret);
		goto END1;
	}

    /************************************************
    step4:  start vdec
    *************************************************/
	for(i = 0; i < u32VdecMux; i++){
		s32Ret = sample_preview_vdec_open(i,PT_H265,640,360);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_preview_vdec_open failed! ret = 0x%08x\n", s32Ret);
			goto END2;
		}
		stVdecThreadPara[i].s32DevId = i;
		sprintf(stVdecThreadPara[i].cFileName,DECODE_FILE);
		stVdecThreadPara[i].enType = PT_H265;
		stVdecThreadPara[i].enVideoMode = VIDEO_MODE_FRAME;
		stVdecThreadPara[i].u32BufSize = 640 * 360 * 3 / 4;
		stVdecThreadPara[i].enCtrlSig = VDEC_CTRL_START;
		stVdecThreadPara[i].s32FpsCtrl = 25;
		stVdecThreadPara[i].bLoopPlayback = IMP_TRUE;
		pthread_create(&sendTid[i], 0, sample_vdec_send_stream_thread, (void *)&stVdecThreadPara[i]);
	}
	s32MaxVdecChn = u32VdecMux;
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
			test_preview_usage();
		}else if(strncmp("1mux", Cmd, strlen("1mux")) == 0){
			u32FirstChn = atoi(&Cmd[strlen("1mux")]);
			sample_preview_switch(VO_MODE_1MUX,u32FirstChn);
		}else if(strncmp("4mux", Cmd, strlen("4mux")) == 0){
			u32FirstChn = atoi(&Cmd[strlen("4mux")]);
			sample_preview_switch(VO_MODE_4MUX,u32FirstChn);
		}else if(strncmp("9mux", Cmd, strlen("9mux")) == 0){
			u32FirstChn = atoi(&Cmd[strlen("9mux")]);
			sample_preview_switch(VO_MODE_9MUX,u32FirstChn);
		}else if(strncmp("16mux", Cmd, strlen("16mux")) == 0){
			u32FirstChn = atoi(&Cmd[strlen("16mux")]);
			sample_preview_switch(VO_MODE_16MUX,u32FirstChn);
		}else if(strncmp("25mux", Cmd, strlen("25mux")) == 0){
			u32FirstChn = atoi(&Cmd[strlen("25mux")]);
			sample_preview_switch(VO_MODE_25MUX,u32FirstChn);
		}else if(strncmp("36mux", Cmd, strlen("36mux")) == 0){
			u32FirstChn = atoi(&Cmd[strlen("36mux")]);
			sample_preview_switch(VO_MODE_36MUX,u32FirstChn);
		}else if(strncmp("pause", Cmd, strlen("pause")) == 0){
			u32PauseChn = atoi(&Cmd[strlen("pause")]);
			IMP_VO_PauseChn(s32VoLayer, u32PauseChn);
		}else if(strncmp("resume", Cmd, strlen("resume")) == 0){
			u32PauseChn = atoi(&Cmd[strlen("resume")]);
			IMP_VO_ResumeChn(s32VoLayer, u32PauseChn);
		}else if(strncmp("step", Cmd, strlen("step")) == 0){
			u32PauseChn = atoi(&Cmd[strlen("step")]);
			IMP_VO_StepChn(s32VoLayer, u32PauseChn);
		}else if(strncmp("brightness", Cmd, strlen("brightness")) == 0){
			u8Brightness = atoi(&Cmd[strlen("brightness")]);
			bcshChange = IMP_TRUE;
		}else if(strncmp("contrast", Cmd, strlen("contrast")) == 0){
			u8Contrast = atoi(&Cmd[strlen("contrast")]);
			bcshChange = IMP_TRUE;
		}else if(strncmp("saturation", Cmd, strlen("saturation")) == 0){
			u8Saturation = atoi(&Cmd[strlen("saturation")]);
			bcshChange = IMP_TRUE;
		}else if(strncmp("hue", Cmd, strlen("hue")) == 0){
			u8Hue = atoi(&Cmd[strlen("hue")]);
			bcshChange = IMP_TRUE;
		}else if(strncmp("snap", Cmd, strlen("snap")) == 0){
			u32SnapChn = atoi(&Cmd[strlen("snap")]);
			IMP_VO_GetChnFrame(s32VoLayer,u32SnapChn,&pstFrame);
			sprintf(access_name, "/tmp/layer%d-chn%d.nv12", s32VoLayer, u32SnapChn);
			voSnapfd = fopen(access_name, "w+");
			fwrite((void *)pstFrame->u32VirAddr, 1, pstFrame->u32Size, voSnapfd);
			fclose(voSnapfd);
			IMP_VO_ReleaseChnFrame(s32VoLayer,u32SnapChn,pstFrame);
		}else if(strncmp("quit", Cmd, strlen("quit")) == 0){
			break;
		}else{
			printf("Unkown command ,enter 'help' to see usage\n");
		}
		if(bcshChange == IMP_TRUE){
			stBcsh.u8Brightness = u8Brightness;
			stBcsh.u8Contrast = u8Contrast;
			stBcsh.u8Saturation = u8Saturation;
			stBcsh.u8Hue = u8Hue;
			IMP_VO_SetVideoLayerBcsh(s32VoLayer,&stBcsh);
		}
	}
END2:
	for(i = 0; i < u32VdecMux; i++){
		stVdecThreadPara[i].enCtrlSig = VDEC_CTRL_STOP;
		if (sendTid[i]){
			pthread_join(sendTid[i],IMP_NULL);
		}
		sendTid[i] = 0;
		sample_preview_vdec_close(i);
	}
	sample_disable_vo_all(u32VoMux);
END1:
	for(i = 0; i < u32VdecMux; i++){
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = i;
		stVdecCell.u32ChnId = s32VdecChn;
		stVoCell.enModId = MOD_ID_VO;
		stVoCell.u32DevId = s32VoLayer;
		stVoCell.u32ChnId = i;
		IMP_System_UnBind(&stVdecCell,&stVoCell);
	}
END0:
	sample_system_exit();

	return s32Ret;
}
