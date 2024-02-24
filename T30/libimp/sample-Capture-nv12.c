/*
 * sample-Encoder-video.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>

#include "sample-common.h"

#define TAG "Sample-FrameSource"

#define CAPTURE_NV12_FRAME 1

extern struct chn_conf chn[];



static int  get_nv12_frame(void )
{
	int i = 0, ret = 0;
    int k = 0;
	IMPFrameInfo *frame = NULL;
	char framefilename[64];
	int fd = -1;
    for (k = 0; k < 3; k++){
	    if (PIX_FMT_NV12 == chn[k].fs_chn_attr.pixFmt) {
		    sprintf(framefilename, "frame%dx%d.nv12", chn[k].fs_chn_attr.picWidth, chn[k].fs_chn_attr.picHeight);
	    } else {
		    sprintf(framefilename, "frame%dx%d.raw", chn[k].fs_chn_attr.picWidth, chn[k].fs_chn_attr.picHeight);
	    }

	    fd = open(framefilename, O_RDWR | O_CREAT, 0x644);
	    if (fd < 0) {
		    IMP_LOG_ERR(TAG, "open %s failed:%s\n", framefilename, strerror(errno));
		    goto err_open_framefilename;
	    }

	    ret = IMP_FrameSource_SetFrameDepth(k, 1);
	    if (ret < 0) {
		    IMP_LOG_ERR(TAG, "IMP_FrameSource_SetFrameDepth(%d,%d) failed\n", k, chn[k].fs_chn_attr.nrVBs * 2);
		    goto err_IMP_FrameSource_SetFrameDepth_1;
	    }

	    for (i = 0; i < CAPTURE_NV12_FRAME; i++) {
		    ret = IMP_FrameSource_GetFrame(k, &frame);
		    if (ret < 0) {
			    IMP_LOG_ERR(TAG, "IMP_FrameSource_GetFrame(%d) i=%d failed\n", k, i);
			    goto err_IMP_FrameSource_GetFrame_i;
		    }
            sleep(2);
		    if (write(fd, (void *)frame->virAddr, frame->size) != frame->size) {
			    IMP_LOG_ERR(TAG, "chnNum=%d write frame i=%d failed\n", k, i);
			    goto err_write_frame;
		    }
		    ret = IMP_FrameSource_ReleaseFrame(k, frame);
		    if (ret < 0) {
			    IMP_LOG_ERR(TAG, "IMP_FrameSource_ReleaseFrame(%d) i=%d failed\n", k, i);
			    goto err_IMP_FrameSource_ReleaseFrame_i;
		    }
	    }

	    IMP_FrameSource_SetFrameDepth(k, 0);
    }
	return 0;

err_IMP_FrameSource_ReleaseFrame_i:
err_write_frame:
	IMP_FrameSource_ReleaseFrame(k, frame);
err_IMP_FrameSource_GetFrame_i:
	goto err_IMP_FrameSource_SetFrameDepth_1;
	IMP_FrameSource_SetFrameDepth(k, 0);
err_IMP_FrameSource_SetFrameDepth_1:
	close(fd);
err_open_framefilename:
	return -1;
}


int main(int argc, char *argv[])
{
	int ret;

	printf("Usage:%s [fpsnum0 [fpsnum1]]]\n", argv[0]);

	if (argc >= 2) {
		chn[0].fs_chn_attr.outFrmRateNum = atoi(argv[1]);
	}

	if (argc >= 3) {
		chn[1].fs_chn_attr.outFrmRateNum = atoi(argv[2]);
	}

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

	/* Step.3 Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

	/* Step.4 Get frame */
	ret = get_nv12_frame();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get frame failed\n");
		return -1;
	}

	/* Exit sequence as follow */

	/* Step.5 Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	/* Step.6 FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.7 System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
