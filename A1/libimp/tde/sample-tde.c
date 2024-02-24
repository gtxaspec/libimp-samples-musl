/*********************************************************
 ** File Name   : sample_tde.c
 ** Author      : Keven
 ** Mail        : keven.ywhan@ingenic.com
 ** Brief       :
 ** Created Time: 2023-03-06 16:36
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

#define FB0DEV "/dev/fb0"

#define NV12_TEST_FILE "640x360.nv12"
#define ARGB1555_TEST_FILE "640x360.argb1555"

#define CANVAS_WIDTH (1920)
#define CANVAS_HEIGHT (1080)
#define CANVAS_BPP (16)

#define SRC_PIXFMT TDE_PIXFMT_ARGB1555
#define SRC_WIDTH (640)
#define SRC_HEIGHT (360)
#define SRC_BPP (16)
#define SRC_STRIDE (SRC_WIDTH * (SRC_BPP >> 3))
#define SRC_SIZE ((SRC_WIDTH * SRC_HEIGHT * SRC_BPP) >> 3)


typedef struct MemBuffer
{
	IMP_CHAR s8PoolName[16];
	IMP_U32 u32BufferPool;
	IMP_U32 u32PoolSize;
} MemBuffer_st;
MemBuffer_st g_stMemBuffer;

typedef struct FbAttrInfo
{
	IMP_S32 s32Fd;
	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;
	IMP_VOID *pBufferAddr;
	FB_ALPHA_S stAlpha;
	FB_COLORKEY_S stColorKey;

} FbAttrInfo_st;
FbAttrInfo_st g_stFbAttrInfo;

static void draw_osd(void *addr, int w, int h, int bpp)
{
	uint32_t i, j;
	uint32_t Colors;

	if(bpp==32)
		Colors = 0xff0000 | 0xff<<24;
	else
		Colors = 0x7d00 | 0x1<<15;

	for(j = 0; j < h; j++) {
		for(i = 0; i < w; i++) {
			if(i < 100 && j < 100)
				memcpy(addr + (i * w + j) * (bpp >> 3), &Colors, bpp>> 3);
			else
				memset(addr + (i * w + j) * (bpp >> 3), 0x0, bpp>> 3);
		}
	}

	return;
}

static int load_picture(void *addr, char *filename)
{
	int ret;
	int32_t read_fd = 0;
	int32_t size = 0;

	read_fd = open(filename, O_RDONLY);
	if(read_fd < 0) {
		printf("open test file(%s) Failed\n", filename);
		goto err_open;
	}

	size = lseek(read_fd, 0, SEEK_END);

	lseek(read_fd, 0, SEEK_SET);
	ret = read(read_fd, addr, size);
	if(ret < 0) {
		printf("read %s Failed\n", NV12_TEST_FILE);
		goto err_read;
	}
	return 0;

err_read:
err_open:
	return -1;
}

static int sample_fb_init(FbAttrInfo_st *stFbInfo)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	FB_ALPHA_S stAlpha;
	FB_COLORKEY_S stColorKey;
	IMP_BOOL bShow = IMP_FALSE;

	stFbInfo->s32Fd = open(FB0DEV, O_RDWR);
	if (stFbInfo->s32Fd < 0) {
		printf("FB0DEV open Error!\n");
		return -1;
	}

	/* get framebuffer's var_info */
	if ((s32Ret = ioctl(stFbInfo->s32Fd, FBIOGET_VSCREENINFO, &stFbInfo->var_info)) < 0) {
		printf("FBIOGET_VSCREENINFO failed");
		goto Err_get_or_put_info;
	}

	if(CANVAS_BPP == 16) {
		/* ARGB1555 */
		stFbInfo->var_info.activate = FB_ACTIVATE_FORCE;
		stFbInfo->var_info.xres = CANVAS_WIDTH;
		stFbInfo->var_info.yres = CANVAS_HEIGHT;
		stFbInfo->var_info.bits_per_pixel = 16;
		//A
		stFbInfo->var_info.transp.offset = 15;
		stFbInfo->var_info.transp.length = 1;
		stFbInfo->var_info.transp.msb_right = 0;
		//R
		stFbInfo->var_info.red.offset = 10;
		stFbInfo->var_info.red.length = 5;
		stFbInfo->var_info.red.msb_right = 0;
		//G
		stFbInfo->var_info.green.offset = 5;
		stFbInfo->var_info.green.length = 5;
		stFbInfo->var_info.green.msb_right = 0;
		//B
		stFbInfo->var_info.blue.offset = 0;
		stFbInfo->var_info.blue.length = 5;
		stFbInfo->var_info.blue.msb_right = 0;
	} else if(CANVAS_BPP == 32) {
		/* ARGB8888 */
		stFbInfo->var_info.activate = FB_ACTIVATE_FORCE;
		stFbInfo->var_info.xres = CANVAS_WIDTH;
		stFbInfo->var_info.yres = CANVAS_HEIGHT;
		stFbInfo->var_info.bits_per_pixel = 32;
		//A
		stFbInfo->var_info.transp.offset = 24;
		stFbInfo->var_info.transp.length = 8;
		stFbInfo->var_info.transp.msb_right = 0;
		//R
		stFbInfo->var_info.red.offset = 16;
		stFbInfo->var_info.red.length = 8;
		stFbInfo->var_info.red.msb_right = 0;
		//G
		stFbInfo->var_info.green.offset = 8;
		stFbInfo->var_info.green.length = 8;
		stFbInfo->var_info.green.msb_right = 0;
		//B
		stFbInfo->var_info.blue.offset = 0;
		stFbInfo->var_info.blue.length = 8;
		stFbInfo->var_info.blue.msb_right = 0;
	}

	/* put framebuffer's var_info */
	if ((s32Ret = ioctl(stFbInfo->s32Fd, FBIOPUT_VSCREENINFO, &stFbInfo->var_info)) < 0) {
		printf("FBIOPUT_VSCREENINFO failed");
		goto Err_get_or_put_info;
	}

	if ((s32Ret = ioctl(stFbInfo->s32Fd, FBIOGET_VSCREENINFO, &stFbInfo->var_info)) < 0) {
		printf("FBIOGET_VSCREENINFO failed");
		goto Err_get_or_put_info;
	}

	/* get framebuffer's fix_info */
	if ((s32Ret = ioctl(stFbInfo->s32Fd, FBIOGET_FSCREENINFO, &stFbInfo->fix_info)) < 0) {
		printf("FBIOGET_FSCREENINFO failed");
		goto Err_get_or_put_info;
	}
	printf("[xres,yres]=[%d,%d]\n", stFbInfo->var_info.xres, stFbInfo->var_info.yres);
	printf("[xres_virtual,yres_virtual]=[%d,%d]\n",
			stFbInfo->var_info.xres_virtual, stFbInfo->var_info.yres_virtual);
	printf("[bits_per_pixel]=[%d]\n",stFbInfo->var_info.bits_per_pixel);

	stFbInfo->pBufferAddr = mmap(0, stFbInfo->fix_info.smem_len,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			stFbInfo->s32Fd, 0);
	if(!stFbInfo->pBufferAddr) {
		printf("Map failed");
		goto Err_mmap;
	}
	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);

	/* set colorkey */
	memset(&stColorKey, 0x00, sizeof(stColorKey));
	stColorKey.s32KeyEnable = IMP_FALSE;
	if ((s32Ret = ioctl(stFbInfo->s32Fd, FBIOPUT_COLORKEY, &stColorKey)) < 0) {
		printf("FBIOPUT_COLORKEY failed");
		goto Err_colorkey;
	}

	/* set alpha */
	memset(&stAlpha, 0x00, sizeof(stAlpha));
	stAlpha.enAlphaMode = ALPHA_GLOBAL;
	stAlpha.u8GlobalAlpha = IMP_FALSE;
	if ((s32Ret = ioctl(stFbInfo->s32Fd, FBIOPUT_ALPHA, &stAlpha)) < 0) {
		printf("FBIOPUT_ALPHA failed");
		goto Err_alpha;
	}

	/* show enable */
	bShow = IMP_TRUE;
	if ((s32Ret = ioctl(stFbInfo->s32Fd, FBIOPUT_SHOW, &bShow)) < 0) {
		printf("FBIOPUT_SHOW failed");
		goto Err_show;
	}

	return 0;
Err_show:
Err_alpha:
Err_colorkey:
Err_mmap:
Err_get_or_put_info:
	close(stFbInfo->s32Fd);
	return -1;
}

void sample_fb_exit()
{
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;

	close(stFbInfo->s32Fd);
}

int sample_fb_display()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;

	s32Ret = ioctl(stFbInfo->s32Fd, FBIOPAN_DISPLAY, &stFbInfo->var_info);
	if(s32Ret < 0) {
		printf("FBIOPAN_DISPLAY Failed!\n");
	}

	return s32Ret;
}

IMP_S32 sample_tde_copy()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;
	IMP_U32 i;
	IMP_U32 u32XPox[4]={0, 1280, 0, 1280};
	IMP_U32 u32YPox[4]={0, 0, 720, 720};

	TDE_HANDLE Handle;
	IMP_VOID *pSrcAddr = NULL;
	IMP_U16 u16OptNum = 1;
	TDE_SURFACE_S pstForeGround;
	TDE_RECT_S pstForeGroundRect;
	TDE_SURFACE_S pstDst;
	TDE_RECT_S pstDstRect;

	TDE_PIXFMT_E enSrcPixFmt = SRC_PIXFMT;
	IMP_U16 u16SrcBpp = SRC_BPP;
	IMP_U16 u16SrcWidth = SRC_WIDTH;
	IMP_U16 u16SrcHeight = SRC_HEIGHT;
	IMP_U16 u16SrcStride = u16SrcWidth * (u16SrcBpp>>3);
	IMP_U32 u32SrcSize = u16SrcStride * u16SrcHeight;

	memset(&pstForeGround, 0, sizeof(pstForeGround));
	memset(&pstForeGroundRect, 0, sizeof(pstForeGroundRect));
	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	pSrcAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32SrcSize, "buffer");
	if(!pSrcAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock;
	}
	memset(pSrcAddr, 0x00, u32SrcSize);

	load_picture(pSrcAddr, ARGB1555_TEST_FILE);
	IMP_System_FlushCache(pSrcAddr, u32SrcSize);

	for(i = 0; i < 4; i++) {
		pstForeGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pSrcAddr);
		pstForeGround.u32CbCrPhyAddr 	= 0;
		pstForeGround.enPixFmt 		= enSrcPixFmt;
		pstForeGround.u16Width 		= u16SrcWidth;
		pstForeGround.u16Height 	= u16SrcHeight;
		pstForeGround.u16Stride 	= u16SrcStride;
		pstForeGround.u16CbCrStride 	= 0;
		pstForeGroundRect.s32Xpos 	= 0;
		pstForeGroundRect.s32Ypos 	= 0;
		pstForeGroundRect.u32Width 	= 0;
		pstForeGroundRect.u32Height 	= 0;

		pstDst.u32PhyAddr		= stFbInfo->fix_info.smem_start;
		pstDst.u32CbCrPhyAddr		= 0;
		pstDst.enPixFmt			= pstForeGround.enPixFmt;
		pstDst.u16Width			= stFbInfo->var_info.xres;
		pstDst.u16Height		= stFbInfo->var_info.yres;
		pstDst.u16Stride		= stFbInfo->fix_info.line_length;
		pstDst.u16CbCrStride		= 0;
		pstDstRect.s32Xpos		= u32XPox[i];
		pstDstRect.s32Ypos		= u32YPox[i];
		pstDstRect.u32Width		= pstForeGround.u16Width;
		pstDstRect.u32Height		= pstForeGround.u16Height;

		/* 1. tde start job */
		Handle = IMP_TDE_BeginJob(u16OptNum);
		if(Handle < 0) {
			printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
			goto Err_IMP_TDE_BeginJob;
		}

		/* 2. tde submit job */
		s32Ret = IMP_TDE_QuickCopy(Handle, &pstForeGround, &pstForeGroundRect,
				&pstDst, &pstDstRect);
		if(s32Ret < 0) {
			printf("IMP_TDE_QuickCopy Error (%d)\n", s32Ret);
			IMP_TDE_CancelJob(Handle);
			goto Err_IMP_TDE_QuickCopy;
		}

		s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
		if(s32Ret < 0) {
			printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
			goto Err_IMP_TDE_EndJob;
		}

		s32Ret = sample_fb_display();
		if(s32Ret < 0) {
			printf("sample_fb_display Error (%d)\n",s32Ret);
			goto Err_sample_fb_display;
		}
		sleep(2);
	}
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);

	return s32Ret;

Err_sample_fb_display:
Err_IMP_TDE_EndJob:
Err_IMP_TDE_QuickCopy:
Err_IMP_TDE_BeginJob:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
Err_IMP_System_GetBlock:
	return -1;
}

IMP_S32 sample_tde_fill()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;
	IMP_U32 i, j;
	IMP_U32 u16ColorBar[] = {0x7d00, 0x02e0, 0x001f, 0x6fef, 0x1f1d};
	IMP_U32 u32ColorBar[] = {0xff0033, 0xff9900,0x0099cc, 0xcc99cc, 0x009933};

	TDE_HANDLE Handle;
	IMP_U16 u16OptNum = 5;
	TDE_SURFACE_S pstDst[8];
	TDE_RECT_S pstDstRect[8];
	IMP_U32	*pFillData;

	TDE_PIXFMT_E enDstPixFmt = SRC_PIXFMT;

	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	if(stFbInfo->var_info.bits_per_pixel == 32) {
		pFillData = u32ColorBar;
	} else {
		pFillData = u16ColorBar;
	}

	for(j = 0; j < 5; j++) {
		/* 1. tde start job */
		Handle = IMP_TDE_BeginJob(u16OptNum);
		if(Handle < 0) {
			printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
			goto Err_IMP_TDE_BeginJob;
		}

		for(i = 0; i < u16OptNum; i++) {
			pstDst[i].u32PhyAddr		= stFbInfo->fix_info.smem_start;
			pstDst[i].u32CbCrPhyAddr	= 0;
			pstDst[i].enPixFmt		= enDstPixFmt;
			pstDst[i].u16Width		= stFbInfo->var_info.xres;
			pstDst[i].u16Height		= stFbInfo->var_info.yres;
			pstDst[i].u16Stride		= stFbInfo->fix_info.line_length;
			pstDst[i].u16CbCrStride		= 0;
		}

		for(i = 0; i < u16OptNum; i++) {
			if(i==0) {
				pstDstRect[i].s32Xpos		= 0;
				pstDstRect[i].s32Ypos		= 0;
				pstDstRect[i].u32Width		= 300;
				pstDstRect[i].u32Height		= 300;
			} else if(i==1) {
				pstDstRect[i].s32Xpos		= pstDst[i].u16Width - 300;
				pstDstRect[i].s32Ypos		= 0;
				pstDstRect[i].u32Width		= 300;
				pstDstRect[i].u32Height		= 300;
			} else if(i==2) {
				pstDstRect[i].s32Xpos		= pstDst[i].u16Width/2 - 150;
				pstDstRect[i].s32Ypos		= pstDst[i].u16Height/2 - 150;
				pstDstRect[i].u32Width		= 300;
				pstDstRect[i].u32Height		= 300;
			} else if(i==3) {
				pstDstRect[i].s32Xpos		= 0;
				pstDstRect[i].s32Ypos		= pstDst[i].u16Height - 300;
				pstDstRect[i].u32Width		= 300;
				pstDstRect[i].u32Height		= 300;
			} else if(i==4) {
				pstDstRect[i].s32Xpos		= pstDst[i].u16Width - 300;
				pstDstRect[i].s32Ypos		= pstDst[i].u16Height - 300;
				pstDstRect[i].u32Width		= 300;
				pstDstRect[i].u32Height		= 300;
			}

			s32Ret = IMP_TDE_QuickFill(Handle, NULL, NULL,
					&pstDst[i], &pstDstRect[i], pFillData[j%5]);
			if(s32Ret < 0) {
				printf("IMP_TDE_QuickFill Error (%d)\n", s32Ret);
				IMP_TDE_CancelJob(Handle);
				goto Err_IMP_TDE_QuickFill;
			}
		}

		/* 2. tde submit job */
		s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
		if(s32Ret < 0) {
			printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
			goto Err_IMP_TDE_EndJob;
		}

		s32Ret = sample_fb_display();
		if(s32Ret < 0) {
			printf("FB_Display Error (%d)\n",s32Ret);
			goto Err_sample_fb_display;
		}
		sleep(2);

	}

	return s32Ret;

Err_sample_fb_display:
Err_IMP_TDE_EndJob:
Err_IMP_TDE_QuickFill:
Err_IMP_TDE_BeginJob:
	return -1;
}

IMP_S32 sample_tde_resize()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;
	IMP_S32 i;

	TDE_HANDLE Handle;
	IMP_VOID *pSrcAddr = NULL;
	IMP_U16 u16OptNum = 1;
	TDE_SURFACE_S pstForeGround;
	TDE_RECT_S pstForeGroundRect;
	TDE_SURFACE_S pstDst;
	TDE_RECT_S pstDstRect;

	TDE_PIXFMT_E enSrcPixFmt = SRC_PIXFMT;
	IMP_U16 u16SrcBpp = SRC_BPP;
	IMP_U16 u16SrcWidth = SRC_WIDTH;
	IMP_U16 u16SrcHeight = SRC_HEIGHT;
	IMP_U16 u16SrcStride = u16SrcWidth * (u16SrcBpp>>3);
	IMP_U32 u32SrcSize = u16SrcStride * u16SrcHeight;

	TDE_PIXFMT_E enDstPixFmt = SRC_PIXFMT;
	IMP_U16 u16DstWidth[5] = {320, 640, 960, 1280, 1920};
	IMP_U16 u16DstHeight[5] = {180, 360, 540, 720, 1080};

	memset(&pstForeGround, 0, sizeof(pstForeGround));
	memset(&pstForeGroundRect, 0, sizeof(pstForeGroundRect));
	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	pSrcAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32SrcSize, "buffer");
	if(!pSrcAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock;
	}
	memset(pSrcAddr, 0x00, u32SrcSize);

	load_picture(pSrcAddr, ARGB1555_TEST_FILE);
	IMP_System_FlushCache(pSrcAddr, u32SrcSize);

	for(i = 0; i < 5; i++) {
		pstForeGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pSrcAddr);
		pstForeGround.u32CbCrPhyAddr 	= 0;
		pstForeGround.enPixFmt 		= enSrcPixFmt;
		pstForeGround.u16Width 		= u16SrcWidth;
		pstForeGround.u16Height 	= u16SrcHeight;
		pstForeGround.u16Stride 	= u16SrcStride;
		pstForeGround.u16CbCrStride 	= 0;
		pstForeGroundRect.s32Xpos 	= 0;
		pstForeGroundRect.s32Ypos 	= 0;
		pstForeGroundRect.u32Width 	= 0;
		pstForeGroundRect.u32Height 	= 0;

		pstDst.u32PhyAddr		= stFbInfo->fix_info.smem_start;
		pstDst.u32CbCrPhyAddr		= 0;
		pstDst.enPixFmt			= enDstPixFmt;
		pstDst.u16Width			= stFbInfo->var_info.xres;
		pstDst.u16Height		= stFbInfo->var_info.yres;
		pstDst.u16Stride		= stFbInfo->fix_info.line_length;
		pstDst.u16CbCrStride		= 0;
		pstDstRect.s32Xpos		= 0;
		pstDstRect.s32Ypos		= 0;
		pstDstRect.u32Width		= u16DstWidth[i];
		pstDstRect.u32Height		= u16DstHeight[i];

		/* 1. tde start job */
		Handle = IMP_TDE_BeginJob(u16OptNum);
		if(Handle < 0) {
			printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
			goto Err_IMP_TDE_BeginJob;
		}

		/* 2. tde submit job */
		s32Ret = IMP_TDE_Resize(Handle, &pstForeGround, &pstForeGroundRect,
				&pstDst, &pstDstRect);
		if(s32Ret < 0) {
			printf("IMP_TDE_Resize Error (%d)\n", s32Ret);
			IMP_TDE_CancelJob(Handle);
			goto Err_IMP_TDE_Resize;
		}

		s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
		if(s32Ret < 0) {
			printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
			goto Err_IMP_TDE_EndJob;
		}

		s32Ret = sample_fb_display();
		if(s32Ret < 0) {
			printf("FB_Display Error (%d)\n",s32Ret);
			goto Err_sample_fb_display;
		}
		sleep(2);
	}

	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);

	return s32Ret;

Err_sample_fb_display:
Err_IMP_TDE_EndJob:
Err_IMP_TDE_Resize:
Err_IMP_TDE_BeginJob:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
Err_IMP_System_GetBlock:
	return -1;
}

IMP_S32 sample_tde_overturn()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;

	TDE_HANDLE Handle;
	IMP_VOID *pSrcAddr = NULL;
	IMP_U16 u16OptNum = 1;
	TDE_SURFACE_S pstForeGround;
	TDE_RECT_S pstForeGroundRect;
	TDE_SURFACE_S pstDst;
	TDE_RECT_S pstDstRect;

	TDE_PIXFMT_E enSrcPixFmt = SRC_PIXFMT;
	IMP_U16 u16SrcBpp = SRC_BPP;
	IMP_U16 u16SrcWidth = SRC_WIDTH;
	IMP_U16 u16SrcHeight = SRC_HEIGHT;
	IMP_U16 u16SrcStride = u16SrcWidth * (u16SrcBpp>>3);
	IMP_U32 u32SrcSize = u16SrcStride * u16SrcHeight;

	memset(&pstForeGround, 0, sizeof(pstForeGround));
	memset(&pstForeGroundRect, 0, sizeof(pstForeGroundRect));
	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	pSrcAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32SrcSize, "buffer");
	if(!pSrcAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock;
	}
	memset(pSrcAddr, 0x00, u32SrcSize);

	load_picture(pSrcAddr, ARGB1555_TEST_FILE);
	IMP_System_FlushCache(pSrcAddr, u32SrcSize);

	/* Start address of the last line */
	pstForeGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pSrcAddr) + u16SrcStride * (u16SrcHeight-1);
	pstForeGround.u32CbCrPhyAddr 	= 0;
	pstForeGround.enPixFmt 		= enSrcPixFmt;
	pstForeGround.u16Width 		= u16SrcWidth;
	pstForeGround.u16Height 	= u16SrcHeight;
	pstForeGround.u16Stride 	= u16SrcStride;
	pstForeGround.u16CbCrStride 	= 0;
	pstForeGroundRect.s32Xpos 	= 0;
	pstForeGroundRect.s32Ypos 	= 0;
	pstForeGroundRect.u32Width 	= 0;
	pstForeGroundRect.u32Height 	= 0;

	pstDst.u32PhyAddr		= stFbInfo->fix_info.smem_start;
	pstDst.u32CbCrPhyAddr		= 0;
	pstDst.enPixFmt			= pstForeGround.enPixFmt;
	pstDst.u16Width			= stFbInfo->var_info.xres;
	pstDst.u16Height		= stFbInfo->var_info.yres;
	pstDst.u16Stride		= stFbInfo->fix_info.line_length;
	pstDst.u16CbCrStride		= 0;
	pstDstRect.s32Xpos		= 0;
	pstDstRect.s32Ypos		= 0;
	pstDstRect.u32Width		= pstForeGround.u16Width;
	pstDstRect.u32Height		= pstForeGround.u16Height;

	/* 1. tde start job */
	Handle = IMP_TDE_BeginJob(u16OptNum);
	if(Handle < 0) {
		printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
		goto Err_IMP_TDE_BeginJob;
	}

	/* 2. tde submit job */
	s32Ret = IMP_TDE_Overturn(Handle, &pstForeGround, &pstForeGroundRect,
			&pstDst, &pstDstRect);
	if(s32Ret < 0) {
		printf("IMP_TDE_Overturn Error (%d)\n", s32Ret);
		IMP_TDE_CancelJob(Handle);
		goto Err_IMP_TDE_Overturn;
	}

	s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
	if(s32Ret < 0) {
		printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
		goto Err_IMP_TDE_EndJob;
	}

	s32Ret = sample_fb_display();
	if(s32Ret < 0) {
		printf("FB_Display Error (%d)\n",s32Ret);
		goto Err_sample_fb_display;
	}
	sleep(1);

	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);

	return s32Ret;

Err_IMP_TDE_EndJob:
Err_IMP_TDE_Overturn:
Err_IMP_TDE_BeginJob:
Err_sample_fb_display:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
Err_IMP_System_GetBlock:
	return -1;
}

IMP_S32 sample_tde_clip()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;
	IMP_S32 i;

	TDE_HANDLE Handle;
	IMP_VOID *pSrcAddr = NULL;
	IMP_U16 u16OptNum = 1;
	TDE_SURFACE_S pstForeGround;
	TDE_RECT_S pstForeGroundRect;
	TDE_SURFACE_S pstDst;
	TDE_RECT_S pstDstRect;
	TDE_OPERATE_S pstOpt;

	TDE_PIXFMT_E enSrcPixFmt = SRC_PIXFMT;
	IMP_U16 u16SrcBpp = SRC_BPP;
	IMP_U16 u16SrcWidth = SRC_WIDTH;
	IMP_U16 u16SrcHeight = SRC_HEIGHT;
	IMP_U16 u16SrcStride = u16SrcWidth * (u16SrcBpp>>3);
	IMP_U32 u32SrcSize = u16SrcStride * u16SrcHeight;

	memset(&pstForeGround, 0, sizeof(pstForeGround));
	memset(&pstForeGroundRect, 0, sizeof(pstForeGroundRect));
	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));
	memset(&pstOpt, 0, sizeof(pstOpt));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	pSrcAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32SrcSize, "buffer");
	if(!pSrcAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock;
	}
	memset(pSrcAddr, 0x00, u32SrcSize);

	load_picture(pSrcAddr, ARGB1555_TEST_FILE);
	IMP_System_FlushCache(pSrcAddr, u32SrcSize);

	for(i = 0; i < 3; i++) {
		/************************
		 * clip outside
		 * **********************/
		memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
		pstForeGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pSrcAddr);
		pstForeGround.u32CbCrPhyAddr 	= 0;
		pstForeGround.enPixFmt 		= enSrcPixFmt;
		pstForeGround.u16Width 		= u16SrcWidth;
		pstForeGround.u16Height 	= u16SrcHeight;
		pstForeGround.u16Stride 	= u16SrcStride;
		pstForeGround.u16CbCrStride 	= 0;
		pstForeGroundRect.s32Xpos 	= 0;
		pstForeGroundRect.s32Ypos 	= 0;
		pstForeGroundRect.u32Width 	= 0;
		pstForeGroundRect.u32Height 	= 0;

		pstDst.u32PhyAddr		= stFbInfo->fix_info.smem_start;
		pstDst.u32CbCrPhyAddr		= 0;
		pstDst.enPixFmt			= pstForeGround.enPixFmt;
		pstDst.u16Width			= stFbInfo->var_info.xres;
		pstDst.u16Height		= stFbInfo->var_info.yres;
		pstDst.u16Stride		= stFbInfo->fix_info.line_length;
		pstDst.u16CbCrStride		= 0;
		pstDstRect.s32Xpos		= 0;
		pstDstRect.s32Ypos		= 0;
		pstDstRect.u32Width		= pstForeGround.u16Width;
		pstDstRect.u32Height		= pstForeGround.u16Height;

		pstOpt.stClipRect.s32Xpos	= 64;
		pstOpt.stClipRect.s32Ypos	= 64;
		pstOpt.stClipRect.u32Width	= 200;
		pstOpt.stClipRect.u32Height	= 100;
		pstOpt.enClipMode		= TDE_CLIPMODE_OUTSIDE;

		/* 1. tde start job */
		Handle = IMP_TDE_BeginJob(u16OptNum);
		if(Handle < 0) {
			printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
			goto Err_IMP_TDE_BeginJob;
		}

		/* 2. tde submit job */
		s32Ret = IMP_TDE_Clip(Handle, &pstForeGround, &pstForeGroundRect,
				&pstDst, &pstDstRect, &pstOpt);
		if(s32Ret < 0) {
			printf("IMP_TDE_Clip Error (%d)\n", s32Ret);
			IMP_TDE_CancelJob(Handle);
			goto Err_IMP_TDE_Clip;
		}

		s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
		if(s32Ret < 0) {
			printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
			goto Err_IMP_TDE_EndJob;
		}

		s32Ret = sample_fb_display();
		if(s32Ret < 0) {
			printf("FB_Display Error (%d)\n",s32Ret);
			goto Err_sample_fb_display;
		}
		sleep(2);

		/************************
		 * clip inside
		 * **********************/
		memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);

		pstOpt.stClipRect.s32Xpos	= 64;
		pstOpt.stClipRect.s32Ypos	= 64;
		pstOpt.stClipRect.u32Width	= 200;
		pstOpt.stClipRect.u32Height	= 100;
		pstOpt.enClipMode = TDE_CLIPMODE_INSIDE;

		/* 1. tde start job */
		Handle = IMP_TDE_BeginJob(u16OptNum);
		if(Handle < 0) {
			printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
			goto Err_IMP_TDE_BeginJob;
		}

		/* 2. tde submit job */
		s32Ret = IMP_TDE_Clip(Handle, &pstForeGround, &pstForeGroundRect,
				&pstDst, &pstDstRect, &pstOpt);
		if(s32Ret < 0) {
			printf("IMP_TDE_Clip Error (%d)\n", s32Ret);
			IMP_TDE_CancelJob(Handle);
			goto Err_IMP_TDE_Clip;
		}

		s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
		if(s32Ret < 0) {
			printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
			goto Err_IMP_TDE_EndJob;
		}

		s32Ret = sample_fb_display();
		if(s32Ret < 0) {
			printf("FB_Display Error (%d)\n",s32Ret);
			goto Err_sample_fb_display;
		}
		sleep(2);

	}
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);

	return s32Ret;

Err_IMP_TDE_EndJob:
Err_IMP_TDE_Clip:
Err_IMP_TDE_BeginJob:
Err_sample_fb_display:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
Err_IMP_System_GetBlock:
	return -1;
}

IMP_S32 sample_tde_mirror()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;

	TDE_HANDLE Handle;
	IMP_VOID *pSrcAddr = NULL;
	IMP_U16 u16OptNum = 1;
	TDE_SURFACE_S pstForeGround;
	TDE_RECT_S pstForeGroundRect;
	TDE_SURFACE_S pstDst;
	TDE_RECT_S pstDstRect;
	TDE_OPERATE_S pstOpt;

	TDE_PIXFMT_E enSrcPixFmt = SRC_PIXFMT;
	IMP_U16 u16SrcBpp = SRC_BPP;
	IMP_U16 u16SrcWidth = SRC_WIDTH;
	IMP_U16 u16SrcHeight = SRC_HEIGHT;
	IMP_U16 u16SrcStride = u16SrcWidth * (u16SrcBpp>>3);
	IMP_U32 u32SrcSize = u16SrcStride * u16SrcHeight;

	memset(&pstForeGround, 0, sizeof(pstForeGround));
	memset(&pstForeGroundRect, 0, sizeof(pstForeGroundRect));
	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));
	memset(&pstOpt, 0, sizeof(pstOpt));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	pSrcAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32SrcSize, "buffer");
	if(!pSrcAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock;
	}
	memset(pSrcAddr, 0x00, u32SrcSize);

	load_picture(pSrcAddr, ARGB1555_TEST_FILE);
	IMP_System_FlushCache(pSrcAddr, u32SrcSize);

	pstForeGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pSrcAddr);
	pstForeGround.u32CbCrPhyAddr 	= 0;
	pstForeGround.enPixFmt 		= enSrcPixFmt;
	pstForeGround.u16Width 		= u16SrcWidth;
	pstForeGround.u16Height 	= u16SrcHeight;
	pstForeGround.u16Stride 	= u16SrcStride;
	pstForeGround.u16CbCrStride 	= 0;
	pstForeGroundRect.s32Xpos 	= 0;
	pstForeGroundRect.s32Ypos 	= 0;
	pstForeGroundRect.u32Width 	= 0;
	pstForeGroundRect.u32Height 	= 0;

	pstDst.u32PhyAddr		= stFbInfo->fix_info.smem_start;
	pstDst.u32CbCrPhyAddr		= 0;
	pstDst.enPixFmt			= pstForeGround.enPixFmt;
	pstDst.u16Width			= stFbInfo->var_info.xres;
	pstDst.u16Height		= stFbInfo->var_info.yres;
	pstDst.u16Stride		= stFbInfo->fix_info.line_length;
	pstDst.u16CbCrStride		= 0;
	pstDstRect.s32Xpos		= 0;
	pstDstRect.s32Ypos		= 0;
	pstDstRect.u32Width		= pstForeGround.u16Width;
	pstDstRect.u32Height		= pstForeGround.u16Height;

	/* 1. tde start job */
	Handle = IMP_TDE_BeginJob(u16OptNum);
	if(Handle < 0) {
		printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
		goto Err_IMP_TDE_BeginJob;
	}

	/* 2. tde submit job */
	s32Ret = IMP_TDE_Mirror(Handle, &pstForeGround, &pstForeGroundRect,
			&pstDst, &pstDstRect);
	if(s32Ret < 0) {
		printf("IMP_TDE_Mirror Error (%d)\n", s32Ret);
		IMP_TDE_CancelJob(Handle);
		goto Err_IMP_TDE_Mirror;
	}

	s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
	if(s32Ret < 0) {
		printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
		goto Err_IMP_TDE_EndJob;
	}

	s32Ret = sample_fb_display();
	if(s32Ret < 0) {
		printf("FB_Display Error (%d)\n",s32Ret);
		goto Err_sample_fb_display;
	}
	sleep(1);

	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);

	return s32Ret;

Err_IMP_TDE_EndJob:
Err_IMP_TDE_Mirror:
Err_IMP_TDE_BeginJob:
Err_sample_fb_display:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
Err_IMP_System_GetBlock:
	return -1;
}

IMP_S32 sample_tde_csc()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;

	TDE_HANDLE Handle;
	IMP_VOID *pSrcAddr = NULL;
	IMP_U16 u16OptNum = 1;
	TDE_SURFACE_S pstForeGround;
	TDE_RECT_S pstForeGroundRect;
	TDE_SURFACE_S pstDst;
	TDE_RECT_S pstDstRect;
	TDE_OPERATE_S pstOpt;

	TDE_PIXFMT_E enSrcPixFmt = TDE_PIXFMT_NV12;
	IMP_U16 u16SrcBpp = 12;
	IMP_U16 u16SrcWidth = 640;
	IMP_U16 u16SrcHeight = 360;
	IMP_U16 u16SrcStride = u16SrcWidth * (u16SrcBpp>>3);
	IMP_U32 u32SrcSize = u16SrcStride * u16SrcHeight * 3 / 2;

	TDE_PIXFMT_E enDstPixFmt = SRC_PIXFMT;

	memset(&pstForeGround, 0, sizeof(pstForeGround));
	memset(&pstForeGroundRect, 0, sizeof(pstForeGroundRect));
	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));
	memset(&pstOpt, 0, sizeof(pstOpt));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	pSrcAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32SrcSize, "buffer");
	if(!pSrcAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock;
	}
	memset(pSrcAddr, 0x00, u32SrcSize);

	load_picture(pSrcAddr, NV12_TEST_FILE);
	IMP_System_FlushCache(pSrcAddr, u32SrcSize);

	pstForeGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pSrcAddr);
	pstForeGround.u32CbCrPhyAddr 	= pstForeGround.u32PhyAddr + u16SrcStride * u16SrcHeight;
	pstForeGround.enPixFmt 		= enSrcPixFmt;
	pstForeGround.u16Width 		= u16SrcWidth;
	pstForeGround.u16Height 	= u16SrcHeight;
	pstForeGround.u16Stride 	= u16SrcStride;
	pstForeGround.u16CbCrStride 	= u16SrcStride;
	pstForeGroundRect.s32Xpos 	= 0;
	pstForeGroundRect.s32Ypos 	= 0;
	pstForeGroundRect.u32Width 	= 0;
	pstForeGroundRect.u32Height 	= 0;

	pstDst.u32PhyAddr		= stFbInfo->fix_info.smem_start;
	pstDst.u32CbCrPhyAddr		= 0;
	pstDst.enPixFmt			= enDstPixFmt;
	pstDst.u16Width			= stFbInfo->var_info.xres;
	pstDst.u16Height		= stFbInfo->var_info.yres;
	pstDst.u16Stride		= stFbInfo->fix_info.line_length;
	pstDst.u16CbCrStride		= 0;
	pstDstRect.s32Xpos		= 0;
	pstDstRect.s32Ypos		= 0;
	pstDstRect.u32Width		= pstForeGround.u16Width;
	pstDstRect.u32Height		= pstForeGround.u16Height;

	pstOpt.enOptType		= TDE_OPT_CSC;
	pstOpt.stOptsEn.bCsc		= IMP_TRUE;
	pstOpt.stCscOpt.enCscMode	= TDE_CSCMODE_YUVTORGB;

	/* 1. tde start job */
	Handle = IMP_TDE_BeginJob(u16OptNum);
	if(Handle < 0) {
		printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
		goto Err_IMP_TDE_BeginJob;
	}

	/* 2. tde submit job */
	s32Ret = IMP_TDE_CSC(Handle, &pstForeGround, &pstForeGroundRect,
			&pstDst, &pstDstRect, &pstOpt);
	if(s32Ret < 0) {
		printf("IMP_TDE_CSC Error (%d)\n", s32Ret);
		IMP_TDE_CancelJob(Handle);
		goto Err_IMP_TDE_CSC;
	}

	s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
	if(s32Ret < 0) {
		printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
		goto Err_IMP_TDE_EndJob;
	}

	s32Ret = sample_fb_display();
	if(s32Ret < 0) {
		printf("FB_Display Error (%d)\n",s32Ret);
		goto Err_sample_fb_display;
	}

	sleep(1);
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);

	return s32Ret;

Err_IMP_TDE_EndJob:
Err_IMP_TDE_CSC:
Err_IMP_TDE_BeginJob:
Err_sample_fb_display:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
Err_IMP_System_GetBlock:
	return -1;
}

IMP_S32 sample_tde_osd()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;

	TDE_HANDLE Handle;
	IMP_VOID *pSrcAddr = NULL;
	IMP_U16 u16OptNum = 1;
	TDE_SURFACE_S pstBackGround;
	TDE_RECT_S pstBackGroundRect;
	TDE_SURFACE_S pstForeGround;
	TDE_RECT_S pstForeGroundRect;
	TDE_SURFACE_S pstDst;
	TDE_RECT_S pstDstRect;
	TDE_OPERATE_S pstOpt;

	TDE_PIXFMT_E enSrcPixFmt = SRC_PIXFMT;
	IMP_U16 u16SrcBpp = SRC_BPP;
	IMP_U16 u16SrcWidth = SRC_WIDTH;
	IMP_U16 u16SrcHeight = SRC_HEIGHT;
	IMP_U16 u16SrcStride = u16SrcWidth * (u16SrcBpp>>3);
	IMP_U32 u32SrcSize = u16SrcStride * u16SrcHeight;

	IMP_VOID *pBgAddr = NULL;
	TDE_PIXFMT_E enBgPixFmt = SRC_PIXFMT;
	IMP_U16 u16BgBpp = SRC_BPP;
	IMP_U16 u16BgWidth = SRC_WIDTH;
	IMP_U16 u16BgHeight = SRC_HEIGHT;
	IMP_U16 u16BgStride = u16BgWidth * (u16BgBpp>>3);
	IMP_U32 u32BgSize = u16BgStride * u16BgHeight;

	memset(&pstBackGround, 0, sizeof(pstBackGround));
	memset(&pstBackGroundRect, 0, sizeof(pstBackGroundRect));
	memset(&pstForeGround, 0, sizeof(pstForeGround));
	memset(&pstForeGroundRect, 0, sizeof(pstForeGroundRect));
	memset(&pstDst, 0, sizeof(pstDst));
	memset(&pstDstRect, 0, sizeof(pstDstRect));
	memset(&pstOpt, 0, sizeof(pstOpt));

	memset(stFbInfo->pBufferAddr, 0x0, stFbInfo->fix_info.smem_len);
	pBgAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32BgSize, "buffer-bg");
	if(!pBgAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock_bg;
	}
	memset(pBgAddr, 0x00, u32BgSize);

	pSrcAddr = IMP_System_GetBlock(pstMemBuffer->u32BufferPool, NULL, u32SrcSize, "buffer");
	if(!pSrcAddr){
		printf("IMP_System_GetBlock is Failed!\n");
		goto Err_IMP_System_GetBlock;
	}
	memset(pSrcAddr, 0x00, u32SrcSize);

	load_picture(pBgAddr, ARGB1555_TEST_FILE);
	IMP_System_FlushCache(pBgAddr, u32BgSize);

	draw_osd(pSrcAddr, u16SrcWidth, u16SrcHeight, u16SrcBpp);
	IMP_System_FlushCache(pSrcAddr, u32SrcSize);

	pstBackGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pBgAddr);
	pstBackGround.u32CbCrPhyAddr 	= 0;
	pstBackGround.enPixFmt 		= enBgPixFmt;
	pstBackGround.u16Width 		= u16BgWidth;
	pstBackGround.u16Height 	= u16BgHeight;
	pstBackGround.u16Stride 	= u16BgStride;
	pstBackGround.u16CbCrStride 	= 0;
	pstBackGroundRect.s32Xpos 	= 0;
	pstBackGroundRect.s32Ypos 	= 0;
	pstBackGroundRect.u32Width 	= 0;
	pstBackGroundRect.u32Height 	= 0;

	pstForeGround.u32PhyAddr 	= (IMP_U32)IMP_System_Block2PhyAddr(pSrcAddr);
	pstForeGround.u32CbCrPhyAddr 	= 0;
	pstForeGround.enPixFmt 		= enSrcPixFmt;
	pstForeGround.u16Width 		= u16SrcWidth;
	pstForeGround.u16Height 	= u16SrcHeight;
	pstForeGround.u16Stride 	= u16SrcStride;
	pstForeGround.u16CbCrStride 	= 0;
	pstForeGroundRect.s32Xpos 	= 0;
	pstForeGroundRect.s32Ypos 	= 0;
	pstForeGroundRect.u32Width 	= 0;
	pstForeGroundRect.u32Height 	= 0;

	pstDst.u32PhyAddr		= stFbInfo->fix_info.smem_start;
	pstDst.u32CbCrPhyAddr		= 0;
	pstDst.enPixFmt			= pstForeGround.enPixFmt;
	pstDst.u16Width			= stFbInfo->var_info.xres;
	pstDst.u16Height		= stFbInfo->var_info.yres;
	pstDst.u16Stride		= stFbInfo->fix_info.line_length;
	pstDst.u16CbCrStride		= 0;
	pstDstRect.s32Xpos		= 0;
	pstDstRect.s32Ypos		= 0;
	pstDstRect.u32Width		= SRC_WIDTH;
	pstDstRect.u32Height		= SRC_HEIGHT;

	pstOpt.enOptType		= TDE_OPT_MIRROR;
	pstOpt.stOptsEn.bBlend		= IMP_TRUE;
	pstOpt.stBlendOpt.bGlobalAlphaEnable = IMP_TRUE;
	pstOpt.stBlendOpt.eBlendCmd 	= TDE_BLENDCMD_NONE;
	pstOpt.stBlendOpt.eSrc1BlendMode = TDE_BLEND_ONE;
	pstOpt.stBlendOpt.eSrc2BlendMode = TDE_BLEND_ONE;

	/* 1. tde start job */
	Handle = IMP_TDE_BeginJob(u16OptNum);
	if(Handle < 0) {
		printf("IMP_TDE_BeginJob get Invalid Handle (%d)\n", Handle);
		goto Err_IMP_TDE_BeginJob;
	}

	/* 2. tde submit job */
	s32Ret = IMP_TDE_Bitblit(Handle, &pstBackGround, &pstBackGroundRect,
			&pstForeGround, &pstForeGroundRect,
			&pstDst, &pstDstRect,
			&pstOpt);
	if(s32Ret < 0) {
		printf("IMP_TDE_Bitblit Error (%d)\n", s32Ret);
		IMP_TDE_CancelJob(Handle);
		goto Err_IMP_TDE_Bitblit;
	}

	s32Ret = IMP_TDE_EndJob(Handle, 0, 0, -1);
	if(s32Ret < 0) {
		printf("IMP_TDE_EndJob Error (%d)\n",s32Ret);
		goto Err_IMP_TDE_EndJob;
	}

	s32Ret = sample_fb_display();
	if(s32Ret < 0) {
		printf("FB_Display Error (%d)\n",s32Ret);
		goto Err_sample_fb_display;
	}
	sleep(1);

	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pBgAddr);

	return s32Ret;

Err_IMP_TDE_EndJob:
Err_IMP_TDE_Bitblit:
Err_IMP_TDE_BeginJob:
Err_sample_fb_display:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pSrcAddr);
Err_IMP_System_GetBlock:
	IMP_System_ReleaseBlock(pstMemBuffer->u32BufferPool, pBgAddr);
Err_IMP_System_GetBlock_bg:
	return -1;
}

IMP_VOID sample_tde_handle_sig(IMP_S32 s32Signo)
{
	IMP_S32 s32VoMod = SAMPLE_VO_MODULE;

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	if(s32Signo == SIGINT || s32Signo == SIGTERM) {
		printf("catch signal SIGINT or SIGTERM\nprogram exit abnormally\n");
	}

	IMP_System_DestroyPool(g_stMemBuffer.u32BufferPool, g_stMemBuffer.s8PoolName);
	/* system exit */
	sample_fb_exit();
	sample_vo_hdmi_stop();
	sample_vo_stop_module(s32VoMod);
	sample_system_exit();
	exit(0);
}

void sample_tde_usage(void)
{
	printf("\n\n/************************************/\n");
	printf("please choose the cmd which you want to run:\n");
	printf("\t0/q 	: quit the program\n");
	printf("\t1 	: tde copy test\n");
	printf("\t2 	: tde fill test\n");
	printf("\t3 	: tde resize test\n");
	printf("\t4 	: tde overturn test\n");
	printf("\t5 	: tde clip test\n");
	printf("\t6 	: tde mirror test\n");
	printf("\t7 	: tde csc test\n");
	printf("\t8 	: tde osd test\n");
}

/*
 * file		: sample_tde_show.c
 * description	: this sample shows how to use TDE interface to draw graphic
 * */
int main(int argc, char *argv[])
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32VoMod;
	VO_PUB_ATTR_S stVoPubAttr;
	MemBuffer_st *pstMemBuffer = &g_stMemBuffer;
	FbAttrInfo_st *stFbInfo = &g_stFbAttrInfo;
	IMP_CHAR Cmd[16] = {0};

	signal(SIGINT, sample_tde_handle_sig);
	signal(SIGTERM, sample_tde_handle_sig);

	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_system_init failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	/* system init */
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_system_init failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	/* vo init */
	s32VoMod = SAMPLE_VO_MODULE;
	IMP_VO_Disable(s32VoMod);
	stVoPubAttr.u32BgColor = 0x00000000;
	stVoPubAttr.stCanvasSize.u32Width = 1920;
	stVoPubAttr.stCanvasSize.u32Height = 1080;
	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	stVoPubAttr.enIntfType = VO_INTF_VGA | VO_INTF_HDMI;
	/* stVoPubAttr.stCanvasSize.u32Width = 3840; */
	/* stVoPubAttr.stCanvasSize.u32Height = 2160; */
	/* stVoPubAttr.enIntfSync = VO_OUTPUT_3840x2160_30; */
	/* stVoPubAttr.enIntfType = VO_INTF_HDMI; */
	s32Ret = sample_vo_start_module(s32VoMod, &stVoPubAttr);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_vo_start_module failed! ret = 0x%08x\n", s32Ret);
		goto Err_sample_vo_start_module;
	}

	s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_vo_hdmi_start failed! ret = 0x%08x\n", s32Ret);
		goto Err_sample_vo_hdmi_start;
	}
	s32Ret = sample_fb_init(stFbInfo);
	if (s32Ret != IMP_SUCCESS) {
		printf("sample_fb_init failed! ret = 0x%08x\n", s32Ret);
		goto Err_sample_fb_init;
	}

	sprintf(pstMemBuffer->s8PoolName, "tde-bufferpool");
	pstMemBuffer->u32PoolSize = 2 * SRC_SIZE;
	s32Ret = IMP_System_CreatPool(&pstMemBuffer->u32BufferPool, pstMemBuffer->u32PoolSize,
			1, pstMemBuffer->s8PoolName);
	if (s32Ret != IMP_SUCCESS) {
		printf("IMP_buf_createpool failed! ret = 0x%08x\n", s32Ret);
		goto Err_IMP_buf_createpool;
	}

	while(1) {
		sample_tde_usage();
		printf(">");
		fflush(stdin);
		if (fgets(Cmd, 15, stdin) == NULL) {
			return -1;
		}
		if (Cmd[strlen(Cmd) - 1] == '\n')
			Cmd[strlen(Cmd) - 1] = '\0';

		if (strncmp("help", Cmd, strlen("help")) == 0) {
			sample_tde_usage();
		}
		if (strncmp("q", Cmd, strlen("q")) == 0) {
			return 0;
		}
		switch (Cmd[0]) {
		case '0':
		case 'q':
			exit(0);
			break;
		case '1':
			s32Ret = sample_tde_copy();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		case '2':
			s32Ret = sample_tde_fill();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		case '3':
			s32Ret = sample_tde_resize();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		case '4':
			s32Ret = sample_tde_overturn();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		case '5':
			s32Ret = sample_tde_clip();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		case '6':
			s32Ret = sample_tde_mirror();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		case '7':
			s32Ret = sample_tde_csc();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		case '8':
			s32Ret = sample_tde_osd();
			if(s32Ret < 0)
				goto Err_Test_xxx;
			break;
		default :
			printf("Invalid cmd, please again input\n");
			break;

		}
	}

	IMP_System_DestroyPool(pstMemBuffer->u32BufferPool, pstMemBuffer->s8PoolName);
	sample_fb_exit();
	sample_vo_hdmi_stop();
	sample_vo_stop_module(s32VoMod);
	sample_system_exit();
	return s32Ret;


Err_Test_xxx:
	IMP_System_DestroyPool(pstMemBuffer->u32BufferPool, pstMemBuffer->s8PoolName);
Err_IMP_buf_createpool:
	sample_fb_exit();
Err_sample_fb_init:
	sample_vo_hdmi_stop();
Err_sample_vo_hdmi_start:
	sample_vo_stop_module(s32VoMod);
Err_sample_vo_start_module:
	sample_system_exit();
	return s32Ret;
}
