/*
 * sample-Encoder-jpeg.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <stdio.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Encoder-jpeg"

extern struct chn_conf chn[];

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
	ret = sample_jpeg_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}
	IMPEncoderJpegeQl pstJpegeQl;
	unsigned char ql[128] =
		{
			32,24,24,28,24,20,32,28,
			28,28,36,36,32,40,48,80,
			52,48,44,44,48,100,72,76,
			60,80,116,104,124,120,116,104,
			112,112,128,144,184,156,128,136,
			176,140,112,112,160,220,164,176,
			192,196,208,208,208,124,156,228,
			244,224,200,240,184,204,208,200,
			36,36,36,48,44,48,96,52,
			52,96,200,132,112,132,200,200,
			200,200,200,200,200,200,200,200,
			200,200,200,200,200,200,200,200,
			200,200,200,200,200,200,200,200,
			200,200,200,200,200,200,200,200,
			200,200,200,200,200,200,200,200,
			200,200,200,200,200,200,200,200,
		};
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			IMP_Encoder_GetJpegeQl(3+chn[i].index, &pstJpegeQl);
			pstJpegeQl.user_ql_en = 1;
			memcpy(pstJpegeQl.qmem_table, ql, sizeof(pstJpegeQl.qmem_table));
			IMP_Encoder_SetJpegeQl(3+chn[i].index, &pstJpegeQl);
		}
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

	/* drop several pictures of invalid data */
	sleep(SLEEP_TIME);

	/* Step.6 Get Snap */
	ret = sample_get_jpeg_snap();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get H264 stream failed\n");
		return -1;
	}

	/* Exit sequence as follow... */
	/* Step.a Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
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

	/* Step.d FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
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
