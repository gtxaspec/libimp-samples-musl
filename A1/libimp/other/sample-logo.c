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

#define TAG "SAMPLE_LOGO"
#define JPEG_FILE "1920x1080.jpg"

/*
   使用JPEG软件解码，解码后数据送给G层显示。默认解码后数据格式为ARGB1555。
 */
int main(int argc, char *argv[])
{
	IMP_S32 s32VoMod;
	VO_PUB_ATTR_S stVoPubAttr;
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 fd = 0;
	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;
	IMP_VOID *pBufferAddr;
	IMP_BOOL bShow = IMP_FALSE;
	JPEGD_INSTANCE_S stJpegInst;
	IMP_U32 u32ImageWidth = 1920;
	IMP_U32 u32ImageHeight = 1080;

	/************************************************
    step1:  read jpeg file and init system
    *************************************************/
	FILE *in = fopen(JPEG_FILE, "rb");
	if (in == NULL) {
		IMP_LOG_INFO("file %s is not existed!\n", JPEG_FILE);
		goto err_file;
	}
	fseek(in, 0, SEEK_END);
	IMP_U32 size = ftell(in);
	IMP_U8 *jpeg = (IMP_U8 *)malloc(size);
	fseek(in, 0, SEEK_SET);
	fread(jpeg, 1, size, in);

	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_system_init failed! ret = 0x%08x\n", s32Ret);
		goto err_systeminit;
	}

	/************************************************
    step2:  start vo
    *************************************************/
	s32VoMod = SAMPLE_VO_MODULE;
	IMP_VO_Disable(s32VoMod);
	stVoPubAttr.u32BgColor = 0x00000000;
	stVoPubAttr.stCanvasSize.u32Width = 1920;
	stVoPubAttr.stCanvasSize.u32Height = 1080;
	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;

	stVoPubAttr.enIntfType = VO_INTF_HDMI | VO_INTF_VGA;
	s32Ret = sample_vo_start_module(s32VoMod,&stVoPubAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_start_module failed! ret = 0x%08x\n", s32Ret);
		goto err_vo_start_module;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto err_vo_hdmi_start;
	}

	/************************************************
    step3:  start fb
    *************************************************/
	fd = open("/dev/fb0", O_RDWR);
	if (fd < 0) {
		perror("/dev/fb0 open error!\n");
		s32Ret = -1;
		goto err_openfb;
	}

	/* get framebuffer's var_info */
	if ((s32Ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	//ARGB1555
	var_info.activate = 0x00000000;
	var_info.xres = u32ImageWidth;
	var_info.yres = u32ImageHeight;
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
	if ((s32Ret = ioctl(fd, FBIOPUT_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOPUT_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	if ((s32Ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto err_get_or_put_info;
	}

	/* get framebuffer's fix_info */
	if ((s32Ret = ioctl(fd, FBIOGET_FSCREENINFO, &fix_info)) < 0) {
		perror("FBIOGET_FSCREENINFO failed");
		goto err_get_or_put_info;
	}
	printf("[xres,yres]=[%d,%d]\n",var_info.xres,var_info.yres);
	printf("[xres_virtual,yres_virtual]=[%d,%d]\n",var_info.xres_virtual,var_info.yres_virtual);
	printf("[bits_per_pixel]=[%d]\n",var_info.bits_per_pixel);

	pBufferAddr = mmap(0, fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (!pBufferAddr) {
		perror("Map failed");
		goto err_mmap;
	}
	memset(pBufferAddr, 0x00, fix_info.smem_len);

	/************************************************
    step4:  start software jpeg decoding
    *************************************************/
	stJpegInst.pu8SrcAddr = jpeg;
	stJpegInst.u32Len = size;
	stJpegInst.pu8DstAddr = pBufferAddr;
	s32Ret = IMP_JPEGD_Init(&stJpegInst);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_JPEGD_Init failed! ret = 0x%x\n", s32Ret);
		goto err_jpeg_init;
	}

	s32Ret = IMP_JPEGD_Decode(&stJpegInst);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_JPEGD_Decode failed! ret = 0x%x\n", s32Ret);
		goto err_decode;
	}

	bShow = IMP_TRUE;
	if ((s32Ret = ioctl(fd, FBIOPUT_SHOW, &bShow)) < 0) {
		perror("FBIOPUT_SHOW failed");
		goto err_show;
	}

	getchar();
err_show:
err_decode:
	IMP_JPEGD_DeInit(&stJpegInst);
err_jpeg_init:
	munmap(pBufferAddr, fix_info.smem_len);
err_mmap:
err_get_or_put_info:
	close(fd);
err_openfb:
	sample_vo_hdmi_stop();
err_vo_hdmi_start:
	sample_vo_stop_module(s32VoMod);
err_vo_start_module:
	sample_system_exit();
err_systeminit:
	free(jpeg);
	fclose(in);
err_file:
	return s32Ret;
}
