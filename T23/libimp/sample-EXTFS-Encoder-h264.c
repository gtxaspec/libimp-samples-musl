/*
 * sample-Encoder-h264.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Encoder-h264"

#define GAIN_HSV_DATE

extern struct chn_conf chn[];
extern struct chn_conf chn_ext_hsv[];

int sample_snap_fs_hsv(void)
{
	int ret = 0;
	int i = 0;
	FILE *fp;
	FILE *dsthsv_h;
	FILE *dsthsv_s;
	FILE *dsthsv_v;
	IMPFrameInfo *frame;

	fp = fopen("/tmp/snap.hsv", "wb");
	if(fp == NULL) {
		IMP_LOG_ERR(TAG, "%s(%d):open error !\n", __func__, __LINE__);
		return -1;
	}

	dsthsv_h = fopen("/tmp/dsthsv_h.dat", "wb");
	if(fp == NULL) {
		IMP_LOG_ERR(TAG, "%s(%d):open error !\n", __func__, __LINE__);
		return -1;
	}

	dsthsv_s = fopen("/tmp/dsthsv_s.dat", "wb");
	if(fp == NULL) {
		IMP_LOG_ERR(TAG, "%s(%d):open error !\n", __func__, __LINE__);
		return -1;
	}

	dsthsv_v = fopen("/tmp/dsthsv_v.dat", "wb");
	if(fp == NULL) {
		IMP_LOG_ERR(TAG, "%s(%d):open error !\n", __func__, __LINE__);
		return -1;
	}
	for (i = 0; i < 100; i++) {
		/* Snap HSV */
		ret = IMP_FrameSource_SetFrameDepth(3, 1);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
			return -1;
		}

		ret = IMP_FrameSource_GetFrame(3, &frame);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetFrame failed\n", __func__, __LINE__);
			return -1;
		}

		if (80 == i) {
			fwrite((void *)frame->virAddr, frame->size, 1, fp);
/* This is use to debug hsv date*/
#ifdef GAIN_HSV_DATE
            fwrite((void *)frame->virAddr, (frame->width*frame->height)*2, 1, dsthsv_h);
			fwrite((void *)(frame->virAddr + (frame->width*frame->height)*2), frame->width*frame->height, 1, dsthsv_s);
			fwrite((void *)(frame->virAddr + (frame->width*frame->height)*3), frame->width*frame->height, 1, dsthsv_v);
			fclose(dsthsv_h);
			fclose(dsthsv_s);
			fclose(dsthsv_v);
#endif
            fclose(fp);
		}
		IMP_FrameSource_ReleaseFrame(3, frame);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_ReleaseFrame failed\n", __func__, __LINE__);
			return -1;
		}
		ret = IMP_FrameSource_SetFrameDepth(3, 0);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
			return -1;
		}
	}
	/* end */
	return 0;
}

int sample_framesource_ext_hsv_init()
{
	int ret;
    chn[3].enable = 0;

	ret = IMP_FrameSource_CreateChn(3, &chn_ext_hsv[0].fs_chn_attr);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", 3);
		return -1;
	}

	ret = IMP_FrameSource_SetChnAttr(3, &chn_ext_hsv[0].fs_chn_attr);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n", 3);
		return -1;
	}
	return 0;
}

int sample_framesource_ext_hsv_streamon()
{
	int ret = 0;
	/* Enable channels */
	ret = IMP_FrameSource_EnableChn(3);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, 3);
		return -1;
	}
	return 0;
}

int sample_framesource_ext_hsv_streamoff()
{
	int ret = 0;
	/* Enable channels */
	ret = IMP_FrameSource_DisableChn(3);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, 3);
		return -1;
	}
	return 0;
}

int sample_framesource_ext_hsv_exit()
{
	int ret;

	ret = IMP_FrameSource_DestroyChn(3);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn() error: %d\n", ret);
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int i, ret;

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	/* Step.2 FrameSource init */
	ret = sample_framesource_ext_hsv_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init ext hsv failed\n");
		return -1;
	}

	ret = sample_framesource_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init failed\n");
		return -1;
	}


	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_Encoder_CreateGroup(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", i);
				return -1;
			}
		}
	}

	/* Step.3 Encoder init */
	ret = sample_encoder_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}

	/* Step.4 Bind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	/* Step.5 Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}
	ret = sample_framesource_ext_hsv_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn ext hsv failed\n");
		return -1;
	}


	/* Step.6 Get stream */

#if 1
	ret = sample_snap_fs_hsv();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get FS HSV failed\n");
		return -1;
	}
#endif

	ret = sample_get_video_stream();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get H264 stream failed\n");
		return -1;
	}

	/* Exit sequence as follow */

	/* Step.a Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	ret = sample_framesource_ext_hsv_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff  ext hsv failed\n");
		return -1;
	}

	/* Step.b UnBind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	/* Step.c Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_Encoder_DestroyGroup(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", i);
				return -1;
			}
		}
	}

	/* Step.d FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}
	ret = sample_framesource_ext_hsv_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit hsv exit failed\n");
		return -1;
	}

	/* Step.e System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
