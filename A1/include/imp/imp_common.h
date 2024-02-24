/*
 * IMP common data structure header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_COMMON_H__
#define __IMP_COMMON_H__

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include "imp_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 */

/**
 * IMP 设备ID枚举定义.
 */
typedef enum {
	MOD_ID_VI,			/**< 视频源 */
	MOD_ID_VDEC,		/**< 解码器 */
	MOD_ID_VENC,		/**< 编码器 */
	MOD_ID_OSD,			/**< 图像叠加，放缩等 */
	MOD_ID_VO,			/**< 图像显示 */
	MOD_ID_IVS,			/**< 算法 */
	MOD_ID_FB,			/**< framebuffer */
	MOD_ID_TDE,			/**< 图像的2D操作 */
	MOD_ID_RESERVED_START,
	MOD_ID_RESERVED_END = 15,
	NR_MAX_MODULES,
} MODULE_ID_E;

/**
 * IMP图像格式定义.
 */
typedef enum {
	PIX_FMT_YUV420P,   /**< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples) */
	PIX_FMT_YUYV422,   /**< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr */
	PIX_FMT_UYVY422,   /**< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1 */
	PIX_FMT_YUV422P,   /**< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples) */
	PIX_FMT_YUV444P,   /**< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples) */
	PIX_FMT_YUV410P,   /**< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples) */
	PIX_FMT_YUV411P,   /**< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) */
	PIX_FMT_GRAY8,     /**<	   Y	    ,  8bpp */
	PIX_FMT_MONOWHITE, /**<	   Y	    ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb */
	PIX_FMT_MONOBLACK, /**<	   Y	    ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb */

	PIX_FMT_NV12,      /**< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V) */
	PIX_FMT_NV21,      /**< as above, but U and V bytes are swapped */
	PIX_FMT_NV16,      /**< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V) */
	PIX_FMT_NV61,      /**< as above, but U and V bytes are swapped */

	PIX_FMT_RGB24,     /**< packed RGB 8:8:8, 24bpp, RGBRGB... */
	PIX_FMT_BGR24,     /**< packed RGB 8:8:8, 24bpp, BGRBGR... */

	PIX_FMT_ARGB,      /**< packed ARGB 8:8:8:8, 32bpp, ARGBARGB... */
	PIX_FMT_RGBA,	   /**< packed RGBA 8:8:8:8, 32bpp, RGBARGBA... */
	PIX_FMT_ABGR,	   /**< packed ABGR 8:8:8:8, 32bpp, ABGRABGR... */
	PIX_FMT_BGRA,	   /**< packed BGRA 8:8:8:8, 32bpp, BGRABGRA... */

	PIX_FMT_RGB565BE,  /**< packed RGB 5:6:5, 16bpp, (msb)	  5R 6G 5B(lsb), big-endian */
	PIX_FMT_RGB565LE,  /**< packed RGB 5:6:5, 16bpp, (msb)	  5R 6G 5B(lsb), little-endian */
	PIX_FMT_RGB555BE,  /**< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), big-endian, most significant bit to 0 */
	PIX_FMT_RGB555LE,  /**< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0 */

	PIX_FMT_BAYER_BGGR8,    /**< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
	PIX_FMT_BAYER_RGGB8,    /**< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
	PIX_FMT_BAYER_GBRG8,    /**< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
	PIX_FMT_BAYER_GRBG8,    /**< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */

	PIX_FMT_RAW,

	PIX_FMT_HSV,

	PIX_FMT_NB,
	PIX_FMT_YUV422,
	PIX_FMT_YVU422,
	PIX_FMT_UVY422,
	PIX_FMT_VUY422,
	PIX_FMT_RAW8,
	PIX_FMT_RAW16,
} PIXEL_FORMAT_E;

/**
 * IMPCell结构体定义.
 */
typedef struct {
	MODULE_ID_E		enModId;		/**< 模块ID */
	IMP_U32			u32DevId;		/**< 设备ID */
	IMP_U32			u32ChnId;		/**< 通道ID */
} IMP_CELL_S;

/**
 * IMP帧图像信息定义.
 */
typedef struct {
	IMP_S32 s32Index;			/**< 帧序号 */

	IMP_U32 u32Width;			/**< 帧宽 */
	IMP_U32 u32Height;			/**< 帧高 */
	IMP_U32 u32Stride;			/**< 跨度 */
	PIXEL_FORMAT_E stPixfmt;	/**< 帧的图像格式 */
	IMP_U32 u32Size;			/**< 帧所占用空间大小 */

	IMP_U32 u32PhyAddr;			/**< 帧的物理地址 */
	IMP_U32 u32VirAddr;			/**< 帧的虚拟地址 */

	IMP_U64 u64FrameTimeStamp;		/**< 帧的时间戳 */
	IMP_U64 u64SysTimeStamp;	/**< 系统的时间戳 */
	IMP_VOID*  Priv;			/* 私有数据指针 */
} FRAME_INFO_S;

/**
 * IMP帧时间参数.
 */
typedef struct {
	IMP_U64 u64Ts;						/**< 时间 */
	IMP_U64 u64Minus;						/**< 下限 */
	IMP_U64 u64Plus;						/**< 上限 */
} FRAME_TIMESTAMP_S;

/**
 * 编解码协议类型
 */
typedef enum {
	PT_JPEG,					/**< JPEG图像协议类型 */
	PT_H264,					/**< H264视频协议类型 */
	PT_H265,					/**< H265视频协议类型 */
} PAYLOAD_TYPE_E;

/**
 * IMP点坐标信息.
 */
typedef struct {
	IMP_S32		s32X;			/**<横坐标  */
	IMP_S32		s32Y;			/**<纵坐标  */
} IMP_POINT_S;

/**
 * IMP 矩形区域信息.
 */
typedef struct {
	IMP_S32		s32X;			/**<横坐标  */
	IMP_S32		s32Y;			/**<纵坐标  */
	IMP_U32		u32Width;		/**<宽度  */
	IMP_U32		u32Height;		/**<高度  */
} IMP_RECT_S;

typedef struct {
	IMP_U32		u32Width;		/**<宽度  */
	IMP_U32		u32Height;		/**<高度  */
}IMP_SIZE_S;

typedef struct {
	IMP_POINT_S		stP0;		/**<直线的端点0*/
	IMP_POINT_S		stP1;		/**<直线的端点1*/
}IMP_LINE_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_COMMON_H__ */
