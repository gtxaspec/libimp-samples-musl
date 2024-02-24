/*
 * sample-common.h
 *
 * Copyright (C) 2021 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __SAMPLE_COMMON_H__
#define __SAMPLE_COMMON_H__

#include <unistd.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_hdmi.h>
#include <imp/imp_vo.h>
#include <imp/imp_fb.h>
#include <imp/imp_log.h>
#include <imp/imp_vdec.h>
#include <imp/imp_venc.h>
#include <imp/imp_tde.h>
#include <imp/imp_audio.h>
#include <pthread.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define ALIGN_UP(x, a)              ((x+(a)-1)&(~((a)-1)))
#define ALIGN_BACK(x, a)            ((a) * (((x) / (a))))
#define MAX_VDEC_DEVS		(36)
#define MAX_VO_CHN			(36)
#define MAX_VENC_CHN		(32)
#define SAMPLE_VO_MODULE	(MOD_ID_VO)
#define SAMPLE_VO_LATER0	(0)
#define SAMPLE_VO_LATER1	(1)
#define SAMPLE_VO_CHN0		(0)
#define SAMPLE_VDEC_DEV0	(0)
#define SAMPLE_VDEC_CHN0	(0)
#define ORIGINAL_X			(0)
#define ORIGINAL_Y			(0)

#define SAMPLE_AI_DEVID            (1)
#define SAMPLE_AO_DEVID            (0)
#define SAMPLE_HDMI_AO_DEVID       (1)
#define SAMPLE_AUDIO_CHN0          (0)

#define SAMPLE_H264_STREAM_PATH		"640x360.h264"
#define SAMPLE_H265_STREAM_PATH		"640x360.hevc"
#define SAMPLE_JPEG_STREAM_PATH		"640x360.mjpeg"
#define SAMPLE_JPEG_PIC_PATH		"640x360.jpg"
#define SAMPLE_JPEG_OUT_PIC_PATH	"640x360.argb1555"
#define STREAM_WIDTH				640
#define STREAM_HEIGHT				360
#define NUM_SAVE_FRAME				20

typedef enum{
	VO_MODE_1MUX  = 1,
	VO_MODE_4MUX = 4,
	VO_MODE_9MUX = 9,
	VO_MODE_16MUX = 16,
	VO_MODE_25MUX = 25,
	VO_MODE_36MUX = 36,
	VO_MODE_BUTT
}SAMPLE_VO_MODE_E;

int sample_system_init(void);
int sample_system_exit(void);

IMP_S32 sample_vo_getwh(VO_INTF_SYNC_E enIntfSync, IMP_U32 *pu32W,IMP_U32 *pu32H, IMP_U32 *pu32Frm);
IMP_S32 sample_vo_start_module(IMP_S32 S32VoMod,VO_PUB_ATTR_S *pstPubAttr);
IMP_S32 sample_vo_stop_module(IMP_S32 S32VoMod);
IMP_S32 sample_vo_start_layer(IMP_S32 s32VoLayer,VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
IMP_S32 sample_vo_stop_layer(IMP_S32 s32VoLayer);
IMP_S32 sample_vo_start_chns(IMP_S32 s32Volayer,SAMPLE_VO_MODE_E enMode);
IMP_S32 sample_vo_stop_chns(IMP_S32 s32Volayer, SAMPLE_VO_MODE_E enMode);
IMP_S32 sample_vo_start_chn(IMP_S32 s32Volayer,IMP_S32 s32VoChn,VO_CHN_ATTR_S *pstChnAttr);
IMP_S32 sample_vo_stop_chn(IMP_S32 s32Volayer,IMP_S32 s32VoChn);
IMP_S32 sample_vo_hdmi_start(VO_INTF_SYNC_E	enIntfSync);
IMP_VOID sample_vo_hdmi_stop(IMP_VOID);

typedef enum{
	VDEC_CTRL_START,
	VDEC_CTRL_PAUSE,
	VDEC_CTRL_STOP,
}VDEC_CTRL_SIG_E;

typedef struct{
	IMP_S32			s32DevId;
	IMP_S32			s32ChnId;
	IMP_CHAR		cFileName[64];
	PAYLOAD_TYPE_E	enType;
	VIDEO_MODE_E	enVideoMode;
	IMP_U32			u32BufSize;
	VDEC_CTRL_SIG_E enCtrlSig;			/* 控制送帧线程结束的标志位 */
	IMP_S32			s32FpsCtrl;			/* 使用SDK内部帧率控制功能 */
	IMP_BOOL		bLoopPlayback;		/* 是否循环播放码流 */
} SAMPLE_VDEC_THREAD_S;


IMP_S32 sample_vdec_start_dev(IMP_S32 s32DevNum, VDEC_DEV_ATTR_S *pstDevAttr);
IMP_S32 sample_vdec_stop_dev(IMP_S32 s32DevNum);
IMP_S32 sample_vdec_start_chn(IMP_S32 s32DevNum,IMP_S32 s32ChnNum,VDEC_CHN_ATTR_S *pstChnAttr);
IMP_S32 sample_vdec_stop_chn(IMP_S32 s32DevNum,IMP_S32 s32ChnNum);
IMP_S32 sample_vdec_start_recv_stream(IMP_S32 s32DevNum);
IMP_S32 sample_vdec_stop_recv_stream(IMP_S32 s32DevNum);
IMP_VOID sample_vdec_thread_param(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara,VDEC_DEV_ATTR_S *pstDevAttr,char *pStreamFileName);
IMP_S32 sample_vdec_start_send_stream(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara, pthread_t *pVdecThread);
IMP_VOID sample_vdec_stop_send_stream(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara, pthread_t *pVdecThread);
IMP_VOID *sample_vdec_send_stream_thread(IMP_VOID *pArg);
IMP_VOID *sample_vdec_get_frame_thread(IMP_VOID *parg);
IMP_VOID sample_vdec_stop_get_frame(IMP_S32 s32DevCnt,SAMPLE_VDEC_THREAD_S *pstVdecThreadPara, pthread_t *pVdecThread);

IMP_S32 sample_venc_start_chn(IMP_S32 s32Chn, VENC_CHN_ATTR_S *chnAttr);
IMP_S32 sample_venc_stop_chn(IMP_S32 s32Chn);
IMP_S32 sample_venc_start_send_frame(IMP_S32 s32Chn[], IMP_S32 s32Cnt, IMP_S32 s32FrmCnt, FILE *inFile);
IMP_S32 sample_venc_stop_send_frame(IMP_VOID);
IMP_S32 sample_venc_start_get_stream(IMP_S32 s32Chn[], IMP_S32 s32Cnt);
IMP_S32 sample_venc_stop_get_stream(IMP_VOID);

typedef struct{
	IMP_S32   s32DevId;
	IMP_S32   s32Chn;
	IMP_S32   s32CodecId;
	IMP_S32   s32GetFrmNum;
	IMP_S32   s32BufSize;
	IMP_CHAR* pcFileName;
	IMP_CHAR* pcRefFileName;
	IMP_S32	  isStart;
	pthread_t thread_id;
}SAMPLE_AUDIO_THREAD_S;

typedef struct{
	int isCodec;//is enable codec
	int isAlg;
	int isMute;
	int isDebug;
	int isSaveFrm;
	int isPause;

	IMP_AUDIO_SAMPLE_RATE_E eAiRate;
	IMP_AUDIO_SAMPLE_RATE_E eAoRate;
	IMP_AUDIO_SAMPLE_RATE_E eHdmiRate;
	IMP_AUDIO_SAMPLE_RATE_E eStreamRate;
	int setVol;
	IMP_FLOAT vol;// *0.5
	IMP_FLOAT gain;// *1.5
	IMP_FLOAT muteStatus;//mute before
	IMP_AUDIO_PALY_LOAD_TYPE_E eCodec;
	char codecName[64];

	//hdmi
	IMP_U32 u32BgColor;
	IMP_U32 u32CanvasWidth;
	IMP_U32 u32CanvasHeight;
	VO_INTF_TYPE_E enIntfType;
	VO_INTF_SYNC_E enIntfSync;

}SAMPLE_AUDIO_S;

extern SAMPLE_AUDIO_S g_sample_audio;

#define F_CHN_VOL_DB				(0.0)
#define F_CHN_GAIN_DB				(21.0)
#define AUDIO_RECORD_FILENAME	"test_aec_record"
#define AUDIO_PLAY_FILENAME		"test_reply"
#define AUDIO_REF_FILENAME		"test_ref"

//hdmi-vo settings
#define SAMPLE_AUDIO_HDMI_BG_COLOR 	0xf5f4f3f2
#define SAMPLE_AUDIO_HDMI_WIDTH		1280
#define SAMPLE_AUDIO_HDMI_HEIGHT	720
#define SAMPLE_AUDIO_HDMI_INTF_TYPE	VO_INTF_HDMI
#define SAMPLE_AUDIO_HDMI_INTF_SYNC	VO_OUTPUT_720P60

#define SAMPLE_DBG(s32Ret)	do {\
	IMP_LOG_ERR(TAG, "%s failed! s32Ret = 0x%08x\n", __FUNCTION__, s32Ret);\
} while(0)

#define SAMPLE_AUDIO_TODO_D()	do {\
	sleep(1);\
	SAMPLE_AUDIO_TODO_Print();\
	int exit = 0;\
	char ch = '\0';\
	do {\
		ch = getchar();\
		if (ch == 10)\
		continue;\
		exit = SAMPLE_AUDIO_TODO(ch);\
	} while (exit == 0);\
} while (0)

//sample-common-audio.c
IMP_S32 SAMPLE_AUDIO_TODO(char ch);
IMP_VOID SAMPLE_AUDIO_TODO_Print();
IMP_S32 SAMPLE_AUDIO_ParseParam(int argc, char **argv);
IMP_VOID SAMPLE_AUDIO_Usage(IMP_VOID);
IMP_VOID SAMPLE_AUDIO_Init();
FILE *SAMPLE_AUDIO_OpenCreatefile(IMP_CHAR *filename, IMP_AUDIO_PALY_LOAD_TYPE_E eType);
FILE *SAMPLE_AUDIO_OpenOnlyfile(IMP_CHAR *filename, IMP_AUDIO_PALY_LOAD_TYPE_E eType);
IMP_S32 SAMPLE_AUDIO_AENC_DealStream(IMP_S32 s32AencChn, IMP_AUDIO_FRAME_S *stStreamIn, FILE *file);
IMP_S32 SAMPLE_AUDIO_ADEC_DealStream(SAMPLE_AUDIO_THREAD_S *pstTrd, IMP_AUDIO_STREAM_S *stStreamIn, IMP_AUDIO_FRAME_S *stFrm, FILE *file);
IMP_VOID SAMPLE_AUDIO_AdjustVol();
IMP_S32 SAMPLE_AUDIO_SetVol(int isAi, IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb, IMP_FLOAT *pfGainDb);
IMP_S32 SAMPLE_AUDIO_AdecCreate(IMP_S32 s32AdecChn);
IMP_S32 SAMPLE_AUDIO_AdecDestroy(IMP_S32 s32AdecChn);
IMP_S32 SAMPLE_AUDIO_AencCreate(IMP_S32 s32AencChn);
IMP_S32 SAMPLE_AUDIO_AencDestroy(IMP_S32 s32AencChn);
IMP_S32 SAMPLE_AUDIO_EnableResmp(IMP_S32 isAi, IMP_S32 s32DevId, IMP_S32 s32Chn);
IMP_S32 SAMPLE_AUDIO_DisableResmp(IMP_S32 isAi, IMP_S32 s32DevId, IMP_S32 s32Chn);
IMP_S32 SAMPLE_AUDIO_EnableAlg(int isAi, IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr);
IMP_S32 SAMPLE_AUDIO_DisableAlg(int isAi, IMP_S32 s32DevId, IMP_S32 s32Chn);
IMP_S32 SAMPLE_AUDIO_AOPause(IMP_S32 s32DevId, IMP_S32 s32Chn);
IMP_S32 SAMPLE_AUDIO_AOEnable(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr);
IMP_S32 SAMPLE_AUDIO_AODisable(IMP_S32 s32DevId, IMP_S32 s32Chn);
IMP_S32 SAMPLE_AUDIO_HdmiAoEnable(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr);
IMP_S32 SAMPLE_AUDIO_HdmiAoDisable(IMP_S32 s32DevId,IMP_S32 s32Chn);
IMP_S32 SAMPLE_AUDIO_AIEnable(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr);
IMP_S32 SAMPLE_AUDIO_AIDisable(IMP_S32 s32DevId, IMP_S32 s32Chn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_COMMON_H__ */



