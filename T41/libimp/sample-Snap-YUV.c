/*
 * sample-snap-raw.c
 *
 * Copyright (C) 2018 Ingenic Semiconductor Co.,Ltd
 */

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Snap-RAW"
extern struct chn_conf chn[];

int main(int argc, char *argv[])
{
	int ret;

	IMPFrameInfo *frame_bak;
	IMPFSChnAttr fs_chn_attr[2];
	FILE *fp;

	fp = fopen("/tmp/snap.yuv", "wb");
	if(fp == NULL) {
		IMP_LOG_ERR(TAG, "%s(%d):open error !\n", __func__, __LINE__);
		return -1;
	}

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	/* Step.2 FrameSource init */
	if (chn[0].enable) {
		ret = IMP_FrameSource_CreateChn(chn[0].index, &chn[0].fs_chn_attr);
		if(ret < 0){
			IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", chn[0].index);
			return -1;
		}

		ret = IMP_FrameSource_SetChnAttr(chn[0].index, &chn[0].fs_chn_attr);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  chn[0].index);
			return -1;
		}
	}

	/* Step.3 FrameSource channel config */
	ret = IMP_FrameSource_GetChnAttr(0, &fs_chn_attr[0]);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetChnAttr failed\n", __func__, __LINE__);
		return -1;
	}

	fs_chn_attr[0].pixFmt = PIX_FMT_NV12;
	ret = IMP_FrameSource_SetChnAttr(0, &fs_chn_attr[0]);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetChnAttr failed\n", __func__, __LINE__);
		return -1;
	}

	/* Step.4 Enable Encoder channel,Stream On */
	if (chn[0].enable){
		ret = IMP_FrameSource_EnableChn(chn[0].index);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn[0].index);
			return -1;
		}
	}

	/* Step.4 Snap raw */
	{
        /* Set the number of channel cache frames */
		ret = IMP_FrameSource_SetFrameDepth(0, 1);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
			return -1;
		}

		int m = 0;

		for (m=1;m<=51;m++) {
		/* Get YUV data */
			ret = IMP_FrameSource_GetFrame(0, &frame_bak);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetFrame failed\n", __func__, __LINE__);
				return -1;
			}
			if(m%50==0) {
				fwrite((void *)frame_bak->virAddr, frame_bak->size, 1, fp);
				fclose(fp);
			}
			/* Release the acquired YUV data */
			IMP_FrameSource_ReleaseFrame(0, frame_bak);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_ReleaseFrame failed\n", __func__, __LINE__);
				 return -1;
			}
		}
		/* To stop getting YUV data, you need to set the channel cache to 0 */
		ret = IMP_FrameSource_SetFrameDepth(0, 0);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
			return -1;
		}
	}

	/* Step.5 Stream Off */
	if (chn[0].enable){
		ret = IMP_FrameSource_DisableChn(chn[0].index);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, chn[0].index);
			return -1;
		}
	}

	/* Step.6 FrameSource exit */
	if (chn[0].enable) {
		/* Destroy channel */
		ret = IMP_FrameSource_DestroyChn(0);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn() error: %d\n", ret);
			return -1;
		}
	}

	/* Step.7 System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}