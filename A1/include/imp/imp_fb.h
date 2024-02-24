/*
 * IMP Framebuf header file.
 *
 * Copyright (C) 2022 Ingenic Semiconductor Co.,Ltd
 */
#ifndef __IMP_FRAMEBUF_H__
#define __IMP_FRAMEBUF_H__

#include <imp/imp_common.h>
#include <linux/fb.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */


/* *
 * alpha 模式类型
 * */
typedef enum {
	FB_LAYER_FORMAT_ARGB8888 = 0x00,
	FB_LAYER_FORMAT_ARBG8888 = 0x01,
	FB_LAYER_FORMAT_AGBR8888 = 0x02,
	FB_LAYER_FORMAT_AGRB8888 = 0x03,
	FB_LAYER_FORMAT_ABGR8888 = 0x04,
	FB_LAYER_FORMAT_ABRG8888 = 0x05,
	FB_LAYER_FORMAT_RGBA8888 = 0x06,
	FB_LAYER_FORMAT_RBGA8888 = 0x07,
	FB_LAYER_FORMAT_GBRA8888 = 0x08,
	FB_LAYER_FORMAT_GRBA8888 = 0x09,
	FB_LAYER_FORMAT_BGRA8888 = 0x0a,
	FB_LAYER_FORMAT_BRGA8888 = 0x0b,
	FB_LAYER_FORMAT_ARGB1555 = 0x0c,
	FB_LAYER_FORMAT_ARBG1555 = 0x0d,
	FB_LAYER_FORMAT_AGBR1555 = 0x0e,
	FB_LAYER_FORMAT_AGRB1555 = 0x0f,
	FB_LAYER_FORMAT_ABGR1555 = 0x10,
	FB_LAYER_FORMAT_ABRG1555 = 0x11,
	FB_LAYER_FORMAT_RGBA5551 = 0x12,
	FB_LAYER_FORMAT_RBGA5551 = 0x13,
	FB_LAYER_FORMAT_GBRA5551 = 0x14,
	FB_LAYER_FORMAT_GRBA5551 = 0x15,
	FB_LAYER_FORMAT_BGRA5551 = 0x16,
	FB_LAYER_FORMAT_BRGA5551 = 0x17,
	FB_LAYER_FORMAT_NV12 = 0x20,
	FB_LAYER_FORMAT_NV21 = 0x24,
	FB_LAYER_FORMAT_NV16 = 0x21,
	FB_LAYER_FORMAT_NV61 = 0x25,
}FB_LAYER_PIXFMT_E;

/* *
 * alpha 模式类型
 * */
typedef enum {
	ALPHA_GLOBAL, /* 全局alpha */
	ALPHA_PIXEL, /* 像素alpha */
	ALPHA_FUSIONS, /* 融合alpha */
}FB_ALPHA_MODE_E;

/* *
 * colorkey 结构体
 * */
typedef struct {
	IMP_S32 s32KeyEnable; /* colorkey 使能 */
	IMP_U32 u32KeyColor; /* colorkey 值 */
}FB_COLORKEY_S;

/* *
 * alpha 结构体
 * */
typedef struct {
	FB_ALPHA_MODE_E	enAlphaMode; /* alpha 模式 */
	IMP_U8 u8Alpha0; /* alpha0 值, 在ARGB1555格式下使用 */
	IMP_U8 u8Alpha1; /* alpha1 值, 在ARGB1555格式下使用 */
	IMP_U8 u8GlobalAlpha; /* 全局alpha值 */
}FB_ALPHA_S;

 /* *
  * Surface属性结构体, 表示gui画布的属性或者显示的属性
  * */
typedef struct {
	IMP_U32 u32Width; /* surface 宽 */
	IMP_U32 u32Height; /* surface 高 */
	IMP_U32 u32Stride; /* surface 跨度 */
	FB_LAYER_PIXFMT_E enPixfmt; /* surface 像素格式 */
}FB_SURFACE_S;

/* *
 * fb point 结构体
 * */
typedef struct {
	IMP_S32 s32X; /* horizontal position */
	IMP_S32 s32Y; /* vertical position*/
}FB_POINT_S;

#define IOC_TYPE_JZFB       		'F'
/*  获得keycolor属性*/
#define FBIOGET_COLORKEY		_IOR(IOC_TYPE_JZFB, 10, FB_COLORKEY_S)
/* 设置keycolor属性 */
#define FBIOPUT_COLORKEY		_IOW(IOC_TYPE_JZFB, 11, FB_COLORKEY_S)
/* 获得alpha属性 */
#define FBIOGET_ALPHA			_IOR(IOC_TYPE_JZFB, 12, FB_ALPHA_S)
/* 设置alpha属性 */
#define FBIOPUT_ALPHA			_IOW(IOC_TYPE_JZFB, 13, FB_ALPHA_S)
/* 设置当前层在屏幕上显示的起始点坐标 */
#define FBIOPUT_SCREEN_ORIGIN   _IOW(IOC_TYPE_JZFB, 15, FB_POINT_S)
/* 获得当前层在屏幕上显示的起始点坐标 */
#define FBIOGET_SCREEN_ORIGIN   _IOR(IOC_TYPE_JZFB, 14, FB_POINT_S)
/* 设置当前层显示或不显示 */
#define FBIOPUT_SHOW			_IOW(IOC_TYPE_JZFB, 21, IMP_S32)
/* 获得当前层显示状态 */
#define FBIOGET_SHOW			_IOR(IOC_TYPE_JZFB, 22, IMP_S32)
/* 设置当前层显示属性 */
#define FBIOPUT_SCREEN_SIZE		_IOW(IOC_TYPE_JZFB, 23, FB_SURFACE_S)
/* 获得当前层显示属性 */
#define FBIOGET_SCREEN_SIZE		_IOR(IOC_TYPE_JZFB, 24, FB_SURFACE_S)
/* 设置当前层的压缩开启或关闭 */
#define FBIOPUT_COMPRESSION		_IOW(IOC_TYPE_JZFB, 25, IMP_S32)
/* 获得当前层的压缩状态 */
#define FBIOGET_COMPRESSION		_IOR(IOC_TYPE_JZFB, 26, IMP_S32)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
