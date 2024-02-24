/*
 * IMP VideoOutput header file.
 *
 * Copyright (C) 2021 Ingenic Semiconductor Co.,Ltd
 */
#ifndef __IMP_VIDEOOUTPUT_H__
#define __IMP_VIDEOOUTPUT_H__

#include <imp/imp_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP视频输出模块头文件
 */

/**
 * @defgroup IMP_VO
 * @ingroup imp
 * @brief 视频输出模块，包括视频输出，高清视频层，视频输出通道，视频层算法
 * @section vo_summary 1 概述
 * 视频输出模块主要完成从相应内存位置读取视频和图形数据，然后通过相应的显示接口输出。
 * 1.支持两个高清视频层(L0和L1),L0支持36个通道，L1支持2个通道
 * 2.支持一个图形层(G0)
 * 3.支持一个鼠标层(M0)
 * 4.最大输出时序3840x2160@60Hz
 * 5.输出接口支持HDMI，VGA和TFT屏幕
 * @section vo_video_layer 2 视频层简介
 * 1.支持格式: NV12/21和 NV16/61。
 * 2.支持分辨率范围: 32x32到3840x2160，宽高必须是2的倍数。
 * 3.支持的每个通道，最小分辨率是32x32。
 * 4.视频层L0支持对整个层做BCSH算法处理。
 * 5.视频层L0支持对最多9个窗口的PEAK处理。
 * 6.视频层L0支持对最多9个窗口的DLI处理。
 * 7.视频层L1的优先级高于视频层L0，显示效果为L1叠加在L0上
 * @section vo_graphics_layer 3 图形层简介
 * 1.支持格式: ARGB8888、ARGB1555和压缩图像。
 * 2.支持分辨率范围: 32x32到3840x2160，宽高必须是2的倍数。
 * @section vo_mouse_layer 4 鼠标层简介
 * 1.支持格式: ARGB8888、ARGB1555。
 * 2.支持分辨率范围: 16x16到32x32，宽高必须是2的倍数。
 * @section vo_interface 5 输出接口简介
 * 1.VGA最大支持2k@60Hz视频输出。
 * 2.HDMI最大支持4K@60Hz视频输出。
 * 3.TFT最大支持2K@60Hz视频输出。
 * 4.支持VGA和HDMI同时同源输出。
 * 5.TFT和VGA不可以同时同源输出。
 * @section vo_layer_priority 6 层优先级
 * L0 < L1 < G0 < M0
 * @{
 */

/**
 * 视频输出接口类型
 */
#define VO_INTF_VGA		(0x01L<<0)
#define VO_INTF_HDMI	(0x01L<<1)
#define VO_INTF_TFT		(0x01L<<2)

typedef IMP_S32 VO_INTF_TYPE_E;

/**
 * 视频输出分辨率类型
 */
typedef enum {
	VO_OUTPUT_PAL = 0,
	VO_OUTPUT_NTSC,

	VO_OUTPUT_1080P24,
	VO_OUTPUT_1080P25,
	VO_OUTPUT_1080P30,

	VO_OUTPUT_720P50,
	VO_OUTPUT_720P60,
	VO_OUTPUT_1080P50,
	VO_OUTPUT_1080P60,

	VO_OUTPUT_576P50,
	VO_OUTPUT_480P60,

	VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
	VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1440x900_60_RB,	     /* VESA 1440 x 900 at 60 Hz (non-interlaced) Reduced Blanking */
	VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
	VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
	VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
	VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/
	VO_OUTPUT_2560x1440_30,          /* 2560x1440_30 */
	VO_OUTPUT_2560x1440_60,          /* 2560x1440_60 */
	VO_OUTPUT_2560x1600_60,          /* 2560x1600_60 */
	VO_OUTPUT_3840x2160_24,
	VO_OUTPUT_3840x2160_25,          /* 3840x2160_25 */
	VO_OUTPUT_3840x2160_30,          /* 3840x2160_30 */
	VO_OUTPUT_3840x2160_50,          /* 3840x2160_50 */
	VO_OUTPUT_3840x2160_60,          /* 3840x2160_60 */
	VO_OUTPUT_USER,
	VO_OUTPUT_BUTT
}VO_INTF_SYNC_E;

/**
 * 视频输出时序结构体定义
 */
typedef struct{
	IMP_BOOL bSyncMode;
	IMP_BOOL bIop;
	IMP_U8	 u8Intfb;
	IMP_U16   u16Freq;
	IMP_U16   u16Vact;  /* vertical active area */
	IMP_U16   u16Vbp;    /* vertical back blank porch */
	IMP_U16   u16Vfp;    /* vertical front blank porch */

	IMP_U16   u16Hact;   /* herizontal active area */
	IMP_U16   u16Hbp;    /* herizontal back blank porch */
	IMP_U16   u16Hfp;    /* herizontal front blank porch */

	IMP_U16   u16Hpw;    /* horizontal pulse width */
	IMP_U16   u16Vpw;    /* vertical pulse width */

	IMP_BOOL  bIhs;      /* inverse horizontal synch signal */
	IMP_BOOL  bIvs;      /* inverse vertical syncv signal */
}VO_SYNC_INFO_S;

/**
 * 视频输出色彩空间矩阵类型枚举
 */
typedef enum{
    VO_CSC_MATRIX_BT601_FULL,
    VO_CSC_MATRIX_BT601_LIMIT,
    VO_CSC_MATRIX_BT709_FULL,
    VO_CSC_MATRIX_BT709_LIMIT,
    VO_CSC_MATRIX_BT2020_FULL,
    VO_CSC_MATRIX_BT2020_LIMIT,
    VO_CSC_MATRIX_INVALID,
}VO_CSC_E;

/**
 * 视频输出模块公共属性结构体定义
 */
typedef struct{
   	IMP_U32			u32BgColor;
	IMP_SIZE_S		stCanvasSize;
    VO_INTF_TYPE_E	enIntfType;
    VO_INTF_SYNC_E	enIntfSync;
    VO_SYNC_INFO_S	stSyncInfo;
}VO_PUB_ATTR_S;

/**
 * 视频输出接口状态枚举
 */
typedef enum{
	VO_DEV_STATUS_NOPLUG = 0,
	VO_DEV_STATUS_PLUG,
	VO_DEV_STATUS_BUTT,
}VO_DEVINTF_STATUS_E;

/**
 * 视频输出接口状态结构体定义
 */
typedef struct{
    VO_DEVINTF_STATUS_E enPlugStatus;
}VO_DEVINTF_STATUS_S;

/**
 * @fn IMP_S32 IMP_VO_SetPubAttr(IMP_S32 s32VoMod,VO_PUB_ATTR_S *pstPubAttr)
 *
 * 设置视频输出模块公共属性.
 *
 * @param[in] s32VoMod 视频输出模块号.
 * @param[in] pstPubAttr 视频输出模块公共属性指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 视频输出模块使能之前，必须设置视频输出模块公共属性.
 */
IMP_S32 IMP_VO_SetPubAttr(IMP_S32 s32VoMod,VO_PUB_ATTR_S *pstPubAttr);

/**
 * @fn IMP_S32 IMP_VO_GetPubAttr(IMP_S32 s32VoMod,VO_PUB_ATTR_S *pstPubAttr)
 *
 * 获取视频输出模块公共属性.
 *
 * @param[in] s32VoMod 视频输出模块号.
 * @param[in] pstPubAttr 视频输出模块公共属性指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetPubAttr(IMP_S32 s32VoMod,VO_PUB_ATTR_S *pstPubAttr);

/**
 * @fn IMP_S32 IMP_VO_Enable(IMP_S32 s32VoMod)
 *
 * 启用视频输出模块.
 *
 * @param[in] s32VoMod 视频输出模块号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 视频输出模块使能之前，必须设置视频输出模块公共属性.
 */
IMP_S32 IMP_VO_Enable(IMP_S32 s32VoMod);

/**
 * @fn IMP_S32 IMP_VO_Disable(IMP_S32 s32VoMod)
 *
 * 禁用视频输出模块.
 *
 * @param[in] s32VoMod 视频输出模块号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 重新设置分辨率需要先禁用视频输出设备.
 *
 * @attention 禁用视频输出模块之前需要禁用视频层.
 */
IMP_S32 IMP_VO_Disable(IMP_S32 s32VoMod);

/**
 * @fn IMP_S32 IMP_VO_QueryDevInfStatus(VO_INTF_TYPE_E enDevIntf,VO_DEVINTF_STATUS_S *pstIntfStatus)
 *
 * 获取视频输出模块显示接口HDMI/VGA状态.
 *
 * @param[in] enDevIntf 设备接口.
 * @param[in] pstIntfStatus 接口状态.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_QueryDevInfStatus(VO_INTF_TYPE_E enDevIntf,VO_DEVINTF_STATUS_S *pstIntfStatus);

/**
 * @fn IMP_S32 IMP_VO_SetCSC(IMP_S32 s32VoMod,VO_CSC_E enCscMatrix)
 *
 * 设置输出模块color space模式.
 *
 * @param[in] s32VoMod 视频输出模块号.
 * @param[in] enCscMatrix color space模式.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_SetCSC(IMP_S32 s32VoMod,VO_CSC_E	enCscMatrix);

/**
 * @fn IMP_S32 IMP_VO_GetCSC(IMP_S32 s32VoMod,VO_CSC_E enCscMatrix)
 *
 * 获取输出模块color space模式.
 *
 * @param[in] s32VoMod 视频输出模块号.
 * @param[in] penCscMatrix color space模式.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetCSC(IMP_S32 s32VoMod,VO_CSC_E	*penCscMatrix);

/**
 * 视频层属性结构体定义
 */
typedef struct{
	IMP_U32			u32DispFrmRt;
	IMP_RECT_S		stDispRect;
	IMP_SIZE_S		stImageSize;
	PIXEL_FORMAT_E	enPixFmt;
}VO_VIDEO_LAYER_ATTR_S;

/**
 * @fn IMP_S32 IMP_VO_SetVideoLayerAttr(IMP_S32 s32VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
 *
 * 设置视频层属性.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] pstLayerAttr 视频层属性结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 设置视频层属性时不介意是否建立绑定关系.
 *
 * @attention 设置视频层属性必须在视频层禁止的情况下进行.
 */
IMP_S32 IMP_VO_SetVideoLayerAttr(IMP_S32 s32VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

/**
 * @fn IMP_S32 IMP_VO_GetVideoLayerAttr(IMP_S32 s32VoLayer,VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
 *
 * 获取视频层属性.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] pstLayerAttr 视频层属性结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetVideoLayerAttr(IMP_S32 s32VoLayer,VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

/**
 * @fn IMP_S32 IMP_VO_EnableVideoLayer(IMP_S32 s32VoLayer)
 *
 * 启用视频层.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 视频层使能前必须保证该视频层已经配置.
 */
IMP_S32 IMP_VO_EnableVideoLayer(IMP_S32 s32VoLayer);

/**
 * @fn IMP_S32 IMP_VO_DisableVideoLayer(IMP_S32 s32VoLayer)
 *
 * 禁用视频层.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 视频层禁止前必须保证其上的视频输出通道全部禁止.
 */
IMP_S32 IMP_VO_DisableVideoLayer(IMP_S32 s32VoLayer);

/**
 * 视频通道属性结构体定义
 */
typedef struct{
    IMP_U32		u32Priority;
    IMP_BOOL	bDeflicker;
    IMP_RECT_S	stRect;
}VO_CHN_ATTR_S;

/**
 * @fn IMP_S32 IMP_VO_SetChnAttr(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,const VO_CHN_ATTR_S *pstChnAttr)
 *
 * 设置指定视频通道属性.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] pstChnAttr 视频输出通道属性结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 通道的显示区域不能超过视频层中设定的画布大小.
 */
IMP_S32 IMP_VO_SetChnAttr(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,const VO_CHN_ATTR_S *pstChnAttr);

/**
 * @fn IMP_S32 IMP_VO_GetChnAttr(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,VO_CHN_ATTR_S *pstChnAttr)
 *
 * 设置指定视频通道属性.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] pstChnAttr 视频输出通道属性结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetChnAttr(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,VO_CHN_ATTR_S *pstChnAttr);

/**
 * @fn IMP_S32 IMP_VO_EnableChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
 *
 * 启用指定视频输出通道.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 启用视频输出通道前，需要对通道属性进行配置.
 */
IMP_S32 IMP_VO_EnableChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn);

/**
 * @fn IMP_S32 IMP_VO_DisableChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
 *
 * 禁用指定视频输出通道.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention .
 */
IMP_S32 IMP_VO_DisableChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn);

/**
 * @fn IMP_S32 IMP_VO_SetChnDispPos(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,const IMP_POINT_S *pstPoint)
 *
 * 设置指定视频通道位置.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] pstPoint 通道显示位置指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 调用此接口前先使能视频层和视频输出通道.视频通道显示区域不能超出视频层范围.
 */
IMP_S32 IMP_VO_SetChnDispPos(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,const IMP_POINT_S *pstPoint);

/**
 * @fn IMP_S32 IMP_VO_GetChnDispPos(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,IMP_POINT_S *pstPoint)
 *
 * 获取指定视频通道位置.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] pstPoint 通道显示位置指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetChnDispPos(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,IMP_POINT_S *pstPoint);

/**
 * @fn IMP_S32 IMP_VO_PauseChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
 *
 * 暂停指定视频通道显示.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_PauseChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn);

/**
 * @fn IMP_S32 IMP_VO_ResumeChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
 *
 * 恢复指定视频通道显示.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 此接口用于恢复正在暂停的通道或者正在单帧播放的通道显示.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_ResumeChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn);

/**
 * @fn IMP_S32 IMP_VO_StepChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
 *
 * 单帧播放指定的视频通道.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 使用此接口前需要先暂停视频通道显示.
 */
IMP_S32 IMP_VO_StepChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn);

/**
 * @fn IMP_S32 IMP_VO_ShowChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
 *
 * 设置显示视频通道.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 默认情况下视频通道处于显示状态.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_ShowChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn);

/**
 * @fn IMP_S32 IMP_VO_HideChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn)
 *
 * 设置隐藏视频通道.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_HideChn(IMP_S32 s32VoLayer,IMP_S32 s32VoChn);

/**
 * @fn IMP_S32 IMP_VO_HideChnAll(IMP_S32 s32VoLayer)
 *
 * 设置隐藏所有视频通道.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_HideChnAll(IMP_S32 s32VoLayer);

/**
 * 视频输出局部放大结构体定义
 */
typedef struct{
	IMP_RECT_S stZoomRect;
}VO_ZOOM_ATTR_S;

/**
 * @fn IMP_S32 IMP_VO_SetZoomInWindow(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,VO_ZOOM_ATTR_S *stZoomAttr)
 *
 * 设置视频输出局部放大窗口.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] stZoomAttr 局部放大属性结构体.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 调用前确保使能视频层和通道.
 *
 * @attention 只有高清视频层0具备此功能.只能在单通道全屏显示的情境下使用此接口.
 */
IMP_S32 IMP_VO_SetZoomInWindow(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,VO_ZOOM_ATTR_S *stZoomAttr);

/**
 * @fn IMP_S32 IMP_VO_GetZoomInWindow(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,VO_ZOOM_ATTR_S *stZoomAttr)
 *
 * 获取视频输出局部放大窗口参数.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] stZoomAttr 局部放大属性结构体.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetZoomInWindow(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,VO_ZOOM_ATTR_S *stZoomAttr);

/**
 * @fn IMP_S32 IMP_VO_ClearChnBuffer(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,IMP_BOOL bClrAll)
 *
 * 清空指定输出通道的缓存帧数据.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] bClrAll 是否将通道buffer中的数据全部清空.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_ClearChnBuffer(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,IMP_BOOL bClrAll);

/**
 * @fn IMP_VO_GetChnFrame(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,FRAME_INFO_S **pstFrame)
 *
 * 获取通道帧
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[out] pstFrame 视频数据信息指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetChnFrame(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,FRAME_INFO_S **pstFrame);

/**
 * @fn IMP_S32 IMP_VO_ReleaseChnFrame(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,FRAME_INFO_S *pstFrame)
 *
 * 释放输出通道图像数据.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] pstFrame 释放的输出通道图像数据信息结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_ReleaseChnFrame(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,FRAME_INFO_S *pstFrame);

/**
 * @fn IMP_VO_GetChnPts(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,IMP_U64 *u64Pts)
 *
 * 获取当前通道显示帧的时间戳.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] s32VoChn 视频通道号.
 * @param[in] u64Pts 获取当前通道显示帧的时间戳指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetChnPts(IMP_S32 s32VoLayer,IMP_S32 s32VoChn,IMP_U64 *u64Pts);

/**
 * 视频层亮度对比度饱和度色度算法结构体定义
 */
typedef struct{
    IMP_U8		u8Brightness;
    IMP_U8		u8Contrast;
    IMP_U8		u8Saturation;
    IMP_U8		u8Hue;
}VO_BCSH_S;

/**
 * @fn IMP_S32 IMP_VO_EnableBcsh(IMP_S32 s32VoLayer)
 *
 * 启用视频层亮度对比度饱和度色度算法.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 只有高清视频层0具有此功能.
 *
 * @attention 使用前确保开启视频输出模块和视频层.开启BCSH不设置算法参数，默认按照原值输出。
 */
IMP_S32 IMP_VO_EnableBcsh(IMP_S32 s32VoLayer);

/**
 * @fn IMP_S32 IMP_VO_EnableBcsh(IMP_S32 s32VoLayer)
 *
 * 禁用视频层亮度对比度饱和度色度算法.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_DisableBcsh(IMP_S32 s32VoLayer);

/**
 * @fn IMP_S32 IMP_VO_SetVideoLayerBcsh(IMP_S32 s32VoLayer,const VO_BCSH_S *pstBcsh)
 *
 * 设置视频层亮度对比度饱和度色度.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] pstBcsh 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 调用此接口前需要开启BCSH.
 */
IMP_S32 IMP_VO_SetVideoLayerBcsh(IMP_S32 s32VoLayer,const VO_BCSH_S *pstBcsh);

/**
 * @fn IMP_S32 IMP_VO_GetVideoLayerBcsh(IMP_S32 s32VoLayer,VO_BCSH_S *pstBcsh)
 *
 * 设置视频层亮度对比度饱和度色度.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_GetVideoLayerBcsh(IMP_S32 s32VoLayer,VO_BCSH_S *pstBcsh);

/**
 * @fn IMP_S32 IMP_VO_EnablePeak(IMP_S32 s32VoLayer)
 *
 * 设置视频层锐化算法.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 只有高清视频层0具有此功能.
 *
 * @attention 使用前确保开启视频输出模块和视频层.开启PEAK不设置算法参数，默认按照原值输出.
 */
IMP_S32 IMP_VO_EnablePeak(IMP_S32 s32VoLayer);

/**
 * @fn IMP_S32 IMP_VO_DisablePeak(IMP_S32 s32VoLayer)
 *
 * 设置视频层亮度对比度饱和度色度.
 *
 * @param[in] s32VoLayer 视频层号.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_DisablePeak(IMP_S32 s32VoLayer);

/**
 * @fn IMP_S32 IMP_VO_SetVideoLayerPeak(IMP_S32 s32VoLayer,const IMP_U8 u8PeakStrength)
 *
 * 设置视频层所有锐化通道强度.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] u8PeakStrength 所有锐化通道的锐化强度.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 使用此接口前需要开启PEAK视频层PEAK算法.
 */
IMP_S32 IMP_VO_SetVideoLayerPeak(IMP_S32 s32VoLayer,const IMP_U8 u8PeakStrength);

/**
 * @fn IMP_S32 IMP_VO_GetVideoLayerPeak(IMP_S32 s32VoLayer,IMP_U8 *pu8PeakStrength)
 *
 * 获取视频层所有锐化通道强度.
 *
 * @param[in] s32VoLayer 视频层号.
 * @param[in] pu8PeakStrength 所有锐化通道的锐化强度指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 使用此接口前需要开启PEAK视频层PEAK算法.
 */
IMP_S32 IMP_VO_GetVideoLayerPeak(IMP_S32 s32VoLayer,IMP_U8 *pu8PeakStrength);

/**
 * @fn IMP_S32 IMP_VO_SetAttrBegin(IMP_VOID);
 *
 * 设置视频层上通道的属性开始.
 *
 * @param 无.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_SetAttrBegin(IMP_VOID);

/**
 * @fn IMP_S32 IMP_VO_SetAttrEnd(IMP_VOID)
 *
 * 设置视频层上通道的属性结束.
 *
 * @param 无.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_VO_SetAttrEnd(IMP_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
