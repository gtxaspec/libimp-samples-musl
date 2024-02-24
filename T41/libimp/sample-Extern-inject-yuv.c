/*
 * sample-Extern-inject-yuv.c
 * Inject external YUV data into framesource for further OSD overlay, video encoding module, etc.
 * The sample take out data from channel 0, and inject to channel 1.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Extern-inject-yuv"
#define SAVE_FRAME_NUM           200

static int stop_flag;

static void *extern_inject_video(void *args)
{
	int ret;
	int i, src_index, dst_index;

	src_index = ((int)args >> 16) & 0xffff;
	dst_index = (int)args & 0xffff;

	IMPFrameInfo *frame;
	IMPFrameInfo *frame0;

	ret = IMP_FrameSource_SetFrameDepth(src_index, 2);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_SetFrameDepthfailed\n");
	}

	while (1) {
		ret = IMP_FrameSource_GetFrame(src_index, &frame0);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_GetFrame 0 failed\n");
		}
		ret =  IMP_FrameSource_DequeueBuffer(dst_index, &frame);
		if(ret) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DequeueBuffer fail\n");
			continue;
		}

		/* printf("frame addr %p, frame->size =%d, frame0->size=%d.\n", frame->virAddr, frame->size, frame0->size); */
		memcpy(frame->virAddr, frame0->virAddr,  frame0->size);
		ret =  IMP_FrameSource_QueueBuffer(dst_index, frame);
		if(ret) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_QueueBuffer error\n");
		}

		ret = IMP_FrameSource_ReleaseFrame(src_index, frame0);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_ReleaseFrame 0 failed\n");
		}

		if (stop_flag == 1) {
			ret = IMP_FrameSource_SetFrameDepth(src_index, 0);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_FrameSource_SetFrameDepthfailed\n");
			}
			break;
		}

	}
}

static int save_stream_file(int fd, IMPEncoderStream *stream)
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


int main(int argc, char *argv[])
{
	int i, ret;
	int byGetFd;
	if (argc >= 2) {
		byGetFd = atoi(argv[1]);
	}

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	int main_index = 0, extern_index = 1;
	IMPFSChnAttr fs_main = {
		.i2dattr.i2d_enable = 0,

		.pixFmt = PIX_FMT_NV12,
		.outFrmRateNum = FIRST_SENSOR_FRAME_RATE_NUM,
		.outFrmRateDen = FIRST_SENSOR_FRAME_RATE_DEN,
		.nrVBs = 2,
		.type = FS_PHY_CHANNEL,

		.scaler.enable = 0,
		.scaler.outwidth = FIRST_SENSOR_WIDTH,
		.scaler.outheight = FIRST_SENSOR_HEIGHT,

		.crop.enable = FIRST_CROP_EN,
		.crop.top = 0,
		.crop.left = 0,
		.crop.width = FIRST_SENSOR_WIDTH,
		.crop.height = FIRST_SENSOR_HEIGHT,

		.picWidth = FIRST_SENSOR_WIDTH,
		.picHeight = FIRST_SENSOR_HEIGHT,
	};

	IMPFSChnAttr fs_extern = {
		.i2dattr.i2d_enable = 0,

		.pixFmt = PIX_FMT_NV12,
		.outFrmRateNum = FIRST_SENSOR_FRAME_RATE_NUM,
		.outFrmRateDen = FIRST_SENSOR_FRAME_RATE_DEN,
		.nrVBs = 2,
		/*Use external yuv data  */
		.type = FS_INJ_CHANNEL,

		.scaler.enable = 0,
		.scaler.outwidth = FIRST_SENSOR_WIDTH,
		.scaler.outheight = FIRST_SENSOR_HEIGHT,

		.crop.enable = 0,
		.crop.top = 0,
		.crop.left = 0,
		.crop.width = FIRST_SENSOR_WIDTH,
		.crop.height = FIRST_SENSOR_HEIGHT,

		.picWidth = FIRST_SENSOR_WIDTH,
		.picHeight = FIRST_SENSOR_HEIGHT,
	};

	/* Step.2 FrameSource init */
	ret = IMP_FrameSource_CreateChn(main_index, &fs_main);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", main_index);
		return -1;
	}

	ret = IMP_FrameSource_SetChnAttr(main_index, &fs_main);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  main_index);
		return -1;
	}


	ret = IMP_FrameSource_ExternInject_CreateChn(extern_index, &fs_extern);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_FrameSource_ExternInject_CreateChn(chn%d) error !\n", extern_index);
		return -1;
	}

	/* Step.3 Encoder init */
	ret = IMP_Encoder_CreateGroup(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", extern_index);
		return -1;
	}


	IMPEncoderChnAttr channel_attr;
	int uTargetBitRate = 1000;
	ret = IMP_Encoder_SetDefaultParam(&channel_attr, IMP_ENC_PROFILE_HEVC_MAIN, IMP_ENC_RC_MODE_CBR,
					  fs_main.picWidth, fs_main.picHeight,
					  fs_main.outFrmRateNum, fs_main.outFrmRateDen,
					  fs_main.outFrmRateNum * 2 / fs_main.outFrmRateDen, 2,
					  -1,
					  uTargetBitRate);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_SetDefaultParam(%d) error !\n", main_index);
		return -1;
	}


	ret = IMP_Encoder_CreateChn(extern_index, &channel_attr);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error !\n", extern_index);
		return -1;
	}

	ret = IMP_Encoder_RegisterChn(extern_index, extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(%d, %d) error: %d\n", extern_index, extern_index, ret);
		return -1;
	}

	/* Step.4 Bind */
	IMPCell framesource_extern_chn = {DEV_ID_FS, extern_index, 0 };
	IMPCell encoder_extern_chn = {DEV_ID_ENC, extern_index, 0 };

	ret = IMP_System_Bind(&framesource_extern_chn, &encoder_extern_chn);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel and Encoder failed\n");
		return -1;
	}


	/* Step.5 Stream On */
	ret = IMP_FrameSource_EnableChn(main_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", main_index);
		return -1;
	}

	ret =  IMP_FrameSource_ExternInject_EnableChn(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_ExternInject_EnableChn failed\n");
		return -1;
	}


	pthread_t tid;
	/* arg is framesource channel： src index | dst index */
	int arg = main_index | extern_index;
	ret = pthread_create(&tid, NULL, extern_inject_video, (void *)arg);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Create ChnNum get_video_stream failed\n");
	}

	/* Step.6 Get stream */
	char stream_path[64];
	int stream_fd = -1, totalSaveCnt = SAVE_FRAME_NUM;

	ret = IMP_Encoder_StartRecvPic(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", extern_index);
		return ((void *)-1);
	}
	sprintf(stream_path, "%s/stream-%d.h265", STREAM_FILE_PATH_PREFIX, extern_index);

	stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (stream_fd < 0) {
		IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
		return ((void *)-1);
	}

	for (i = 0; i < totalSaveCnt; i++) {
		ret = IMP_Encoder_PollingStream(extern_index, 1000);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_PollingStream(%d) timeout\n", extern_index);
			continue;
		}

		IMPEncoderStream stream;
		ret = IMP_Encoder_GetStream(extern_index, &stream, 1);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream(%d) failed\n", extern_index);
			return ((void *)-1);
		}

		ret = save_stream_file(stream_fd, &stream);
		if (ret < 0) {
			close(stream_fd);
			printf("save stream to file error!\n");
		}

		IMP_Encoder_ReleaseStream(extern_index, &stream);
	}
	stop_flag = 1;

	close(stream_fd);

	ret = IMP_Encoder_StopRecvPic(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic(%d) failed\n", extern_index);
		return ((void *)-1);
	}

	/* Exit sequence as follow */
	/* Step.a Stream Off */
	pthread_join(tid, NULL);

	ret = IMP_FrameSource_DisableChn(main_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, main_index);
		return -1;
	}

	ret = IMP_FrameSource_ExternInject_DisableChn(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, extern_index);
		return -1;
	}

	/* Step.b UnBind */
	ret = IMP_System_UnBind(&framesource_extern_chn, &encoder_extern_chn);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel and Encoder failed\n");
		return -1;
	}

	/* Step.c Encoder exit */
	ret = IMP_Encoder_UnRegisterChn(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_UnRegisterChn(%d) error: %d\n", extern_index, ret);
		return -1;
	}

	ret = IMP_Encoder_DestroyChn(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyChn(%d) error: %d\n", extern_index, ret);
		return -1;
	}

	ret = IMP_Encoder_DestroyGroup(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyGroup(%d) error: %d\n", extern_index, ret);
		return -1;
	}


	/* Step.d FrameSource exit */
	ret = IMP_FrameSource_DestroyChn(main_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn(%d) error: %d\n", main_index, ret);
		return -1;
	}

	ret = IMP_FrameSource_ExternInject_DestroyChn(extern_index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn(%d) error: %d\n", main_index, ret);
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
