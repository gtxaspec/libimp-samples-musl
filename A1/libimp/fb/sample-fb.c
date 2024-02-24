/*********************************************************
 ** File Name   : sample_fb.c
 ** Author      : Keven
 ** Mail        : keven.ywhan@ingenic.com
 ** Brief       :
 ** Created Time: 2022-02-22 09:29
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>

#include "../common/sample-common.h"

#define FB0DEV	"/dev/fb0"
#define FB1DEV	"/dev/fb1"

#define CANVAS_WIDTH		(1920)
#define CANVAS_HEIGHT		(1080)
#define DISPLAY_WIDTH		(1920)
#define DISPLAY_HEIGHT		(1080)
#define DISPLAY_INTFSYNC	(VO_OUTPUT_1080P30)
#define VIDEO_FILE_NAME		"640x360.hevc"
#define VIDEO_TYPE		(PT_H265)
#define VIDEO_WIDTH		(640)
#define VIDEO_HEIGHT		(360)

IMP_VOID DrawBox(IMP_VOID *Addr, IMP_RECT_S *stRect, IMP_S32 Length, IMP_S32 Bpp, IMP_S32 Index)
{
	IMP_U32 u16ColorBar[] = {0x60e0, 0x1f1f, 0x1010, 0x6fef, 0x0505};
	IMP_U32 u32ColorBar[] = {0xff0033, 0xff9900,0x0099cc, 0xcc99cc, 0x009933};

	IMP_S32 i, j;
	IMP_U32 *Colors;

	if(Bpp == 32)
		Colors = &u32ColorBar[0];
	else
		Colors = &u16ColorBar[0];

	if(Index%2) {
		for(i = stRect->s32Y; i < stRect->u32Height + stRect->s32Y; i++) {
			for(j = stRect->s32X; j < stRect->u32Width + stRect->s32X; j++) {
				if(j%100 < 20)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 0, Bpp>> 3);
				else if(j%100 < 40)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 1, Bpp>> 3);
				else if(j%100 < 60)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 2, Bpp>> 3);
				else if(j%100 < 80)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 3, Bpp>> 3);
				else
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 4, Bpp>> 3);
			}
		}
	} else {
		for(i = stRect->s32Y; i < stRect->u32Height + stRect->s32Y; i++) {
			for(j = stRect->s32X; j < stRect->u32Width + stRect->s32X; j++) {
				if(i%100 < 20)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 0, Bpp>> 3);
				else if(i%100 < 40)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 1, Bpp>> 3);
				else if(i%100 < 60)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 2, Bpp>> 3);
				else if(i%100 < 80)
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 3, Bpp>> 3);
				else
					memcpy(Addr + (i * Length + j) * (Bpp >> 3), Colors + 4, Bpp>> 3);
			}
		}
	}

}

void *display_gui_thread(void *parg)
{
	IMP_S32 ret = IMP_SUCCESS;
	IMP_S32 fd = 0;
	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;
	IMP_VOID *pBufferAddr;
	IMP_BOOL bShow = IMP_FALSE;
	FB_ALPHA_S stAlpha;
	FB_COLORKEY_S stColorKey;

	IMP_U32 u32Colors[] = {0x60e0, 0x1f1f, 0x1010, 0x6fef, 0x0505};
	IMP_RECT_S stRect;
	IMP_U32 bpp;
	IMP_S32 i;
	IMP_U32 index = 0;

	fd = open(FB0DEV, O_RDWR);
	if (fd < 0) {
		perror("FB0DEV open error!\n");
		return NULL;
	}

	/* get framebuffer's var_info */
	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		printf("FBIOGET_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	//ARGB1555
	//var_info.activate = FB_ACTIVATE_NOW;
	var_info.activate = FB_ACTIVATE_FORCE;
	var_info.xres = CANVAS_WIDTH;
	var_info.yres = CANVAS_HEIGHT;
	var_info.bits_per_pixel = 16;
	//A
	var_info.transp.offset = 15;
	var_info.transp.length = 1;
	var_info.transp.msb_right = 0;
	//R
	var_info.red.offset = 10;
	var_info.red.length = 5;
	var_info.red.msb_right = 0;
	//G
	var_info.green.offset = 5;
	var_info.green.length = 5;
	var_info.green.msb_right = 0;
	//B
	var_info.blue.offset = 0;
	var_info.blue.length = 5;
	var_info.blue.msb_right = 0;

	/* put framebuffer's var_info */
	if ((ret = ioctl(fd, FBIOPUT_VSCREENINFO, &var_info)) < 0) {
		printf("FBIOPUT_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		printf("FBIOGET_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	/* get framebuffer's fix_info */
	if ((ret = ioctl(fd, FBIOGET_FSCREENINFO, &fix_info)) < 0) {
		printf("FBIOGET_FSCREENINFO failed");
		goto err_get_or_put_info;
	}
	printf("[xres,yres]=[%d,%d]\n",var_info.xres,var_info.yres);
	printf("[xres_virtual,yres_virtual]=[%d,%d]\n",var_info.xres_virtual,var_info.yres_virtual);
	printf("[bits_per_pixel]=[%d]\n",var_info.bits_per_pixel);

	pBufferAddr = mmap(0, fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(!pBufferAddr) {
		printf("Map failed");
		goto err_mmap;
	}

	stRect.u32Width = var_info.xres / 2;
	stRect.u32Height= var_info.yres / 2;
	stRect.s32X = 400;
	stRect.s32Y = 200;
	bpp = var_info.bits_per_pixel;

	for(i = 0; i < var_info.xres * var_info.yres;i++) {
		if(i < var_info.xres * var_info.yres / 2)
			memcpy(pBufferAddr + i * (bpp>>3)  , &u32Colors[0], bpp>>3);
		else
			memcpy(pBufferAddr + i * (bpp>>3)  , &u32Colors[1], bpp>>3);
	}

	bShow = IMP_TRUE;
	if ((ret = ioctl(fd, FBIOPUT_SHOW, &bShow)) < 0) {
		printf("FBIOPUT_SHOW failed");
		goto err_show;
	}

	/* set colorkey */
	printf("Test Colorkey\n");
	sleep(5);
	memset(&stColorKey, 0x00, sizeof(stColorKey));
	stColorKey.s32KeyEnable = 1;
	stColorKey.u32KeyColor = u32Colors[0];
	ioctl(fd, FBIOPUT_COLORKEY, &stColorKey);
	sleep(2);
	stColorKey.s32KeyEnable = 0;
	ioctl(fd, FBIOPUT_COLORKEY, &stColorKey);
	sleep(2);

	/* set alpha */
	printf("Test Alpha\n");
	memset(&stAlpha, 0x00, sizeof(stAlpha));
	stAlpha.enAlphaMode = ALPHA_GLOBAL;
	stAlpha.u8Alpha0 = 0;
	stAlpha.u8Alpha1 = 0;
	stAlpha.u8GlobalAlpha = 128;
	ioctl(fd, FBIOPUT_ALPHA, &stAlpha);
	sleep(2);
	stAlpha.u8GlobalAlpha = 0;
	ioctl(fd, FBIOPUT_ALPHA, &stAlpha);
	sleep(2);

	memset(&stColorKey, 0x00, sizeof(stColorKey));
	stColorKey.s32KeyEnable = IMP_TRUE;
	stColorKey.u32KeyColor = 0x00;
	ioctl(fd, FBIOPUT_COLORKEY, &stColorKey);

	memset(pBufferAddr, 0x00, fix_info.smem_len);

	while(1) {
		DrawBox(pBufferAddr, &stRect, var_info.xres, bpp, index);
		index++;
		ioctl(fd,FBIOPAN_DISPLAY, &var_info);
		usleep(1000*1000);
	}

err_show:
	munmap(pBufferAddr, fix_info.smem_len);
err_mmap:
err_get_or_put_info:
	close(fd);
	return NULL;
}

void *display_mouse_thread(void *parg)
{
	IMP_S32 ret = IMP_SUCCESS;
	IMP_S32 fd = 0;
	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;
	IMP_VOID *pBufferAddr;
	IMP_BOOL bShow = IMP_FALSE;

	FB_POINT_S stPoint;
	IMP_U32 bpp;
	IMP_U32 u32Colors[] = {0x7c00}; //red
	IMP_U16 u16PointX = 0;
	IMP_U16 u16PointY = 0;
	IMP_U32 u32MoveFlag = 0;
	IMP_S32 i = 0;

	fd = open(FB1DEV, O_RDWR);
	if (fd < 0) {
		perror("FB1DEV open error!\n");
		return NULL;
	}

	/* get framebuffer's var_info */
	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	//ARGB1555
	//var_info.activate = FB_ACTIVATE_NOW;
	var_info.activate = FB_ACTIVATE_FORCE;
	var_info.xres = 32;
	var_info.yres = 32;
	var_info.bits_per_pixel = 16;
	//A
	var_info.transp.offset = 15;
	var_info.transp.length = 1;
	var_info.transp.msb_right = 0;
	//R
	var_info.red.offset = 10;
	var_info.red.length = 5;
	var_info.red.msb_right = 0;
	//G
	var_info.green.offset = 5;
	var_info.green.length = 5;
	var_info.green.msb_right = 0;
	//B
	var_info.blue.offset = 0;
	var_info.blue.length = 5;
	var_info.blue.msb_right = 0;

	/* put framebuffer's var_info */
	if ((ret = ioctl(fd, FBIOPUT_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOPUT_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	if ((ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	/* get framebuffer's fix_info */
	if ((ret = ioctl(fd, FBIOGET_FSCREENINFO, &fix_info)) < 0) {
		perror("FBIOGET_FSCREENINFO failed");
		goto err_get_or_put_info;
	}
	printf("[xres,yres]=[%d,%d]\n",var_info.xres,var_info.yres);
	printf("[xres_virtual,yres_virtual]=[%d,%d]\n",var_info.xres_virtual,var_info.yres_virtual);
	printf("[bits_per_pixel]=[%d]\n",var_info.bits_per_pixel);

	pBufferAddr = mmap(0,fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(!pBufferAddr) {
		perror("Map failed");
		goto err_mmap;
	}

	memset(pBufferAddr, 0x00, fix_info.smem_len);

	bShow = IMP_TRUE;
	if ((ret = ioctl(fd, FBIOPUT_SHOW, &bShow)) < 0) {
		perror("FBIOPUT_SHOW failed");
		goto err_show;
	}

	bpp = var_info.bits_per_pixel;

	for(i = 0; i < var_info.xres * var_info.yres; i++) {
		memcpy(pBufferAddr + i * (bpp >> 3), &u32Colors[0],
				bpp >> 3);
	}

	var_info.xoffset = 0;
	var_info.yoffset = 0;
	ioctl(fd, FBIOPAN_DISPLAY, &var_info);

	while(1) {
		if(u32MoveFlag == 0){
			u16PointX += 2;
			u16PointY += 2;
		}

		if(u32MoveFlag == 1){
			u16PointX += 2;
			u16PointY -= 2;
		}

		if(u32MoveFlag == 2){
			u16PointX -= 2;
			u16PointY += 2;
		}

		if(u32MoveFlag == 3){
			u16PointX -= 2;
			u16PointY -= 2;
		}

		if(u16PointX <= 0){
			u32MoveFlag &= 1;
			u16PointX = 0;
		}

		if(u16PointY <= 0){
			u32MoveFlag &= 2;
			u16PointY = 0;
		}

		if(u16PointX >= DISPLAY_WIDTH - 1) {
			u32MoveFlag |= 2;
		}
		if(u16PointY >= DISPLAY_HEIGHT - 1) {
			u32MoveFlag |= 1;
		}
		stPoint.s32X = u16PointX;
		stPoint.s32Y = u16PointY;
		ret = ioctl(fd, FBIOPUT_SCREEN_ORIGIN, &stPoint);
		if(ret < 0) {
			printf("FBIOPUT_SCREEN_ORIGIN failed\n");
		}
		usleep(50*1000);
	}

err_show:
	munmap(pBufferAddr, fix_info.smem_len);
err_mmap:
err_get_or_put_info:
	close(fd);
	return NULL;
}

void *display_video_thread(void *parg)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_CELL_S stVdecCell = {0};
	IMP_CELL_S stVoCell = {0};
	SAMPLE_VO_MODE_E enVoMux = VO_MODE_16MUX;
	SAMPLE_VO_MODE_E enVdecMux = VO_MODE_16MUX;

	IMP_S32 s32VoLayer = SAMPLE_VO_LATER0;
	VO_CHN_ATTR_S stVoChnAttr;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;

	VDEC_DEV_ATTR_S stVdecDevAttr;
	VDEC_CHN_ATTR_S stVdecChnAttr;
	SAMPLE_VDEC_THREAD_S stVdecThreadPara[MAX_VDEC_DEVS];
	pthread_t sendTid[MAX_VDEC_DEVS];
	IMP_S32 s32VdecChn = 0;
	IMP_S32 i;

	memset(&stVoLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));

	memset(&stVdecDevAttr,0,sizeof(VDEC_DEV_ATTR_S));
	memset(&stVdecChnAttr,0,sizeof(VDEC_CHN_ATTR_S));

	for(i = 0; i < enVdecMux; i++) {
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = i;
		stVdecCell.u32ChnId = s32VdecChn;
		stVoCell.enModId = MOD_ID_VO;
		stVoCell.u32DevId = s32VoLayer;
		stVoCell.u32ChnId = i;
		s32Ret = IMP_System_Bind(&stVdecCell, &stVoCell);
		if (s32Ret != IMP_SUCCESS) {
			printf("IMP_System_Bind failed! ret = 0x%08x\n", s32Ret);
			goto err_IMP_System_Bind;
		}
	}

	/* init vo layer0 */
	s32VoLayer = SAMPLE_VO_LATER0;
	stVoLayerAttr.enPixFmt = PIX_FMT_NV12;
	s32Ret = sample_vo_getwh(DISPLAY_INTFSYNC, &stVoLayerAttr.stDispRect.u32Width,
			&stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_vo_getwh failed! ret = %d\n", s32Ret);
		goto err_sample_vo_getwh;
	}
	stVoLayerAttr.stDispRect.s32X = ORIGINAL_X;
	stVoLayerAttr.stDispRect.s32Y = ORIGINAL_Y;
	stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
	stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
	s32Ret = sample_vo_start_layer(s32VoLayer, &stVoLayerAttr);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_vo_start_layer failed! ret = 0x%08x\n", s32Ret);
		goto err_sample_vo_start_layer;
	}

	s32Ret = sample_vo_start_chns(s32VoLayer, enVoMux);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_vo_start_chn failed! ret = 0x%08x\n", s32Ret);
		goto err_sample_vo_start_chns;
	}
	IMP_VO_EnableBcsh(s32VoLayer);

	/* init vdec device */
	stVdecDevAttr.enType = VIDEO_TYPE;
	stVdecDevAttr.enMode = VIDEO_MODE_FRAME;
	stVdecDevAttr.u32PicWidth = VIDEO_WIDTH;
	stVdecDevAttr.u32PicHeight = VIDEO_HEIGHT;
	stVdecDevAttr.u32StreamBufSize = stVdecDevAttr.u32PicWidth * stVdecDevAttr.u32PicHeight * 3 / 4;
	stVdecDevAttr.u32RefFrameNum = 2;

	for(i = 0; i < enVdecMux; i++) {
		s32Ret = sample_vdec_start_dev(i, &stVdecDevAttr);
		if (s32Ret != IMP_SUCCESS) {
			printf("sample_vdec_init failed! ret = 0x%08x\n", s32Ret);
			goto err_sample_vdec_start_dev;
		}

		IMP_VO_GetChnAttr(s32VoLayer, 1, &stVoChnAttr);
		stVdecChnAttr.enOutputFormat = PIX_FMT_NV12;
		stVdecChnAttr.u32OutputWidth = ALIGN_UP(stVoChnAttr.stRect.u32Width, 8);
		stVdecChnAttr.u32OutputHeight = stVoChnAttr.stRect.u32Height;
		stVdecChnAttr.bEnableCrop = IMP_FALSE;
		stVdecChnAttr.u32FrameBufCnt = 3;
		s32Ret = sample_vdec_start_chn(i, s32VdecChn,&stVdecChnAttr);
		if (s32Ret != IMP_SUCCESS) {
			printf("sample_vdec_start_chn failed! ret = 0x%08x\n", s32Ret);
			goto err_sample_vdec_start_chn;
		}

		s32Ret = sample_vdec_start_recv_stream(i);
		if (s32Ret != IMP_SUCCESS) {
			printf("sample_vdec_start_chn failed! ret = 0x%08x\n", s32Ret);
			goto err_sample_vdec_start_recv_stream;
		}

		stVdecThreadPara[i].s32DevId = i;
		sprintf(stVdecThreadPara[i].cFileName, VIDEO_FILE_NAME);
		stVdecThreadPara[i].enType = VIDEO_TYPE;
		stVdecThreadPara[i].enVideoMode = VIDEO_MODE_FRAME;
		stVdecThreadPara[i].u32BufSize = VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 4;
		stVdecThreadPara[i].enCtrlSig = VDEC_CTRL_START;
		stVdecThreadPara[i].s32FpsCtrl = 25;
		stVdecThreadPara[i].bLoopPlayback = IMP_TRUE;
		pthread_create(&sendTid[i], 0, sample_vdec_send_stream_thread, (void *)&stVdecThreadPara[i]);
	}

	for(i = 0; i < enVdecMux; i++){
		if (sendTid[i]){
			pthread_join(sendTid[i], IMP_NULL);
		}
		sendTid[i] = 0;
		sample_vdec_stop_recv_stream(i);
		sample_vdec_stop_chn(i, s32VdecChn);
		sample_vdec_stop_dev(i);
	}
	sample_vo_stop_chns(s32VoLayer, enVoMux);
	sample_vo_stop_layer(s32VoLayer);

	for(i = 0; i < enVdecMux; i++){
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = i;
		stVdecCell.u32ChnId = s32VdecChn;
		stVoCell.enModId = MOD_ID_VO;
		stVoCell.u32DevId = s32VoLayer;
		stVoCell.u32ChnId = i;
		IMP_System_UnBind(&stVdecCell, &stVoCell);
	}

	return IMP_SUCCESS;

err_sample_vdec_start_recv_stream:
err_sample_vdec_start_chn:
err_sample_vdec_start_dev:
	i++;
	while(i--) {
		if (sendTid[i]){
			pthread_join(sendTid[i], IMP_NULL);
		}
		sendTid[i] = 0;
		sample_vdec_stop_recv_stream(i);
		sample_vdec_stop_chn(i, s32VdecChn);
		sample_vdec_stop_dev(i);
		stVdecCell.enModId = MOD_ID_VDEC;
		stVdecCell.u32DevId = 1;
		stVdecCell.u32ChnId = s32VdecChn;
		stVoCell.enModId = MOD_ID_VO;
		stVoCell.u32DevId = s32VoLayer;
		stVoCell.u32ChnId = 1;
		IMP_System_UnBind(&stVdecCell, &stVoCell);

	}
	sample_vo_stop_chns(s32VoLayer, enVoMux);
err_sample_vo_start_chns:
	sample_vo_stop_layer(s32VoLayer);
err_sample_vo_start_layer:
err_sample_vo_getwh:
err_IMP_System_Bind:
	return IMP_SUCCESS;
}

/*
 * fb 	: fb -> vo (gui layer)
 * mouse: fb -> vo (mouse layer)
 * video: vdec -> vo (video layer0)
 *
 * */

int main(int argc, char *argv[])
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	pthread_t gui_thread;
	pthread_t mouse_thread;
	pthread_t video_thread;

	IMP_S32 s32VoMod;
	VO_PUB_ATTR_S stVoPubAttr;

	memset(&stVoPubAttr, 0, sizeof(VO_PUB_ATTR_S));

	/* system init */
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_system_init failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	/* vo init */
	s32VoMod = SAMPLE_VO_MODULE;
	IMP_VO_Disable(s32VoMod);
	stVoPubAttr.stCanvasSize.u32Width = DISPLAY_WIDTH;
	stVoPubAttr.stCanvasSize.u32Height = DISPLAY_HEIGHT;
	stVoPubAttr.enIntfSync = DISPLAY_INTFSYNC;
	stVoPubAttr.enIntfType = VO_INTF_HDMI;
	s32Ret = sample_vo_start_module(s32VoMod, &stVoPubAttr);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_vo_start_module failed! ret = 0x%08x\n", s32Ret);
		goto err_sample_vo_start_module;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto err_sample_vo_hdmi_start;
	}

	/* Created gui display thread */
	pthread_create(&gui_thread, 0, display_gui_thread, NULL);
	/* Created mouse display thread */
	pthread_create(&mouse_thread, 0, display_mouse_thread, NULL);
	/* Created video display thread */
	pthread_create(&video_thread, 0, display_video_thread, NULL);

	pthread_join(gui_thread, NULL);
	pthread_join(mouse_thread, NULL);
	pthread_join(video_thread, NULL);


	sample_vo_hdmi_stop();
	sample_vo_stop_module(s32VoMod);
	sample_system_exit();
	return s32Ret;

err_sample_vo_hdmi_start:
	sample_vo_stop_module(s32VoMod);
err_sample_vo_start_module:
	sample_system_exit();
	return s32Ret;
}
