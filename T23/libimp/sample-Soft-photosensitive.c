/*
 * sample_Soft_photosensitive.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 *
 * The specific explanations for all the APIs called in this file can be found in the header files under the directory "proj/sdk-lv3/include/api/cn/imp/".
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_isp.h>

#include "sample-common.h"

#define TAG "Sample_Soft_photosensitive"

extern struct chn_conf chn[];
static int byGetFd = 0;
static int  g_soft_ps_running = 1;
//#define SOFT_PHOTOSENSITIVE_DEBUG


int sample_SetIRCUT(int enable)
{
	int fd, fd79, fd80;
	char on[4], off[4];

	if (!access("/tmp/setir",0)) {
		if (enable) {
			system("/tmp/setir 0 1");
		} else {
			system("/tmp/setir 1 0");
		}
		return 0;
	}

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if(fd < 0) {
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/export error !");
		return -1;
	}

	write(fd, "79", 2);
	write(fd, "80", 2);

	close(fd);

	fd79 = open("/sys/class/gpio/gpio79/direction", O_RDWR);
	if(fd79 < 0) {
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio79/direction error !");
		return -1;
	}

	fd80 = open("/sys/class/gpio/gpio80/direction", O_RDWR);
	if(fd80 < 0) {
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio80/direction error !");
		return -1;
	}

	write(fd79, "out", 3);
	write(fd80, "out", 3);

	close(fd79);
	close(fd80);

	fd79 = open("/sys/class/gpio/gpio79/active_low", O_RDWR);
	if(fd79 < 0) {
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio79/active_low error !");
		return -1;
	}

	fd80 = open("/sys/class/gpio/gpio80/active_low", O_RDWR);
	if(fd80 < 0) {
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio80/active_low error !");
		return -1;
	}

	write(fd79, "0", 1);
	write(fd80, "0", 1);

	close(fd79);
	close(fd80);

	fd79 = open("/sys/class/gpio/gpio79/value", O_RDWR);
	if(fd79 < 0) {
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio79/value error !");
		return -1;
	}

	fd80 = open("/sys/class/gpio/gpio80/value", O_RDWR);
	if(fd80 < 0) {
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio80/value error !");
		return -1;
	}

	sprintf(on, "%d", enable);
	sprintf(off, "%d", !enable);

	write(fd79, "0", 1);
	usleep(10*1000);

	write(fd79, on, strlen(on));
	write(fd80, off, strlen(off));

	if (!enable) {
		usleep(10*1000);
		write(fd79, off, strlen(off));
	}

	close(fd79);
	close(fd80);

	return 0;
}


void *sample_soft_photosensitive_ctrl(void *p)
{
	int i = 0;
	float gb_gain,gr_gain;
	float iso_buf;
	bool ircut_status = true;
	g_soft_ps_running = 1;
	int night_count = 0;
	int day_count = 0;
	//int day_oth_count = 0;
	//bayer域的 (g分量/b分量) 统计值
	float gb_gain_record = 200;
	float gr_gain_record = 200;
	float gb_gain_buf = 200, gr_gain_buf = 200;
	IMPISPRunningMode pmode;
	IMPISPEVAttr ExpAttr;
	IMPISPWB wb;
	IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_DAY);
	sample_SetIRCUT(1);

	while (g_soft_ps_running) {
		//获取曝光AE信息
		int ret = IMP_ISP_Tuning_GetEVAttr(&ExpAttr);
		if (ret ==0) {
			printf("u32ExposureTime: %d\n", ExpAttr.ev);
			printf("u32AnalogGain: %d\n", ExpAttr.again);
			printf("u32DGain: %d\n", ExpAttr.dgain);
		} else {
			return NULL;
        }
		iso_buf = ExpAttr.ev;
		printf(" iso buf ==%f\n",iso_buf);
		ret = IMP_ISP_Tuning_GetWB_Statis(&wb);
		if (ret == 0) {
			gr_gain =wb.rgain;
			gb_gain =wb.bgain;
			// printf("gb_gain: %f\n", gb_gain);
			// printf("gr_gain: %f\n", gr_gain);
			// printf("gr_gain_record: %f\n", gr_gain_record);
		} else {
			return NULL;
        }

		//平均亮度小于20，则切到夜视模式
		if (iso_buf >1900000) {
			night_count++;
			printf("night_count==%d\n",night_count);
			if (night_count>5) {
				IMP_ISP_Tuning_GetISPRunningMode(&pmode);
				if (pmode!=IMPISP_RUNNING_MODE_NIGHT) {
					printf("### entry night mode ###\n");
					IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_NIGHT);
					sample_SetIRCUT(0);
					ircut_status = true;
				}
				//切夜视后，取20个gb_gain的的最小值，作为切换白天的参考数值gb_gain_record ，gb_gain为bayer的G/B
				for (i=0; i<20; i++) {
					IMP_ISP_Tuning_GetWB_Statis(&wb);
					gr_gain =wb.rgain;
					gb_gain =wb.bgain;
					if (i==0) {
						gb_gain_buf = gb_gain;
						gr_gain_buf = gr_gain;
					}
					gb_gain_buf = ((gb_gain_buf>gb_gain)?gb_gain:gb_gain_buf);
					gr_gain_buf = ((gr_gain_buf>gr_gain)?gr_gain:gr_gain_buf);
					usleep(300000);
					gb_gain_record = gb_gain_buf;
					gr_gain_record = gr_gain_buf;
					// printf("gb_gain == %f,iso_buf=%f",gb_gain,iso_buf);
					// printf("gr_gain_record == %f\n ",gr_gain_record);
				}
			}
		} else {
			night_count = 0;
        }
		//满足这3个条件进入白天切换判断条件
		if (((int)iso_buf < 479832) &&(ircut_status == true) &&(gb_gain>gb_gain_record+15)) {
			if ((iso_buf<361880)||(gb_gain >145)) {
				day_count++;
            } else {
				day_count=0;
            }
			// printf("gr_gain_record == %f gr_gain =%f line=%d\n",gr_gain_record,gr_gain,__LINE__);
			// printf("day_count == %d\n",day_count);
			if (day_count>3) {
				printf("### entry day mode ###\n");
				IMP_ISP_Tuning_GetISPRunningMode(&pmode);
				if (pmode!=IMPISP_RUNNING_MODE_DAY) {
					IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_DAY);
					sample_SetIRCUT(1);
					ircut_status = false;
				}
			}
		} else {
            day_count = 0;
        }
		sleep(1);
	}
	return NULL;
}


pthread_t tid[FS_CHN_NUM];
int main(int argc, char *argv[])
{
	int i, ret;

    if (argc >= 2) {
        byGetFd = atoi(argv[1]);
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

	/* Step.6 Create soft photosensitive thread */
	ret = pthread_create(&tid[i], NULL, sample_soft_photosensitive_ctrl, (void *)argv);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Create sample_soft_photosensitive_thread failed\n");
	}

	/* Step.7 Get stream */
    if (byGetFd) {
        ret = sample_get_video_stream_byfd();
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "Get video stream byfd failed\n");
            return -1;
        }
    } else {
        ret = sample_get_video_stream();
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "Get video stream failed\n");
            return -1;
        }
    }

	/* Exit sequence as follow */
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

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_Encoder_DestroyGroup(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[i].index);
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

	/* Step.e System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
