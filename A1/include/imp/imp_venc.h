/*
 * IMP Encoder func header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_VENC_H__
#define __IMP_VENC_H__

#include <stdint.h>
#include <stdio.h>
#include "imp_type.h"
#include "imp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP 视频编码头文件
 */

/**
 * 定义编码帧码流包结构体
 */
typedef struct {
	IMP_U32 u32PhyAddr;   /**< 码流包物理地址 */
	IMP_U32 u32VirAddr;   /**< 码流包虚拟地址 */
	IMP_U32 u32Len;       /**< 码流包长度 */
	IMP_U64 u64Ts;        /**< 时间戳，单位us */
	IMP_BOOL bFrameEnd;   /**< 帧结束标识 */
} VENC_PACK_S;

/**
 * 定义编码帧码流类型结构体
 */
typedef struct {
	VENC_PACK_S *pack;       /**< 帧码流包结构 */
	IMP_U32 u32PackCount;    /**< 一帧码流的所有包的个数 */
	IMP_U32 u32Seq;          /**< 编码帧码流序列号 */
} VENC_STREAM_S;

/**
 * 定义编码器属性结构体
 */
typedef struct {
	IMP_BOOL enCrop;
	IMP_U32 u32X;
	IMP_U32 u32Y;
	IMP_U32 u32W;
	IMP_U32 u32H;
} VENC_CROP_S;

/**
 * 定义编码器属性结构体
 */
typedef struct {
	PAYLOAD_TYPE_E enType;    /**< 编码协议类型 */
	IMP_U32 u32BufSize;       /**< 配置 buffer 大小 */
	IMP_U32 u32ProFile;       /**< 编码的等级, 0: baseline; 1:MP; 2:HP */
	IMP_U32 u32PicWidth;      /**< 编码输入图像宽度 */
	IMP_U32 u32PicHeight;     /**< 编码输入图像高度 */
	IMP_U32 u32OutPicWidth;   /**< 编码输出图像宽度 */
	IMP_U32 u32OutPicHeight;  /**< 编码输出图像高度 */
	VENC_CROP_S stCrop;       /**< 编码裁减属性 */
} VENC_CHN_ATTR_S;

/**
 * 定义编码Channel的状态结构体
 */
typedef struct {
	IMP_U32 u32LeftStreamBytes;   /**< 码流buffer剩余的byte数 */
	IMP_U32 u32LeftStreamFrames;  /**< 码流buffer剩余的帧数 */
	IMP_U32 u32CurPacks;          /**< 当前帧的码流包个数 */
	IMP_BOOL bWorkDone;           /**< 通道程序运行状态，0：正在运行，1，未运行 */
} VENC_CHN_STATUS_S;

/*
 * 定义JPEG编码量化表参数结构体
 */
typedef struct {
    IMP_BOOL bUserQl;          /**< 0: 使用默认量化表; 1:使用用户量化表*/
    IMP_U8 u8QmemTable[128];   /**< 用户自定义量化表*/
} VENC_JPEGQL_S;

/**
 * @fn IMP_S32 IMP_VENC_CreateChn(IMP_S32 s32Chn, VENC_CHN_ATTR_S *pstChnAttr)
 *
 * 创建编码Chn
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] pstChnAttr 编码Chn属性指针
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 指针pstChnAttr为NULL, 返回错误码
 */
IMP_S32 IMP_VENC_CreateChn(IMP_S32 s32Chn, VENC_CHN_ATTR_S *pstChnAttr);

/**
 * @fn IMP_S32 IMP_Encoder_DestroyChn(IMP_S32 s32Chn)
 *
 * 销毁编码Chn
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 调用该函数之前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_DestroyChn(IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_VENC_GetChnAttr(IMP_S32 s32Chn, VENC_CHN_ATTR_S *pstChnAttr)
 *
 * 获取编码Chn属性
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] pstChnAttr 编码Chn属性
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 调用该函数之前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_GetChnAttr(IMP_S32 s32Chn, VENC_CHN_ATTR_S *pstChnAttr);

/**
 * @fn IMP_S32 IMP_VENC_StartRecvStream(IMP_S32 s32Chn)
 *
 * 开启编码Chn接收图像
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 开启编码Chn接收图像后才能开始编码
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_StartRecvStream(IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_VENC_StopRecvStream(IMP_S32 s32Chn)
 *
 * 停止编码Chn接收图像
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 调用该函数仅停止接收原始数据, 码流buffer不会释放
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_StopRecvStream(IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_VENC_SendFrame(IMP_S32 s32Chn, FRAME_INFO_S *pstFrame, IMP_S64 s64MillSec)
 *
 * 发送用户数据进行编码
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] pstFrame 帧结构体指针
 * @param[in] s64MillSec 码流获取方式, -1:阻塞, 0:非阻塞, >0:超时时间
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 指针pstFrame为NULL, 返回错误
 * @remarks 调用该函数前, 确保编码Chn已经创建并且已经开始接受图像
 */
IMP_S32 IMP_VENC_SendFrame(IMP_S32 s32Chn, FRAME_INFO_S *pstFrame, IMP_S64 s64MillSec);

/**
 * @fn IMP_S32 IMP_VENC_Query(IMP_S32 s32Chn, VENC_CHN_STATUS_S *pstChnStat)
 *
 * 查询编码Chn状态
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[out] pstChnStat 编码Chn状态
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_Query(IMP_S32 s32Chn, VENC_CHN_STATUS_S *pstChnStat);

/**
 * @fn IMP_S32 IMP_VENC_GetStream(IMP_S32 s32Chn, VENC_STREAM_S *pstStream, IMP_S64 s64MillSec)
 *
 * 获取编码的码流
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] pstStream 码流结构体指针
 * @param[in] s64MillSec 码流获取方式, -1:阻塞, 0:非阻塞, >0:超时时间
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks IMP_VENC_ReleaseStream必须和IMP_VENC_GetStream配套使用
 * @remarks 指针pstStream为NULL, 返回错误
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_GetStream(IMP_S32 s32Chn, VENC_STREAM_S *pstStream, IMP_S64 s64MillSec);

/**
 * @fn IMP_S32 IMP_VENC_ReleaseStream(IMP_S32 s32Chn, VENC_STREAM_S *pstStream)
 *
 * 释放码流缓存
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] pstStream 码流结构体指针
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks IMP_VENC_ReleaseStream必须和IMP_VENC_GetStream配套使用
 * @remarks 指针pststream为NULL, 返回错误
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_ReleaseStream(IMP_S32 s32Chn, VENC_STREAM_S *pstStream);

/**
 * @fn IMP_S32 IMP_VENC_GetFd(IMP_S32 s32Chn)
 *
 * 获取编码Chn对应的设备文件句柄
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 *
 * @retval >=0 成功, 返回设备文件描述符
 * @retval < 0 失败
 *
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_GetFd(IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_VENC_CloseFd(IMP_S32 s32Chn)
 *
 * 关闭编码Chn对应的设备文件句柄
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 *
 * @retval >=0 成功
 * @retval < 0 失败
 *
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_CloseFd(IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_VENC_GetBufferSize(VENC_CHN_ATTR_S *pstChnAttr)
 *
 * 获取编码通道需要的内存大小
 *
 * @param[in] pstChnAttr 编码通道属性
 *
 * @retval >=0 成功
 * @retval < 0 失败
 *
 * @remarks 调用该函数前, 确保编码Chn没有创建
 */
IMP_S32 IMP_VENC_GetBufferSize(VENC_CHN_ATTR_S *pstChnAttr);

/**
 * @fn IMP_S32 IMP_VENC_AttachVbPool(IMP_S32 s32Chn, IMP_U32 u32PoolId)
 *
 * 将编码设备绑定到指定视频缓存池
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] u32PoolId 视频缓存池Id
 *
 * @retval >=0 成功
 * @retval < 0 失败
 *
 * @remarks 调用该函数前, 确保编码Chn没有创建
 */
IMP_S32 IMP_VENC_AttachVbPool(IMP_S32 s32Chn, IMP_U32 u32PoolId);

/**
 * @fn IMP_S32 IMP_VENC_DetachVbPool(IMP_S32 s32Chn)
 *
 * 将编码设备解绑到指定视频缓存池
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 *
 * @retval >=0 成功
 * @retval < 0 失败
 *
 * @remarks 调用该函数前, 确保编码Chn没有创建
 */
IMP_S32 IMP_VENC_DetachVbPool(IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_VENC_SetJpegeQl(IMP_S32 s32Chn, const VENC_JPEGQL_S *pstJpegeQl)
 *
 * 设置JPEG编码Chn的量化表配置参数
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] pstJpegeQl JPEG编码Chn的量化表配置参数, 128个字节
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_SetJpegeQl(IMP_S32 s32Chn, const VENC_JPEGQL_S *pstJpegeQl);

/**
 * @fn IMP_S32 IMP_VENC_GetJpegeQl(IMP_S32 s32Chn, VENC_JPEGQL_S *pstJpegeQl)
 *
 * 获取JPEG编码Chn的用户量化表配置参数
 *
 * @param[in] s32Chn 编码Chn号, 取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[out] pstJpegeQl 返回JPEG编码Chn的量化表配置参数
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_GetJpegeQl(IMP_S32 s32Chn, VENC_JPEGQL_S *pstJpegeQl);

/**
 * @fn IMP_S32 IMP_VENC_GetStreamCnt(IMP_S32 s32Chn, IMP_U32 *u32StreamCnt)
 *
 * 获取JPEG编码Chn的码流缓冲区个数
 *
 * @param[in] s32Chn 编码Chn号, 取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[out] u32StreamCnt 返回JPEG编码Chn的码流缓冲区个数
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 调用该函数前, 确保编码Chn已经创建
 */
IMP_S32 IMP_VENC_GetStreamCnt(IMP_S32 s32Chn, IMP_U32 *u32StreamCnt);

/*
 * @fn IMP_S32 IMP_VENC_CreateChnEXT(IMP_S32 s32Chn, VENC_CHN_ATTR_S *pstChnAttr, IMP_U32 u32StreamCnt)
 *
 * 创建编码Chn拓展接口
 *
 * @param[in] s32Chn 编码Chn号,取值范围: [0, NR_MAX_ENC_CHN - 1]
 * @param[in] pstChnAttr 编码Chn属性指针
 * @param[in] u32StreamCnt 音频流buffer的数量 0:表示系统自动设置 大于0:设置buffer的数量
 *
 * @retval 0 成功
 * @retval 非0 失败, 返回错误码
 *
 * @remarks 指针pstChnAttr为NULL, 返回错误码
 */
IMP_S32 IMP_VENC_CreateChnEXT(IMP_S32 s32Chn, VENC_CHN_ATTR_S *pstChnAttr, IMP_U32 u32StreamCnt);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_ENCODER_H__ */
