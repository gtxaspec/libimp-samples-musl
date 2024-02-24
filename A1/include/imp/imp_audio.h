/*
 * Audio utils header file.

 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_AUDIO_H__
#define __IMP_AUDIO_H__
#include <stdint.h>
#include "imp_type.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP 音频输入输出头文件
 */

/**
 * @defgroup IMP_Audio
 * @ingroup imp
 * @brief 音频模块，包含录放音、音频编解码、音量及增益设置、回声消除、自动增益等功能

 * @section audio_summary 1 概述
 * 音频功能包含音频输入,音频输出,回音消除,音频编码和音频解码5个模块. \n
 * 其中音频输入和音频输出存在设备和通道的概念.其中一个MIC我们认为是一个Device,而一个MIC可以有多路Channel输入. \n
 * 同样的一个SPK我们认为是一个放音Device,而一个SPK也可以有多路Channel输出. \n
 * 当前版本的音频API一个Device只支持一个Channel. \n
 * 回音消除位于音频输入接口中,具体说明在功能描述中体现. \n
 * 音频编码当前音频API中支持PT_G711A、PT_G711U和PT_G726格式音频编码,如需要增加新的编码方式,需要注册编码器. \n
 * 音频解码当前音频API中支持PT_G711A、PT_G711U和PT_G726格式音频解码,如需要增加新的解码方式,需要注册解码器. \n
 * @section audio_function_description 2 功能描述
 * 以下是对每个模块的具体说明
 * @subsection audio_in 2.1 音频输入
 * 音频输入Device ID 对应关系, 0: 对应数字MIC  1: 对应模拟MIC \n
 * 音频输入Channel当前API只支持1个通道. \n
 * 音频输入的音量设置,音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.其中60是音量设置的一个临界点，\n
 * 在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB。
 * @subsection audio_out 2.2 音频输出
 * 音频输出Device ID 对应关系, 0: 对应默认SPK 1: 对应其他SPK \n
 * 音频输出Channel当前API只支持1个通道. \n
 * 音频输出的音量设置,音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.其中60是音量设置的一个临界点，\n
 * 在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB. \n
 * @subsection audio_aec 2.3 回音消除
 * 回音消除属于音频输入接口中的一项功能,所以在使能回音消除时必须先使能音频输入设备和通道. \n
 * 回音消除当前支持音频采样率为8K和16K，一帧数据采样数为10ms音频数据的整数倍（如：8K采样率， 送入的数据为：8000 × 2 / 100 = 160byte的整数倍）。\n
 * 回音消除针对不同的设备，不同的封装，回音消除会有不同的效果. \n
 * 回音消除目前不支持自适应，所以针对不同设备有单独的回音消除参数, \n
 * 回音消除的参数文件位于/etc/webrtc_profile.ini 配置文件中. \n
 * 配置文件格式为如下(以下列出主要需要调试的三个参数): \n
 * [Set_Far_Frame] \n
 * Frame_V=0.3 \n
 * [Set_Near_Frame] \n
 * Frame_V=0.1 \n
 * delay_ms=150 \n

 * 其中第一个标签[Set_Far_Frame]中的内容代表远端参数,即SPK端放音数据参数. \n
 * Fram_V 代表音频幅值比例,调节该参数可以调节放音数据的幅度(此幅度只用于回音消除). \n
 * 其中第一个标签[Set_Near_Frame]中的内容代表近端参数,即MIC端录音数据参数. \n
 * Fram_V 代表音频幅值比例,调节该参数可以调节录音数据的幅度(此幅度只用于回音消除). \n
 * delay_ms 由于软件和硬件具有延时性,且SPK与MIC的安放有一定距离,SPK放音数据会重新被MIC采样,所以SPK数据在MIC数据中体现会存在一定延时. \n
 * 该时间代表放音数据在录音数据中的时间差. \n

 * @subsection audio_enc 2.4 音频编码
 * 音频编码目前音频API支持PT_G711A、PT_G711U和PT_G726格式音频编码,如果需要增加新的编码方式,需要调用IMP_AENC_RegisterEncoder接口进行注册编码器.
 * @subsection audio_dec 2.5 音频解码
 * 音频解码目前音频API支持PT_G711A、PT_G711U和PT_G726格式音频解码,如果需要增加新的解码方式,需要调用IMP_ADEC_RegisterDecoder接口进行注册解码器.
 * @{
 */

/**
 * 最大音频帧缓存数
 */
#define MAX_AUDIO_FRAME_NUM 50

/**
 * 音频流阻塞类型
 */
typedef enum {
	NOBLOCK = 0,			/**< 非阻塞 */
	BLOCK   = 1,			/**< 阻塞 */
}IMP_BLOCK_E;

/**
 * 音频采样率定义.
 */
typedef enum {
	AUDIO_SAMPLE_RATE_8000	= 8000,		/**< 8KHz采样率 */
	AUDIO_SAMPLE_RATE_16000 = 16000,	/**< 16KHz采样率 */
	AUDIO_SAMPLE_RATE_24000 = 24000,	/**< 24KHz采样率 */
	AUDIO_SAMPLE_RATE_32000 = 32000,	/**< 32KHz采样率 */
	AUDIO_SAMPLE_RATE_44100 = 44100,	/**< 44.1KHz采样率 */
	AUDIO_SAMPLE_RATE_48000 = 48000,	/**< 48KHz采样率 */
	AUDIO_SAMPLE_RATE_96000 = 96000,	/**< 96KHz采样率 */
}IMP_AUDIO_SAMPLE_RATE_E;

/**
 * 音频采样精度定义.
 */
typedef enum {
	AUDIO_BIT_WIDTH_16 = 16,		/**< 16bit采样精度 */
	AUDIO_BIT_WIDTH_20 = 20,		/**< 20bit采样精度 */
	AUDIO_BIT_WIDTH_24 = 24,		/**< 24bit采样精度 */
}IMP_AUDIO_BIT_WIDTH_E;

/**
 * 音频声道模式定义.
 */
typedef enum {
	AUDIO_SOUND_MODE_MONO	= 1,	/**< 单声道 */
	AUDIO_SOUND_MODE_STEREO = 2,	/**< 双声道 */
}IMP_AUDIO_SOUND_MODE_E;

/**
 * 回音消除通道选择.
 */
typedef enum {
	AUDIO_AEC_CHANNEL_FIRST_LEFT   = 0,	/**< 选择左声道或者通道1做回音消除*/
	AUDIO_AEC_CHANNEL_SECOND_RIGHT = 1,	/**< 选择右声道或者通道2做回音消除*/
	AUDIO_AEC_CHANNEL_THIRD  = 2,		/**< 选择通道3做回音消除*/
	AUDIO_AEC_CHANNEL_FOURTH = 3,		/**< 选择通道4回音消除*/
}IMP_AUDIO_AEC_CHN_E;

/**
 * 定义音频净荷类型枚举.
 */
typedef enum {
	PT_PCM		= 0,
	PT_G711A	= 1,
	PT_G711U	= 2,
	PT_G726 	= 3,
	PT_AEC		= 4,
	PT_ADPCM	= 5,
	PT_MAX		= 6,
}IMP_AUDIO_PALY_LOAD_TYPE_E;

/**
 * 定义解码方式.
 */
typedef enum {
	ADEC_MODE_PACK   = 0,	/**< Pack 方式解码 */
	ADEC_MODE_STREAM = 1,	/**< Stream 方式解码 */
}IMP_AUDIO_ADEC_MODE_S;

/**
 * 音频输入输出设备属性.
 */
typedef struct {
	IMP_AUDIO_SAMPLE_RATE_E eSamplerate;	/**< 音频采样率 */
	IMP_AUDIO_BIT_WIDTH_E   eBitwidth;		/**< 音频采样精度 */
	IMP_AUDIO_SOUND_MODE_E  eSoundmode; 	/**< 音频声道模式 */
	IMP_S32 s32NumPerFrm;					/**< 每帧的采样点个数 */
	IMP_S16 s16ChnCnt;						/**< 支持的通道数目 */
}IMP_AUDIO_IOATTR_S;

/**
 * 音频帧结构体.
 */
typedef struct {
	IMP_AUDIO_BIT_WIDTH_E  eBitwidth;	/**< 音频采样精度 */
	IMP_AUDIO_SOUND_MODE_E eSoundmode;	/**< 音频声道模式 */
	IMP_U32 *pu32VirAddr;				/**< 音频帧数据虚拟地址 */
	IMP_U32  u32PhyAddr;				/**< 音频帧数据物理地址 */
	IMP_S64  s64TimeStamp;				/**< 音频帧数据时间戳 */
	IMP_S32  s32Seq;					/**< 音频帧序号 */
	IMP_S32  s32Len;					/**< 音频帧长度 */
}IMP_AUDIO_FRAME_S;

/**
 * 音频通道参数结构体.
 */
typedef struct {
	IMP_AUDIO_AEC_CHN_E eAecChn;		/**< 选择回音消除的通道*/
	IMP_S32 s32Rev;						/**< 保留 */
}IMP_AUDIO_CHN_PARAM_S;

/**
 * 定义音频码流结构体.
 */
typedef struct {
	IMP_U8 *pu8Stream;			/**< 数据流指针 */
	IMP_U32 u32PhyAddr;			/**< 数据流物理地址 */
	IMP_S32 s32Len;				/**< 音频码流长度 */
	IMP_S64 S64TimeStamp;		/**< 时间戳 */
	IMP_S32 s32Seq;				/**< 音频码流序号 */
}IMP_AUDIO_STREAM_S;

/**
 * 定义音频编码通道属性结构体.
 */
typedef struct {
	IMP_AUDIO_PALY_LOAD_TYPE_E eType;	/**< 音频净荷数据类型 */
	IMP_S32 s32BufSize;					/**<  buf 大小，以帧为单位，[2～MAX_AUDIO_FRAME_NUM] */
	IMP_U32 *pu32Value;					/**< 协议属性指针 */
}IMP_AUDIO_ENC_CHN_ATTR_S;

/**
 * 定义编码器属性结构体.
 */
typedef struct {
	IMP_AUDIO_PALY_LOAD_TYPE_E eType;	/**< 编码协议类型 */
	IMP_S32  s32MaxFrmLen;				/**< 最大码流长度 */
	IMP_CHAR charName[16];				/**< 编码器名称 */
	IMP_S32 (*openEncoder)(IMP_VOID *pvoidEncoderAttr, IMP_VOID **pvoidEncoder);
	IMP_S32 (*encoderFrm)(IMP_VOID **pvoidEncoder, IMP_AUDIO_FRAME_S *pstData, IMP_U8 *pu8Outbuf, IMP_S32 *ps32OutLen);
	IMP_S32 (*closeEncoder)(IMP_VOID **pvoidEncoder);
}IMP_AUDIO_ENCODER_S;

/**
 * 定义解码通道属性结构体.
 */
typedef struct {
	IMP_AUDIO_PALY_LOAD_TYPE_E eType;	/**< 音频解码协议类型 */
	IMP_S32 s32BufSize;					/**< 音频解码缓存大小 */
	IMP_AUDIO_ADEC_MODE_S stMode;		/**< 解码方式 */
	IMP_VOID *pvoidValue;				/**< 具体协议属性指针 */
}IMP_AUDIO_DEC_CHN_ATTR_S;

/**
 * 定义解码器属性结构体.
 */
typedef struct {
	IMP_AUDIO_PALY_LOAD_TYPE_E eType;	/**< 音频解码协议类型 */
	IMP_CHAR charName[16];				/**< 音频解码器名字 */
	IMP_S32 (*openDecoder)(IMP_VOID *pvoidDecoderAttr, IMP_VOID **pvoidDecoder);
	IMP_S32 (*decodeFrm)(IMP_VOID **pvoidDecoder, IMP_U8 *pu8Inbuf, IMP_S32 s32InLen, IMP_U16 *pu16Outbuf, IMP_S32* ps32OutLen, IMP_S32 *ps32Chns);
	IMP_S32 (*getFrmInfo)(IMP_VOID *pvoidDecoder, IMP_VOID *pvoidInfo);
	IMP_S32 (*closeDecoder)(IMP_VOID **decoder);
}IMP_AUDIO_DECODER_S;

/**
 * 定义AGC增益结构体.
 */
typedef struct {
	IMP_S32 s32TargetLevelDbfs;		/**< 增益级别,取值为[0, 31], 这指目标音量级别,单位为db,为负值.值越小,音量越大. */
	IMP_S32 s32CompressionGaindB;	/**< 设置最大的增益值,[0, 90],0代表无增益,值越大,增益越高. */
}IMP_AUDIO_AGC_CONFIG_S;

/**
 * 定义噪声抑制级别.
 */
enum Level_ns {
	NS_LOW,			/**< 低等级级别噪声抑制 */
	NS_MODERATE,	/**< 中等级级别噪声抑制 */
	NS_HIGH,		/**< 高等级级别噪声抑制 */
	NS_VERYHIGH		/**< 最高等级级别噪声抑制 */
};

/**
 * @fn IMP_S32 IMP_AI_SetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr)

 * 设置音频输入设备属性.

 * @param[in] s32DevId 音频设备号.
 * @param[in] pstAttr  音频设备属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_S32 devID = 1;
 * IMP_AUDIO_IOATTR_S attr;
 * attr.samplerate = AUDIO_SAMPLE_RATE_8000;
 * attr.bitwidth   = AUDIO_BIT_WIDTH_16;
 * attr.soundmode  = AUDIO_SOUND_MODE_MONO;
 * attr.numPerFrm  = 320;
 * attr.chnCnt = 1;
 * ret = IMP_AI_SetPubAttr(devID, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Set Audio in %d attr err: %d\n", devID, ret);
 *		return ret;
 * }
 * @endcode

 * @attention 需要在IMP_AI_Enable前调用.
 */
IMP_S32 IMP_AI_SetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_AI_GetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr)
 * 获取音频输入设备属性.

 * @param[in]  s32DevId 音频设备号.
 * @param[out] pstAttr  音频设备属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_GetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_AI_Enable(IMP_S32 s32DevId)
 * 启用音频输入设备.

 * @param[in] s32DevId 音频设备号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 在调用此函数前必须调用 IMP_AI_SetPubAttr().
 */
IMP_S32 IMP_AI_Enable(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_AI_Disable(IMP_S32 s32DevId)

 * 禁用音频输入设备.

 * @param[in] s32DevId 音频设备号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 与IMP_AI_Enable配套使用,在系统休眠前必须执行IMP_AI_Disable.
 */
IMP_S32 IMP_AI_Disable(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_AI_EnableChn(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 启用音频输入通道.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 必须先使能device.
 */
IMP_S32 IMP_AI_EnableChn(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AI_DisableChn(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 禁用音频输入通道.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 与IMP_AI_EnableChn配套使用.
 */
IMP_S32 IMP_AI_DisableChn(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AI_GetFrame(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm)

 * 获取音频帧.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] pstFrm   音频帧结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_AUDIO_FRAME_S frm;
 * // 获取音频帧
 * ret = IMP_AI_GetFrame(devID, chnID, &frm);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
 *		return ret;
 * }

 * fwrite(frm.virAddr, 1, frm.len, record_file); // 使用音频帧数据
 * @endcode

 * @attention 无.
 */
IMP_S32 IMP_AI_GetFrame(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm);

/**
 * @fn IMP_AI_GetFrameTimeout(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_S32 s32Timeout)

 * 获取音频帧.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] pstFrm   音频帧结构体指针.
 * @param[in] s32Timeout   获取音频帧阻塞等待时间，-1表示阻塞模式，0 表示非阻塞模式，>0 表示阻塞多少毫秒，超时则报错返回.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_AUDIO_FRAME_S frm;
 * // 获取音频帧
 * ret = IMP_AI_GetFrameTimeout(devID, chnID, &frm, -1);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
 *		return ret;
 * }

 * fwrite(frm.virAddr, 1, frm.len, record_file); // 使用音频帧数据
 * @endcode

 * @attention s32Timeout 如果设置阻塞时间，一般建议设置为音频帧时间的两倍及以上.
 */
IMP_S32 IMP_AI_GetFrameTimeout(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_S32 s32Timeout);

/**
 * @fn IMP_S32 IMP_AI_SetChnParam(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_CHN_PARAM_S *pstParam)

 * 设置音频输入通道参数.

 * @param[in] s32DevId  音频设备号.
 * @param[in] s32Chn 	音频输入通道号.
 * @param[in] pstParam  音频通道参数指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_S32 chnID = 0;
 * IMP_AUDIO_CHN_PARAM_S chnParam;
 * chnParam.usrFrmDepth = 20;	// the range of valid value is [2, MAX_AUDIO_FRAME_NUM].
 * ret = IMP_AI_SetChnParam(devID, chnID, &chnParam);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "set ai %d channel %d attr err: %d\n", devID, chnID, ret);
 *		return ret;
 * }
 * @endcode

 * @attention 在IMP_AI_EnableChn前调用.
 */
IMP_S32 IMP_AI_SetChnParam(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_CHN_PARAM_S *pstParam);

/**
 * @fn IMP_S32 IMP_AI_GetChnParam(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_CHN_PARAM_S *pstParam)

 * 获取音频输入通道参数.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] pstParam 音频通道参数指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_GetChnParam(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_CHN_PARAM_S *pstParam);

/**
 * @fn IMP_S32 IMP_AI_EnableAec(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 启用指定音频输入和音频输出的回声抵消功能.

 * @param[in] aiDevId 需要进行回声抵消的音频输入设备号.
 * @param[in] s32Chn 需要进行回声抵消的音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 回音消除针对不同的设备，不同的封装，回音消除会有不同的效果.
 * @remarks 回音消除目前不支持自适应，所以针对不同设备有单独的回音消除参数,
 * @remarks 仅仅只是使能该功能效果不一定会好。
 * @remarks 回音消除的参数文件位于/etc/webrtc_profile.ini 配置文件中.
 * @remarks 配置文件格式为如下(以下列出主要需要调试的三个参数):
 * @remarks [Set_Far_Frame]
 * @remarks Frame_V=0.3
 * @remarks [Set_Near_Frame]
 * @remarks Frame_V=0.1
 * @remarks delay_ms=150

 * @remarks 其中第一个标签[Set_Far_Frame]中的内容代表远端参数,即SPK端放音数据参数.
 * @remarks Fram_V 代表音频幅值比例,调节该参数可以调节放音数据的幅度(此幅度只用于回音消除).
 * @remarks 其中第一个标签[Set_Near_Frame]中的内容代表近端参数,即MIC端录音数据参数.
 * @remarks Fram_V 代表音频幅值比例,调节该参数可以调节录音数据的幅度(此幅度只用于回音消除).
 * @remarks delay_ms 由于软件和硬件具有延时性,且SPK与MIC的安放有一定距离,SPK放音数据会重新被MIC采样,所以SPK数据在MIC数据中体现会存在一定延时.
 * @remarks 该时间代表放音数据在录音数据中的时间差.

 * @attention 实际上接口只会检查aiDevId和s32Chn.但是最好在两个通道同时使能后调用. \n
 * 在关闭音频输入通道的同时,回音消除功能同时关闭.如果再次使用需要再次打开.
 */
IMP_S32 IMP_AI_EnableAec(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AI_DisableAec(IMP_S32 s32DevId, IMP_S32 s32Chn)
 * 禁用回声抵消功能.

 * @param[in] aiDevId 音频输入设备号.
 * @param[in] s32Chn 音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_DisableAec(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AI_EnableNs(IMP_AUDIO_IOATTR_S *pstAttr, IMP_S32 s32Mode)

 * 启用指定音频输入的噪声抑制功能.

 * @param[in] attr 需要进行噪声抑制的音频属性.
 * @param[in] mode 噪声抑制的级别0~3,参见 Level_ns.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 噪声抑制的mode参数表示噪声抑制的级别,范围为[0 ~ 3],级别越高,噪声抑制的越干净.
 * @remarks 然而,噪声抑制的越干净同时也就会丢失更多的声音细节,所以这里有一个矛盾点,需要在
 * @remarks 使用的时候进行权衡.

 * @attention 回声消除包含了噪声抑制功能,如果启用回声抵消,就不需要做噪声抑制.
 */
IMP_S32 IMP_AI_EnableNs(IMP_AUDIO_IOATTR_S *pstAttr, IMP_S32 s32Mode);

/**
 * @fn IMP_S32 IMP_AI_DisableNs(IMP_VOID)

 * 禁用噪声抑制功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_DisableNs(IMP_VOID);

/**
 * @fn IMP_S32 IMP_AI_EnableAgc(IMP_AUDIO_IOATTR_S *pstAttr, IMP_AUDIO_AGC_CONFIG_S stAgcConfig)

 * 启用音频输入的自动增益功能.

 * @param[in] attr 需要进行自动增益的音频属性.
 * @param[in] agcConfig 自动增益的参数配置,配置放大倍数.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 需要注意agcConfig的配置,AGC的放大倍数主要有该参数配置,具体增益见IMP_AUDIO_AGC_CONFIG_S说明.
 * @remarks 需要注意的是,AGC可以将声音的增益放大,但是如果增益的参数不合适,就会导致破音等情况,请在具体使用时自行调整.

 * @attention 回声消除包含了 AGC 功能,如果启用回声抵消,就不需要做自动增益.
 */
IMP_S32 IMP_AI_EnableAgc(IMP_AUDIO_IOATTR_S *pstAttr, IMP_AUDIO_AGC_CONFIG_S stAgcConfig);

/**
 * @fn IMP_S32 IMP_AI_DisableAgc(IMP_VOID)

 * 禁用AI自动增益功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_DisableAgc(IMP_VOID);

/**
 * @fn IMP_S32 IMP_AO_EnableAgc(IMP_AUDIO_IOATTR_S *pstAttr, IMP_AUDIO_AGC_CONFIG_S stAgcConfig)

 * 启用音频输出的自动增益功能.

 * @param[in] attr 需要进行自动增益的音频属性.
 * @param[in] agcConfig 自动增益的参数配置,配置放大倍数.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 需要注意agcConfig的配置,AGC的放大倍数主要有该参数配置,具体增益见IMP_AUDIO_AGC_CONFIG_S说明.
 * @remarks 需要注意的是,AGC可以将声音的增益放大,但是如果增益的参数不合适,就会导致破音等情况,请在具体使用时自行调整.

 * @attention 回声消除包含了 AGC 功能,如果启用回声抵消,就不需要做自动增益.
 */
IMP_S32 IMP_AO_EnableAgc(IMP_AUDIO_IOATTR_S *pstAttr, IMP_AUDIO_AGC_CONFIG_S stAgcConfig);

/**
 * @fn IMP_S32 IMP_AO_DisableAgc(IMP_VOID)

 * 禁用AO自动增益功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_DisableAgc(IMP_VOID);

/**
 * @fn IMP_S32 IMP_AO_EnableNs(IMP_AUDIO_IOATTR_S *pstAttr, IMP_S32 s32Mode)

 * 启用指定音频输出的噪声抑制功能.

 * @param[in] attr 需要进行噪声抑制的音频属性.
 * @param[in] mode 噪声抑制的级别0~3,参见 Level_ns.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 噪声抑制的mode参数表示噪声抑制的级别,范围为[0 ~ 3],级别越高,噪声抑制的越干净.
 * @remarks 然而,噪声抑制的越干净同时也就会丢失更多的声音细节,所以这里有一个矛盾点,需要在
 * @remarks 使用的时候进行权衡.

 * @attention 回声消除包含了噪声抑制功能,如果启用回声抵消,就不需要做噪声抑制.
 */
IMP_S32 IMP_AO_EnableNs(IMP_AUDIO_IOATTR_S *pstAttr, IMP_S32 s32Mode);

/**
 * @fn IMP_S32 IMP_AO_DisableNs(IMP_VOID)

 * 禁用输出噪声抑制功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_DisableNs(IMP_VOID);

/**
 * @fn IMP_S32 IMP_HDMI_AO_EnableAgc(IMP_AUDIO_IOATTR_S *pstAttr, IMP_AUDIO_AGC_CONFIG_S stAgcConfig)

 * 启用音频输出的自动增益功能.

 * @param[in] attr 需要进行自动增益的音频属性.
 * @param[in] agcConfig 自动增益的参数配置,配置放大倍数.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 需要注意agcConfig的配置,AGC的放大倍数主要有该参数配置,具体增益见IMP_AUDIO_AGC_CONFIG_S说明.
 * @remarks 需要注意的是,AGC可以将声音的增益放大,但是如果增益的参数不合适,就会导致破音等情况,请在具体使用时自行调整.

 * @attention 回声消除包含了 AGC 功能,如果启用回声抵消,就不需要做自动增益.
 */
IMP_S32 IMP_HDMI_AO_EnableAgc(IMP_AUDIO_IOATTR_S *pstAttr, IMP_AUDIO_AGC_CONFIG_S stAgcConfig);

/**
 * @fn IMP_S32 IMP_HDMI_AO_DisableAgc(IMP_VOID)

 * 禁用AO自动增益功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_DisableAgc(IMP_VOID);

/**
 * @fn IMP_S32 IMP_HDMI_AO_EnableNs(IMP_AUDIO_IOATTR_S *pstAttr, IMP_S32 s32Mode)

 * 启用指定音频输出的噪声抑制功能.

 * @param[in] attr 需要进行噪声抑制的音频属性.
 * @param[in] mode 噪声抑制的级别0~3,参见 Level_ns.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 噪声抑制的mode参数表示噪声抑制的级别,范围为[0 ~ 3],级别越高,噪声抑制的越干净.
 * @remarks 然而,噪声抑制的越干净同时也就会丢失更多的声音细节,所以这里有一个矛盾点,需要在
 * @remarks 使用的时候进行权衡.

 * @attention 回声消除包含了噪声抑制功能,如果启用回声抵消,就不需要做噪声抑制.
 */
IMP_S32 IMP_HDMI_AO_EnableNs(IMP_AUDIO_IOATTR_S *pstAttr, IMP_S32 s32Mode);

/**
 * @fn IMP_S32 IMP_HDMI_AO_DisableNs(IMP_VOID)

 * 禁用输出噪声抑制功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_DisableNs(IMP_VOID);

/**
 * @fn IMP_S32 IMP_AI_EnableHpf(IMP_AUDIO_IOATTR_S *pstAttr)

 * 启用音频输入的高通滤波.

 * @param[in] attr 需要进行高通滤波的音频属性.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 回声消除包含了 HPF 功能,如果启用回声抵消,就不需要做 HPF.
 */
IMP_S32 IMP_AI_EnableHpf(IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_AI_SetHpfCoFrequency(IMP_S32 s32Cofrequency)

 * 设置音频输入的高通滤波器的截止频率.

 * @param[in] cofrequency 高通滤波的截止频率

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 开启高通滤波器之前先设置截至频率
 */
IMP_S32 IMP_AI_SetHpfCoFrequency(IMP_S32 s32Cofrequency);

/**
 * @fn IMP_S32 IMP_AI_DisableHpf(IMP_VOID)

 * 禁用AI高通滤波功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_DisableHpf(IMP_VOID);

/**
 * @fn IMP_S32 IMP_AO_EnableHpf(IMP_AUDIO_IOATTR_S *pstAttr)

 * 启用音频输出的高通滤波.

 * @param[in] attr 需要进行高通滤波的音频属性.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 回声消除包含了 HPF 功能,如果启用回声抵消,就不需要做 HPF.
 */
IMP_S32 IMP_AO_EnableHpf(IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_AO_SetHpfCoFrequency(IMP_S32 s32Cofrequency)

 * 设置音频输出的高通滤波器的截止频率.

 * @param[in] cofrequency 高通滤波的截止频率

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 开启高通滤波器之前先设置截止频率.
 */
IMP_S32 IMP_AO_SetHpfCoFrequency(IMP_S32 s32Cofrequency);

/**
 * @fn IMP_S32 IMP_AO_DisableHpf(IMP_VOID)

 * 禁用AO高通滤波功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_DisableHpf(IMP_VOID);

/**
 * @fn IMP_S32 IMP_HDMI_AO_EnableHpf(IMP_AUDIO_IOATTR_S *pstAttr)

 * 启用音频输出的高通滤波.

 * @param[in] attr 需要进行高通滤波的音频属性.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 回声消除包含了 HPF 功能,如果启用回声抵消,就不需要做 HPF.
 */
IMP_S32 IMP_HDMI_AO_EnableHpf(IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_HDMI_AO_SetHpfCoFrequency(IMP_S32 s32Cofrequency)

 * 设置音频输出的高通滤波器的截止频率.

 * @param[in] cofrequency 高通滤波的截止频率

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 开启高通滤波器之前先设置截止频率.
 */
IMP_S32 IMP_HDMI_AO_SetHpfCoFrequency(IMP_S32 s32Cofrequency);

/**
 * @fn IMP_S32 IMP_HDMI_AO_DisableHpf(IMP_VOID)

 * 禁用AO高通滤波功能.

 * @param 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_DisableHpf(IMP_VOID);

/**
 * @fn IMP_S32 IMP_AO_SetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr)

 * 设置音频输入输出设备属性.

 * @param[in] s32DevId 音频设备号.
 * @param[in] pstAttr  音频输出设备属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_SetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_HDMI_AO_SetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr)

 * 设置HDMI音频输入输出设备属性.

 * @param[in] s32DevId 音频设备号.
 * @param[in] pstAttr  音频输出设备属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_SetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_AO_GetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

 * 获取音频输入输出设备属性.

 * @param[in]  s32DevId 音频设备号.
 * @param[out] pstAttr  音频输出设备属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_GetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_HDMI_AO_GetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

 * 获取HDMI音频输入输出设备属性.

 * @param[in]  s32DevId 音频设备号.
 * @param[out] pstAttr  音频输出设备属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_GetPubAttr(IMP_S32 s32DevId, IMP_AUDIO_IOATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_AO_Enable(IMP_S32 s32DevId)

 * 启用音频输出设备.

 * @param[in] s32DevId 音频设备号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 在使能之前必须先调用IMP_AO_SetPubAttr.
 */
IMP_S32 IMP_AO_Enable(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_HDMI_AO_Enable(IMP_S32 s32DevId)

 * 启用HDMI音频输出设备.

 * @param[in] s32DevId 音频设备号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 在使能之前必须先调用IMP_HDMI_AO_SetPubAttr.
 */
IMP_S32 IMP_HDMI_AO_Enable(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_AO_Disable(IMP_S32 s32DevId)

 * 禁用音频输出设备.

 * @param[in] s32DevId 音频设备号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_Disable(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_HDMI_AO_Disable(IMP_S32 s32DevId)

 * 禁用HDMI音频输出设备.

 * @param[in] s32DevId 音频设备号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_Disable(IMP_S32 s32DevId);

/**
 * @fn IMP_S32 IMP_AO_EnableChn(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 启用音频输出通道.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_EnableChn(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_HDMI_AO_EnableChn(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 启用HDMI音频输出通道.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_EnableChn(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AO_DisableChn(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 禁用音频输出通道.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_DisableChn(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_HDMI_AO_DisableChn(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 禁用HDMI音频输出通道.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_DisableChn(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AO_SendFrame(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock)

 * 发送音频输出帧.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] pstFrm   音频帧结构体指针.
 * @param[in] enBlock  阻塞/非阻塞

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * while(1) {
 *		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
 *		if(size < IMP_AUDIO_BUF_SIZE)
 *			break;

 *		IMP_AUDIO_FRAME_S frm;
 *		frm.virAddr = (IMP_U32 *)buf;
 *		frm.len = size;
 *		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "send Frame Data error\n");
 *			return ret;
 *		}
 * }
 * @endcode

 * @attention 无.
 */
IMP_S32 IMP_AO_SendFrame(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock);

/**
 * @fn IMP_S32 IMP_AO_SendFrame_EXT(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock)

 * 向AO通道发送非标准字节流音频帧.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] pstFrm   音频帧结构体指针.
 * @param[in] enBlock  阻塞/非阻塞

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * while(1) {
 *		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
 *		if(size < IMP_AUDIO_BUF_SIZE)
 *			break;

 *		IMP_AUDIO_FRAME_S frm;
 *		frm.virAddr = (IMP_U32 *)buf;
 *		frm.len = size;
 *		ret = IMP_AO_SendFrame_EXT(devID, chnID, &frm, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "send Frame Data error\n");
 *			return ret;
 *		}
 * }
 * s32Ret = IMP_AO_ClearChnBuf(s32DevID, s32ChnID);
 * if (s32Ret != IMP_SUCCESS) {
 * 		IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error: %x\n",s32Ret);
 *		return NULL;
 * }
 * @endcode

 * @attention 该接口可兼容播放一帧非40ms的音频流,相比于IMP_AO_SendFrame，优点是增加了播放数据的灵活性；\n
 * 缺点是额外消耗一标准帧缓冲区大小的内存.需注意在结束播放音频后调用IMP_AO_ClearChnBuf,避免遗失数据.
 */
IMP_S32 IMP_AO_SendFrame_EXT(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock);

/**
 * @fn IMP_S32 IMP_S32 IMP_HDMI_AO_SendFrame(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock)

 * 发送HDMI音频输出帧.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] pstFrm   音频帧结构体指针.
 * @param[in] enBlock  阻塞/非阻塞

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * while(1) {
 *		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
 *		if(size < IMP_AUDIO_BUF_SIZE)
 *			break;

 *		IMP_AUDIO_FRAME_S frm;
 *		frm.virAddr = (IMP_U32 *)buf;
 *		frm.len = size;
 *		ret = IMP_HDMI_AO_SendFrame(devID, chnID, &frm, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "send Frame Data error\n");
 *			return ret;
 *		}
 * }
 * @endcode

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_SendFrame(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock);

/**
 * @fn IMP_S32 IMP_S32 IMP_HDMI_AO_SendFrame_EXT(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock)

 * 向HDMI AO通道发送非标准字节流音频帧.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] pstFrm   音频帧结构体指针.
 * @param[in] enBlock  阻塞/非阻塞

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * while(1) {
 *		size = fread(buf, 1, IMP_AUDIO_BUF_SIZE, play_file);
 *		if(size < IMP_AUDIO_BUF_SIZE)
 *			break;

 *		IMP_AUDIO_FRAME_S frm;
 *		frm.virAddr = (IMP_U32 *)buf;
 *		frm.len = size;
 *		ret = IMP_HDMI_AO_SendFrame_EXT(devID, chnID, &frm, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "send Frame Data error\n");
 *			return ret;
 *		}
 * }
 * s32Ret = IMP_HDMI_AO_ClearChnBuf(s32DevID, s32ChnID);
 * if (s32Ret != IMP_SUCCESS) {
 * 		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_ClearChnBuf error: %x\n",s32Ret);
 *		return NULL;
 * }
 * @endcode

 * @attention 该接口可兼容播放一帧非40ms的音频流,相比于IMP_HDMI_AO_SendFrame，优点是增加了播放数据的灵活性；\n
 * 缺点是额外消耗一标准帧缓冲区大小的内存.需注意在结束播放音频后调用IMP_AO_ClearChnBuf,避免遗失数据.
 */
IMP_S32 IMP_HDMI_AO_SendFrame_EXT(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_BLOCK_E enBlock);

/**
 * @fn IMP_S32 IMP_AO_ClearChnBuf(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 清除音频输出通道中当前的音频数据缓存.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_ClearChnBuf(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_HDMI_AO_ClearChnBuf(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 清除HDMI音频输出通道中当前的音频数据缓存.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_ClearChnBuf(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AENC_CreateChn(IMP_S32 s32AeChn, IMP_AUDIO_ENC_CHN_ATTR_S *pstAttr)

 * 创建音频编码通道.

 * @param[in] s32AeChn 通道号.
 * @param[in] pstAttr  音频编码通道属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_S32 AeChn = 0;
 * IMP_AUDIO_ENC_CHN_ATTR_S attr;
 * attr.type = PT_G711A;
 * attr.bufSize = 20;
 * ret = IMP_AENC_CreateChn(AeChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio encode create channel failed\n");
 *		return ret;
 * }
 * @endcode

 * @attention 目前SDK支持PT_G711A、PT_G711U和PT_G726编码. \n
 * 所以使用SDK中的编码,只需要attr.type = PT_G711A即可. \n
 * 如何需要使用自定义的编码器,则需要注册编码器,示例代码在注册接口中说明.
 */
IMP_S32 IMP_AENC_CreateChn(IMP_S32 s32AeChn, IMP_AUDIO_ENC_CHN_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_AENC_DestroyChn(IMP_S32 s32AeChn)

 * 销毁音频编码通道.

 * @param[in] s32AeChn 通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 与IMP_AENC_CreateChn配套使用.
 */
IMP_S32 IMP_AENC_DestroyChn(IMP_S32 s32AeChn);

/**
 * @fn IMP_S32 IMP_AENC_SendFrame(IMP_S32 s32AeChn, IMP_AUDIO_FRAME_S *pstFrm)

 * 发送音频编码音频帧.

 * @param[in] s32AeChn 通道号.
 * @param[in] pstFrm   音频帧结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * while(1) {
 *		// 读取一帧数据
 *		ret = fread(buf_pcm, 1, IMP_AUDIO_BUF_SIZE, file_pcm);
 *		if(ret < IMP_AUDIO_BUF_SIZE)
 *			break;

 *		// 编码
 *		IMP_AUDIO_FRAME_S frm;
 *		frm.virAddr = (uIMP_S3232_t *)buf_pcm;
 *		frm.len = ret;
 *		ret = IMP_AENC_SendFrame(AeChn, &frm);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
 *			return ret;
 *		}

 *		// 获取编码码流
 *		IMP_AUDIO_STREAM_S stream;
 *		ret = IMP_AENC_GetStream(AeChn, &stream, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode get stream failed\n");
 *			return ret;
 *		}

 *		// 使用编码码流
 *		fwrite(stream.stream, 1, stream.len, file_g711);

 *		// 释放编码码流
 *		ret = IMP_AENC_ReleaseStream(AeChn, &stream);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode release stream failed\n");
 *			return ret;
 *		}
 * }
 * @endcode

 * @attention 无.
 */
IMP_S32 IMP_AENC_SendFrame(IMP_S32 s32AeChn, IMP_AUDIO_FRAME_S *pstFrm);

/**
 * @fn IMP_S32 IMP_AENC_PollingStream(IMP_S32 s32AeChn, IMP_U32 u32TimeMs)

 * Polling编码音频流缓存.

 * @param[in] s32AeChn  音频编码输入通道号.
 * @param[in] u32TimeMs Polling超时时间.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 在使用IMP_AENC_GetStream之前使用该接口，当该接口调用成功之后表示音频
 *			  编码数据已经准备完毕，可以使用IMP_AENC_GetStream获取编码完毕的数据.
 */
IMP_S32 IMP_AENC_PollingStream(IMP_S32 s32AeChn, IMP_U32 u32TimeMs);

/**
 * @fn IMP_S32 IMP_AENC_GetStream(IMP_S32 s32AeChn, IMP_AUDIO_STREAM_S *pstStream, IMP_BLOCK_E eBlock)

 * 获取编码后码流.

 * @param[in] s32AeChn  通道号.
 * @param[in] pstStream 获取音频码流指针.
 * @param[in] eBlock    阻塞/非阻塞标识.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码见IMP_AENC_SendFrame函数说明.

 * @attention 无.
 */
IMP_S32 IMP_AENC_GetStream(IMP_S32 s32AeChn, IMP_AUDIO_STREAM_S *pstStream, IMP_BLOCK_E eBlock);

/**
 * @fn IMP_S32 IMP_AENC_ReleaseStream(IMP_S32 s32AeChn, IMP_AUDIO_STREAM_S *pstStream)

 * 释放从音频编码通道获取的码流.

 * @param[in] s32AeChn  通道号.
 * @param[in] pstStream 获取音频码流指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码见IMP_AENC_SendFrame函数说明.

 * @attention 无.
 */
IMP_S32 IMP_AENC_ReleaseStream(IMP_S32 s32AeChn, IMP_AUDIO_STREAM_S *pstStream);

/**
 * @fn IMP_S32 IMP_AENC_RegisterEncoder(IMP_S32 *ps32Handle, IMP_AUDIO_ENCODER_S *pstEncoder)

 * 注册编码器.

 * @param[in] ps32Handle 注册句柄指针.
 * @param[in] pstEncoder 编码器属性结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_S32 handle_g711a = 0;
 * IMP_AUDIO_ENCODER_S my_encoder;
 * my_encoder.maxFrmLen = 1024;
 * sprIMP_S32f(my_encoder.name, "%s", "MY_G711A");
 * my_encoder.openEncoder = NULL; // 编码器回调函数
 * my_encoder.encoderFrm = MY_G711A_Encode_Frm; // 编码器回调函数
 * my_encoder.closeEncoder = NULL; // 编码器回调函数

 * ret = IMP_AENC_RegisterEncoder(&handle_g711a, &my_encoder);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "IMP_AENC_RegisterEncoder failed\n");
 *		return ret;
 * }

 * // 使用编码器
 * IMP_S32 AeChn = 0;
 * IMP_AUDIO_ENC_CHN_ATTR_S attr;
 * attr.type = handle_g711a; // 编码器type等于注册成功返回的handle_g711a的值即可.
 * attr.bufSize = 20;
 * ret = IMP_AENC_CreateChn(AeChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "imp audio encode create channel failed\n");
 *		return ret;
 * }
 * @endcode

 * @attention 注册之后使用方法和使用SDK自带编码器一样.
 */
IMP_S32 IMP_AENC_RegisterEncoder(IMP_S32 *ps32Handle, IMP_AUDIO_ENCODER_S *pstEncoder);

/**
 * @fn IMP_S32 IMP_AENC_UnRegisterEncoder(IMP_S32 *ps32Handle)

 * 注销编码器.

 * @param[in] ps32handle 注册句柄指针(注册编码器时获得的句柄).

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AENC_UnRegisterEncoder(IMP_S32 *ps32Handle);

/**
 * @fn IMP_S32 IMP_ADEC_CreateChn(IMP_S32 s32AdChn, IMP_AUDIO_DEC_CHN_ATTR_S *pstAttr)

 * 创建音频解码通道.

 * @param[in] s32AdChn 通道号.
 * @param[in] pstAttr  通道属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_S32 adChn = 0;
 * IMP_AUDIO_DEC_CHN_ATTR_S attr;
 * attr.type = PT_G711A;
 * attr.bufSize = 20;
 * attr.mode = ADEC_MODE_PACK;
 * ret = IMP_ADEC_CreateChn(adChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
 *		return ret;
 * }
 * @endcode

 * @attention 无.
 */
IMP_S32 IMP_ADEC_CreateChn(IMP_S32 s32AdChn, IMP_AUDIO_DEC_CHN_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_ADEC_DestroyChn(IMP_S32 s32AdChn)

 * 销毁音频解码通道.

 * @param[in] s32AdChn 通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_ADEC_DestroyChn(IMP_S32 s32AdChn);

/**
 * @fn IMP_S32 IMP_ADEC_SendStream(IMP_S32 s32AdChn, IMP_AUDIO_STREAM_S *pstStream, IMP_BLOCK_E eBlock)

 * 发送音频码流到音频解码通道.

 * @param[in] s32AdChn  通道号.
 * @param[in] pstStream 音频码流指针.
 * @param[in] eBlock    阻塞/非阻塞标识.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * while(1) {
 *		// 获取需要解码的数据
 *		ret = fread(buf_g711, 1, IMP_AUDIO_BUF_SIZE/2, file_g711);
 *		if(ret < IMP_AUDIO_BUF_SIZE/2)
 *			break;

 *		// 发送解码数据
 *		IMP_AUDIO_STREAM_S stream_in;
 *		stream_in.stream = (uIMP_S328_t *)buf_g711;
 *		stream_in.len = ret;
 *		ret = IMP_ADEC_SendStream(adChn, &stream_in, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
 *			return ret;
 *		}

 *		// 获取解码后的数据
 *		IMP_AUDIO_STREAM_S stream_out;
 *		ret = IMP_ADEC_GetStream(adChn, &stream_out, BLOCK);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio decoder get stream failed\n");
 *			return ret;
 *		}

 *		// 使用解码后的数据
 *		fwrite(stream_out.stream, 1, stream_out.len, file_pcm);

 *		// 释放解码后的数据
 *		ret = IMP_ADEC_ReleaseStream(adChn, &stream_out);
 *		if(ret != 0) {
 *			IMP_LOG_ERR(TAG, "imp audio decoder release stream failed\n");
 *			return ret;
 *		}
 * }
 * @endcode

 * @attention 无.
 */
IMP_S32 IMP_ADEC_SendStream(IMP_S32 s32AdChn, IMP_AUDIO_STREAM_S *pstStream, IMP_BLOCK_E eBlock);

/**
 * @fn IMP_S32 IMP_ADEC_PollingStream(IMP_S32 s32AdChn, IMP_U32 u32TimeMs)

 * Polling解码音频流缓存.

 * @param[in] s32AdChn  音频解码输入通道号.
 * @param[in] u32TimeMs Polling超时时间.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 在使用IMP_ADEC_GetStream之前使用该接口，当该接口调用成功之后表示音频
 *			  解码数据已经准备完毕，可以使用IMP_ADEC_GetStream获取解码完毕的数据.
 */
IMP_S32 IMP_ADEC_PollingStream(IMP_S32 s32AdChn, IMP_U32 u32TimeMs);

/**
 * @fn IMP_S32 IMP_ADEC_GetStream(IMP_S32 s32AdChn, IMP_AUDIO_STREAM_S *pstStream, IMP_BLOCK_E eBlock)

 * 获取解码后码流.

 * @param[in] s32AdChn  通道号.
 * @param[in] pstStream 获取解码码流指针.
 * @param[in] eBlock    阻塞/非阻塞标识.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码见IMP_ADEC_SendStream函数说明.

 * @attention 无.
 */
IMP_S32 IMP_ADEC_GetStream(IMP_S32 s32AdChn, IMP_AUDIO_STREAM_S *pstStream, IMP_BLOCK_E eBlock);

/**
 * @fn IMP_S32 IMP_ADEC_ReleaseStream(IMP_S32 s32AdChn, IMP_AUDIO_STREAM_S *pstStream)
 * 释放从音频解码通道获取的码流.

 * @param[in] s32AdChn  通道号.
 * @param[in] pstStream 音频码流指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码见IMP_ADEC_SendStream函数说明.

 * @attention 无.
 */
IMP_S32 IMP_ADEC_ReleaseStream(IMP_S32 s32AdChn, IMP_AUDIO_STREAM_S *pstStream);

/**
 * @fn IMP_S32 IMP_ADEC_ClearChnBuf(IMP_S32 s32AdChn)

 * 清除音频解码通道中当前的音频数据缓存.

 * @param[in] s32AdChn 通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_ADEC_ClearChnBuf(IMP_S32 s32AdChn);

/**
 * @fn IMP_S32 IMP_ADEC_RegisterDecoder(IMP_S32 *ps32Handle, IMP_AUDIO_DECODER_S *pstDecoder)

 * 注册解码器.

 * @param[in] ps32handle 注册句柄指针.
 * @param[in] pstDecoder 解码器属性结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_S32 handle_g711a = 0;
 * IMP_AUDIO_DECODER_S my_decoder;
 * sprIMP_S32f(my_decoder.name, "%s", "MY_G711A");
 * my_decoder.openDecoder = NULL; // 解码器回调函数
 * my_decoder.decodeFrm = MY_G711A_Decode_Frm; // 解码器回调函数
 * my_decoder.getFrmInfo = NULL; // 解码器回调函数
 * my_decoder.closeDecoder = NULL; // 解码器回调函数

 * // 注册解码器
 * ret = IMP_ADEC_RegisterDecoder(&handle_g711a, &my_decoder);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "IMP_ADEC_RegisterDecoder failed\n");
 *		return ret;
 * }

 * // 使用解码器
 * IMP_S32 adChn = 0;
 * IMP_AUDIO_DEC_CHN_ATTR_S attr;
 * attr.type = handle_g711a; // 解码type等于解码器注册返回的handle_g711a.
 * attr.bufSize = 20;
 * attr.mode = ADEC_MODE_PACK;
 * // 创建解码通道
 * ret = IMP_ADEC_CreateChn(adChn, &attr);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
 *		return ret;
 * }
 * @endcode

 * @attention 注册之后使用方法和使用SDK自带解码器一样.
 */
IMP_S32 IMP_ADEC_RegisterDecoder(IMP_S32 *ps32Handle, IMP_AUDIO_DECODER_S *pstDecoder);

/**
 * @fn IMP_S32 IMP_ADEC_UnRegisterDecoder(IMP_S32 *ps32Handle)

 * 注销解码器.

 * @param[in] ps32handle 注册句柄(注册解码器时获得的句柄).

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_ADEC_UnRegisterDecoder(IMP_S32 *ps32Handle);

/**
 * ACODEC配置.
 */
/**
 * @fn IMP_S32 IMP_AI_SetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb)

 * 设置音频输入音量.

 * @param[in] aiDevId 音频输入设备号.
 * @param[in] s32Chn  音频输入通道号.
 * @param[in] fVolDb  音频输入音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 音量的取值范围为[-45.0 ~ 30]. -45代表静音,声音最大可以放大30dB,步长0.5dB,必须是0.5的整数倍
 * @remarks 其中0是音量设置的一个临界点，在这个值上软件不对音量做增加或减小

 * 示例代码
 * @code
 * IMP_S32 fVolDb = 10.0;
 * ret = IMP_AI_SetVolDb(devID, chnID, &fVolDb);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Record set fVolDb failed\n");
 *		return ret;
 * }
 * @endcode
 * @attention 如果输入的fVolDb超过了[-45.0 ~ 30]的范围,小于-45的将会取-45,大于30的取30db.
 */
IMP_S32 IMP_AI_SetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb);

/**
 * @fn IMP_S32 IMP_AI_SetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Vol)

 * 设置音频输入音量.

 * @param[in] s32DevId 音频输入设备号.
 * @param[in] s32Chn   音频输入通道号.
 * @param[in] s32Vol   音频输入音量大小.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.
 * @remarks 其中60是音量设置的一个临界点，在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB。

 * 示例代码
 * @code
 * int volume = 60;
 * ret = IMP_AI_SetVol(devID, chnID, volume);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
 *		return ret;
 * }
 * @endcode
 * @attention 如果输入的aiVol超过了[-30 ~ 120]的范围,小于-30的将会取-30,大于120的取120.
 */
IMP_S32 IMP_AI_SetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Vol);

/**
 * @fn IMP_S32 IMP_AI_GetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT* pfVolDb)

 * 获取音频输入音量.

 * @param[in]  s32DevId 音频输入设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] pfVolDb  音频输入通道音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_GetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT* pfVolDb);

/**
 * @fn IMP_S32 IMP_AI_GetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Vol)

 * 获取音频输入音量.

 * @param[in]  s32DevId 音频输入设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] ps32Vol  音频输入通道音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_GetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Vol);

/**
 * @fn IMP_S32 IMP_AI_SetVolMute(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Mute)

 * 设置音频输入静音.

 * @param[in]  aiDevId 音频输入设备号.
 * @param[in]  s32Chn  音频输入通道号.
 * @param[out] s32Mute 音频输入静音标志, s32Mute = 0:关闭静音, s32Mute = 1:打开静音.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 调用该接口可立刻静音.

 * @attention 无.
 */
IMP_S32 IMP_AI_SetVolMute(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Mute);

/**
 * @fn IMP_S32 IMP_AO_SetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb);

 * 设置音频输出通道音量.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] fVolDb   音频输出音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 音量的取值范围为[-45.0 ~ 30]. -45代表静音,声音最大可以放大30dB,步长0.5dB,必须是0.5的整数倍
 * @remarks 其中0是音量设置的一个临界点，在这个值上软件不对音量做增加或减小
 */
IMP_S32 IMP_AO_SetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb);

/**
 * @fn IMP_IMP_HDMI_AO_SetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb);

 * 设置HDMI音频输出通道音量.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] fVolDb   音频输出音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 音量的取值范围为[-45.0 ~ 30]. -45代表静音,声音最大可以放大30dB,步长0.5dB,必须是0.5的整数倍
 * @remarks 其中0是音量设置的一个临界点，在这个值上软件不对音量做增加或减小
 */
IMP_S32 IMP_HDMI_AO_SetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb);

/**
 * @fn IMP_S32 IMP_AO_SetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Vol)

 * 设置音频输出通道音量.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] s32Vol   音频输出音量.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.
 * @remarks 其中60是音量设置的一个临界点，在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB。

 * @attention 如果输入的aoVol超过了[-30 ~ 120]的范围,小于-30的将会取-30,大于120的取120.
 */
IMP_S32 IMP_AO_SetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Vol);

/**
 * @fn IMP_S32 IMP_HDMI_AO_SetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Vol)

 * 设置HDMI音频输出通道音量.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输出通道号.
 * @param[in] s32Vol   音频输出音量.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 音量的取值范围为[-30 ~ 120]. -30代表静音,120表示将声音放大30dB,步长0.5dB.
 * @remarks 其中60是音量设置的一个临界点，在这个值上软件不对音量做增加或减小，当音量值小于60时，每下降1，音量减小0.5dB；当音量值大于60时，上增加1，音量增加0.5dB。

 * @attention 如果输入的aoVol超过了[-30 ~ 120]的范围,小于-30的将会取-30,大于120的取120.
 */
IMP_S32 IMP_HDMI_AO_SetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Vol);

/**
 * @fn IMP_S32 IMP_AO_GetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT* pfVolDb)

 * 获取音频输出通道音量.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] pfVolDb  音频输出音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_GetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT* pfVolDb);

/**
 * @fn IMP_S32 IMP_HDMI_AO_GetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT* pfVolDb)

 * 获取HDMI音频输出通道音量.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] pfVolDb  音频输出音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_GetVolDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT* pfVolDb);

/**
 * @fn IMP_S32 IMP_AO_GetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Vol);

 * 获取音频输出通道音量.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] ps32Vol  音频输出音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_GetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Vol);

/**
 * @fn IMP_S32 IMP_HDMI_AO_GetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Vol);

 * 获取HDMI音频输出通道音量.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] ps32Vol  音频输出音量指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_GetVol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Vol);

/**
 * @fn IMP_S32 IMP_AO_SetVolMute(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Mute)

 * 设置音频输出静音.

 * @param[in]  s32DevId 音频输出设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] s32Mute  音频输出静音标志, s32Mute = 0:关闭静音,  s32Mute = 1:打开静音.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 调用该接口可立刻静音.

 * @attention 无.
 */
IMP_S32 IMP_AO_SetVolMute(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Mute);

/**
 * @fn IMP_S32 IMP_HDMI_AO_SetVolMute(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Mute)

 * 设置HDMI音频输出静音.

 * @param[in]  s32DevId 音频输出设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] s32Mute 音频输出静音标志, s32Mute = 0:关闭静音,  s32Mute = 1:打开静音.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 调用该接口可立刻静音.

 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_SetVolMute(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Mute);

/**
 * @fn IMP_S32 IMP_AI_SetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb)

 * 设置音频输入模拟增益.

 * @param[in] s32DevId 音频输入设备号.
 * @param[in] s32Chn 音频输入通道号.
 * @param[in] fGainDb 音频通道输入增益.一定是1.5的整数倍，否则报错

 * 对应[-18dB ~ 28.5dB],步长1.5dB.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 */
IMP_S32 IMP_AI_SetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb);

/**
 * @fn IMP_S32 IMP_AI_SetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Gain)

 * 设置音频输入模拟增益.

 * @param[in] s32DevId 音频输入设备号.
 * @param[in] s32Chn   音频输入通道号.
 * @param[in] s32Gain  音频通道输入增益.

 * 范围[0 ~ 31],对应[-18dB ~ 28.5dB],步长1.5dB.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention aiGain的范围为[0 ~ 31],如果输入的值小于0,则aiGain的值将会\n
 * 被设置为0.如果值大于31,aiGain的值会被设置为31.

 */
IMP_S32 IMP_AI_SetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Gain);

/**
 * @fn IMP_S32 IMP_AI_GetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb)

 * 获取音频输入模拟增益值.

 * @param[in]  s32DevId 音频输入设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] pfGainDb 音频通道输入增益属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_GetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb);

/**
 * @fn IMP_S32 IMP_AI_GetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Gain)

 * 获取音频输入模拟增益值.

 * @param[in]  s32DevId 音频输入设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] ps32Gain 音频通道输入增益属性指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AI_GetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 *ps32Gain);

/**
 * @fn IMP_S32 IMP_AO_SetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb)

 * 设置音频输出增益.

 * @param[in]  s32DevId 音频输出设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] fGainDb  对应[-39dB ~ 6dB],步长1.5dB.
 * @retval 0 成功.
 * @retval 非0 失败.
 * @remarks 无.
 */
IMP_S32 IMP_AO_SetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb);

/**
 * @fn IMP_S32 IMP_AO_SetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Gain)

 * 设置音频输出增益.

 * @param[in]  s32DevId 音频输出设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] s32Gain  音频输出增益,范围[0 ~ 0x1f],对应[-39dB ~ 6dB],步长1.5dB.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention aoGain的范围为[0 ~ 31],如果输入的值小于0,则aoGain的值将会\n
 * 被设置为0.如果值大于31,aoGain的值会被设置为31.

 */
IMP_S32 IMP_AO_SetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32 s32Gain);

/**
 * @fn IMP_S32 IMP_AO_GetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb)

 * 获取音频输出增益.

 * @param[in]  s32DevId 音频输出设备号.
 * @param[in]  s32Chn   音频输出通道号.
 * @param[out] fGainDb  音频输出增益指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_GetGainDb(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfGainDb);

/**
 * @fn IMP_S32 IMP_AO_GetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32* ps32Gain)

 * 获取音频输出增益.

 * @param[in]  s32DevId  音频输出设备号.
 * @param[in]  s32Chn    音频输出通道号.
 * @param[out] ps32Gain  音频输出增益指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.

 * @attention 无.
 */
IMP_S32 IMP_AO_GetGain(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_S32* ps32Gain);

/**
 * @fn IMP_S32 IMP_AI_GetFrameAndRef(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_AUDIO_FRAME_S *pstRef);
 * 获取音频帧和输出参考帧.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] pstFrm   音频帧结构体指针.
 * @param[out] pstRef   参考帧结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_AUDIO_FRAME_S frm;
 * IMP_AUDIO_FRAME_S ref;
 * // 获取音频帧和输出参考帧
 * ret = IMP_AI_GetFrameAndRef(devID, chnID, &frm, &ref);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
 *		return ret;
 * }

 * fwrite(frm.virAddr, 1, frm.len, record_file); // 使用音频帧数据
 * fwrite(ref.virAddr, 1, ref.len, ref_file); // 使用音频参考帧

 * @endcode

 * @attention 无.
 */
IMP_S32 IMP_AI_GetFrameAndRef(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_AUDIO_FRAME_S *pstRef);

/**
 * @fn IMP_AI_GetFrameAndRefTimeout(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_AUDIO_FRAME_S *pstRef, IMP_S32 s32Timeout);
 * 获取音频帧和输出参考帧.

 * @param[in]  s32DevId 音频设备号.
 * @param[in]  s32Chn   音频输入通道号.
 * @param[out] pstFrm   音频帧结构体指针.
 * @param[out] pstRef   参考帧结构体指针.
 * @param[in] s32Timeout   获取音频帧阻塞等待时间，-1表示阻塞模式，0 表示非阻塞模式，>0 表示阻塞多少毫秒，超时则报错返回.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 示例代码
 * @code
 * IMP_AUDIO_FRAME_S frm;
 * IMP_AUDIO_FRAME_S ref;
 * // 获取音频帧和输出参考帧
 * ret = IMP_AI_GetFrameAndRef(devID, chnID, &frm, &ref);
 * if(ret != 0) {
 *		IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
 *		return ret;
 * }

 * fwrite(frm.virAddr, 1, frm.len, record_file); // 使用音频帧数据
 * fwrite(ref.virAddr, 1, ref.len, ref_file); // 使用音频参考帧

 * @endcode

 * @attention s32Timeout 如果设置阻塞时间，一般建议设置为音频帧时间的两倍及以上.
 */
IMP_S32 IMP_AI_GetFrameAndRefTimeout(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_FRAME_S *pstFrm, IMP_AUDIO_FRAME_S *pstRef, IMP_S32 s32Timeout);

/**
 * @fn IMP_S32 IMP_AI_EnableAecRefFrame(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 打开获取参考帧.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 调用IMP_AI_GetFrameAndRef之前调用此接口.
 * @attention 无.
 */
IMP_S32 IMP_AI_EnableAecRefFrame(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AI_DisableAecRefFrame(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 关闭获取参考帧.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 无.
 */
IMP_S32 IMP_AI_DisableAecRefFrame(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AO_EnableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_SAMPLE_RATE_E enSrcSampleRate)

 * 打开音频输出重采样.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.
 * @param[in] enSrcSampleRate 输入数据的采样率.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 无.
 */
IMP_S32 IMP_AO_EnableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_SAMPLE_RATE_E enSrcSampleRate);

/**
 * @fn IMP_S32 IMP_AO_DisableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 关闭音频输出重采样.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 无.
 */
IMP_S32 IMP_AO_DisableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_AI_EnableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_SAMPLE_RATE_E enDstSampleRate)

 * 打开音频输入重采样.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.
 * @param[in] enDstSampleRate 输出数据的采样率.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 无.
 */
IMP_S32 IMP_AI_EnableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_SAMPLE_RATE_E enDstSampleRate);

/**
 * @fn IMP_S32 IMP_S32 IMP_AI_DisableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 关闭音频输出重采样.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 无.
 */
IMP_S32 IMP_AI_DisableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn);

/**
 * @fn IMP_S32 IMP_HDMI_AO_EnableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_SAMPLE_RATE_E enSrcSampleRate)

 * 打开HDMI音频输出重采样.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.
 * @param[in] enSrcSampleRate 输入数据的采样率.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_EnableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_SAMPLE_RATE_E enSrcSampleRate);

/**
 * @fn IMP_S32 IMP_HDMI_AO_DisableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn)

 * 关闭HDMI音频输出重采样.

 * @param[in] s32DevId 音频设备号.
 * @param[in] s32Chn   音频输入通道号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 无.
 */
IMP_S32 IMP_HDMI_AO_DisableReSmp(IMP_S32 s32DevId, IMP_S32 s32Chn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_AUDIO_H__ */
