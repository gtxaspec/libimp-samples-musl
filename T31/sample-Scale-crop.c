/*
 * sample-Change-Resolution.c
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 */

/*
 * This example is used to test the image upscaling function of AutoZoom (scaler+crop).
 * Where zoom_test array holds the magnification factor to be tested
 * Test on T31X，DDR 600M，ISP 200M，The maximum magnification factor is about 1.8 times
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include "sample-common.h"

#define TAG "sample-Change-Resolution"

extern struct chn_conf chn[];

struct frame_image_format {
	unsigned int type;
	struct v4l2_pix_format pix;

	/* crop */
	bool	crop_enable;
	unsigned int crop_top;
	unsigned int crop_left;
	unsigned int crop_width;
	unsigned int crop_height;

	/* scaler */
	bool	scaler_enable;
	unsigned int scaler_out_width;
	unsigned int scaler_out_height;

	unsigned int rate_bits;
	unsigned int rate_mask;

	/* crop front */
	bool	fcrop_enable;
	unsigned int fcrop_top;
	unsigned int fcrop_left;
	unsigned int fcrop_width;
	unsigned int fcrop_height;
};

static int save_stream(int fd, IMPEncoderStream *stream)
{
	int ret, i, nr_pack = stream->packCount;

	for (i = 0; i < nr_pack; i++) {
		IMPEncoderPack *pack = &stream->pack[i];
		if(pack->length){
			uint32_t remSize = stream->streamSize - pack->offset;
			if(remSize < pack->length){
				ret = write(fd, (void *)(stream->virAddr + pack->offset), remSize);
				if (ret != remSize) {
					IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].remSize(%d) error:%s\n", ret, i, remSize, strerror(errno));
					return -1;
				}
				ret = write(fd, (void *)stream->virAddr, pack->length - remSize);
				if (ret != (pack->length - remSize)) {
					IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].(length-remSize)(%d) error:%s\n", ret, i, (pack->length - remSize), strerror(errno));
					return -1;
				}
			}else {
				ret = write(fd, (void *)(stream->virAddr + pack->offset), pack->length);
				if (ret != pack->length) {
					IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].length(%d) error:%s\n", ret, i, pack->length, strerror(errno));
					return -1;
				}
			}
		}
	}
	return 0;
}

static void *res_get_video_stream(void *args)
{
	int val, i, chnNum, ret;
	char stream_path[64];
	IMPEncoderEncType encType;
	int stream_fd = -1, totalSaveCnt = 0;

	val = (int)args;
	chnNum = val & 0xffff;
	encType = (val >> 16) & 0xffff;

	ret = IMP_Encoder_StartRecvPic(chnNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
		return ((void *)-1);
	}

	sprintf(stream_path, "%s/stream-%d-%dx%d.%s", STREAM_FILE_PATH_PREFIX, chnNum,
		chn[chnNum].fs_chn_attr.picWidth, chn[chnNum].fs_chn_attr.picHeight,
		(encType == IMP_ENC_TYPE_AVC) ? "h264" : "h265");

	IMP_LOG_DBG(TAG, "Video ChnNum=%d Open Stream file %s ", chnNum, stream_path);
	stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (stream_fd < 0) {
		IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
		return ((void *)-1);
	}
	IMP_LOG_DBG(TAG, "OK\n");

	totalSaveCnt = NR_FRAMES_TO_SAVE;
	float zoom_test[] = {1.2, 1.4, 1.5, 1.6, 1.8};
	for (i = 0; i < totalSaveCnt; i++) {
#if 1
		if ((chnNum == 0) && (i != 0) && (i % 30 == 0)) {
			IMPISPAutoZoom autozoom;
			int ret = -1;
			autozoom.chan = chnNum;
			autozoom.scaler_enable = 1;
			autozoom.scaler_outwidth = (unsigned int)(SENSOR_WIDTH * zoom_test[i/30-1]);
			autozoom.scaler_outheight =  (unsigned int)(SENSOR_HEIGHT * zoom_test[i/30-1]);
			autozoom.crop_enable = 1;
			autozoom.crop_left = 0;
			autozoom.crop_top = 0;
			autozoom.crop_width = SENSOR_WIDTH;
			autozoom.crop_height = SENSOR_HEIGHT;
			ret = IMP_ISP_Tuning_SetAutoZoom(&autozoom);
			printf("IMP_ISP_Tuning_SetAutoZoom(%f) ret = %d\n", zoom_test[i/30-1], ret);
		}
#endif

		ret = IMP_Encoder_PollingStream(chnNum, 1000);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_PollingStream(%d) timeout\n", chnNum);
			continue;
		}

		IMPEncoderStream stream;
		/* Get H264 or H265 Stream */
		ret = IMP_Encoder_GetStream(chnNum, &stream, 1);

		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream(%d) failed\n", chnNum);
			return ((void *)-1);
		}

		ret = save_stream(stream_fd, &stream);
		if (ret < 0) {
			close(stream_fd);
			return ((void *)ret);
		}
		IMP_Encoder_ReleaseStream(chnNum, &stream);
	}

	close(stream_fd);

	ret = IMP_Encoder_StopRecvPic(chnNum);
	if (ret < 0) {
	IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic(%d) failed\n", chnNum);
	return ((void *)-1);
	}

	return ((void *)0);
}

static int sample_res_get_video_stream()
{
	unsigned int i;
	int ret;
	pthread_t tid[FS_CHN_NUM];

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			int arg = 0;
			arg = (((chn[i].payloadType >> 24) << 16) | chn[i].index);
			ret = pthread_create(&tid[i], NULL, res_get_video_stream, (void *)arg);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "Create ChnNum%d res_get_video_stream failed\n", chn[i].index);
			}
		}
	}

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			pthread_join(tid[i],NULL);
		}
	}
	return 0;
}

int sample_res_init()
{
	int ret, i;

	/* Step.2 FrameSource init */
	ret = sample_framesource_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init failed\n");
		return -1;
	}

	/* Step.3 Encoder init */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_Encoder_CreateGroup(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[i].index);
				return -1;
			}
		}
	}

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
	return 0;
}

int sample_res_deinit()
{
	int ret, i;
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
	return 0;
}

int main(int argc, char *argv[])
{
	int ret;

	//only enable chn[0] & chn[1]
	chn[0].enable = 1;
	chn[1].enable = 1;
	chn[2].enable = 0;
	chn[3].enable = 0;

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	ret = sample_res_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_init failed\n");
		return -1;
	}

	ret = sample_res_get_video_stream();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get video stream failed\n");
		return -1;
	}

	ret = sample_res_deinit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_res_deinit failed\n");
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
