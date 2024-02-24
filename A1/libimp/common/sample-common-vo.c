#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <imp/imp_type.h>
#include <imp/imp_log.h>
#include <imp/imp_vo.h>
#include "sample-common.h"

#define TAG "Sample-Common-Vo"

static IMP_HDMI_CALLBACK_FUNC_S g_stCallbackFunc;

IMP_S32 sample_vo_getwh(VO_INTF_SYNC_E enIntfSync, IMP_U32 *pu32W,IMP_U32 *pu32H, IMP_U32 *pu32Frm)
{
	switch (enIntfSync){
		case VO_OUTPUT_PAL       :  *pu32W = 720;  *pu32H = 576;  *pu32Frm = 25; break;
		case VO_OUTPUT_NTSC      :  *pu32W = 720;  *pu32H = 480;  *pu32Frm = 30; break;
		case VO_OUTPUT_576P50    :  *pu32W = 720;  *pu32H = 576;  *pu32Frm = 50; break;
		case VO_OUTPUT_480P60    :  *pu32W = 720;  *pu32H = 480;  *pu32Frm = 60; break;
		case VO_OUTPUT_640x480_60:  *pu32W = 640;  *pu32H = 480;  *pu32Frm = 60; break;
		case VO_OUTPUT_800x600_60:  *pu32W = 800;  *pu32H = 600;  *pu32Frm = 60; break;
		case VO_OUTPUT_720P50    :  *pu32W = 1280; *pu32H = 720;  *pu32Frm = 50; break;
		case VO_OUTPUT_720P60    :  *pu32W = 1280; *pu32H = 720;  *pu32Frm = 60; break;
		case VO_OUTPUT_1080P24   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 24; break;
		case VO_OUTPUT_1080P25   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 25; break;
		case VO_OUTPUT_1080P30   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 30; break;
		case VO_OUTPUT_1080P50   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 50; break;
		case VO_OUTPUT_1080P60   :  *pu32W = 1920; *pu32H = 1080; *pu32Frm = 60; break;
		case VO_OUTPUT_1024x768_60:  *pu32W = 1024; *pu32H = 768;  *pu32Frm = 60; break;
		case VO_OUTPUT_1280x1024_60: *pu32W = 1280; *pu32H = 1024; *pu32Frm = 60; break;
		case VO_OUTPUT_1366x768_60:  *pu32W = 1366; *pu32H = 768;  *pu32Frm = 60; break;
		case VO_OUTPUT_1440x900_60_RB:
		case VO_OUTPUT_1440x900_60:  *pu32W = 1440; *pu32H = 900;  *pu32Frm = 60; break;
		case VO_OUTPUT_1280x800_60:  *pu32W = 1280; *pu32H = 800;  *pu32Frm = 60; break;
		case VO_OUTPUT_1600x1200_60: *pu32W = 1600; *pu32H = 1200; *pu32Frm = 60; break;
		case VO_OUTPUT_1680x1050_60: *pu32W = 1680; *pu32H = 1050; *pu32Frm = 60; break;
		case VO_OUTPUT_1920x1200_60: *pu32W = 1920; *pu32H = 1200; *pu32Frm = 60; break;
		case VO_OUTPUT_2560x1440_30: *pu32W = 2560; *pu32H = 1440; *pu32Frm = 30; break;
		case VO_OUTPUT_2560x1440_60: *pu32W = 2560; *pu32H = 1440; *pu32Frm = 60; break;
		case VO_OUTPUT_2560x1600_60: *pu32W = 2560; *pu32H = 1600; *pu32Frm = 60; break;
		case VO_OUTPUT_3840x2160_24: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 24; break;
		case VO_OUTPUT_3840x2160_25: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 25; break;
		case VO_OUTPUT_3840x2160_30: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 30; break;
		case VO_OUTPUT_3840x2160_50: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 50; break;
		case VO_OUTPUT_3840x2160_60: *pu32W = 3840; *pu32H = 2160; *pu32Frm = 60; break;
		case VO_OUTPUT_USER    :     *pu32W = 720;  *pu32H = 576;  *pu32Frm = 25; break;
		default:
			IMP_LOG_ERR(TAG,"vo enIntfSync not support!\n");
			return IMP_FAILURE;
	}
	return IMP_SUCCESS;
}

IMP_S32 sample_vo_start_module(IMP_S32 s32VoMod,VO_PUB_ATTR_S *pstPubAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VO_SetPubAttr(s32VoMod, pstPubAttr);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_SetPubAttr:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_VO_Enable(s32VoMod);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_Enable:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 sample_vo_stop_module(IMP_S32 s32VoMod)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VO_Disable(s32VoMod);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_Disable:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 sample_vo_start_layer(IMP_S32 s32VoLayer,VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VO_SetVideoLayerAttr(s32VoLayer, pstLayerAttr);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_SetVideoLayerAttr:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_VO_EnableVideoLayer(s32VoLayer);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_EnableVideoLayer:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 sample_vo_stop_layer(IMP_S32 s32VoLayer)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VO_DisableVideoLayer(s32VoLayer);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_DisableVideoLayer:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 sample_vo_start_chns(IMP_S32 s32VoLayer,SAMPLE_VO_MODE_E enMode)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 i;
	IMP_U32 u32WndNum = 0;
	IMP_U32 u32Square = 0;
	IMP_U32 u32Width = 0;
	IMP_U32 u32Height = 0;
	VO_VIDEO_LAYER_ATTR_S stLayerAttr;
	VO_CHN_ATTR_S stChnAttr;

	switch (enMode){
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
		IMP_LOG_ERR(TAG,"IMP_VO_GetVideoLayerAttr:failed with %#x!\n",s32Ret);
		return IMP_FAILURE;
	}
	u32Width = stLayerAttr.stImageSize.u32Width;
	u32Height = stLayerAttr.stImageSize.u32Height;

	for (i = 0; i < u32WndNum; i++){
		stChnAttr.stRect.s32X       = ALIGN_BACK(ALIGN_BACK(u32Width/u32Square,2) * (i%u32Square), 2);
		stChnAttr.stRect.s32Y       = ALIGN_BACK((u32Height/u32Square) * (i/u32Square), 2);
		stChnAttr.stRect.u32Width   = ALIGN_BACK(u32Width/u32Square, 2);
		stChnAttr.stRect.u32Height  = ALIGN_BACK(u32Height/u32Square, 2);
		stChnAttr.u32Priority       = 0;
		stChnAttr.bDeflicker        = IMP_FALSE;

		s32Ret = IMP_VO_SetChnAttr(s32VoLayer, i, &stChnAttr);
		if (s32Ret != IMP_SUCCESS){
			IMP_LOG_ERR(TAG,"IMP_VO_SetChnAttr:failed with %#x!\n",s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_VO_EnableChn(s32VoLayer, i);
		if (s32Ret != IMP_SUCCESS){
			IMP_LOG_ERR(TAG,"IMP_VO_EnableChn:failed with %#x!\n",s32Ret);
			return s32Ret;
		}
	}
	return IMP_SUCCESS;
}

IMP_S32 sample_vo_stop_chns(IMP_S32 s32VoLayer, SAMPLE_VO_MODE_E enMode)
{
	IMP_S32 i;
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_U32 u32WndNum = 0;

	switch (enMode){
		case VO_MODE_1MUX:
			u32WndNum = 1;
			break;
		case VO_MODE_4MUX:
			u32WndNum = 4;
			break;
		case VO_MODE_9MUX:
			u32WndNum = 9;
			break;
		case VO_MODE_16MUX:
			u32WndNum = 16;
			break;
		case VO_MODE_25MUX:
			u32WndNum = 25;
			break;
		case VO_MODE_36MUX:
			u32WndNum = 36;
			break;
		default:
			IMP_LOG_ERR(TAG,"vo display mode not support!\n");
			return IMP_FAILURE;
	}
	for (i = 0; i < u32WndNum; i++){
		s32Ret = IMP_VO_DisableChn(s32VoLayer, i);
		if (s32Ret != IMP_SUCCESS){
			IMP_LOG_ERR(TAG,"IMP_VO_DisableChn:failed with %#x!\n",s32Ret);
			return s32Ret;
		}
	}
	return IMP_SUCCESS;
}

IMP_S32 sample_vo_start_chn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,VO_CHN_ATTR_S *pstChnAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VO_SetChnAttr(s32VoLayer, s32VoChn, pstChnAttr);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_SetChnAttr:failed with %#x!\n",s32Ret);
		return s32Ret;
	}
	s32Ret = IMP_VO_EnableChn(s32VoLayer, s32VoChn);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_EnableChn:failed with %#x!\n",s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

IMP_S32 sample_vo_stop_chn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_VO_DisableChn(s32VoLayer, s32VoChn);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_VO_DisableChn:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

static IMP_VOID sample_vo_hdmi_convert_sync(VO_INTF_SYNC_E enIntfSync,IMP_HDMI_VIDEO_FMT_E *penVideoFmt)
{
	switch (enIntfSync)
	{
		case VO_OUTPUT_PAL:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_PAL;
			break;
		case VO_OUTPUT_NTSC:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_NTSC;
			break;
		case VO_OUTPUT_1080P24:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_1080P_24;
			break;
		case VO_OUTPUT_1080P25:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_1080P_25;
			break;
		case VO_OUTPUT_1080P30:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_1080P_30;
			break;
		case VO_OUTPUT_720P50:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_720P_50;
			break;
		case VO_OUTPUT_720P60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_720P_60;
			break;
		case VO_OUTPUT_1080P50:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_1080P_50;
			break;
		case VO_OUTPUT_1080P60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_1080P_60;
			break;
		case VO_OUTPUT_576P50:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_576P_50;
			break;
		case VO_OUTPUT_480P60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_480P_60;
			break;
		case VO_OUTPUT_640x480_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_861D_640X480_60;
			break;
		case VO_OUTPUT_800x600_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_800X600_60;
			break;
		case VO_OUTPUT_1024x768_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1024X768_60;
			break;
		case VO_OUTPUT_1280x1024_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1280X1024_60;
			break;
		case VO_OUTPUT_1366x768_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1366X768_60;
			break;
		case VO_OUTPUT_1440x900_60_RB:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1440X900_60_RB;
			break;
		case VO_OUTPUT_1440x900_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1440X900_60;
			break;
		case VO_OUTPUT_1280x800_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1280X800_60;
			break;
		case VO_OUTPUT_1680x1050_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1680X1050_60;
			break;
		case VO_OUTPUT_1600x1200_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1600X1200_60;
			break;
		case VO_OUTPUT_1920x1200_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_VESA_1920X1200_60;
			break;
		case VO_OUTPUT_2560x1440_30:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_2560x1440_30;
			break;
		case VO_OUTPUT_2560x1440_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_2560x1440_60;
			break;
		case VO_OUTPUT_2560x1600_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_2560x1600_60;
			break;
		case VO_OUTPUT_3840x2160_24:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_3840X2160P_24;
			break;
		case VO_OUTPUT_3840x2160_25:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_3840X2160P_25;
			break;
		case VO_OUTPUT_3840x2160_30:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_3840X2160P_30;
			break;
		case VO_OUTPUT_3840x2160_50:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_3840X2160P_50;
			break;
		case VO_OUTPUT_3840x2160_60:
			*penVideoFmt = IMP_HDMI_VIDEO_FMT_3840X2160P_60;
			break;
		default :
			IMP_LOG_ERR(TAG,"Unkonw VO_INTF_SYNC_E value!\n");
			break;
	}
	return;
}

IMP_VOID HDMI_EventCallback(IMP_HDMI_EVENT_TYPE_E event, IMP_VOID *pPrivateData)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_HDMI_ATTR_S stHdmiAttr;

	switch(event){
		case IMP_HDMI_EVENT_HOTPLUG:
			s32Ret = IMP_HDMI_GetAttr(IMP_HDMI_ID_0,&stHdmiAttr);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_HDMI_GetAttr:failed with = %#x\n", s32Ret);
			}

			s32Ret = IMP_HDMI_SetAttr(IMP_HDMI_ID_0,&stHdmiAttr);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_HDMI_SetAttr:failed ret = %#x\n", s32Ret);
			}

			s32Ret = IMP_HDMI_Start(IMP_HDMI_ID_0);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_HDMI_Start:failed ret = %d\n", s32Ret);
			}
			IMP_LOG_INFO(TAG,"hdmi interface is connect!\n");
			break;
		case IMP_HDMI_EVENT_NO_PLUG:
			s32Ret = IMP_HDMI_Stop(IMP_HDMI_ID_0);
			if (s32Ret != IMP_SUCCESS) {
				IMP_LOG_ERR(TAG, "IMP_HDMI_Stop:failed ret = %d\n", s32Ret);
			}
			IMP_LOG_INFO(TAG,"hdmi interface is disconnect!\n");
			break;
		default:
			break;
	}
}

IMP_S32 sample_vo_hdmi_start(VO_INTF_SYNC_E	enIntfSync)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_HDMI_ATTR_S stHdmiAttr;

	s32Ret = IMP_HDMI_Init();
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_HDMI_Init:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	g_stCallbackFunc.pfnHdmiEventCallback = HDMI_EventCallback;
	g_stCallbackFunc.pPrivateData = IMP_HDMI_ID_0;
	s32Ret = IMP_HDMI_RegCallbackFunc(IMP_HDMI_ID_0,&g_stCallbackFunc);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_HDMI_RegCallbackFunc:failed with %#x!\n",s32Ret);
		return s32Ret;
	}
	s32Ret = IMP_HDMI_GetAttr(IMP_HDMI_ID_0,&stHdmiAttr);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_HDMI_GetAttr:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	sample_vo_hdmi_convert_sync(enIntfSync,&stHdmiAttr.enVideoFmt);
	stHdmiAttr.enVideoMode = IMP_HDMI_VIDEO_MODE_RGB444;
	stHdmiAttr.enDeepColor = IMP_HDMI_DEEP_COLOR_24BIT;
	stHdmiAttr.bEnableHdmi = IMP_TRUE;
	s32Ret = IMP_HDMI_SetAttr(IMP_HDMI_ID_0,&stHdmiAttr);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_HDMI_SetAttr:failed with %#x!\n",s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_HDMI_Start(IMP_HDMI_ID_0);
	if (s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG,"IMP_HDMI_Start:failed with %#x!\n",s32Ret);
		return s32Ret;
	}
	return IMP_SUCCESS;
}

IMP_VOID sample_vo_hdmi_stop(IMP_VOID)
{
	IMP_HDMI_Stop(IMP_HDMI_ID_0);
	IMP_HDMI_DeInit();
	IMP_HDMI_UnRegCallbackFunc(IMP_HDMI_ID_0,&g_stCallbackFunc);
}
