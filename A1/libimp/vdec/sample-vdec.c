#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>

#include "../common/sample-common.h"

#define TAG "sample-vdec"

IMP_VOID SAMPLE_VDEC_Usage(IMP_VOID)
{
    printf("\n\n/************************************/\n");
    printf("please choose the case which you want to run:\n");
    printf("\t0:  H264 decoding and displaying\n");
    printf("\t1:  H265 decoding and displaying\n");
    printf("\t2:  JPEG decoding\n");
    printf("\t3:  H265 decoding and geting frames\n");
    printf("\t4:  SW Jpeg decoding\n");
    printf("\tq:  quit the whole sample\n");
    printf("sample command:");
}

/*
   SAMPLE_VDEC_H264是基础的H264解码加显示的sample，码流分辨率
   默认640x360；显示的默认分辨率为1080p，16分屏显示。
 */
IMP_S32 SAMPLE_VDEC_H264(IMP_VOID)
{
	IMP_S32 i = 0;
	IMP_S32 s32VdecChn = 0;
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	IMP_S32 s32VoMod;
	IMP_S32 s32VoLayer;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	SAMPLE_VDEC_THREAD_S stThreadParam[MAX_VDEC_DEVS];
	pthread_t sendTid[MAX_VDEC_DEVS];
	VDEC_DEV_ATTR_S stVdecDevAttr[MAX_VDEC_DEVS];
	VDEC_CHN_ATTR_S stVdecChnAttr[MAX_VDEC_DEVS];
	VDEC_BUFFER_PARAM_S stVdecBufferParam[MAX_VDEC_DEVS];
	IMP_U32 u32VoMux = VO_MODE_16MUX;
	IMP_U32 u32VdecMux = VO_MODE_16MUX;
	IMP_U32 u32Square = sqrt(u32VoMux);
	IMP_BOOL bUserPool = IMP_FALSE;
	IMP_U32 u32PoolId[MAX_VDEC_DEVS];
	char s8Name[64];

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
	for(i = 0; i < u32VdecMux; i++){
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = i;
		stVdecCell.u32ChnId = s32VdecChn;
		stVoCell.enModId = MOD_ID_VO;
		stVoCell.u32DevId = s32VoLayer;
		stVoCell.u32ChnId = i;
		s32Ret = IMP_System_Bind(&stVdecCell,&stVoCell);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
			goto END0;
		}
	}

	/************************************************
    step3:  start vo
    *************************************************/
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
		goto END1;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END3;
	}

	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(s32VoLayer, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END3;
	}

	s32Ret = sample_vo_start_chns(s32VoLayer,u32VoMux);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END4;
	}

	/************************************************
    step4:  start vdec
    *************************************************/
	for(i = 0; i < u32VdecMux; i++){
		stVdecDevAttr[i].enType = PT_H264;
		stVdecDevAttr[i].enMode = VIDEO_MODE_FRAME;
		stVdecDevAttr[i].u32PicWidth = STREAM_WIDTH;
		stVdecDevAttr[i].u32PicHeight = STREAM_HEIGHT;
		stVdecDevAttr[i].u32StreamBufSize = STREAM_WIDTH*STREAM_HEIGHT*3/4;
		stVdecDevAttr[i].u32RefFrameNum = 2;

		//将解码设备绑定到外部申请的内存池上
		if (bUserPool) {
			memcpy(&stVdecBufferParam[i].stDevAttr, stVdecDevAttr + i, sizeof(VDEC_DEV_ATTR_S));
			stVdecBufferParam[i].bEnableChn0 = IMP_TRUE;
			stVdecBufferParam[i].stChn0Attr.enOutputFormat = PIX_FMT_NV12;
			stVdecBufferParam[i].stChn0Attr.u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32FrameBufCnt = 3;
			stVdecBufferParam[i].bEnableChn1 = IMP_FALSE;

			sprintf(s8Name, "Dev%d-pool", i);
			s32Ret = IMP_System_CreatPool(u32PoolId + i, IMP_VDEC_GetBufferSize(stVdecBufferParam + i), 1, s8Name);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}

			s32Ret = IMP_VDEC_AttachVbPool(i, u32PoolId[i]);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VDEC_AttachVbPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}
		}

		s32Ret = sample_vdec_start_dev(i, &stVdecDevAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END5;
		}
		stVdecChnAttr[i].enOutputFormat = PIX_FMT_NV12;
		stVdecChnAttr[i].u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
		stVdecChnAttr[i].u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
		stVdecChnAttr[i].bEnableCrop = IMP_FALSE;
		stVdecChnAttr[i].u32FrameBufCnt = 3;
		s32Ret = sample_vdec_start_chn(i,s32VdecChn,&stVdecChnAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END6;
		}
		sample_vdec_start_recv_stream(i);
	}

	/************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(u32VdecMux,&stThreadParam[0],&stVdecDevAttr[0],SAMPLE_H264_STREAM_PATH);
	s32Ret = sample_vdec_start_send_stream(u32VdecMux,&stThreadParam[0], &sendTid[0]);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END7;
	}

	for(i = 0; i < u32VdecMux; i++){
		pthread_join(sendTid[i], IMP_NULL);
	}
	sample_vdec_stop_send_stream(u32VdecMux,&stThreadParam[0], &sendTid[0]);

END7:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_chn(i,s32VdecChn);
	}
END6:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_dev(i);
		if (bUserPool) {
			IMP_VDEC_DetachVbPool(i);
			IMP_System_DestroyPool(u32PoolId[i], NULL);
		}
	}
END5:
	sample_vo_stop_chns(s32VoLayer, u32VoMux);
END4:
	sample_vo_stop_layer(s32VoLayer);
END3:
	sample_vo_hdmi_stop();
END2:
	sample_vo_stop_module(s32VoMod);
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

/*
   SAMPLE_VDEC_H265是基础的H265解码加显示的sample，码流分辨率
   默认640x360；显示的默认分辨率为1080p，16分屏显示。
 */
IMP_S32 SAMPLE_VDEC_H265(IMP_VOID)
{
	IMP_S32 i = 0;
	IMP_S32 s32VdecChn = 0;
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	IMP_S32 s32VoMod;
	IMP_S32 s32VoLayer;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	SAMPLE_VDEC_THREAD_S stThreadParam[MAX_VDEC_DEVS];
	pthread_t sendTid[MAX_VDEC_DEVS];
	VDEC_DEV_ATTR_S stVdecDevAttr[MAX_VDEC_DEVS];
	VDEC_CHN_ATTR_S stVdecChnAttr[MAX_VDEC_DEVS];
	VDEC_BUFFER_PARAM_S stVdecBufferParam[MAX_VDEC_DEVS];
	IMP_U32 u32VoMux = VO_MODE_16MUX;
	IMP_U32 u32VdecMux = VO_MODE_16MUX;
	IMP_U32 u32Square = sqrt(u32VoMux);
	IMP_BOOL bUserPool = IMP_FALSE;
	IMP_U32 u32PoolId[MAX_VDEC_DEVS];
	char s8Name[64];

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
		goto END1;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END3;
	}

	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(s32VoLayer, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END3;
	}

	s32Ret = sample_vo_start_chns(s32VoLayer,u32VoMux);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END4;
	}

	/************************************************
    step4:  start vdec
    *************************************************/
	for(i = 0; i < u32VdecMux; i++){
		stVdecDevAttr[i].enType = PT_H265;
		stVdecDevAttr[i].enMode = VIDEO_MODE_FRAME;
		stVdecDevAttr[i].u32PicWidth = STREAM_WIDTH;
		stVdecDevAttr[i].u32PicHeight = STREAM_HEIGHT;
		stVdecDevAttr[i].u32StreamBufSize = STREAM_WIDTH*STREAM_HEIGHT*3/4;
		stVdecDevAttr[i].u32RefFrameNum = 2;

		//将解码设备绑定到外部申请的内存池上
		if (bUserPool) {
			memcpy(&stVdecBufferParam[i].stDevAttr, stVdecDevAttr + i, sizeof(VDEC_DEV_ATTR_S));
			stVdecBufferParam[i].bEnableChn0 = IMP_TRUE;
			stVdecBufferParam[i].stChn0Attr.enOutputFormat = PIX_FMT_NV12;
			stVdecBufferParam[i].stChn0Attr.u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32FrameBufCnt = 3;
			stVdecBufferParam[i].bEnableChn1 = IMP_FALSE;

			sprintf(s8Name, "Dev%d-pool", i);
			s32Ret = IMP_System_CreatPool(u32PoolId + i, IMP_VDEC_GetBufferSize(stVdecBufferParam + i), 1, s8Name);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}

			s32Ret = IMP_VDEC_AttachVbPool(i, u32PoolId[i]);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VDEC_AttachVbPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}
		}

		s32Ret = sample_vdec_start_dev(i, &stVdecDevAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END5;
		}
		stVdecChnAttr[i].enOutputFormat = PIX_FMT_NV12;
		stVdecChnAttr[i].u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
		stVdecChnAttr[i].u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
		stVdecChnAttr[i].bEnableCrop = IMP_FALSE;
		stVdecChnAttr[i].u32FrameBufCnt = 3;
		s32Ret = sample_vdec_start_chn(i,s32VdecChn,&stVdecChnAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END6;
		}
		sample_vdec_start_recv_stream(i);
	}

	/************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(u32VdecMux,&stThreadParam[0],&stVdecDevAttr[0],SAMPLE_H265_STREAM_PATH);
	s32Ret = sample_vdec_start_send_stream(u32VdecMux,&stThreadParam[0], &sendTid[0]);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END7;
	}

	for(i = 0; i < u32VdecMux; i++){
		pthread_join(sendTid[i], IMP_NULL);
	}
	sample_vdec_stop_send_stream(u32VdecMux,&stThreadParam[0], &sendTid[0]);

END7:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_chn(i,s32VdecChn);
	}
END6:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_dev(i);
		if (bUserPool) {
			IMP_VDEC_DetachVbPool(i);
			IMP_System_DestroyPool(u32PoolId[i], NULL);
		}
	}
END5:
	sample_vo_stop_chns(s32VoLayer, u32VoMux);
END4:
	sample_vo_stop_layer(s32VoLayer);
END3:
	sample_vo_hdmi_stop();
END2:
	sample_vo_stop_module(s32VoMod);
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

/*
   SAMPLE_VDEC_JPEG是基础的JPEG解码加显示的sample，码流分辨率
   默认640x360；显示的默认分辨率为1080p，16分屏显示。
 */
IMP_S32 SAMPLE_VDEC_JPEG(IMP_VOID)
{
	IMP_S32 i = 0;
	IMP_S32 s32VdecChn = 0;
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	IMP_S32 s32VoMod;
	IMP_S32 s32VoLayer;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	SAMPLE_VDEC_THREAD_S stThreadParam[MAX_VDEC_DEVS];
	pthread_t sendTid[MAX_VDEC_DEVS];
	VDEC_DEV_ATTR_S stVdecDevAttr[MAX_VDEC_DEVS];
	VDEC_CHN_ATTR_S stVdecChnAttr[MAX_VDEC_DEVS];
	VDEC_BUFFER_PARAM_S stVdecBufferParam[MAX_VDEC_DEVS];
	IMP_U32 u32VoMux = VO_MODE_16MUX;
	IMP_U32 u32VdecMux = VO_MODE_16MUX;
	IMP_U32 u32Square = sqrt(u32VoMux);
	IMP_BOOL bUserPool = IMP_FALSE;
	IMP_U32 u32PoolId[MAX_VDEC_DEVS];
	char s8Name[64];

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
		goto END1;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END3;
	}

	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(s32VoLayer, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END3;
	}

	s32Ret = sample_vo_start_chns(s32VoLayer,u32VoMux);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END4;
	}

	/************************************************
    step4:  start vdec
    *************************************************/
	for(i = 0; i < u32VdecMux; i++){
		stVdecDevAttr[i].enType = PT_JPEG;
		stVdecDevAttr[i].enMode = VIDEO_MODE_FRAME;
		stVdecDevAttr[i].u32PicWidth = STREAM_WIDTH;
		stVdecDevAttr[i].u32PicHeight = STREAM_HEIGHT;
		stVdecDevAttr[i].u32StreamBufSize = STREAM_WIDTH*STREAM_HEIGHT*3/4;
		stVdecDevAttr[i].u32RefFrameNum = 2;

		//将解码设备绑定到外部申请的内存池上
		if (bUserPool) {
			memcpy(&stVdecBufferParam[i].stDevAttr, stVdecDevAttr + i, sizeof(VDEC_DEV_ATTR_S));
			stVdecBufferParam[i].bEnableChn0 = IMP_TRUE;
			stVdecBufferParam[i].stChn0Attr.enOutputFormat = PIX_FMT_NV12;
			stVdecBufferParam[i].stChn0Attr.u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32FrameBufCnt = 3;
			stVdecBufferParam[i].bEnableChn1 = IMP_FALSE;

			sprintf(s8Name, "Dev%d-pool", i);
			s32Ret = IMP_System_CreatPool(u32PoolId + i, IMP_VDEC_GetBufferSize(stVdecBufferParam + i), 1, s8Name);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}

			s32Ret = IMP_VDEC_AttachVbPool(i, u32PoolId[i]);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VDEC_AttachVbPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}
		}

		s32Ret = sample_vdec_start_dev(i, &stVdecDevAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END5;
		}
		stVdecChnAttr[i].enOutputFormat = PIX_FMT_NV12;
		stVdecChnAttr[i].u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
		stVdecChnAttr[i].u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
		stVdecChnAttr[i].bEnableCrop = IMP_FALSE;
		stVdecChnAttr[i].u32FrameBufCnt = 3;
		s32Ret = sample_vdec_start_chn(i,s32VdecChn,&stVdecChnAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END6;
		}
		sample_vdec_start_recv_stream(i);
	}

	/************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(u32VdecMux,&stThreadParam[0],&stVdecDevAttr[0],SAMPLE_JPEG_STREAM_PATH);
	s32Ret = sample_vdec_start_send_stream(u32VdecMux,&stThreadParam[0], &sendTid[0]);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END7;
	}

	for(i = 0; i < u32VdecMux; i++){
		pthread_join(sendTid[i], IMP_NULL);
	}
	sample_vdec_stop_send_stream(u32VdecMux,&stThreadParam[0], &sendTid[0]);

END7:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_chn(i,s32VdecChn);
	}
END6:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_dev(i);
		if (bUserPool) {
			IMP_VDEC_DetachVbPool(i);
			IMP_System_DestroyPool(u32PoolId[i], NULL);
		}
	}
END5:
	sample_vo_stop_chns(s32VoLayer, u32VoMux);
END4:
	sample_vo_stop_layer(s32VoLayer);
END3:
	sample_vo_hdmi_stop();
END2:
	sample_vo_stop_module(s32VoMod);
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

/*
   SAMPLE_VDEC_H265_GetFrame是H265解码加显示的sample，在显示的过程
   中，会获取当前解码出的NV12图片，存放到当前路径下，默认抓取
   NUM_SAVE_FRAME张图片；码流分辨率默认640x360；显示的默认分辨率为
   1080p，全屏显示。
 */
IMP_S32 SAMPLE_VDEC_H265_GetFrame(IMP_VOID)
{
	IMP_S32 i = 0;
	IMP_S32 s32VdecChn = 0;
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	IMP_S32 s32VoMod;
	IMP_S32 s32VoLayer;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	SAMPLE_VDEC_THREAD_S stSendThreadParam[MAX_VDEC_DEVS];
	SAMPLE_VDEC_THREAD_S stSaveThreadParam[MAX_VDEC_DEVS];
	pthread_t sendTid[MAX_VDEC_DEVS];
	pthread_t saveTid[MAX_VDEC_DEVS];
	VDEC_DEV_ATTR_S stVdecDevAttr[MAX_VDEC_DEVS];
	VDEC_CHN_ATTR_S stVdecChnAttr[MAX_VDEC_DEVS];
	VDEC_BUFFER_PARAM_S stVdecBufferParam[MAX_VDEC_DEVS];
	IMP_U32 u32VoMux = VO_MODE_1MUX;
	IMP_U32 u32VdecMux = VO_MODE_1MUX;
	IMP_U32 u32Square = sqrt(u32VoMux);
	IMP_BOOL bUserPool = IMP_FALSE;
	IMP_U32 u32PoolId[MAX_VDEC_DEVS];
	char s8Name[64];

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
		goto END1;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto END2;
	}

	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	s32Ret = sample_vo_getwh(stVoPubAttr.enIntfSync, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto END3;
	}

	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(s32VoLayer, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto END3;
	}

	s32Ret = sample_vo_start_chns(s32VoLayer,u32VoMux);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto END4;
	}

	/************************************************
    step4:  start vdec
    *************************************************/
	for(i = 0; i < u32VdecMux; i++){
		stVdecDevAttr[i].enType = PT_H265;
		stVdecDevAttr[i].enMode = VIDEO_MODE_FRAME;
		stVdecDevAttr[i].u32PicWidth = STREAM_WIDTH;
		stVdecDevAttr[i].u32PicHeight = STREAM_HEIGHT;
		stVdecDevAttr[i].u32StreamBufSize = STREAM_WIDTH*STREAM_HEIGHT*3/4;
		stVdecDevAttr[i].u32RefFrameNum = 2;

		//将解码设备绑定到外部申请的内存池上
		if (bUserPool) {
			memcpy(&stVdecBufferParam[i].stDevAttr, stVdecDevAttr + i, sizeof(VDEC_DEV_ATTR_S));
			stVdecBufferParam[i].bEnableChn0 = IMP_TRUE;
			stVdecBufferParam[i].stChn0Attr.enOutputFormat = PIX_FMT_NV12;
			stVdecBufferParam[i].stChn0Attr.u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
			stVdecBufferParam[i].stChn0Attr.u32FrameBufCnt = 3;
			stVdecBufferParam[i].bEnableChn1 = IMP_FALSE;

			sprintf(s8Name, "Dev%d-pool", i);
			s32Ret = IMP_System_CreatPool(u32PoolId + i, IMP_VDEC_GetBufferSize(stVdecBufferParam + i), 1, s8Name);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}

			s32Ret = IMP_VDEC_AttachVbPool(i, u32PoolId[i]);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_VDEC_AttachVbPool failed! ret = 0x%08x\n", s32Ret);
				goto END5;
			}
		}

		s32Ret = sample_vdec_start_dev(i, &stVdecDevAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END5;
		}
		stVdecChnAttr[i].enOutputFormat = PIX_FMT_NV12;
		stVdecChnAttr[i].u32OutputWidth = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Width/u32Square, 2);
		stVdecChnAttr[i].u32OutputHeight = ALIGN_BACK(stVoLayerAttr.stImageSize.u32Height/u32Square, 2);
		stVdecChnAttr[i].bEnableCrop = IMP_FALSE;
		stVdecChnAttr[i].u32FrameBufCnt = 3;
		s32Ret = sample_vdec_start_chn(i,s32VdecChn,&stVdecChnAttr[i]);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto END6;
		}

		//缓存buffer深度为3，非拷贝模式
		s32Ret = IMP_VDEC_SetDepth(i,s32VdecChn,3,IMP_FALSE);
		if (s32Ret != IMP_SUCCESS) {
			IMP_LOG_ERR(TAG, "IMP_VDEC_SetDepth failed! ret = 0x%08x\n", s32Ret);
			goto END7;
		}
		sample_vdec_start_recv_stream(i);
	}

	/************************************************
    step5:  send stream to vdec
    *************************************************/
	sample_vdec_thread_param(u32VdecMux,&stSendThreadParam[0],&stVdecDevAttr[0],SAMPLE_H265_STREAM_PATH);
	s32Ret = sample_vdec_start_send_stream(u32VdecMux,&stSendThreadParam[0], &sendTid[0]);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vdec_start_send_stream failed! ret = 0x%08x\n", s32Ret);
		goto END8;
	}

	/************************************************
    step6:  save nv12 picture
    *************************************************/
	for (i = 0; i < u32VdecMux; i++) {
		stSaveThreadParam[i].s32DevId = i;
		stSaveThreadParam[i].s32ChnId = s32VdecChn;
		stSaveThreadParam[i].enCtrlSig = VDEC_CTRL_START;
		sprintf(stSaveThreadParam[i].cFileName, "Dev%d-Chn%d.yuv",i,s32VdecChn);
		if (pthread_create(&saveTid[i],NULL,sample_vdec_get_frame_thread,(void *)&stSaveThreadParam[i]) < 0) {
			IMP_LOG_ERR(TAG, "sample_vdec_get_frame_thread create thread failed!\n");
			goto END9;
		}
	}

	for(i = 0; i < u32VdecMux; i++){
		pthread_join(sendTid[i],IMP_NULL);
	}

	for(i = 0; i < u32VdecMux; i++){
		pthread_join(saveTid[i],IMP_NULL);
	}

END9:
	sample_vdec_stop_send_stream(u32VdecMux,&stSendThreadParam[0],&sendTid[0]);

END8:
	for(i = 0; i < u32VdecMux; i++){
		IMP_VDEC_SetDepth(i,s32VdecChn,0,IMP_FALSE);
	}
END7:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_chn(i,s32VdecChn);
	}
END6:
	for(i = 0; i < u32VdecMux; i++){
		sample_vdec_stop_dev(i);
		if (bUserPool) {
			IMP_VDEC_DetachVbPool(i);
			IMP_System_DestroyPool(u32PoolId[i],NULL);
		}
	}
END5:
	sample_vo_stop_chns(s32VoLayer,u32VoMux);
END4:
	sample_vo_stop_layer(s32VoLayer);
END3:
	sample_vo_hdmi_stop();
END2:
	sample_vo_stop_module(s32VoMod);
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

/*
   SAMPLE_VDEC_SWJpeg是基础的Jpeg软件解码的sample，jpeg图片分辨率
   默认640x360解码后的图片默认存放到当前目录下
 */
IMP_S32 SAMPLE_VDEC_SWJpeg(IMP_VOID)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	FILE *pFileIn = NULL;
	FILE *pFileOut = NULL;
	IMP_U8 *pu8SrcAddr = NULL;
	IMP_U8 *pu8DstAddr = NULL;
	IMP_U32 u32Len = 0;
	JPEGD_INSTANCE_S stJpegInst;

    /************************************************
    step1:  open jpeg file
    *************************************************/
	pFileOut = fopen(SAMPLE_JPEG_OUT_PIC_PATH, "wb+");
	if (pFileOut == NULL) {
		IMP_LOG_ERR(TAG, "jpeg output file open failed!\n");
		s32Ret = -1;
		goto END0;
	}

	pFileIn = fopen(SAMPLE_JPEG_PIC_PATH, "rb");
	if (pFileIn == NULL) {
		IMP_LOG_ERR(TAG, "jpeg file open failed!\n");
		s32Ret = -1;
		goto END1;
	}
	fseek(pFileIn, 0, SEEK_END);
	u32Len = ftell(pFileIn);
	fseek(pFileIn, 0, SEEK_SET);

	pu8SrcAddr = (IMP_U8 *)malloc(u32Len);
	if (pu8SrcAddr == NULL) {
		IMP_LOG_ERR(TAG, "src memory malloc failed!\n");
		s32Ret = -1;
		goto END2;
	}

	//argb1555
	pu8DstAddr = (IMP_U8 *)malloc(STREAM_WIDTH * STREAM_HEIGHT * 2);
	if (pu8DstAddr == NULL) {
		IMP_LOG_ERR(TAG, "dst memory malloc failed!\n");
		s32Ret = -1;
		goto END3;
	}

	s32Ret = fread(pu8SrcAddr, 1, u32Len, pFileIn);
	if (s32Ret != u32Len) {
		IMP_LOG_ERR(TAG, "jpeg file fread failed!\n");
		goto END4;
	}

	/************************************************
    step2:  init system
    *************************************************/
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_system_init failed! ret = 0x%08x\n", s32Ret);
		goto END5;
	}

	/************************************************
    step3:  start sw jpeg decode
    *************************************************/
	stJpegInst.pu8SrcAddr = pu8SrcAddr;
	stJpegInst.u32Len = u32Len;
	stJpegInst.pu8DstAddr = pu8DstAddr;
	s32Ret = IMP_JPEGD_Init(&stJpegInst);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_JPEGD_Init failed! ret = 0x%x\n", s32Ret);
		goto END6;
	}

	s32Ret = IMP_JPEGD_Decode(&stJpegInst);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_JPEGD_Decode failed! ret = 0x%x\n", s32Ret);
		goto END7;
	}

	/************************************************
    step4:  output decoded picture
    *************************************************/
	fwrite(pu8DstAddr, 1, STREAM_WIDTH * STREAM_HEIGHT * 2, pFileOut);

END7:
END6:
	IMP_JPEGD_DeInit(&stJpegInst);
END5:
	sample_system_exit();
END4:
	free(pu8DstAddr);
END3:
	free(pu8SrcAddr);
END2:
	fclose(pFileIn);
END1:
	fclose(pFileOut);
END0:

	return s32Ret;
}

int main(int argc,char *argv[])
{
    IMP_S32 s32Ret = IMP_SUCCESS;
    IMP_CHAR ch;
    IMP_BOOL bExit = IMP_FALSE;

	while(1){
		SAMPLE_VDEC_Usage();
        ch = getchar();
		if (10 == ch){
			continue;
        }
        getchar();
        switch (ch){
            case '0':{
				printf("\nH264 decoding,Please wait...");
    	        SAMPLE_VDEC_H264();
    	        break;
            }
            case '1':{
				printf("\nH265 decoding,Please wait...");
    	        SAMPLE_VDEC_H265();
    	        break;
            }
            case '2':{
				printf("\nJPEG decoding,Please wait...");
    	        SAMPLE_VDEC_JPEG();
    	        break;
            }
            case '3':{
				printf("\nH265 getting frames,Please wait...");
    	        SAMPLE_VDEC_H265_GetFrame();
    	        break;
            }
            case '4':{
				printf("\nSW Jpeg decoding,Please wait...");
				SAMPLE_VDEC_SWJpeg();
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
