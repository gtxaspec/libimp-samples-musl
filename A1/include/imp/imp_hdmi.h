/*
 * IMP HDMI header file.
 *
 * Copyright (C) 2021 Ingenic Semiconductor Co.,Ltd
 */
#ifndef __IMP_HDMI_H__
#define __IMP_HDMI_H__

#include <imp/imp_common.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**the max audio smprate count*/
#define IMP_HDMI_MAX_SAMPE_RATE_NUM		8

/**the max audio bit depth count*/
#define IMP_HDMI_MAX_BIT_DEPTH_NUM		6
/**
 *定义 HDMI 接口号
 */
typedef enum{
	IMP_HDMI_ID_0 = 0,		/**< HDMI 接口 0 */
	IMP_HDMI_ID_BUTT
}IMP_HDMI_ID_E;

/**
 *定义 HDMI Video Format,参考《CEA-861-D》&《CEA-861-F》
 */
typedef enum{
	IMP_HDMI_VIDEO_FMT_1080P_60 = 0,
	IMP_HDMI_VIDEO_FMT_1080P_50,
	IMP_HDMI_VIDEO_FMT_1080P_30,
	IMP_HDMI_VIDEO_FMT_1080P_25,
	IMP_HDMI_VIDEO_FMT_1080P_24,
	IMP_HDMI_VIDEO_FMT_1080i_60,
	IMP_HDMI_VIDEO_FMT_1080i_50,
	IMP_HDMI_VIDEO_FMT_720P_60,
	IMP_HDMI_VIDEO_FMT_720P_50,
	IMP_HDMI_VIDEO_FMT_576P_50,
	IMP_HDMI_VIDEO_FMT_480P_60,
	IMP_HDMI_VIDEO_FMT_PAL,						/**< B D G H I PAL */
	IMP_HDMI_VIDEO_FMT_PAL_N,					/**< (N)PAL */
	IMP_HDMI_VIDEO_FMT_PAL_Nc,					/**< (Nc)PAL */
	IMP_HDMI_VIDEO_FMT_NTSC,					/**< (M)NTSC */
	IMP_HDMI_VIDEO_FMT_NTSC_J,					/**< NTSC-J */
	IMP_HDMI_VIDEO_FMT_NTSC_PAL_M,				/**< (M)PAL */
	IMP_HDMI_VIDEO_FMT_SECAM_SIN,				/**< SECAM_SIN*/
	IMP_HDMI_VIDEO_FMT_SECAM_COS,				/**< SECAM_COS*/
	IMP_HDMI_VIDEO_FMT_861D_640X480_60,
	IMP_HDMI_VIDEO_FMT_VESA_800X600_60,
	IMP_HDMI_VIDEO_FMT_VESA_1024X768_60,
	IMP_HDMI_VIDEO_FMT_VESA_1280X720_60,
	IMP_HDMI_VIDEO_FMT_VESA_1280X800_60,
	IMP_HDMI_VIDEO_FMT_VESA_1280X1024_60,
	IMP_HDMI_VIDEO_FMT_VESA_1366X768_60,
	IMP_HDMI_VIDEO_FMT_VESA_1440X900_60,
	IMP_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
	IMP_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
	IMP_HDMI_VIDEO_FMT_VESA_1600X1200_60,
	IMP_HDMI_VIDEO_FMT_VESA_1680X1050_60,
	IMP_HDMI_VIDEO_FMT_VESA_1920X1080_60,
	IMP_HDMI_VIDEO_FMT_VESA_1920X1200_60,
	IMP_HDMI_VIDEO_FMT_VESA_2048X1152_60,
	IMP_HDMI_VIDEO_FMT_2560x1440_30,
	IMP_HDMI_VIDEO_FMT_2560x1440_60,
	IMP_HDMI_VIDEO_FMT_2560x1600_60,
	IMP_HDMI_VIDEO_FMT_1920x2160_30,

	IMP_HDMI_VIDEO_FMT_3840X2160P_24,
	IMP_HDMI_VIDEO_FMT_3840X2160P_25,
	IMP_HDMI_VIDEO_FMT_3840X2160P_30,
	IMP_HDMI_VIDEO_FMT_3840X2160P_50,
	IMP_HDMI_VIDEO_FMT_3840X2160P_60,

	IMP_HDMI_VIDEO_FMT_4096X2160P_24,
	IMP_HDMI_VIDEO_FMT_4096X2160P_25,
	IMP_HDMI_VIDEO_FMT_4096X2160P_30,
	IMP_HDMI_VIDEO_FMT_4096X2160P_50,
	IMP_HDMI_VIDEO_FMT_4096X2160P_60,
	IMP_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE,
	IMP_HDMI_VIDEO_FMT_BUTT,
}IMP_HDMI_VIDEO_FMT_E;

/**
 *定义 HDMI 颜色空间类型枚举
 */
typedef enum{
	IMP_HDMI_VIDEO_MODE_RGB444,			/**< RGB444 输出模式,默认输出为RGB444*/
	IMP_HDMI_VIDEO_MODE_YCBCR422,		/**< YCBCR422 输出模式*/
	IMP_HDMI_VIDEO_MODE_YCBCR444,		/**< YCBCR444 输出模式*/
	IMP_HDMI_VIDEO_MODE_YCBCR420,		/**< YCBCR420 输出模式*/
	IMP_HDMI_VIDEO_MODE_BUTT
}IMP_HDMI_VIDEO_MODE_E;

/**
 *定义 HDMI Deep Color模式枚举
 */
typedef enum{
	IMP_HDMI_DEEP_COLOR_24BIT	= 0x00,	/**< HDMI Deep Color 24bit 模式*/
	IMP_HDMI_DEEP_COLOR_30BIT,			/**< HDMI Deep Color 30bit 模式*/
	IMP_HDMI_DEEP_COLOR_36BIT,			/**< HDMI Deep Color 36bit 模式*/
	IMP_HDMI_DEEP_COLOR_BUTT
}IMP_HDMI_DEEP_COLOR_E;

/**
 *定义 CSC 输出量化范围枚举 ,默认使用HDMI_QUANTIZATION_FULL_RANGE，当前HDMI_QUANTIZATION_LIMITED_RANGE不支持
 */
typedef enum{
	HDMI_QUANTIZATION_LIMITED_RANGE,	/**< CSC输出量化限制模式*/
	HDMI_QUANTIZATION_FULL_RANGE,		/**< CSC输出量化无限制模式*/
	HDMI_QUANTIZATION_BUTT
}IMP_HDMI_QUANTIZATION_E;

/**
 *定义音频采样频率枚举.Just for HDMI Infoframe,如要设置音频属性请参考HDMI Audio相关SDK接口
 */
typedef enum{
	IMP_HDMI_SAMPLE_RATE_UNKNOW = 0,
	IMP_HDMI_SAMPLE_RATE_8K		= 8000,		/**< 音频采样率8KHz*/
	IMP_HDMI_SAMPLE_RATE_11K	= 11025,	/**< 音频采样率11KHz*/
	IMP_HDMI_SAMPLE_RATE_12K	= 12000,	/**< 音频采样率12KHz*/
	IMP_HDMI_SAMPLE_RATE_16K	= 16000,	/**< 音频采样率16KHz*/
	IMP_HDMI_SAMPLE_RATE_22K	= 22000,	/**< 音频采样率22KHz*/
	IMP_HDMI_SAMPLE_RATE_24K	= 24000,	/**< 音频采样率24KHz*/
	IMP_HDMI_SAMPLE_RATE_32K	= 32000,	/**< 音频采样率32KHz*/
	IMP_HDMI_SAMPLE_RATE_44K	= 44000,	/**< 音频采样率44KHz*/
	IMP_HDMI_SAMPLE_RATE_48K	= 48000,	/**< 音频采样率48KHz*/
	IMP_HDMI_SAMPLE_RATE_88K	= 88000,	/**< 音频采样率88KHz*/
	IMP_HDMI_SAMPLE_RATE_96K	= 96000,	/**< 音频采样率96KHz*/
	IMP_HDMI_SAMPLE_RATE_176K	= 176000,	/**< 音频采样率176KHz*/
	IMP_HDMI_SAMPLE_RATE_192K	= 192000,	/**< 音频采样率192KHz*/
	IMP_HDMI_SAMPLE_RATE_768K	= 768000,	/**< 音频采样率768KHz*/
	IMP_HDMI_SAMPLE_RATE_BUTT
}IMP_HDMI_SAMPLE_RATE_E;

/**
 *定义音频采样位宽枚举.Just for HDMI Infoframe,如要设置音频属性请参考HDMI Audio相关SDK接口
 */
typedef enum{
	IMP_HDMI_BIT_DEPTH_UNKNOWN =0,
	IMP_HDMI_BIT_DEPTH_8 = 8,			/**< 音频采样位宽8Bit*/
	IMP_HDMI_BIT_DEPTH_16 = 16,			/**< 音频采样位宽16Bit*/
	IMP_HDMI_BIT_DEPTH_18 = 18,			/**< 音频采样位宽18Bit*/
	IMP_HDMI_BIT_DEPTH_20 = 20,			/**< 音频采样位宽20Bit*/
	IMP_HDMI_BIT_DEPTH_24 = 24,			/**< 音频采样位宽24Bit*/
	IMP_HDMI_BIT_DEPTH_BUTT
}IMP_HDMI_AUDIO_BIT_WIDTH_E;

/**
 *定义 HDMI 内部 EDID 读取失败时使用的强制输出模式枚举
 */
typedef enum{
	IMP_HDMI_FORCE_NULL,		/**< 标准模式（HDMI模式）*/
	IMP_HDMI_FORCE_HDMI,		/**< HDMI模式*/
	IMP_HDMI_FORCE_DVI,			/**< DVI模式*/
	IMP_HDMI_INIT_BOOT_CONFIG
}IMP_HDMI_FORCE_ACTION_E;

/**
 *定义 HDMI 音频格式枚举.Just for HDMI Infoframe,此属性无法设置，只能从EDID中获取，仅供用户读取使用
 */
typedef enum{
	IMP_HDMI_AUDIO_FORMAT_CODE_RESERVED = 0x00,
	IMP_HDMI_AUDIO_FORMAT_CODE_PCM,				/**< PCM格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_AC3,				/**< AC3格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_MPEG1,			/**< MPEG1格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_MP3,				/**< MP3格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_MPEG2,			/**< MPEG2格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_AAC,				/**< AAC格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_DTS,				/**< DTS格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_ATRAC,			/**< ATRAC格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_ONE_BIT,			/**< ONE_BIT格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_DDP,				/**< DDP格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_DTS_HD,			/**< DTS_HD格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_MAT,				/**< MAT格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_DST,				/**< DST格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_WMA_PRO,			/**< WMA_PRO格式*/
	IMP_HDMI_AUDIO_FORMAT_CODE_BUTT,
}IMP_HDMI_AUDIO_FORMAT_CODE_E;

/**
 *定义扬声器位置，参考《CEA-861-F》 7.5.3
 */
typedef enum{
	HDMI_AUDIO_SPEAKER_FL_FR,
	HDMI_AUDIO_SPEAKER_LFE,
	HDMI_AUDIO_SPEAKER_FC,
	HDMI_AUDIO_SPEAKER_RL_RR,
	HDMI_AUDIO_SPEAKER_RC,
	HDMI_AUDIO_SPEAKER_FLC_FRC,
	HDMI_AUDIO_SPEAKER_RLC_RRC,
	HDMI_AUDIO_SPEAKER_FLW_FRW,
	HDMI_AUDIO_SPEAKER_FLH_FRH,
	HDMI_AUDIO_SPEAKER_TC,
	HDMI_AUDIO_SPEAKER_FCH,
	HDMI_AUDIO_SPEAKER_BUTT,
}IMP_HDMI_AUDIO_SPEAKER_E;

/**
 *定义 HDMI 音频能力集信息
 */
typedef struct{
	IMP_HDMI_AUDIO_FORMAT_CODE_E enAudFmtCode;										/**< 支持的音频格式*/
	IMP_HDMI_SAMPLE_RATE_E enSupportSampleRate[IMP_HDMI_MAX_SAMPE_RATE_NUM];		/**< 支持的音频采样率*/
	IMP_U8 u8AudChannel;															/**< 支持的音频通道数 */
	IMP_HDMI_AUDIO_BIT_WIDTH_E enSupportBitDepth[IMP_HDMI_MAX_BIT_DEPTH_NUM];		/**< 支持的采样 bit 深度*/
	IMP_U32 u32SupportBitDepthNum;													/**< 支持不同的 bit 深度种类个数*/
	IMP_U32 u32MaxBitRate;															/**< 最大比特率*/
}IMP_HDMI_AUDIO_INFO_S;

/**
 *定义 HDMI Sink 端能力集结构体
 */
typedef struct{
	IMP_BOOL bConnected;											/**< 设备是否连接*/
	IMP_BOOL bSupportHdmi;											/**< 设备是否支持 HDMI（默认为 HDMI1.4），如果不支持，则为 DVI 设备*/
	IMP_BOOL bIsSinkPowerOn;										/**< Sink 设备是否上电*/
	IMP_BOOL bIsRealEDID;											/**< EDID 是否是真正从 sink 设备获取标志*/
	IMP_HDMI_VIDEO_FMT_E enNativeVideoFormat;						/**< 显示设备物理分辨率*/
	IMP_BOOL bVideoFmtSupported[IMP_HDMI_VIDEO_FMT_BUTT];			/**< 视频能力集*/
	IMP_BOOL bSupportYCbCr;											/**< 是否支持 YCBCR 显示*/
	IMP_BOOL bSupportxvYCC601;										/**< 是否支持 xvYCC601 颜色格式*/
	IMP_BOOL bSupportxvYCC709;										/**< 是否支持 xvYCC709 颜色格式*/
	IMP_U8 u8MDBit;													/**< xvYCC601 支持的传输*/
	IMP_U32 u32AudioInfoNum;										/**< 支持的音频信息个数，取值范围[1~16]*/
	IMP_HDMI_AUDIO_INFO_S  stAudioInfo;								/**< 支持的音频信息*/
	IMP_BOOL bSpeaker[HDMI_AUDIO_SPEAKER_BUTT];					    /**< 扬声器位置*/
	IMP_U8 u8IDManufactureName[4];									/**< 设备厂商标识*/
	IMP_U32 u32IDProductCode;										/**< 设备 ID*/
	IMP_U32 u32IDSerialNumber;										/**< 设备序列号*/
	IMP_U32 u32WeekOfManufacture;									/**< 设备生产日期(周)*/
	IMP_U32 u32YearOfManufacture;									/**< 设备生产日期(年)*/
	IMP_U8 u8Version;												/**< 设备版本号*/
	IMP_U8 u8Revision;												/**< 设备子版本号*/
	IMP_U8 u8EDIDExternBlockNum;									/**< EDID 扩展块数目*/
	IMP_BOOL bIsPhyAddrValid;										/**< CEC 物理地址是否有效标志*/
	IMP_U8 u8PhyAddr_A;												/**< CEC 物理地址 A*/
	IMP_U8 u8PhyAddr_B;												/**< CEC 物理地址 B*/
	IMP_U8 u8PhyAddr_C;												/**< CEC 物理地址 C*/
	IMP_U8 u8PhyAddr_D;												/**< CEC 物理地址 D*/
	IMP_BOOL bSupportDVIDual;										/**< 是否支持 DVI dual-link 操作*/
	IMP_BOOL bSupportDeepColorYCBCR444;								/**< 是否支持 YCBCR 4:4:4 Deep Color 模式*/
	IMP_BOOL bSupportDeepColor30Bit;								/**< 是否支持 Deep Color 30bit 模式*/
	IMP_BOOL bSupportDeepColor36Bit;								/**< 是否支持 Deep Color 36bit 模式*/
	IMP_BOOL bSupportDeepColor48Bit;								/**< 是否支持 Deep Color 48bit 模式*/
	IMP_BOOL bSupportAI;											/**< 是否支持 Supports_AI 模式*/
	IMP_U32 u32MaxTMDSClock;										/**< 最大 TMDS 时钟*/
	IMP_BOOL bI_Latency_Fields_Present;								/**< 延时标志位*/
	IMP_BOOL bLatency_Fields_Present;								/**< Video_Latency 和 Audio_Latency fields 是否存在*/
	IMP_BOOL bHDMI_Video_Present;									/**< 特殊的视频格式*/
	IMP_U8 u8Video_Latency;											/**< 视频延时*/
	IMP_U8 u8Audio_Latency;											/**< 音频延时*/
	IMP_U8 u8Interlaced_Video_Latency;								/**< 隔行视频模式下的视频延时*/
	IMP_U8 u8Interlaced_Audio_Latency;								/**< 隔行音频模式下的音频延时*/
	IMP_BOOL bSupportY420DC30Bit;									/**< 是否支持 YCbCr420 的 deep-color 30bit 模式*/
	IMP_BOOL bSupportY420DC36Bit;									/**< 是否支持 YCbCr420 的 deep-color 36bit 模式*/
	IMP_BOOL bSupportY420DC48Bit;									/**< 是否支持 YCbCr420 的 deep-color 48bit 模式*/
	IMP_BOOL bSupportHdmi_2_0;										/**< 是否支持 hdmi2.0*/
	IMP_BOOL bSupportY420Format[IMP_HDMI_VIDEO_FMT_BUTT];			/**< 支持 YCbCr420 的视频制式*/
	IMP_BOOL bOnlySupportY420Format[IMP_HDMI_VIDEO_FMT_BUTT];		/**< 只支持 YCbCr420 的视频制式*/
}IMP_HDMI_SINK_CAPABILITY_S;

/**
 *定义 HDMI 属性结构体
 */
typedef struct{
	IMP_BOOL					bEnableHdmi;				/**< 是否 输出HDMI*/

	IMP_BOOL					bEnableVideo;				/**< 是否 使能视频*/
	IMP_HDMI_VIDEO_FMT_E		enVideoFmt;					/**< 视频制式，此参数需要与 VO 配置的制式保持一致*/

	IMP_HDMI_VIDEO_MODE_E		enVideoMode;				/**< HDMI 输出视频模式*/
	IMP_HDMI_DEEP_COLOR_E		enDeepColor;				/**< DeepColor 输出模式*/
	IMP_HDMI_QUANTIZATION_E		enOutQuantization;			/**< HDMI 输出视频量化范围*/
	IMP_BOOL					bxvYCCMode;					/**< 是否使能 xvYCC 输出模式*/

	IMP_BOOL					bEnableAudio;				/**< 是否使能音频*/

	IMP_BOOL					bEnableAviInfoframe;		/**< 是否使能 AVI InfoFrame*/
	IMP_BOOL					bEnableAudInfoframe;		/**< 是否使能 AUDIO InfoFrame*/
	IMP_BOOL					bEnableSpdInfoframe;		/**< 是否使能 SPD InfoFrame*/
	IMP_BOOL					bEnableMpegInfoframe;		/**< 是否使能 MPEG InfoFrame*/

	IMP_BOOL					bDebugFlag;					/**< 是否使能打开 hdmi 内部 debug 信息*/
	IMP_BOOL					bHDCPEnable;				/**< 是否激活 HDCP*/

	IMP_BOOL					b3DEnable;					/**< 是否激活 3D 模式*/
	IMP_U8						u83DParam;					/**< 3D参数,默认为0*/
	IMP_BOOL					bAuthMode;					/**< EDID 读取失败时，HDMI 内部强制的视频输出模式枚举*/
	IMP_HDMI_FORCE_ACTION_E		enDefaultMode;				/**< 使能该模式，HDMI 强制输出*/
	IMP_BOOL					bEnableVidModeAdapt;		/**< 是否打开驱动颜色空间自适应功能*/
	IMP_BOOL					bEnableDeepClrAdapt;		/**< 是否打开驱动 Deepcolor 自适应策略*/
	IMP_U32						u32PixelClk;				/**< 用户设置自定义时序的像素时钟（单位：kHz）*/

}IMP_HDMI_ATTR_S;

/**
 *定义 HDMI 事件通知枚举
 */
typedef enum{
	IMP_HDMI_EVENT_HOTPLUG = 0x10,				/**< HDMI Cable 插入事件*/
	IMP_HDMI_EVENT_NO_PLUG,						/**< HDMI Cable 拔出事件*/
	IMP_HDMI_EVENT_EDID_FAIL,					/**< HDMI EDID 读取失败事件*/
	IMP_HDMI_EVENT_HDCP_FAIL,					/**< HDCP 验证失败事件*/
	IMP_HDMI_EVENT_HDCP_SUCCESS,				/**< HDCP 验证成功*/
	IMP_HDMI_EVENT_HDCP_USERSETTING = 0x17,		/**< HDCP Reset*/
	IMP_HDMI_EVENT_BUTT
}IMP_HDMI_EVENT_TYPE_E;

/**
 *定义 HDMI 回调函数指针类型
 */
typedef void (*IMP_HDMI_CallBack)(IMP_HDMI_EVENT_TYPE_E event, IMP_VOID *pPrivateData);

/**
 *定义 HDMI 回调函数结构体
 */
typedef struct{
	IMP_HDMI_CallBack pfnHdmiEventCallback;		/**< 事件处理回调函数*/
	IMP_VOID *pPrivateData;						/**< 回调函数参数私有数据*/
}IMP_HDMI_CALLBACK_FUNC_S;

/**
 *定义 HDMI 色彩空间结构体,此结构体为API参数
 */
typedef struct{
	IMP_HDMI_VIDEO_MODE_E enVideoMode;			/**< 视频模式枚举*/
	IMP_HDMI_DEEP_COLOR_E enDeepColor;			/**< Deep Color枚举*/
}IMP_HDMI_COLOR_SPACE_S;

/**
 *定义 HDMI EDID 信息结构体
 */
typedef struct{
	IMP_U32 u32Edidlength;			/**< EDID 信息数据长度*/
	IMP_U8 u8Edid[512];				/**< EDID 内容*/
}IMP_HDMI_EDID_S;

/**
 *定义 HDMI 信息帧类型枚举
 */
typedef enum{
	IMP_INFOFRAME_TYPE_AVI,			/**< AVI类型Infoframe*/
	IMP_INFOFRAME_TYPE_SPD,			/**< SPD类型Infoframe*/
	IMP_INFOFRAME_TYPE_AUDIO,		/**< AUDIO类型Infoframe*/
	IMP_INFOFRAME_TYPE_MPEG,		/**< MPEG类型Infoframe*/
	IMP_INFOFRAME_TYPE_VENDORSPEC,	/**< VENDORSPEC类型Infoframe*/
	IMP_INFOFRAME_TYPE_BUTT
}IMP_HDMI_INFOFRAME_TYPE_E;

/**
 *定义颜色空间枚举,此枚举为从EDID获取的显示器相关信息描述
 */
typedef enum{
	IMP_HDMI_COLOR_SPACE_RGB444,		/**< RGB444 输出模式*/
	IMP_HDMI_COLOR_SPACE_YCBCR422,		/**< YCBCR422 输出模式*/
	IMP_HDMI_COLOR_SPACE_YCBCR444,		/**< YCBCR444 输出模式*/
	IMP_HDMI_COLOR_SPACE_YCBCR420,		/**< YCBCR420 输出模式*/
}IMP_HDMI_COLOR_SPACE_E;

/**
 *定义 Bar 信息枚举
 */
typedef enum{
	IMP_HDMI_BAR_INFO_NOT_VALID,
	IMP_HDMI_BAR_INFO_V,
	IMP_HDMI_BAR_INFO_H,
	IMP_HDMI_BAR_INFO_VH
}IMP_HDMI_BARINFO_E;

/**
 *定义 Scan 信息枚举
 */
typedef enum{
	HDMI_SCAN_INFO_NO_DATA = 0,
	HDMI_SCAN_INFO_OVERSCANNED = 1,		/**< Sink端支持Overscan*/
	HDMI_SCAN_INFO_UNDERSCANNED = 2,	/**< Sink端不支持Overscan*/
	HDMI_SCAN_INFO_FUTURE
}IMP_HDMI_SCANINFO_E;

/**
 *定义 Colorimetry 信息枚举
 */
typedef enum{
	HDMI_COLORIMETRY_NO_DATA,
	HDMI_COLORIMETRY_ITU601,			/**< Colorimetry 601*/
	HDMI_COLORIMETRY_ITU709,			/**< Colorimetry 709*/
	HDMI_COLORIMETRY_EXTENDED,
}IMP_HDMI_COLORIMETRY_E;

/**
 *定义 拓展Colorimetry 信息枚举
 */
typedef enum{
	HDMI_COLORIMETRY_XVYCC_601,					/**< Colorimetry 601*/
	HDMI_COLORIMETRY_XVYCC_709,					/**< Colorimetry 709*/
	HDMI_COLORIMETRY_S_YCC_601,					/**< Colorimetry YCC601*/
	HDMI_COLORIMETRY_ADOBE_YCC_601,				/**< Colorimetry RGB*/
	HDMI_COLORIMETRY_ADOBE_RGB,					/**< Colorimetry 601*/
	HDMI_COLORIMETRY_2020_CONST_LUMINOUS,		/**< Colorimetry 2020 Luminous*/
	HDMI_COLORIMETRY_2020_NON_CONST_LUMINOUS,	/**< Colorimetry 2020 no Luminous*/
	HDMI_COLORIMETRY_RESERVED
}IMP_HDMI_EXT_COLORIMETRY_E;

/**
 *定义图像宽高比枚举
 */
typedef enum{
	IMP_HDMI_PIC_ASP_RATIO_NO_DATA,
	IMP_HDMI_PIC_ASP_RATIO_4TO3,				/**< 4:3模式*/
	IMP_HDMI_PIC_ASP_RATIO_16TO9,				/**< 16:9模式*/
	IMP_HDMI_PIC_ASP_RATIO_64TO27,				/**< 64:27模式*/
	IMP_HDMI_PIC_ASP_RATIO_256TO135,			/**< 256:135模式*/
	IMP_HDMI_PIC_ASP_RATIO_RESERVED,
}IMP_HDMI_PIC_ASPECT_RATIO_E;

/**
 *定义实际图像宽高比枚举
 */
typedef enum{
	IMP_HDMI_ACT_ASP_RATIO_RESERVED_0,
	IMP_HDMI_ACT_ASP_RATIO_RESERVED_1,
	IMP_HDMI_ACT_ASP_RATIO_16TO9_TOP,
	IMP_HDMI_ACT_ASP_RATIO_14TO9_TOP,
	IMP_HDMI_ACT_ASP_RATIO_16TO9_BOX_CENTER,
	IMP_HDMI_ACT_ASP_RATIO_RESERVED_5,
	IMP_HDMI_ACT_ASP_RATIO_RESERVED_6,
	IMP_HDMI_ACT_ASP_RATIO_RESERVED_7,
	IMP_HDMI_ACT_ASP_RATIO_SAME_PIC,
	IMP_HDMI_ACT_ASP_RATIO_4TO3_CENTER,
	IMP_HDMI_ACT_ASP_RATIO_16TO9_CENTER,
	IMP_HDMI_ACT_ASP_RATIO_14TO9_CENTER,
	IMP_HDMI_ACT_ASP_RATIO_RESERVED_12,
	IMP_HDMI_ACT_ASP_RATIO_4TO3_14_9,
	IMP_HDMI_ACT_ASP_RATIO_16TO9_14_9,
	IMP_HDMI_ACT_ASP_RATIO_16TO9_4_3,
}IMP_HDMI_ACT_ASPECT_RATIO_E;

/**
 *定义图像扫描信息枚举
 */
typedef enum{
	IMP_HDMI_PICTURE_NON_UNIFORM_SCALING,
	IMP_HDMI_PICTURE_SCALING_H,
	IMP_HDMI_PICTURE_SCALING_V,
	IMP_HDMI_PICTURE_SCALING_HV
}IMP_HDMI_PICTURE_SCALING_E;

/**
 *定义 RGB 量化范围枚举
 */
typedef enum{
	IMP_HDMI_RGB_QUANTIZATION_DEFAULT_RANGE,
	IMP_HDMI_RGB_QUANTIZATION_LIMITED_RANGE,
	IMP_HDMI_RGB_QUANTIZATION_FULL_RANGE,
	IMP_HDMI_RGB_QUANTIZATION_FULL_RESERVED
}IMP_HDMI_RGB_QUAN_RAGE_E;

/**
 *定义像素复制次数枚举
 */
typedef enum{
	IMP_HDMI_PIXEL_REPET_NO,
	IMP_HDMI_PIXEL_REPET_2_TIMES,
	IMP_HDMI_PIXEL_REPET_3_TIMES,
	IMP_HDMI_PIXEL_REPET_4_TIMES,
	IMP_HDMI_PIXEL_REPET_5_TIMES,
	IMP_HDMI_PIXEL_REPET_6_TIMES,
	IMP_HDMI_PIXEL_REPET_7_TIMES,
	IMP_HDMI_PIXEL_REPET_8_TIMES,
	IMP_HDMI_PIXEL_REPET_9_TIMES,
	IMP_HDMI_PIXEL_REPET_10_TIMES,
	IMP_HDMI_PIXEL_REPET_RESERVED_A,
	IMP_HDMI_PIXEL_REPET_RESERVED_B,
	IMP_HDMI_PIXEL_REPET_RESERVED_C,
	IMP_HDMI_PIXEL_REPET_RESERVED_D,
	IMP_HDMI_PIXEL_REPET_RESERVED_E,
	IMP_HDMI_PIXEL_REPET_RESERVED_F,
}IMP_HDMI_PIXEL_REPETITION_E;

/**
 *定义内容信息枚举
 */
typedef enum{
	IMP_HDMI_CONTNET_GRAPHIC,
	IMP_HDMI_CONTNET_PHOTO,
	IMP_HDMI_CONTNET_CINEMA,
	IMP_HDMI_CONTNET_GAME
}IMP_HDMI_CONTENT_TYPE_E;

/**
 *定义 YCC 量化范围枚举
 */
typedef enum{
	IMP_HDMI_YCC_QUANTIZATION_LIMITED_RANGE,
	IMP_HDMI_YCC_QUANTIZATION_FULL_RANGE,
	IMP_HDMI_YCC_QUANTIZATION_RESERVED_2,
	IMP_HDMI_YCC_QUANTIZATION_RESERVED_3
}IMP_HDMI_YCC_QUAN_RAGE_E;

/**
 *定义音频通道数枚举
 */
typedef enum{
	IMP_HDMI_AUDIO_CHANEL_CNT_STREAM,
	IMP_HDMI_AUDIO_CHANEL_CNT_2,
	IMP_HDMI_AUDIO_CHANEL_CNT_3,
	IMP_HDMI_AUDIO_CHANEL_CNT_4,
	IMP_HDMI_AUDIO_CHANEL_CNT_5,
	IMP_HDMI_AUDIO_CHANEL_CNT_6,
	IMP_HDMI_AUDIO_CHANEL_CNT_7,
	IMP_HDMI_AUDIO_CHANEL_CNT_8,
}IMP_HDMI_AUDIO_CHANEL_CNT_E;

/**
 *定义音频编码类型枚举
 */
typedef enum{
	IMP_HDMI_AUDIO_CODING_REFER_STREAM_HEAD,
	IMP_HDMI_AUDIO_CODING_PCM,
	IMP_HDMI_AUDIO_CODING_AC3,
	IMP_HDMI_AUDIO_CODING_MPEG1,
	IMP_HDMI_AUDIO_CODING_MP3,
	IMP_HDMI_AUDIO_CODING_MPEG2,
	IMP_HDMI_AUDIO_CODING_AACLC,
	IMP_HDMI_AUDIO_CODING_DTS,
	IMP_HDMI_AUDIO_CODING_ATRAC,
	IMP_HDMI_AUDIO_CODIND_ONE_BIT_AUDIO,
	IMP_HDMI_AUDIO_CODING_ENAHNCED_AC3,
	IMP_HDMI_AUDIO_CODING_DTS_HD,
	IMP_HDMI_AUDIO_CODING_MAT,
	IMP_HDMI_AUDIO_CODING_DST,
	IMP_HDMI_AUDIO_CODING_WMA_PRO,
	IMP_HDMI_AUDIO_CODING_MAX
}IMP_HDMI_CODING_TYPE_E;

/**
 *定义音频采样大小枚举
 */
typedef enum{
	IMP_HDMI_AUDIO_SAMPLE_SIZE_STREAM,
	IMP_HDMI_AUDIO_SAMPLE_SIZE_16,
	IMP_HDMI_AUDIO_SAMPLE_SIZE_20,
	IMP_HDMI_AUDIO_SAMPLE_SIZE_24,
}IMP_HDMI_AUDIO_SAMPLE_SIZE_E;

/**
 *定义音频采样频率枚举
 */
typedef enum{
	IMP_HDMI_AUDIO_SAMPLE_FREQ_STREAM,
	IMP_HDMI_AUDIO_SAMPLE_FREQ_32000,
	IMP_HDMI_AUDIO_SAMPLE_FREQ_44100,
	IMP_HDMI_AUDIO_SAMPLE_FREQ_48000,
	IMP_HDMI_AUDIO_SAMPLE_FREQ_88200,
	IMP_HDMI_AUDIO_SAMPLE_FREQ_96000,
	IMP_HDMI_AUDIO_SAMPLE_FREQ_176400,
	IMP_HDMI_AUDIO_SAMPLE_FREQ_192000,
}IMP_HDMI_AUDIO_SAMPLE_FREQ_E;

/**
 *定义音频 Shift 信息枚举
 */
typedef enum{
	IMP_HDMI_LEVEL_SHIFT_VALUE_0_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_1_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_2_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_3_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_4_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_5_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_6_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_7_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_8_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_9_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_10_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_11_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_12_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_13_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_14_DB,
	IMP_HDMI_LEVEL_SHIFT_VALUE_15_DB,
}IMP_HDMI_LEVEL_SHIFT_VALUE_E;

/**
 *定义音频 Playback 信息枚举
 */
typedef enum{
	IMP_HDMI_LFE_PLAYBACK_NO,
	IMP_HDMI_LFE_PLAYBACK_0_DB,
	IMP_HDMI_LFE_PLAYBACK_10_DB,
	IMP_HDMI_LFE_PLAYBACK_RESEVED,
}IMP_HDMI_LFE_PLAYBACK_LEVEL_E;

/**
 *定义 CSC 输出量化范围枚举
 */
typedef struct{
	IMP_HDMI_VIDEO_FMT_E enTimingMode;
	IMP_HDMI_COLOR_SPACE_E enColorSpace;
	IMP_BOOL bActive_Infor_Present;
	IMP_HDMI_BARINFO_E enBarInfo;
	IMP_HDMI_SCANINFO_E enScanInfo;

	IMP_HDMI_COLORIMETRY_E enColorimetry;
	IMP_HDMI_EXT_COLORIMETRY_E enExtColorimetry;
	IMP_HDMI_PIC_ASPECT_RATIO_E enAspectRatio;
	IMP_HDMI_ACT_ASPECT_RATIO_E enActiveAspectRatio;
	IMP_HDMI_PICTURE_SCALING_E enPictureScaling;

	IMP_HDMI_RGB_QUAN_RAGE_E enRGBQuantization;
	IMP_BOOL bIsITContent;
	IMP_HDMI_PIXEL_REPETITION_E enPixelRepetition;
	IMP_HDMI_CONTENT_TYPE_E enContentType;
	IMP_HDMI_YCC_QUAN_RAGE_E enYCCQuantization;
	IMP_U16 u16LineNEndofTopBar;
	IMP_U16 u16LineNStartofBotBar;
	IMP_U16 u16PixelNEndofLeftBar;
	IMP_U16 u16PixelNStartofRightBar;
}IMP_HDMI_AVI_INFOFRAME_VER2_S;

/**
 *定义 HDMI AUDIO 信息帧（版本 1）单元结构体
 */
typedef struct{
	IMP_HDMI_AUDIO_CHANEL_CNT_E enChannelCount;				/**< 音频通道个数*/
	IMP_HDMI_CODING_TYPE_E enCodingType;					/**< 音频格式*/
	IMP_HDMI_AUDIO_SAMPLE_SIZE_E enSampleSize;				/**< 音频采样深度（位宽）*/
	IMP_HDMI_AUDIO_SAMPLE_FREQ_E enSamplingFrequency;		/**< 音频采样率*/
	IMP_U8 u8ChannelAlloc;									/**< Channel/Speaker 分配*/
	IMP_HDMI_LEVEL_SHIFT_VALUE_E enLevelShift;				/**< Level Shift Value，左移值*/
	IMP_HDMI_LFE_PLAYBACK_LEVEL_E enLfePlaybackLevel;		/**< LFE playback level information，LFE 播放等级信息*/
	IMP_BOOL bDownmixInhibit;								/**< Down-mix Inhibit 标志位*/
}IMP_HDMI_AUD_INFOFRAME_VER1_S;

/**
 *定义 HDMI SPD 信息帧单元结构体
 */
typedef struct{
	IMP_U8 u8VendorName[8];				/**< Source 端供应商名称*/
	IMP_U8 u8ProductDescription[16];	/**< Source 端产品类型描述*/
}IMP_HDMI_SPD_INFOFRAME_S;

/**
 *定义 HDMI MPEG 信息帧单元结构体
 */
typedef struct{
	IMP_U32 u32MPEGBitRate;				/**< MPEG bit 速率*/
	IMP_BOOL bIsFieldRepeated;			/**< 当前帧是否为 repeate 帧*/
}IMP_HDMI_MPEGSOURCE_INFOFRAME_S;

/**
 *定义 HDMI VS 信息帧单元结构体
 */
typedef struct{
	IMP_U32 u32RegistrationId;			/**< IEEE注册码*/
}IMP_HDMI_VENDORSPEC_INFOFRAME_S;

/**
 *定义 HDMI 信息帧单元联合体
 */
typedef union{
	IMP_HDMI_AVI_INFOFRAME_VER2_S stAVIInfoFrame;				/**< AVI 信息帧单元*/
	IMP_HDMI_AUD_INFOFRAME_VER1_S stAUDInfoFrame;				/**< AUDIO 信息帧单元*/
	IMP_HDMI_SPD_INFOFRAME_S stSPDInfoFrame;					/**< SPD 信息帧单元*/
	IMP_HDMI_MPEGSOURCE_INFOFRAME_S stMPEGSourceInfoFrame;		/**< MPEG 信息帧单元*/
	IMP_HDMI_VENDORSPEC_INFOFRAME_S stVendorSpecInfoFrame;		/**< VS 信息帧单元*/
}IMP_HMDI_INFORFRAME_UNIT_U;

/**
 *定义 HDMI 信息帧结构体
 */
typedef struct{
	IMP_HDMI_INFOFRAME_TYPE_E enInfoFrameType;					/**< 信息帧类型*/
	IMP_HMDI_INFORFRAME_UNIT_U unInforUnit;						/**< 信息帧单元(内容)*/
}IMP_HDMI_INFOFRAME_S;

/**
 *定义 HDMI CSC模式枚举
 */
typedef enum{
	BT601_FULL = 0,
	BT601_LIMITED,
	BT709_FULL,
	BT709_LIMITED,
	BT2020_FULL,
	BT2020_LIMITED,
	CSC_VERSION_BUTT,
}IMP_HDMI_CSC_E;

/**
 * @fn IMP_S32 IMP_HDMI_Init(IMP_VOID)

 * HDMI初始化.

 * @param[in] 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_Init(IMP_VOID);

/**
 * @fn IMP_S32 IMP_HDMI_DeInit(IMP_VOID)

 * HDMI去初始化.

 * @param[in] 无.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_DeInit(IMP_VOID);

/**
 * @fn IMP_S32 IMP_HDMI_SetAttr(IMP_HDMI_ID_E enHdmi,IMP_HDMI_ATTR_S *pstAttr)

 * 设置HDMI属性.

 * @param[in] enHdmi HDMI ID号.
 * @param[in] pstAttr HDMI属性结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用;若只设置部分属性，设置前应先获取属性，再赋值该部分属性后再设置.
 */
IMP_S32 IMP_HDMI_SetAttr(IMP_HDMI_ID_E enHdmi,IMP_HDMI_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_HDMI_GetAttr(IMP_HDMI_ID_E enHdmi,IMP_HDMI_ATTR_S *pstAttr)

 * 获取HDMI属性.

 * @param[in]  enHdmi HDMI ID号.
 * @param[out] pstAttr HDMI属性结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用;若只设置部分属性，设置前应先获取属性，再赋值该部分属性后再设置.
 */
IMP_S32 IMP_HDMI_GetAttr(IMP_HDMI_ID_E enHdmi,IMP_HDMI_ATTR_S *pstAttr);

/**
 * @fn IMP_S32 IMP_HDMI_Start(IMP_HDMI_ID_E enHdmi)
 *
 * 启动HDMI输出.

 * @param[in]  enHdmi HDMI ID号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_Start(IMP_HDMI_ID_E enHdmi);

/**
 * @fn IMP_S32 IMP_HDMI_Stop(IMP_HDMI_ID_E enHdmi)
 *
 * 停止HDMI输出.

 * @param[in]  enHdmi HDMI ID号.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_Stop(IMP_HDMI_ID_E enHdmi);

/**
 * @fn IMP_S32 IMP_HDMI_GetSinkCapability(IMP_HDMI_ID_E enHdmi,IMP_HDMI_SINK_CAPABILITY_S *pstSinkCap)
 *
 * 获取Sink端能力集.

 * @param[in]  enHdmi HDMI ID号.
 * @param[out]  pstSinkCap HDMI Sink 端能力集结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用;应在 HDMI 启动且插入线缆之后调用.
 */
IMP_S32 IMP_HDMI_GetSinkCapability(IMP_HDMI_ID_E enHdmi,IMP_HDMI_SINK_CAPABILITY_S *pstSinkCap);

/**
 * @fn IMP_S32 IMP_HDMI_SetAVMute(IMP_HDMI_ID_E enHdmi,IMP_BOOL bAVmute)
 *
 * 设置HDMI音视频Mute.

 * @param[in]  enHdmi HDMI ID号.
 * @param[in]  bAVmute HDMI 音视频 Mute标志位.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_SetAVMute(IMP_HDMI_ID_E enHdmi,IMP_BOOL bAVmute);

/**
 * @fn IMP_S32 IMP_HDMI_Force_GetEDID(IMP_HDMI_ID_E enHdmi,IMP_HDMI_EDID_S *pstEdidData)
 *
 * 获取HDMI原始EDID数据.

 * @param[in]  enHdmi HDMI ID号.
 * @param[out]  pstEdidData HDMI 的 EDID 信息.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用;应在 HDMI 启动且插入线缆之后调用.
 */
IMP_S32 IMP_HDMI_Force_GetEDID(IMP_HDMI_ID_E enHdmi,IMP_HDMI_EDID_S *pstEdidData);

/**
 * @fn IMP_S32 IMP_HDMI_RegCallbackFunc(IMP_HDMI_ID_E enHdmi,IMP_HDMI_CALLBACK_FUNC_S *pstCallbackFunc)
 *
 * 注册HDMI事件回调函数.

 * @param[in]  enHdmi HDMI ID号.
 * @param[in]  pstCallbackFunc HDMI 回调函数结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用;若用户注册了事件回调函数，则退出 HDMI 前，应使用撤销事件回调函数.
 */
IMP_S32 IMP_HDMI_RegCallbackFunc(IMP_HDMI_ID_E enHdmi,IMP_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);

/**
 * @fn IMP_S32 IMP_HDMI_UnRegCallbackFunc(IMP_HDMI_ID_E enHdmi,IMP_HDMI_CALLBACK_FUNC_S *pstCallbackFunc)
 *
 * 注销HDMI事件回调函数.

 * @param[in]  enHdmi HDMI ID号.
 * @param[in]  pstCallbackFunc HDMI 回调函数结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用;若用户注册了事件回调函数，则退出HDMI前，应使用撤销事件回调函数.
 */
IMP_S32 IMP_HDMI_UnRegCallbackFunc(IMP_HDMI_ID_E enHdmi,IMP_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);

/**
 * @fn IMP_S32 IMP_HDMI_SetDeepColor(IMP_HDMI_ID_E enHdmi,IMP_HDMI_COLOR_SPACE_S *pstColorSpace)
 *
 * 设置HDMI DeepColor模式.

 * @param[in]  enHdmi HDMI ID号.
 * @param[in]  pstColorSpace HDMI的Deep Color模式.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_SetDeepColor(IMP_HDMI_ID_E enHdmi,IMP_HDMI_COLOR_SPACE_S *pstColorSpace);

/**
 * @fn IMP_S32 IMP_HDMI_SetDeepColor(IMP_HDMI_ID_E enHdmi,IMP_HDMI_COLOR_SPACE_S *pstColorSpace)
 *
 * 获取HDMI DeepColor模式.

 * @param[in]   enHdmi HDMI ID号.
 * @param[out]  pstColorSpace HDMI的Deep Color模式.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_GetDeepColor(IMP_HDMI_ID_E enHdmi,IMP_HDMI_COLOR_SPACE_S *pstColorSpace);

/**
 * @fn IMP_S32 IMP_HDMI_SetInfoFrame(IMP_HDMI_ID_E enHdmi,IMP_HDMI_INFOFRAME_S *pstInfoFrame)
 *
 * 设置HDMI 信息帧.

 * @param[in]   enHdmi HDMI ID号.
 * @param[in]   pstInfoFrame HDMI 信息帧结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用;在DVI模式下设定所有的信息帧均无法生效.
 */
IMP_S32 IMP_HDMI_SetInfoFrame(IMP_HDMI_ID_E enHdmi,IMP_HDMI_INFOFRAME_S *pstInfoFrame);

/**
 * @fn IMP_S32 IMP_HDMI_SetInfoFrame(IMP_HDMI_ID_E enHdmi,IMP_HDMI_INFOFRAME_S *pstInfoFrame)
 *
 * 获取HDMI 信息帧.

 * @param[in]   enHdmi HDMI ID号.
 * @param[out]  pstInfoFrame HDMI 信息帧结构体指针.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_GetInfoFrame(IMP_HDMI_ID_E enHdmi,IMP_HDMI_INFOFRAME_S *pstInfoFrame);

/**
 * @fn IMP_S32 IMP_HDMI_SET_CSC(IMP_HDMI_ID_E enHdmi,IMP_HDMI_CSC_E enCsc_Version);
 *
 * 设置HDMI CSC模式.

 * @param[in]   enHdmi HDMI ID号.
 * @param[in]   enCsc_Version HDMI CSC模式枚举.

 * @retval 0 成功.
 * @retval 非0 失败.

 * @remarks 无.
 * @attention 需要在VO Enable之后调用.
 */
IMP_S32 IMP_HDMI_SET_CSC(IMP_HDMI_ID_E enHdmi,IMP_HDMI_CSC_E enCsc_Version);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
