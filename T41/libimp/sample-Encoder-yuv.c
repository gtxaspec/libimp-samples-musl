#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <imp/imp_log.h>
#include <imp/imp_system.h>

#include "sample-common.h"

#define TAG "sample-Encoder-yuv"

int main(int argc, char *argv[])
{
	int ret = 0;
	int i = 0, frmSize = 0;
	int width = 1920, height = 1080;
	int encNum = 200;
	char path[32];
	FILE *inFile = NULL, *outFile = NULL;
	uint8_t *src_buf = NULL;
	void *h = NULL;
	IMPFrameInfo frame;
	IMPEncoderYuvOut stream;
	IMPEncoderYuvIn info;

	memset(&info, 0, sizeof(IMPEncoderYuvIn));

	info.type = IMP_ENC_TYPE_HEVC;
	info.mode = IMP_ENC_RC_MODE_CAPPED_QUALITY;
	// info.type = IMP_ENC_TYPE_JPEG;
	// info.mode = IMP_ENC_RC_MODE_FIXQP;
	info.frameRate = 25;
	info.gopLength = 25;
	info.targetBitrate = 4096;
	info.maxBitrate = 4096 * 4 / 3;
	info.initQp = 25;
	info.minQp = 15;
	info.maxQp = 48;
	info.maxPictureSize = info.maxBitrate;

	ret = IMP_System_Init();
	if (ret < 0){
		IMP_LOG_ERR(TAG, "IMP_System_Init failed\n");
		goto err_system_init;
	}

	inFile = fopen("1920x1080.nv12", "rb");
	if (info.type == IMP_ENC_TYPE_HEVC)
		sprintf(path, "%s/out.h265", STREAM_FILE_PATH_PREFIX);
	else if (info.type == IMP_ENC_TYPE_AVC)
		sprintf(path, "%s/out.h264", STREAM_FILE_PATH_PREFIX);

	if (info.type != IMP_ENC_TYPE_JPEG) {
		outFile = fopen(path, "wb");
	}
	ret = IMP_Encoder_YuvInit(&h, width, height, &info);
	if ((ret < 0) || (h == NULL)) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_YuvInit failed\n");
		goto err_encoder_yuvinit;
	}

	frmSize = width*((height+15)&(~15))*3/2;
	src_buf = (uint8_t*)IMP_Encoder_VbmAlloc(frmSize, 256);
	if(src_buf == NULL) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_VbmAlloc failed\n");
		goto err_encoder_vbmalloc;
	}

	frame.width = width;
	frame.height = height;
	frame.size = frmSize;
	frame.phyAddr = (uint32_t)IMP_Encoder_VbmV2P((intptr_t)src_buf);
	frame.virAddr = (uint32_t)src_buf;
	stream.outAddr = (uint8_t *)malloc(frmSize);

	for (i = 0; i < encNum; i++) {
		if (info.type == IMP_ENC_TYPE_JPEG) {
			sprintf(path, "%s/out_%d.jpg", SNAP_FILE_PATH_PREFIX, i);
			outFile = fopen(path, "wb");
		}
		fread(src_buf, 1, width*height, inFile);
		fread(src_buf+(width*((height+15)&(~15))), 1, width*height/2, inFile);

		ret = IMP_Encoder_YuvEncode(h, frame, &stream);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_YuvEncode failed\n");
			goto err_encoder_yuvencode;
		}
		printf("\r%d encode success", i);
		fflush(stdout);
		fwrite(stream.outAddr, 1, stream.outLen, outFile);
		if (info.type == IMP_ENC_TYPE_JPEG)
			fclose(outFile);
	}
	puts("");

err_encoder_yuvencode:
	free(stream.outAddr);
	IMP_Encoder_VbmFree(src_buf);
err_encoder_vbmalloc:
	IMP_Encoder_YuvExit(h);
	if (info.type != IMP_ENC_TYPE_JPEG)
		fclose(outFile);
	fclose(inFile);
err_encoder_yuvinit:
	IMP_System_Exit();
err_system_init:

	return ret;
}
