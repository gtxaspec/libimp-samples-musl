/*
 * sample-OSD.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_osd.h>
#include <imp/imp_utils.h>

#include "sample-common.h"
#include "logodata_100x100_bgra.h"

#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
#include "bitmapinfo.h"
#endif

#define TAG "Sample-OSD"

#define OSD_LETTER_NUM 20

extern struct chn_conf chn[];
static int byGetFd = 0;
extern int gosd_enable;

int grpNum = 0;
IMPRgnHandle g_prHander[2][5];

static int osd_show(void)
{
	int ret, i;

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_OSD_ShowRgn(g_prHander[i][0], i, 1);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() timeStamp error\n");
				return -1;
			}
			ret = IMP_OSD_ShowRgn(g_prHander[i][1], i, 1);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Logo error\n");
				return -1;
			}
			ret = IMP_OSD_ShowRgn(g_prHander[i][2], i, 1);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
				return -1;
			}
			ret = IMP_OSD_ShowRgn(g_prHander[i][3], i, 1);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Rect error\n");
				return -1;
			}
			ret = IMP_OSD_ShowRgn(g_prHander[i][4], i, 1);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Line error\n");
				return -1;
			}
		}
	}

	return 0;
}

static void *update_thread(void *p)
{
	int ret;

	/*generate time*/
	char DateStr[40];
	time_t currTime;
	struct tm *currDate;
	unsigned i = 0, j = 0;
	void *dateData = NULL;
	char *data = NULL;
	IMPOSDRgnAttrData rAttrData;
	IMPOSDRgnAttr rAttr;

	ret = osd_show();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD show error\n");
		return NULL;
	}

	while(1) {
		int penpos = 0;
		int penpos_t = 0;
		int fontadv = 0;
		unsigned int len = 0;

		ret = IMP_OSD_GetRgnAttr(g_prHander[0][1], &rAttr);

		data = p;
		time(&currTime);
		currDate = localtime(&currTime);
		memset(DateStr, 0, 40);
		strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
		len = strlen(DateStr);
		for (i = 0; i < len; i++) {
			switch(DateStr[i]) {
				case '0' ... '9':
#ifdef SUPPORT_COLOR_REVERSE
					if(rAttr.fontData.colType[i] == 1) {
						dateData = (void *)gBitmap_black[DateStr[i] - '0'].pdata;
					} else {
						dateData = (void *)gBitmap[DateStr[i] - '0'].pdata;
					}
#else
					dateData = (void *)gBitmap[DateStr[i] - '0'].pdata;
#endif
					fontadv = gBitmap[DateStr[i] - '0'].width;
					penpos_t += gBitmap[DateStr[i] - '0'].width;
					break;
				case '-':
#ifdef SUPPORT_COLOR_REVERSE
					if(rAttr.fontData.colType[i] == 1) {
						dateData = (void *)gBitmap_black[10].pdata;
					} else {
						dateData = (void *)gBitmap[10].pdata;
					}
#else
					dateData = (void *)gBitmap[10].pdata;
#endif
					fontadv = gBitmap[10].width;
					penpos_t += gBitmap[10].width;
					break;
				case ' ':
					dateData = (void *)gBitmap[11].pdata;
					fontadv = gBitmap[11].width;
					penpos_t += gBitmap[11].width;
					break;
				case ':':
#ifdef SUPPORT_COLOR_REVERSE
					if(rAttr.fontData.colType[i] == 1) {
						dateData = (void *)gBitmap_black[12].pdata;
					} else {
						dateData = (void *)gBitmap[12].pdata;
					}
#else
					dateData = (void *)gBitmap[12].pdata;
#endif
					fontadv = gBitmap[12].width;
					penpos_t += gBitmap[12].width;
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
			for (j = 0; j < gBitmapHight; j++) {
				memcpy((void *)(data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos),
						(void *)(dateData + j*fontadv), fontadv);
			}
			penpos = penpos_t;
#endif
		}

		rAttrData.bitmapData = data;

		for (i = 0; i < FS_CHN_NUM; i++) {
			if (chn[i].enable) {
				IMP_OSD_UpdateRgnAttrData(g_prHander[i][0], &rAttrData);
			}
		}

		sleep(1);
	}

	return NULL;
}

static int sample_osd_init(int grpNum)
{
	int ret = 0;
	IMPRgnHandle rHanderFont = 0;
	IMPRgnHandle rHanderLogo = 0;
	IMPRgnHandle rHanderCover = 0;
	IMPRgnHandle rHanderRect = 0;
	IMPRgnHandle rHanderLine = 0;

	rHanderFont = IMP_OSD_CreateRgn(NULL);
	if (rHanderFont == INVHANDLE) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateRgn TimeStamp error !\n");
		return -1;
	}

	//query osd rgn create status
	IMPOSDRgnCreateStat stStatus;
	memset(&stStatus,0x0,sizeof(IMPOSDRgnCreateStat));
	ret = IMP_OSD_RgnCreate_Query(rHanderFont,&stStatus);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_OSD_RgnCreate_Query error !\n");
		return -1;
	}

	rHanderLogo = IMP_OSD_CreateRgn(NULL);
	if (rHanderLogo == INVHANDLE) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateRgn Logo error !\n");
		return -1;
	}

	rHanderCover = IMP_OSD_CreateRgn(NULL);
	if (rHanderCover == INVHANDLE) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateRgn Cover error !\n");
		return -1;
	}

	rHanderRect = IMP_OSD_CreateRgn(NULL);
	if (rHanderRect == INVHANDLE) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateRgn Rect error !\n");
		return -1;
	}

	rHanderLine = IMP_OSD_CreateRgn(NULL);
	if (rHanderLine == INVHANDLE) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateRgn Line error !\n");
		return -1;
	}

	ret = IMP_OSD_RegisterRgn(rHanderFont, grpNum, NULL);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IVS IMP_OSD_RegisterRgn failed\n");
		return -1;
	}

	//query osd rgn register status
	IMPOSDRgnRegisterStat stRigStatus;
	memset(&stRigStatus,0x0,sizeof(IMPOSDRgnRegisterStat));
	ret = IMP_OSD_RgnRegister_Query(rHanderFont, grpNum,&stRigStatus);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_RgnRegister_Query failed\n");
		return -1;
	}

	ret = IMP_OSD_RegisterRgn(rHanderLogo, grpNum, NULL);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IVS IMP_OSD_RegisterRgn failed\n");
		return -1;
	}

	ret = IMP_OSD_RegisterRgn(rHanderCover, grpNum, NULL);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IVS IMP_OSD_RegisterRgn failed\n");
		return -1;
	}

	ret = IMP_OSD_RegisterRgn(rHanderRect, grpNum, NULL);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IVS IMP_OSD_RegisterRgn failed\n");
		return -1;
	}

	ret = IMP_OSD_RegisterRgn(rHanderLine, grpNum, NULL);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IVS IMP_OSD_RegisterRgn failed\n");
		return -1;
	}

	/* Font */
	IMPOSDRgnAttr rAttrFont;
	memset(&rAttrFont, 0, sizeof(IMPOSDRgnAttr));
	rAttrFont.type = OSD_REG_BITMAP;
	rAttrFont.rect.p0.x = 10;
	rAttrFont.rect.p0.y = 10;
	rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + 20 * OSD_REGION_WIDTH - 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
	rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_REGION_HEIGHT - 1;
#ifdef SUPPORT_RGB555LE
	rAttrFont.fmt = PIX_FMT_RGB555LE;
#else
	rAttrFont.fmt = PIX_FMT_MONOWHITE;
#endif
	rAttrFont.data.bitmapData = NULL;
	ret = IMP_OSD_SetRgnAttr(rHanderFont, &rAttrFont);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetRgnAttr TimeStamp error !\n");
		return -1;
	}
	IMPOSDGrpRgnAttr grAttrFont;
	if (IMP_OSD_GetGrpRgnAttr(rHanderFont, grpNum, &grAttrFont) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_GetGrpRgnAttr Logo error !\n");
		return -1;
	}
	memset(&grAttrFont, 0, sizeof(IMPOSDGrpRgnAttr));
	grAttrFont.show = 0;
	/* Disable Font global alpha, only use pixel alpha. */
	grAttrFont.gAlphaEn = 1;
	grAttrFont.fgAlhpa = 0xff;
	grAttrFont.layer = 3;
	if (IMP_OSD_SetGrpRgnAttr(rHanderFont, grpNum, &grAttrFont) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetGrpRgnAttr Logo error !\n");
		return -1;
	}

	/* Logo */
	IMPOSDRgnAttr rAttrLogo;
	memset(&rAttrLogo, 0, sizeof(IMPOSDRgnAttr));
	int picw = 100;
	int pich = 100;
	rAttrLogo.type = OSD_REG_PIC;
	rAttrLogo.rect.p0.x = 0;
	rAttrLogo.rect.p0.y = 100;

	//p0 is start，and p1 well be epual p0+width(or heigth)-1
	rAttrLogo.rect.p1.x = rAttrLogo.rect.p0.x+picw-1;
	rAttrLogo.rect.p1.y = rAttrLogo.rect.p0.y+pich-1;
	rAttrLogo.fmt = PIX_FMT_BGRA;
	rAttrLogo.data.picData.pData = logodata_100x100_bgra;
	ret = IMP_OSD_SetRgnAttr(rHanderLogo, &rAttrLogo);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetRgnAttr Logo error !\n");
		return -1;
	}
	IMPOSDGrpRgnAttr grAttrLogo;
	if (IMP_OSD_GetGrpRgnAttr(rHanderLogo, grpNum, &grAttrLogo) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_GetGrpRgnAttr Logo error !\n");
		return -1;
	}
	memset(&grAttrLogo, 0, sizeof(IMPOSDGrpRgnAttr));
	grAttrLogo.show = 0;
	/* Set Logo global alpha to 0x7f, it is semi-transparent. */
	grAttrLogo.gAlphaEn = 1;
	grAttrLogo.fgAlhpa = 0x7f;
	grAttrLogo.layer = 2;
	if (IMP_OSD_SetGrpRgnAttr(rHanderLogo, grpNum, &grAttrLogo) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetGrpRgnAttr Logo error !\n");
		return -1;
	}

	/* Cover */
	IMPOSDRgnAttr rAttrCover;
	memset(&rAttrCover, 0, sizeof(IMPOSDRgnAttr));
	rAttrCover.type = OSD_REG_COVER;
	rAttrCover.rect.p0.x = 100;
	rAttrCover.rect.p0.y = 100;
	rAttrCover.rect.p1.x = rAttrCover.rect.p0.x+150 -1;
	rAttrCover.rect.p1.y = rAttrCover.rect.p0.y+150 -1 ;
	rAttrCover.fmt = PIX_FMT_BGRA;
	rAttrCover.data.coverData.color = OSD_IPU_RED;
	ret = IMP_OSD_SetRgnAttr(rHanderCover, &rAttrCover);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetRgnAttr Cover error !\n");
		return -1;
	}
	IMPOSDGrpRgnAttr grAttrCover;
	if (IMP_OSD_GetGrpRgnAttr(rHanderCover, grpNum, &grAttrCover) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_GetGrpRgnAttr Cover error !\n");
		return -1;
	}
	memset(&grAttrCover, 0, sizeof(IMPOSDGrpRgnAttr));
	grAttrCover.show = 0;
	/* Disable Cover global alpha, it is absolutely no transparent. */
	grAttrCover.gAlphaEn = 1;
	grAttrCover.fgAlhpa = 0x7f;
	grAttrCover.layer = 2;
	if (IMP_OSD_SetGrpRgnAttr(rHanderCover, grpNum, &grAttrCover) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetGrpRgnAttr Cover error !\n");
		return -1;
	}

	/* Rect */
	IMPOSDRgnAttr rAttrRect;
	memset(&rAttrRect, 0, sizeof(IMPOSDRgnAttr));
	rAttrRect.type = OSD_REG_RECT;
	rAttrRect.rect.p0.x = 250;
	rAttrRect.rect.p0.y = 100;
	rAttrRect.rect.p1.x = rAttrRect.rect.p0.x + 100 - 1;
	rAttrRect.rect.p1.y = rAttrRect.rect.p0.y + 100 - 1;
	rAttrRect.fmt = PIX_FMT_MONOWHITE;
	rAttrRect.data.lineRectData.color = OSD_YELLOW;
	rAttrRect.data.lineRectData.linewidth = 5;
	ret = IMP_OSD_SetRgnAttr(rHanderRect, &rAttrRect);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetRgnAttr Rect error !\n");
		return -1;
	}
	IMPOSDGrpRgnAttr grAttrRect;
	if (IMP_OSD_GetGrpRgnAttr(rHanderRect, grpNum, &grAttrRect) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_GetGrpRgnAttr Rect error !\n");
		return -1;
	}
	memset(&grAttrRect, 0, sizeof(IMPOSDGrpRgnAttr));
	grAttrRect.show = 0;
	grAttrRect.layer = 1;
	grAttrRect.scalex = 1;
	grAttrRect.scaley = 1;
	if (IMP_OSD_SetGrpRgnAttr(rHanderRect, grpNum, &grAttrRect) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetGrpRgnAttr Rect error !\n");
		return -1;
	}

	/* Line */
	IMPOSDRgnAttr rAttrLine;
	memset(&rAttrLine, 0, sizeof(IMPOSDRgnAttr));
	rAttrLine.type = OSD_REG_HORIZONTAL_LINE;
	rAttrLine.line.p0.x = 350;
	rAttrLine.line.p0.y = 100;
	rAttrLine.data.lineRectData.color = OSD_RED;
	rAttrLine.data.lineRectData.linewidth = 5;
	rAttrLine.data.lineRectData.linelength = 200;
	ret = IMP_OSD_SetRgnAttr(rHanderLine, &rAttrLine);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetRgnAttr Line error !\n");
		return -1;
	}
	IMPOSDGrpRgnAttr grAttrLine;
	if (IMP_OSD_GetGrpRgnAttr(rHanderLine, grpNum, &grAttrLine) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_GetGrpRgnAttr Line error !\n");
		return -1;
	}
	memset(&grAttrLine, 0, sizeof(IMPOSDGrpRgnAttr));
	grAttrLine.show = 0;
	grAttrLine.layer = 1;
	grAttrLine.scalex = 1;
	grAttrLine.scaley = 1;
	if (IMP_OSD_SetGrpRgnAttr(rHanderLine, grpNum, &grAttrLine) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetGrpRgnAttr Line error !\n");
		return -1;
	}

	ret = IMP_OSD_Start(grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_Start TimeStamp, Logo, Cover and Rect error !\n");
		return -1;
	}

	g_prHander[grpNum][0] = rHanderFont;
	g_prHander[grpNum][1] = rHanderLogo;
	g_prHander[grpNum][2] = rHanderCover;
	g_prHander[grpNum][3] = rHanderRect;
	g_prHander[grpNum][4] = rHanderLine;

	return 0;
}

int sample_osd_exit(int grpNum)
{
	int ret;

	ret = IMP_OSD_ShowRgn(g_prHander[grpNum][0], grpNum, 0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn close timeStamp error\n");
	}
	ret = IMP_OSD_ShowRgn(g_prHander[grpNum][0], grpNum, 0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn close timeStamp error\n");
	}
	ret = IMP_OSD_ShowRgn(g_prHander[grpNum][0], grpNum, 0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn close timeStamp error\n");
	}
	ret = IMP_OSD_ShowRgn(g_prHander[grpNum][0], grpNum, 0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn close timeStamp error\n");
	}
	ret = IMP_OSD_ShowRgn(g_prHander[grpNum][0], grpNum, 0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn close timeStamp error\n");
	}

	ret = IMP_OSD_UnRegisterRgn(g_prHander[grpNum][0], grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_UnRegisterRgn timeStamp error\n");
	}
	ret = IMP_OSD_UnRegisterRgn(g_prHander[grpNum][1], grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_UnRegisterRgn timeStamp error\n");
	}
	ret = IMP_OSD_UnRegisterRgn(g_prHander[grpNum][2], grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_UnRegisterRgn timeStamp error\n");
	}
	ret = IMP_OSD_UnRegisterRgn(g_prHander[grpNum][3], grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_UnRegisterRgn timeStamp error\n");
	}
	ret = IMP_OSD_UnRegisterRgn(g_prHander[grpNum][4], grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_UnRegisterRgn timeStamp error\n");
	}

	IMP_OSD_DestroyRgn(g_prHander[grpNum][0]);
	IMP_OSD_DestroyRgn(g_prHander[grpNum][1]);
	IMP_OSD_DestroyRgn(g_prHander[grpNum][2]);
	IMP_OSD_DestroyRgn(g_prHander[grpNum][3]);
	IMP_OSD_DestroyRgn(g_prHander[grpNum][4]);

	ret = IMP_OSD_DestroyGroup(grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_DestroyGroup(0) error\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int i, ret;

	if (argc >= 2) {
		byGetFd = atoi(argv[1]);
	}

	/* only show OSD in chn0 */
	chn[0].enable = 0;
	chn[1].enable = 1;
	chn[2].enable = 0;
	chn[3].enable = 0;

	gosd_enable = 1;
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

	/* Step.4 OSD init */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			if (IMP_OSD_CreateGroup(i) < 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_CreateGroup(%d) error !\n", grpNum);
				return -1;
			}
		}
	}


	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = sample_osd_init(i);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "OSD init failed\n");
				return -1;
			}
		}
	}

	IMPCell osdcell;
	/* Step.5 Bind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			osdcell.deviceID = DEV_ID_OSD;
			osdcell.groupID = i;
			osdcell.outputID = 0;
			ret = IMP_System_Bind(&chn[i].framesource_chn, &osdcell);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "Bind FrameSource channel0 and OSD failed\n");
				return -1;
			}

			ret = IMP_System_Bind(&osdcell, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
				return -1;
			}
		}
	}

#ifdef SUPPORT_RGB555LE
	uint32_t *timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint16_t));
	if (timeStampData == NULL) {
		IMP_LOG_ERR(TAG, "valloc timeStampData error\n");
		return -1;
	}
	memset(timeStampData, 0, OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint16_t));
#else
	uint32_t *timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
	if (timeStampData == NULL) {
		IMP_LOG_ERR(TAG, "valloc timeStampData error\n");
		return -1;
	}
	memset(timeStampData, 0, OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
#endif

	/* Step.6 Create OSD bgramap update thread */
	pthread_t tid;
	ret = pthread_create(&tid, NULL, update_thread, timeStampData);
	if (ret) {
		IMP_LOG_ERR(TAG, "thread create error\n");
		return -1;
	}

	/* Step.7 Stream On */
	IMP_FrameSource_SetFrameDepth(0, 0);
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

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

	/* Exit sequence as follow */
	/* Step.a Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	pthread_cancel(tid);
	pthread_join(tid, NULL);
	free(timeStampData);

	/* Step.b UnBind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			osdcell.deviceID = DEV_ID_OSD;
			osdcell.groupID = i;
			osdcell.outputID = 0;
			ret = IMP_System_UnBind(&osdcell, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
				return -1;
			}

			ret = IMP_System_UnBind(&chn[i].framesource_chn, &osdcell);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "UnBind FrameSource and OSD failed\n");
				return -1;
			}
		}
	}

	/* Step.c OSD exit */

	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = sample_osd_exit(i);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "OSD exit failed\n");
				return -1;
			}
		}
	}

	/* Step.d Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	/* Step.e FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.f System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
