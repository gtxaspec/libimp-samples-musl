/*
 * IMP VDEC func header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_VDEC_H__
#define __IMP_VDEC_H__

#include <stdint.h>
#include "imp/imp_common.h"
#include "imp/imp_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP视频解码头文件
 */

/**
 * 定义码流发送方式枚举
 */
typedef enum VIDEO_MODE_E {
	VIDEO_MODE_STREAM, /* 流式发送 */
	VIDEO_MODE_FRAME, /* 按帧发送 */
	VIDEO_MODE_LOWLATENCY /* 低延时模式发送 */
} VIDEO_MODE_E;

/**
 * 定义解码Device属性结构体
 */
typedef struct VDEC_DEV_ATTR_S {
	PAYLOAD_TYPE_E	enType; /* 码流类型 */
	VIDEO_MODE_E	enMode; /* 码流发送模式 */
	IMP_U32			u32PicWidth; /* 图像宽度 */
	IMP_U32			u32PicHeight; /* 图像高度 */
	IMP_U32			u32StreamBufSize; /* 码流buffer大小（Byte） */
	IMP_U32			u32RefFrameNum; /* 参考帧个数 */
} VDEC_DEV_ATTR_S;

/**
 * 定义解码模式枚举
 */
typedef enum VDEC_DECODE_MODE_E {
	VIDEO_DEC_MODE_IPB, /* IPB帧全部解码 */
	VIDEO_DEC_MODE_IP, /* 只解码IP帧 */
	VIDEO_DEC_MODE_I, /* 只解码I帧 */
} VDEC_DECODE_MODE_E;

/**
 * 定义解码输出顺序枚举
 */
typedef enum VDEC_OUTPUT_ORDER_E {
	VIDEO_OUTPUT_ORDER_DISP, /* 显示序 */
	VIDEO_OUTPUT_ORDER_DEC /* 解码序 */
} VDEC_OUTPUT_ORDER_E;

/**
 * 定义解码Device高级参数结构体
 */
typedef struct VDEC_DEV_PARAM_S {
	VDEC_DECODE_MODE_E		enDecMode; /* 解码方式，IPB、只解码I帧、只解码IP帧 */
	VDEC_OUTPUT_ORDER_E		enOutputOrder; /* 显示序、解码序 */
	IMP_BOOL				bEnableRFC; /* 是否开启参考帧压缩 */
} VDEC_DEV_PARAM_S;

/**
 * 定义解码Channel属性结构体
 */
typedef struct VDEC_CHN_ATTR_S {
	PIXEL_FORMAT_E			enOutputFormat; /* 输出格式 */
	IMP_U32					u32OutputWidth; /* 输出图像宽度 */
	IMP_U32					u32OutputHeight; /* 输出图像高度 */
	IMP_BOOL				bEnableCrop; /* 是否使能裁剪 */
	IMP_U32					u32CropX; /* 裁剪起始宽度坐标 */
	IMP_U32                 u32CropY; /* 裁剪起始高度坐标 */
	IMP_U32					u32CropWidth; /* 裁剪宽度 */
	IMP_U32                 u32CropHeight; /* 裁剪高度 */
	IMP_U32					u32FrameBufCnt; /* 帧buffer个数 */
} VDEC_CHN_ATTR_S;

/**
 * 定义解码Channel裁剪属性结构体
 */
typedef struct VDEC_CHN_CROP_ATTR_S {
	IMP_BOOL				bEnableCrop; /* 是否使能裁剪 */
	IMP_U32					u32CropX; /* 裁剪起始宽度坐标 */
	IMP_U32                 u32CropY; /* 裁剪起始高度坐标 */
	IMP_U32					u32CropWidth; /* 裁剪宽度 */
	IMP_U32                 u32CropHeight; /* 裁剪高度 */
} VDEC_CHN_CROP_ATTR_S;

/**
 * 定义解码错误信息结构体
 */
typedef struct VDEC_DECODE_ERROR_S {
	IMP_U32		u32FormatErr; /* 不支持的码流类型 */
	IMP_U32		u32PicSizeErr; /* 图片尺寸大于设备最大解码尺寸 */
	IMP_U32		u32StreamUnsprt; /* 码流规格不受支持 */
	IMP_U32		u32PackErr; /* 码流包出错 */
	IMP_U32		u32PrtNumErrSet; /* 解码参数不够，eg: slice, pps, sps */
	IMP_U32		u32RefErr; /* 参考帧个数不够 */
	IMP_U32     u32FormatErrSet; /* 解码参数格式出错，eg: slice, pps, sps, vps */
	IMP_U32     u32CountErrSet; /* 解码参数个数超出最大范围，eg: pps, sps, vps */
	IMP_U32		u32StreamSizeOver; /* 码流过大，被迫丢弃码流 */
	IMP_U32		u32VdecStreamNotRelease; /* 码流长时间未被释放 */
} VDEC_DECODE_ERROR_S;

/**
 * 定义解码状态结构体
 */
typedef struct VDEC_DEV_STATUS_S {
	PAYLOAD_TYPE_E			enType; /* 码流类型 */
	IMP_U32					u32LeftStreamBytes; /* 剩余未解码的码流大小（Byte） */
	IMP_U32					u32LeftStreamFrames; /* 剩余未解码的帧数（仅帧模式有效） */
	IMP_U32					u32LeftPics; /* 等待输出的图像个数 */
	IMP_BOOL				bStartRecvStream; /* 是否已经开始接收码流 */
	IMP_U32					u32RecvStreamFrames; /* 已经接收了多少帧码流（仅帧模式有效） */
	IMP_U32					u32DecodeStreamFrames; /* 已经解码了多少帧（仅帧模式有效） */
	VDEC_DECODE_ERROR_S		stVdecDecErr; /* 解码错误信息 */
} VDEC_DEV_STATUS_S;

/**
 * 定义解码码流结构体
 */
typedef struct VDEC_STREAM_S {
	IMP_U32		u32Len; /* 码流长度 */
	IMP_U64		u64PTS; /* 码流时间戳 */
	IMP_BOOL	bEndOfFrame; /* 一帧结束标志 */
	IMP_BOOL	bEndOfStream; /* 所有码流结束标志 */
	IMP_U8		*pu8Addr; /* 码流地址 */
	IMP_BOOL	bDisplay; /* 当前帧是否显示（仅帧模式有效） */
} VDEC_STREAM_S;

/**
 * 定义解码显示模式枚举
 */
typedef enum VIDEO_DISPLAY_MODE_E {
	VIDEO_DISPLAY_MODE_PREVIEW, /* 预览模式 */
	VIDEO_DISPLAY_MODE_PLAYBACK /* 回放模式 */
} VIDEO_DISPLAY_MODE_E;

/**
 * 定义用户数据结构体
 */
typedef struct VDEC_USERDATA_S {
	IMP_U64		u64PhyAddr; /* 用户数据的物理地址 */
	IMP_U32		u32Len; /* 用户数据大小 */
	IMP_BOOL	bValid; /* 是否有效 */
	IMP_U8*		pu8Addr; /* 用户数据的虚拟地址 */
} VDEC_USERDATA_S;

/**
 * 定义解码Buffer参数结构体
 */
typedef struct VDEC_BUFFER_PARAM_S {
	IMP_BOOL			bEnableZoom; /* 是否开启电子放大 */
	IMP_BOOL			bEnableChn0; /* 是否开启解码通道0 */
	IMP_BOOL			bEnableChn1; /* 是否开启解码通道1 */
	VDEC_DEV_ATTR_S		stDevAttr; /* Device属性 */
	VDEC_CHN_ATTR_S		stChn0Attr; /* Channel0属性 */
	VDEC_CHN_ATTR_S		stChn1Attr; /* Channel1属性 */
} VDEC_BUFFER_PARAM_S;

/**
 * 定义软件jpeg解码实例
 */
typedef struct JPEGD_INSTANCE_S {
	IMP_U32			u32ImageWidth; /* 解码获得的图像宽度 */
	IMP_U32			u32ImageHeight; /* 解码获得的图像高度 */
	PIXEL_FORMAT_E	enOutputFormat; /* 输出格式，目前仅支持ARGB1555 */
	IMP_U32			u32Len; /* jpeg图片长度 */
	IMP_U8*			pu8SrcAddr; /* 存放jpeg图片的源地址 */
	IMP_U8*			pu8DstAddr; /* 存放jpeg解码后图片的目的地址 */
	IMP_VOID*		priv; /* 内部使用私有数据 */
} JPEGD_INSTANCE_S;

/**
 * @fn IMP_S32 IMP_VDEC_CreateDev(IMP_S32 s32DevId, const VDEC_DEV_ATTR_S *pstAttr)
 *
 * 创建解码Device
 *
 * @param[in] s32DevId Device号，取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] pstAttr 解码Device属性指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 一路Device仅支持一路码流，不同码流需启动新的Device，一路Device仅支持两路解码channel。
 *
 * @attention 设备号不能超过最大设备号范围。
 * @attention 如果指定的Device已经存在，则返回失败。
 */
IMP_S32 IMP_VDEC_CreateDev(IMP_S32 s32DevId, const VDEC_DEV_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_VDEC_DestroyDev(IMP_S32 s32DevId)
 *
 * 销毁解码Device
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 销毁Device时，必须保证Device为空，即没有任何Channel被使能，同时必须保证解码Device已经停止接收码流，否则返回失败。
 * @attention 销毁并不存在或未创建的Device，则返回失败。
 */
IMP_S32 IMP_VDEC_DestroyDev(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_VDEC_GetDevAttr(IMP_S32 s32DevId, VDEC_DEV_ATTR_S *pstAttr)
 *
 * 获取解码Device的属性
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[out] pstAttr 解码Device属性指针
 *
 * @remarks 无。
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @attention 获取解码Device属性前必须保证解码Device已被创建，否则返回失败。
 */
IMP_S32 IMP_VDEC_GetDevAttr(IMP_S32 s32DevId, VDEC_DEV_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_VDEC_SetDevParam(IMP_S32 s32DevId, const VDEC_DEV_PARAM_S *pstParam)
 *
 * 设置解码Device的参数
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] pstParam 解码Device参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 设备号不能超过最大设备号范围。
 * @attention 如果指定的Device已经存在，则返回失败。
 */
IMP_S32 IMP_VDEC_SetDevParam(IMP_S32 s32DevId, const VDEC_DEV_PARAM_S *pstParam);

/**
 * @fn IMP_S32 IMP_VDEC_GetDevParam(IMP_S32 s32DevId, VDEC_DEV_PARAM_S *pstParam)
 *
 * 设置解码Device的参数
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] pstParam 解码Device参数指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 获取解码Device参数前，必须保证Device已被创建，否则返回错误。
 */
IMP_S32 IMP_VDEC_GetDevParam(IMP_S32 s32DevId, VDEC_DEV_PARAM_S *pstParam);

/**
 * @fn IMP_S32 IMP_VDEC_SetChnAttr(IMP_S32 s32DevId, IMP_S32 s32ChnId, const VDEC_CHN_ATTR_S *pstAttr)
 *
 * 设置解码输出Channel的属性
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 * @param[in] pstAttr 解码Channel属性指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 一路解码Device只有两个输出Channel。Channel0最大支持3840x2160分辨率，Channel1最大支持1920x1080分辨率。
 *
 * @attention 解码Channel未使能之前可设置Channel属性。
 * @attention 解码过程中，可以动态修改Channel属性。
 * @attention 若需要修改解码Channel属性，必须保证解码Channel已经被禁用。
 */
IMP_S32 IMP_VDEC_SetChnAttr(IMP_S32 s32DevId, IMP_S32 s32ChnId, const VDEC_CHN_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_VDEC_GetChnAttr(IMP_S32 s32DevId, IMP_S32 s32ChnId, const VDEC_CHN_ATTR_S *pstAttr)
 *
 * 获取解码输出Channel的属性
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 * @param[out] pstAttr 解码Channel属性指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 获取解码Channel属性前，必须保证解码Channel已被创建，否则返回失败。
 */
IMP_S32 IMP_VDEC_GetChnAttr(IMP_S32 s32DevId, IMP_S32 s32ChnId, VDEC_CHN_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_VDEC_EnableChn(IMP_S32 s32DevId, IMP_S32 s32ChnId)
 *
 * 使能解码输出Channel
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 使能解码通道之前，需要先调用IMP_VDEC_SetChnAttr设置通道属性。
 *
 * @attention 在使能解码Channel之前，必须保证解码Device已被创建，否则返回错误。
 * @attention 使能解码Channel时，若内存空间不足，会返回错误。
 */
IMP_S32 IMP_VDEC_EnableChn(IMP_S32 s32DevId, IMP_S32 s32ChnId);

/**
 * @fn IMP_S32 IMP_VDEC_DisableChn(IMP_S32 s32DevId, IMP_S32 s32ChnId)
 *
 * 禁用解码输出Channel
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 重复禁用解码Channel返回成功。
 */
IMP_S32 IMP_VDEC_DisableChn(IMP_S32 s32DevId, IMP_S32 s32ChnId);

/**
 * @fn IMP_S32 IMP_VDEC_QueryStatus(IMP_S32 s32DevId, VDEC_DEV_STATUS_S *pstStatus)
 *
 * 查询解码Device状态
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] pstStatus 解码Device状态指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_S32 IMP_VDEC_QueryStatus(IMP_S32 s32DevId, VDEC_DEV_STATUS_S *pstStatus);

/**
 * @fn IMP_S32 IMP_VDEC_SetChnCropAttr(IMP_S32 s32DevId, IMP_S32 s32ChnId, const VDEC_CHN_CROP_ATTR_S *pstCropAttr);
 *
 * 设置解码输出Channel的裁剪属性
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 * @param[in] pstCropAttr  裁剪属性
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 动态修改裁剪属性，必须保证解码Channel已使能。
 */
IMP_S32 IMP_VDEC_SetChnCropAttr(IMP_S32 s32DevId, IMP_S32 s32ChnId, const VDEC_CHN_CROP_ATTR_S *pstCropAttr);

/**
 * @fn IMP_S32 IMP_VDEC_StartRecvStream(IMP_S32 s32DevId)
 *
 * 解码Device开始接收码流
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 开始接收码流之前，必须保证解码Device已经创建，否则返回错误。
 */
IMP_S32 IMP_VDEC_StartRecvStream(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_VDEC_StopRecvStream(IMP_S32 s32DevId)
 *
 * 解码Device停止接收码流
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 开始接收码流之前，必须保证解码Device已经创建，否则返回错误。
 */
IMP_S32 IMP_VDEC_StopRecvStream(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_VDEC_SendStream(IMP_S32 s32DevId, VDEC_STREAM_S *pstStream, IMP_S32 s32MilliSec)
 *
 * 向解码Device发送码流
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] pstStream 解码码流数据指针
 * @param[in] s32MilliSec 送码流方式
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 发送码流之前，必须保证解码Device已被创建，并已经开始接收码流，否则会返回错误。
 * @attention 发送码流时，必须按照创建解码Device时设置的发送方式，来发送码流，否则会解码错误。
 * @attention 以非阻塞方式发送码流，如果码流buffer缓冲区已满，会返回IMP_EBUFFULL。
 * @attention 以超时方式发送码流，到达超时时间还不能成功发送码流，会返回IMP_EBUFFULL。
 * @attention 若在发送码流过程中，停止接收码流，该接口会返回IMP_EPERM。
 * @attention pstStream中PTS在VIDEO_MODE_FRAME模式下参数和意义如下:
 * @attention    0:用户不进行帧率控制
 * @attention   -1:图像不会被视频输出模块（VO）显示
 * @attention 其他:视频输出模块（VO）根据用户
 *
 */
IMP_S32 IMP_VDEC_SendStream(IMP_S32 s32DevId, VDEC_STREAM_S *pstStream, IMP_S32 s32MilliSec);

/**
 * @fn IMP_S32 IMP_VDEC_SetDepth(IMP_S32 s32DevId, IMP_S32 s32ChnId, IMP_U32 u32Depth, IMP_BOOL bCopy)
 *
 * 设置解码输出Channel的输出队列深度
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 * @param[in] u32Depth 解码Channel输出队列深度
 * @param[in] bCopy 设置获取Channel数据方式：是否申请物理内存对解码出的帧数据进行拷贝
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 设置输出队列深度之前，必须保证解码Device已被创建、解码Channel已被使能。
 * @attention 利用输出队列获取解码输出的图像，如果使用拷贝方式会消耗系统内存，但是不会占用帧buffer，不会影响到后级模块。
 * @attention 如果使用非拷贝方式会直接将帧buffer放入队列，若将缓存帧取走不及时归还，会影响到后级模块。
 */
IMP_S32 IMP_VDEC_SetDepth(IMP_S32 s32DevId, IMP_S32 s32ChnId, IMP_U32 u32Depth, IMP_BOOL bCopy);

/**
 * @fn IMP_S32 IMP_VDEC_GetFrame(IMP_S32 s32DevId, IMP_S32 s32ChnId, FRAME_INFO_S *pstFrameInfo)
 *
 * 获取解码输出Channel缓存区图像
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 * @param[in] pstFrameInfo 帧信息指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 获取帧信息之前，必须保证解码Device已被创建、解码Channel已被使能。
 * @attention 若取不到帧，会返回失败。
 */
IMP_S32 IMP_VDEC_GetFrame(IMP_S32 s32DevId, IMP_S32 s32ChnId, FRAME_INFO_S **pstFrameInfo);

/**
 * @fn IMP_S32 IMP_VDEC_ReleaseFrame(IMP_S32 s32DevId, IMP_S32 s32ChnId, FRAME_INFO_S *pstFrameInfo)
 *
 * 释放解码输出Channel缓存区图像
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 * @param[in] pstFrameInfo 帧信息指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 释放帧信息之前，必须保证解码Device已被创建、解码Channel已被使能。
 * @attention 传入的帧信息与缓存区的帧信息不匹配，将释放失败，返回错误。
 */
IMP_S32 IMP_VDEC_ReleaseFrame(IMP_S32 s32DevId, IMP_S32 s32ChnId, FRAME_INFO_S *pstFrameInfo);

/**
 * @fn IMP_S32 IMP_VDEC_GetFd(IMP_S32 s32DevId, IMP_S32 s32ChnId)
 *
 * 获取解码输出Channel的文件描述符
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] s32ChnId Channel号,取值范围:[0, @ref NR_MAX_VDEC_CHN - 1]
 *
 * @retval 大于等于0 成功
 * @retval 小于0 失败
 *
 * @remarks 无。
 *
 * @attention 获取Channel的文件描述符之前，必须保证解码Device已被创建、解码Channel已被使能。
 */
IMP_S32 IMP_VDEC_GetFd(IMP_S32 s32DevId, IMP_S32 s32ChnId);

/**
 * @fn IMP_S32 IMP_VDEC_SetUserPic(IMP_S32 s32DevId, const FRAME_INFO_S *pstUsrPic)
 *
 * 设置解码用户图片属性
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] pstUsrPic
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_S32 IMP_VDEC_SetUserPic(IMP_S32 s32DevId, const FRAME_INFO_S *pstUsrPic);

/**
 * @fn IMP_S32 IMP_VDEC_EnableUserPic(IMP_S32 s32DevId, IMP_BOOL bInstant)
 *
 * 使能插入解码用户图片
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] bInstant 使能用户图片方式
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_S32 IMP_VDEC_EnableUserPic(IMP_S32 s32DevId, IMP_BOOL bInstant);

/**
 * @fn IMP_S32 IMP_VDEC_DisableUserPic(IMP_S32 s32DevId, IMP_BOOL bInstant)
 *
 * 禁止使能插入解码用户图片
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_S32 IMP_VDEC_DisableUserPic(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_VDEC_SetDisplayMode(IMP_S32 s32DevId, VIDEO_DISPLAY_MODE_E enDisplayMode)
 *
 * 设置解码Device显示模式
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] enDisplayMode 显示模式枚举
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 预览模式下，解码模块以非阻塞方式获取输出Buffer，即使后级模块将输出Buffer全部占用，解码也可以继续进行，以达到实时预览的效果。
 * @remarks 回放模式下，解码模块以阻塞方式获取输出Buffer，若后级模块将输出Buffer全部占用，解码也会停止，知道获取到输出Buffer。
 *
 * @attention 设置解码Device显示模式之前，必须保证解码Device已被创建。
 */
IMP_S32 IMP_VDEC_SetDisplayMode(IMP_S32 s32DevId, VIDEO_DISPLAY_MODE_E enDisplayMode);

/**
 * @fn IMP_S32 IMP_VDEC_GetDisplayMode(IMP_S32 s32DevId, VIDEO_DISPLAY_MODE_E *penDisplayMode)
 *
 * 获取解码Device显示模式
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] penDisplayMode 显示模式指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 获取解码Device显示模式之前，必须保证解码Device已被创建。
 */
IMP_S32 IMP_VDEC_GetDisplayMode(IMP_S32 s32DevId, VIDEO_DISPLAY_MODE_E *penDisplayMode);

/**
 * @fn IMP_S32 IMP_VDEC_GetUserData(IMP_S32 s32DevId, VDEC_USERDATA_S *pstUserData, IMP_S32 s32MilliSec)
 *
 * 获取解码Device用户数据
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[out] pstUserData 用户数据指针
 * @param[in] s32MilliSec 获取用户数据的方式
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 用户数据为码流中的SEI增强信息。
 *
 * @attention 获取用户数据之前，必须保证解码Device已被创建。
 */
IMP_S32 IMP_VDEC_GetUserData(IMP_S32 s32DevId, VDEC_USERDATA_S *pstUserData, IMP_S32 s32MilliSec);

/**
 * @fn IMP_S32 IMP_VDEC_ReleaseUserData(IMP_S32 s32DevId, const VDEC_USERDATA_S* pstUserData)
 *
 * 释放解码Device用户数据
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] pstUserData 用户数据指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 释放用户数据之前，必须保证解码Device已被创建。
 * @attention 传入的用户数据指针必须与缓存区中一致，否则会释放失败。
 */
IMP_S32 IMP_VDEC_ReleaseUserData(IMP_S32 s32DevId, const VDEC_USERDATA_S* pstUserData);

/**
 * @fn IMP_S32 IMP_VDEC_AttachVbPool(IMP_S32 s32DevId, IMP_U32 u32PoolId)
 *
 * 将解码设备绑定到指定视频缓存池
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] u32PoolId 视频缓存池号
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 如果需要将解码设备绑定到指定缓存池，必须在解码Device创建之前调用该接口。
 */
IMP_S32 IMP_VDEC_AttachVbPool(IMP_S32 s32DevId, IMP_U32 u32PoolId);

/**
 * @fn IMP_S32 IMP_VDEC_DetachVbPool(IMP_S32 s32DevId)
 *
 * 将解码设备从视频缓存池解绑
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remarks 无。
 *
 * @attention 如果需要将解码设备从缓存池解绑，必须在解码Device销毁之后调用该接口。
 */
IMP_S32 IMP_VDEC_DetachVbPool(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_VDEC_GetBufferSize(VDEC_BUFFER_PARAM_S *pstBufferParam)
 *
 * 获取解码所需内存大小
 *
 * @param[in] pstBufferParam 解码buffer参数
 *
 * @retval >0 成功
 * @retval <=0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_S32 IMP_VDEC_GetBufferSize(VDEC_BUFFER_PARAM_S *pstBufferParam);

/**
 * @fn IMP_S32 IMP_VDEC_SetCacheTime(IMP_S32 s32DevId, IMP_U64 u64CacheTime)
 *
 * 设置缓存时长，一定缓存时间后，解码器才开始工作
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 * @param[in] u64CacheTime 缓存时长,单位微秒
 *
 * @retval =0 成功
 * @retval <0 失败
 *
 * @remarks 无。
 *
 * @attention 缓存时间只在预览模式并且启用时间戳时才生效。
 */
IMP_S32 IMP_VDEC_SetCacheTime(IMP_S32 s32DevId, IMP_U64 u64CacheTime);

/**
 * @fn IMP_S32 IMP_VDEC_AttachCPU(IMP_U32 u32CPUNum)
 *
 * 设置解码线程所绑定的CPU
 *
 * @param[in] u32CPUNum CPU号，目前有CPU0和CPU1
 *
 * @retval =0 成功
 * @retval <0 失败
 *
 * @remarks 无。
 *
 * @attention 如果不调用此接口，默认vdec线程在CPU多核间均衡。
 * @attention 该接口必须在IMP_System_Init之前调用，否则调用无效。
 */
IMP_S32 IMP_VDEC_AttachCPU(IMP_U32 u32CPUNum);

/**
 * @fn IMP_S32 IMP_VDEC_EnablePtsCtrl(IMP_S32 s32DevId)
 *
 * 开启解码的帧率控制
 *
 * @param[in] s32DevId Device号,取值范围:[0, @ref NR_MAX_VDEC_DEV - 1]
 *
 * @retval =0 成功
 * @retval <0 失败
 *
 * @remarks 无。
 *
 * @attention 默认关闭解码PTS帧率控制。
 * @attention 如果需要使用pts帧率控制,需在第一次送帧前调用此接口开启。
 * @attention 销毁Device后会关闭PTS帧率控制。
 */
IMP_S32 IMP_VDEC_EnablePtsCtrl(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_JPEGD_Init(JPEGD_INSTANCE_S *pstInstance)
 *
 * 软件jpeg解码句柄初始化
 *
 * @param[in] pstInstance jpeg解码实例
 *
 * @retval =0 成功
 * @retval <0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_S32 IMP_JPEGD_Init(JPEGD_INSTANCE_S *pstInstance);

/**
 * @fn IMP_S32 IMP_JPEGD_Decode(JPEGD_INSTANCE_S *pstInstance)
 *
 * 开始软件jpeg解码
 *
 * @param[in] pstInstance jpeg解码实例
 *
 * @retval =0 成功
 * @retval <0 失败
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_S32 IMP_JPEGD_Decode(JPEGD_INSTANCE_S *pstInstance);

/**
 * @fn IMP_S32 IMP_JPEGD_DeInit(JPEGD_INSTANCE_S *pstInstance)
 *
 * 软件jpeg解码句柄销毁
 *
 * @param[in] pstInstance jpeg解码实例
 *
 * @retval =0 成功
 * @retval <0 失败
 *
 * @remarks 无。
 *
 * @attention 逆初始化时的实例需要和初始化时的实例一致。
 */
IMP_S32 IMP_JPEGD_DeInit(JPEGD_INSTANCE_S *pstInstance);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_VDEC_H__ */
