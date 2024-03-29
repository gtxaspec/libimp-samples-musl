/*
 * sample-snap-raw.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Snap-RAW"
extern struct chn_conf chn[];
extern sensor_bypass[3];
int main(int argc, char *argv[])
{
	int ret;

	IMPFrameInfo *frame_bak;
  	/* must initial first of sensor_bypass,is use after Addsensor */
	sensor_bypass[chn[0].index] = 1;
	FILE *fp;

	fp = fopen("/tmp/snap.raw", "wb");
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

	/* Step.2 Snap raw config */
	/* config sensor reg to output colrbar raw data*/
	chn[0].fs_chn_attr.pixFmt = PIX_FMT_RAW;

	/* Step.3 FrameSource init */
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

	/* Step.4 Stream On */
	if (chn[0].enable){
		ret = IMP_FrameSource_EnableChn(chn[0].index);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn[0].index);
			return -1;
		}
	}

	/* Step.4 Snap raw */
	ret = IMP_FrameSource_SetFrameDepth(0, 1);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
		return -1;
	}

	ret = IMP_FrameSource_GetFrame(0, &frame_bak);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetFrame failed\n", __func__, __LINE__);
		return -1;
	}

	fwrite((void *)frame_bak->virAddr, frame_bak->size, 1, fp);
	fclose(fp);

	IMP_FrameSource_ReleaseFrame(0, frame_bak);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_ReleaseFrame failed\n", __func__, __LINE__);
		return -1;
	}
	ret = IMP_FrameSource_SetFrameDepth(0, 0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
		return -1;
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
		/*Destroy channel i*/
		ret = IMP_FrameSource_DestroyChn(0);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn() error: %d\n", ret);
			return -1;
		}
	}

	/* Step.7 recover sensor reg to output normal image*/
	/* to do */

	/* Step.8 System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
