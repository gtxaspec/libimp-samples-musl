/*
 * sample-IspOsd.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <imp/imp_log.h>
#include <imp/isp_osd.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include "sample-common.h"
#include <string.h>

#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
#include "bgramapinfo.h"
#endif

#define TAG "Sample-IspOsd"

extern struct chn_conf chn[];
static int byGetFd = 0;
extern int gosd_enable;

#define BOSDMODULEEABLE 1
#define OSD_LETTER_NUM 20

uint32_t *timeStampData ;
IMPOSDRgnAttr rIspOsdAttr;
char path[128] = "/mnt/res/64x64_2.rgba";
char *g_pdata = NULL;
FILE *g_fp = NULL;
static int g_main_timehandle = -1;
static int g_main_pichandle = -1;
#ifdef SECSENSOR
static int g_sec_timehandle = -1;
static int g_sec_pichandle = -1;
#endif

static void update_time(void *p)
{
	int ret;

	/*generate time*/
	char DateStr[40];
	time_t currTime;
	struct tm *currDate;
	unsigned i = 0, j = 0;
	void *dateData = NULL;
	uint32_t *data = p;

	while(1) {
		int penpos_t = 0;
		int fontadv = 0;

		time(&currTime);
		currDate = localtime(&currTime);
		memset(DateStr, 0, 40);
		strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
		for (i = 0; i < OSD_LETTER_NUM; i++) {
			switch(DateStr[i]) {
				case '0' ... '9':
					dateData = (void *)gBgramap[DateStr[i] - '0'].pdata;
					fontadv = gBgramap[DateStr[i] - '0'].width;
					penpos_t += gBgramap[DateStr[i] - '0'].width;
					break;
				case '-':
					dateData = (void *)gBgramap[10].pdata;
					fontadv = gBgramap[10].width;
					penpos_t += gBgramap[10].width;
					break;
				case ' ':
					dateData = (void *)gBgramap[11].pdata;
					fontadv = gBgramap[11].width;
					penpos_t += gBgramap[11].width;
					break;
				case ':':
					dateData = (void *)gBgramap[12].pdata;
					fontadv = gBgramap[12].width;
					penpos_t += gBgramap[12].width;
					break;
				default:
					break;
			}
#ifdef SUPPORT_RGB555LE
			for (j = 0; j < OSD_REGION_HEIGHT; j++) {
				memcpy((void *)((uint16_t *)data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
						(void *)((uint16_t *)dateData + j*fontadv), fontadv*sizeof(uint16_t));
			}
#else
			for (j = 0; j < OSD_REGION_HEIGHT; j++) {
				memcpy((void *)((uint32_t *)data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
						(void *)((uint32_t *)dateData + j*fontadv), fontadv*sizeof(uint32_t));
			}

#endif
		}

#ifdef SUPPORT_RGB555LE
		IMPIspOsdAttrAsm stISPOSDAsm;
		stISPOSDAsm.type = ISP_OSD_REG_PIC;
		stISPOSDAsm.stsinglepicAttr.sensornum = 0;
		stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_type = IMP_ISP_PIC_ARGB_1555;
		stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_argb_type = IMP_ISP_ARGB_TYPE_BGRA;
		stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_pixel_alpha_disable = IMPISP_TUNING_OPS_MODE_DISABLE;
		stISPOSDAsm.stsinglepicAttr.pic.pinum =  g_main_timehandle;
		stISPOSDAsm.stsinglepicAttr.pic.osd_enable = 1;
		stISPOSDAsm.stsinglepicAttr.pic.osd_left = 10;
		stISPOSDAsm.stsinglepicAttr.pic.osd_top = 10;
		stISPOSDAsm.stsinglepicAttr.pic.osd_width = OSD_REGION_WIDTH * OSD_LETTER_NUM;
		stISPOSDAsm.stsinglepicAttr.pic.osd_height = OSD_REGION_HEIGHT;
		stISPOSDAsm.stsinglepicAttr.pic.osd_image = (char*)data;
		stISPOSDAsm.stsinglepicAttr.pic.osd_stride = OSD_REGION_WIDTH * OSD_LETTER_NUM * 2;
#else
		IMPIspOsdAttrAsm stISPOSDAsm;
		stISPOSDAsm.type = ISP_OSD_REG_PIC;
		stISPOSDAsm.stsinglepicAttr.sensornum = 0;
		stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_type = IMP_ISP_PIC_ARGB_8888;
		stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_argb_type = IMP_ISP_ARGB_TYPE_BGRA;
		stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_pixel_alpha_disable = IMPISP_TUNING_OPS_MODE_DISABLE;
		stISPOSDAsm.stsinglepicAttr.pic.pinum =  g_main_timehandle;
		stISPOSDAsm.stsinglepicAttr.pic.osd_enable = 1;
		stISPOSDAsm.stsinglepicAttr.pic.osd_left = 10;
		stISPOSDAsm.stsinglepicAttr.pic.osd_top = 10;
		stISPOSDAsm.stsinglepicAttr.pic.osd_width = OSD_REGION_WIDTH * OSD_LETTER_NUM;
		stISPOSDAsm.stsinglepicAttr.pic.osd_height = OSD_REGION_HEIGHT;
		stISPOSDAsm.stsinglepicAttr.pic.osd_image = (char*)data;
		stISPOSDAsm.stsinglepicAttr.pic.osd_stride = OSD_REGION_WIDTH * OSD_LETTER_NUM * 4;
#ifdef SECSENSOR
		IMPIspOsdAttrAsm stISPOSDAsm1;
		stISPOSDAsm1.type = ISP_OSD_REG_PIC;
		stISPOSDAsm1.stsinglepicAttr.sensornum = 1;
		stISPOSDAsm1.stsinglepicAttr.chnOSDAttr.osd_type = IMP_ISP_PIC_ARGB_8888;
		stISPOSDAsm1.stsinglepicAttr.chnOSDAttr.osd_argb_type = IMP_ISP_ARGB_TYPE_BGRA;
		stISPOSDAsm1.stsinglepicAttr.chnOSDAttr.osd_pixel_alpha_disable = IMPISP_TUNING_OPS_MODE_DISABLE;
		stISPOSDAsm1.stsinglepicAttr.pic.pinum =  g_sec_timehandle;
		stISPOSDAsm1.stsinglepicAttr.pic.osd_enable = 1;
		stISPOSDAsm1.stsinglepicAttr.pic.osd_left = 10;
		stISPOSDAsm1.stsinglepicAttr.pic.osd_top = 10;
		stISPOSDAsm1.stsinglepicAttr.pic.osd_width = OSD_REGION_WIDTH * OSD_LETTER_NUM;
		stISPOSDAsm1.stsinglepicAttr.pic.osd_height = OSD_REGION_HEIGHT;
		stISPOSDAsm1.stsinglepicAttr.pic.osd_image = (char*)data;
		stISPOSDAsm1.stsinglepicAttr.pic.osd_stride = OSD_REGION_WIDTH * OSD_LETTER_NUM * 4;
#endif
#endif

		ret = IMP_ISP_Tuning_SetOsdRgnAttr(0, g_main_timehandle, &stISPOSDAsm);
		if(ret < 0) {
			IMP_LOG_ERR(TAG,"IMP_ISP_SetOSDAttr error\n");
			return ;
		}

		ret = IMP_ISP_Tuning_ShowOsdRgn(0, g_main_timehandle, 1);
		if(ret < 0) {
			IMP_LOG_ERR(TAG,"IMP_OSD_ShowRgn_ISP error\n");
			return ;
		}
#ifdef SECSENSOR
		ret = IMP_ISP_Tuning_SetOsdRgnAttr(1, g_sec_timehandle, &stISPOSDAsm1);
		if(ret < 0) {
			IMP_LOG_ERR(TAG,"IMP_ISP_SetOSDAttr error\n");
			return ;
		}

		ret = IMP_ISP_Tuning_ShowOsdRgn(1, g_sec_timehandle, 1);
		if(ret < 0) {
			IMP_LOG_ERR(TAG,"IMP_OSD_ShowRgn_ISP error\n");
			return ;
		}
#endif
		/*更新时间戳*/
		sleep(1);
	}

	return ;
}

int datainit()
{
	/*sample中的示例的图案宽高为64*/
	int w = 64,h = 64,size = 0,ret = 0;
	size = w*h*4;
	if((g_pdata = calloc(1,size)) == NULL){
		IMP_LOG_ERR(TAG,"[%s][%d]calloc error\n",__func__,__LINE__);
		return -1;
	}
	if((g_fp = fopen(path,"r")) == NULL){
		IMP_LOG_ERR(TAG,"[%s][%d]fopen error\n",__func__,__LINE__);
		return -1;
	}
	ret = fread(g_pdata,1,size,g_fp);
	if(ret <= 0){
		IMP_LOG_ERR(TAG,"[%s][%d]fread error\n",__func__,__LINE__);
		return -1;
	}

	return 0;
}

int datadeinit()
{
	fclose(g_fp);
	free(g_pdata);
	g_pdata = NULL;
	return 0;
}
void draw_pic(void)
{
	int ret = 0, sensornum = 0;
	IMPIspOsdAttrAsm stISPOSDAsm;
	stISPOSDAsm.type = ISP_OSD_REG_PIC;
	stISPOSDAsm.stsinglepicAttr.sensornum = 0;
	stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_type = IMP_ISP_PIC_ARGB_8888;
	stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_argb_type = IMP_ISP_ARGB_TYPE_BGRA;
	stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_pixel_alpha_disable = IMPISP_TUNING_OPS_MODE_DISABLE;
	stISPOSDAsm.stsinglepicAttr.pic.pinum =  g_main_pichandle;
	stISPOSDAsm.stsinglepicAttr.pic.osd_enable = 1;
	stISPOSDAsm.stsinglepicAttr.pic.osd_left = 100;
	stISPOSDAsm.stsinglepicAttr.pic.osd_top = 100;
	stISPOSDAsm.stsinglepicAttr.pic.osd_width = 64;
	stISPOSDAsm.stsinglepicAttr.pic.osd_height = 64;
	stISPOSDAsm.stsinglepicAttr.pic.osd_image = g_pdata;
	stISPOSDAsm.stsinglepicAttr.pic.osd_stride = 64*4;

	ret = IMP_ISP_Tuning_SetOsdRgnAttr(sensornum,  g_main_pichandle, &stISPOSDAsm);
	if(ret < 0) {
		IMP_LOG_ERR(TAG,"IMP_ISP_Tuning_SetOsdRgnAttr error\n");
		return ;
	}

	ret = IMP_ISP_Tuning_ShowOsdRgn(sensornum, g_main_pichandle, 1);
	if(ret < 0) {
		IMP_LOG_ERR(TAG,"IMP_ISP_Tuning_ShowOsdRgn error\n");
		return ;
	}

#ifdef SECSENSOR
	stISPOSDAsm.type = ISP_OSD_REG_PIC;
	stISPOSDAsm.stsinglepicAttr.sensornum = 1;
	stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_type = IMP_ISP_PIC_ARGB_8888;
	stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_argb_type = IMP_ISP_ARGB_TYPE_BGRA;
	stISPOSDAsm.stsinglepicAttr.chnOSDAttr.osd_pixel_alpha_disable = IMPISP_TUNING_OPS_MODE_DISABLE;
	stISPOSDAsm.stsinglepicAttr.pic.pinum =  g_sec_pichandle;
	stISPOSDAsm.stsinglepicAttr.pic.osd_enable = 1;
	stISPOSDAsm.stsinglepicAttr.pic.osd_left = 100;
	stISPOSDAsm.stsinglepicAttr.pic.osd_top = 100;
	stISPOSDAsm.stsinglepicAttr.pic.osd_width = 64;
	stISPOSDAsm.stsinglepicAttr.pic.osd_height = 64;
	stISPOSDAsm.stsinglepicAttr.pic.osd_image = g_pdata;
	stISPOSDAsm.stsinglepicAttr.pic.osd_stride = 64*4;

	ret = IMP_ISP_Tuning_SetOsdRgnAttr(sensornum + 1, g_sec_pichandle, &stISPOSDAsm);
	if(ret < 0) {
		IMP_LOG_ERR(TAG,"IMP_ISP_Tuning_SetOsdRgnAttr error\n");
		return ;
	}

	ret = IMP_ISP_Tuning_ShowOsdRgn(sensornum + 1,  g_sec_pichandle, 1);
	if(ret < 0) {
		IMP_LOG_ERR(TAG,"IMP_ISP_Tuning_ShowOsdRgn error\n");
		return ;
	}
#endif
}

void ISPOSDDraw(IMPOsdRgnType type)
{
	int ret = 0;
	memset(&rIspOsdAttr, 0, sizeof(IMPOSDRgnAttr));

	if(OSD_REG_ISP_LINE_RECT == type)
	{
		rIspOsdAttr.type = OSD_REG_ISP_LINE_RECT;
		rIspOsdAttr.osdispdraw.stDrawAttr.pinum = 0;
		rIspOsdAttr.osdispdraw.stDrawAttr.type = IMP_ISP_DRAW_LINE;
		rIspOsdAttr.osdispdraw.stDrawAttr.color_type = IMPISP_MASK_TYPE_YUV;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.enable = 1;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.startx = 700;	/*绘制竖线*/
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.starty = 200;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.endx = 700;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.endy = 600;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.color.ayuv.y_value = 100;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.color.ayuv.u_value = 100;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.color.ayuv.v_value = 100;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.width = 5;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.line.alpha = 4; /*范围为[0,4],数值越小越透明*/

		ret = IMP_OSD_SetRgnAttr_ISP(&rIspOsdAttr, 0);
		if(ret < 0){
			IMP_LOG_ERR(TAG,"[%s][%d]IMP_OSD_SetRgnAttr_ISP err\n",__func__,__LINE__);
		}

#ifdef SECSENSOR
		ret = IMP_OSD_SetRgnAttr_ISP_Sec(&rIspOsdAttr, 0);
		if(ret < 0){
			IMP_LOG_ERR(TAG,"[%s][%d]IMP_OSD_SetRgnAttr_ISP err\n",__func__,__LINE__);
		}
#endif
	}

	if(OSD_REG_ISP_LINE_RECT == type)
	{
		rIspOsdAttr.type = OSD_REG_ISP_LINE_RECT;
		rIspOsdAttr.osdispdraw.stDrawAttr.pinum = 1;
		rIspOsdAttr.osdispdraw.stDrawAttr.type = IMP_ISP_DRAW_WIND;
		rIspOsdAttr.osdispdraw.stDrawAttr.color_type = IMPISP_MASK_TYPE_YUV;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.enable = 1;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.left = 900;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.top = 200;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.width = 300;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.height = 300;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.color.ayuv.y_value = 0;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.color.ayuv.u_value = 255;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.color.ayuv.v_value = 0;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.line_width = 3;
		rIspOsdAttr.osdispdraw.stDrawAttr.cfg.wind.alpha = 4; /*范围为[0,4]*/

		ret = IMP_OSD_SetRgnAttr_ISP(&rIspOsdAttr, 0);
		if(ret < 0){
			IMP_LOG_ERR(TAG,"[%s][%d]IMP_OSD_SetRgnAttr_ISP err\n",__func__,__LINE__);
		}

#ifdef SECSENSOR
		ret = IMP_OSD_SetRgnAttr_ISP_Sec(&rIspOsdAttr, 0);
		if(ret < 0){
			IMP_LOG_ERR(TAG,"[%s][%d]IMP_OSD_SetRgnAttr_ISP err\n",__func__,__LINE__);
		}
#endif
	}

	if(OSD_REG_ISP_COVER == type)
	{
		rIspOsdAttr.type = OSD_REG_ISP_COVER;
		rIspOsdAttr.osdispdraw.stCoverAttr.chx = 0;
		rIspOsdAttr.osdispdraw.stCoverAttr.pinum = 0;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_en = 1;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_pos_top	= 300;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_pos_left = 300;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_width = 300;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_height = 300;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_type = IMPISP_MASK_TYPE_RGB;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_value.argb.r_value = 0;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_value.argb.g_value = 0;
		rIspOsdAttr.osdispdraw.stCoverAttr.mask_value.argb.b_value = 255;

		ret = IMP_OSD_SetRgnAttr_ISP(&rIspOsdAttr, 0);
		if(ret < 0){
			IMP_LOG_ERR(TAG,"[%s][%d]IMP_OSD_SetRgnAttr_ISP err\n",__func__,__LINE__);
		}

#ifdef SECSENSOR
		ret = IMP_OSD_SetRgnAttr_ISP_Sec(&rIspOsdAttr, 0);
		if(ret < 0){
			IMP_LOG_ERR(TAG,"[%s][%d]IMP_OSD_SetRgnAttr_ISP err\n",__func__,__LINE__);
		}
#endif
	}

	return ;
}

void* ISPOSD(void *arg)
{
	/*绘制线、框、矩形遮挡*/
	ISPOSDDraw(OSD_REG_ISP_LINE_RECT);
	ISPOSDDraw(OSD_REG_ISP_COVER);

	/*绘制图片，注意绘制图片类型的ISP接口和绘制线、框、矩形遮挡的接口有所区别*/
	draw_pic();

	/*绘制时间戳*/
	update_time(timeStampData);

	return NULL;
}

int sample_osd_init_isp(void)
{
	int sensorNum = 0,ret = 0;
	ret = datainit();
	if(ret < 0){
		IMP_LOG_ERR(TAG,"[%s][%d]datainit err\n",__func__,__LINE__);
		return -1;
	}

	g_main_timehandle = IMP_ISP_Tuning_CreateOsdRgn(sensorNum, NULL);
	if(g_main_timehandle < 0)
	{
		IMP_LOG_ERR(TAG,"[%s][%d]IMP_ISP_Tuning_CreateOsdRgn err\n",__func__,__LINE__);
		return -1;
	}

	g_main_pichandle = IMP_ISP_Tuning_CreateOsdRgn(sensorNum, NULL);
	if(g_main_pichandle < 0)
	{
		IMP_LOG_ERR(TAG,"[%s][%d]IMP_ISP_Tuning_CreateOsdRgn err\n",__func__,__LINE__);
		return -1;
	}
#ifdef SECSENSOR
	g_sec_pichandle = IMP_ISP_Tuning_CreateOsdRgn(sensorNum + 1, NULL);
	if(g_sec_pichandle < 0)
	{
		IMP_LOG_ERR(TAG,"[%s][%d]IMP_ISP_Tuning_CreateOsdRgn err\n",__func__,__LINE__);
		return -1;
	}
	g_sec_timehandle = IMP_ISP_Tuning_CreateOsdRgn(sensorNum + 1, NULL);
	if(g_sec_timehandle < 0)
	{
		IMP_LOG_ERR(TAG,"[%s][%d]IMP_ISP_Tuning_CreateOsdRgn err\n",__func__,__LINE__);
		return -1;
	}
#endif
	return ret;
}
int sample_osd_exit_isp()
{
	int sensorNum = 0,showflg = 0,ret = 0;
	ret = datadeinit();

	IMP_ISP_Tuning_ShowOsdRgn(sensorNum,g_main_timehandle,showflg);
	IMP_ISP_Tuning_DestroyOsdRgn(sensorNum,g_main_timehandle);

	IMP_ISP_Tuning_ShowOsdRgn(sensorNum,g_main_pichandle,showflg);
	IMP_ISP_Tuning_DestroyOsdRgn(sensorNum,g_main_pichandle);

#ifdef SECSENSOR
	IMP_ISP_Tuning_ShowOsdRgn(sensorNum+1,g_sec_timehandle,showflg);
	IMP_ISP_Tuning_DestroyOsdRgn(sensorNum+1,g_sec_timehandle);

	IMP_ISP_Tuning_ShowOsdRgn(sensorNum + 1,g_sec_pichandle,showflg);
	IMP_ISP_Tuning_DestroyOsdRgn(sensorNum + 1,g_sec_pichandle);
#endif

	return ret;
}

int main(int argc, char *argv[])
{
	int i, ret;

    if (argc >= 2) {
        byGetFd = atoi(argv[1]);
    }

#ifdef SECSENSOR
	chn[3].enable = 1;
#endif

    gosd_enable = 2;
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

	ret = sample_osd_init_isp();//只显示主码流的时间戳区域
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample osd init isp failed\n");
		return -1;
	}

#ifdef SUPPORT_RGB555LE
		timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint16_t));
#else
		timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
#endif

	/* Step.5 Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

	pthread_t tid;
	pthread_create(&tid,NULL,ISPOSD,NULL);

	/* Step.6 Get stream */
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
	sample_osd_exit_isp();//销毁主码流的时间戳区域

	pthread_cancel(tid);
	pthread_join(tid, NULL);
	free(timeStampData);

	/* Exit sequence as follow */
	/* Step.7 Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	/* Step.8 UnBind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	/* Step.9 Encoder exit */
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

	/* Step.10 FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.11 System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
