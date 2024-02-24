/*
 * sample-Encoder-video.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>

#include "sample-common.h"

#define TAG "Sample-FrameSource"

extern struct chn_conf chn[];

/* if define DYNAMIC_SCALER, dynamic scaler function is running */
//#define DYNAMIC_SCALER

#ifdef DYNAMIC_SCALER
void *scale_framesource(void *arg)
{
    //max_w and max_h can be modified according to isp_clk,here is isp_clk:180M,fps:25,max_w:2880,max_h:1620
    int max_w = 2880;
    int max_h = 1620;
    int pic_w = SENSOR_WIDTH;
    int pic_h = SENSOR_HEIGHT;
    int ret = 0;
    int chnNum = 0;
    IMPFSChnAttr chnAttr;
    int scaler_step_w = ((max_w - pic_w) / 32 + 15) & ~15;
    int scaler_step_h = ((max_h - pic_h) / 32 + 7) & ~7;
    int scaler_dir = 1; //1:unscale, -1: downscale;
    int first_dir_judge = 1;
    int loop_time = 500;

    scaler_step_w = (scaler_step_w + 15) & ~15;
    scaler_step_h = (scaler_step_h + 7) & ~7;

    if ((scaler_step_w <= 0) || (scaler_step_h <= 0) || ((max_w - pic_w) < scaler_step_w) || ((max_h - pic_h) < scaler_step_h)) {
        IMP_LOG_ERR(TAG, "no nedd to scale frameource:scaler_step_w=%d, scaler_step_h=%d, max_w=%d, max_h=%d, pic_w=%d, pic_h=%\n", scaler_step_w, scaler_step_h, max_w, max_h, pic_w, pic_h);
        return (void *)-1;
    }

    while(loop_time-- > 0) {
        ret = IMP_FrameSource_GetChnAttr(chnNum, &chnAttr);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_FrameSource_GetChnAttr failed\n");
            return (void *)-1;
        }

        if (!chnAttr.scaler.enable) {
            chnAttr.scaler.outwidth = chnAttr.picWidth;
            chnAttr.scaler.outheight = chnAttr.picHeight;
        }

        if (first_dir_judge) {
            first_dir_judge = 0;
            scaler_dir = chnAttr.scaler.outwidth + scaler_step_w < max_w ?  1 : 0;
        }

        if ((scaler_dir == 1) && (((chnAttr.scaler.outwidth + scaler_step_w) > max_w) || ((chnAttr.scaler.outheight + scaler_step_h) > max_h))) {
            scaler_dir = -1;
        }

        if ((scaler_dir == -1) && (((chnAttr.scaler.outwidth - scaler_step_w) < pic_w) || ((chnAttr.scaler.outheight - scaler_step_h) < pic_h))) {
            scaler_dir = 1;
        }

        chnAttr.scaler.enable = 1;
        chnAttr.scaler.outwidth = chnAttr.scaler.outwidth + scaler_dir * scaler_step_w;
        chnAttr.scaler.outheight = chnAttr.scaler.outheight + scaler_dir * scaler_step_h;
        if ((chnAttr.scaler.outwidth == pic_w) && (chnAttr.scaler.outheight == pic_h)) {
            chnAttr.scaler.enable = 0;
        }

        if (!((chnAttr.scaler.outwidth >= chnAttr.crop.width) && (chnAttr.scaler.outheight >= chnAttr.crop.height))) {
            printf("%s:chnAttr.scaler.outwidth=%d, chnAttr.scaler.outheight=%d, chnAttr.crop.width=%d, chnAttr.crop.height=%d\n", __func__, chnAttr.scaler.outwidth, chnAttr.scaler.outheight, chnAttr.crop.width, chnAttr.crop.height);
            assert(chnAttr.scaler.outwidth >= chnAttr.crop.width && chnAttr.scaler.outheight >= chnAttr.crop.height);
        }

        chnAttr.crop.left = ((chnAttr.scaler.outwidth - chnAttr.crop.width) / 2 + 1) & ~1;
        chnAttr.crop.top = ((chnAttr.scaler.outheight - chnAttr.crop.height) / 2 + 1) & ~1;

        if ((chnAttr.crop.left + chnAttr.crop.width) > chnAttr.scaler.outwidth) {
            chnAttr.crop.left = 0;
        }
        if ((chnAttr.crop.top + chnAttr.crop.height) > chnAttr.scaler.outheight) {
            chnAttr.crop.top = 0;
        }

        printf("%s:loop_time=%d,scaler.outwidth=%d, scaler.outheight=%d, crop.left=%d, crop.top=%d, crop.width=%d, crop.height=%d\n", __func__, loop_time, chnAttr.scaler.outwidth, chnAttr.scaler.outheight, chnAttr.crop.left, chnAttr.crop.top, chnAttr.crop.width, chnAttr.crop.height);

        ret = IMP_FrameSource_SetChnAttr(chnNum, &chnAttr);
        if (ret < 0) {
            IMP_LOG_WARN(TAG, "IMP_FrameSource_setChnAttr failed\n");
        }

        /*
         * 500ms interval is a reasonable value, for 200ms interval which I test isn't stable
         * Another mind is that you should reboot your soc when your run the dynamic scaler test again, this may be
         * the isp status can't be start with dynamic scalered parameters
         */
        usleep(500000);
    }

    return (void *)0;
}
#endif

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

#ifdef DYNAMIC_SCALER
    pthread_t tid;
    ret = pthread_create(&tid, NULL, scale_framesource, NULL);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "pthread_create failed\n");
        return -1;
    }
    pthread_join(tid, NULL);
#endif

	/* Step.4 Get frame */
	ret = sample_get_frame();
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
