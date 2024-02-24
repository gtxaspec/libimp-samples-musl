/*
 * Ingenic IMP A1 Audio Sample.
 *
 * Copyright (C) 2017 Ingenic Semiconductor Co.,Ltd
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/prctl.h>

#include <imp/imp_audio.h>
#include <imp/imp_log.h>
#include <imp/imp_utils.h>

#include "../common/sample-common.h"

#define TAG "Sample-Audio"

#define IMP_TRUE 	1
#define IMP_FALSE	0

/* if set one frame time != 40ms(AUDIO_ONEFRAME_TIME), please use *_EXT interfaces */
//#define SAMPLE_FRAMESIZE_EXT
#ifndef SAMPLE_FRAMESIZE_EXT
#	define AUDIO_ONEFRAME_TIME			(40)	//default : 40ms per frame for audio sdk
#else
#	define AUDIO_ONEFRAME_TIME			(128)	// XXms per frame for audio sdk
#endif

//default : 40ms per frame for audio.ko [You should not modify it!]
#	define AUDIO_ONEFRAME_TIME_DRIVER	(40)

static pthread_t *record_thread_id = NULL;
static pthread_t *play_thread_id = NULL;
static pthread_t *play_hdmi_thread_id = NULL;
static pthread_t *aec_record_thread_id = NULL;
static pthread_t *aec_play_thread_id = NULL;
static pthread_t *ref_record_thread_id = NULL;
static pthread_t *ref_play_thread_id = NULL;

static IMP_VOID SAMPLE_AUDIO_HandleSig(IMP_S32 s32Signo)
{
	IMP_S32 s32Ret = 0;

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if (s32Signo == SIGINT || s32Signo == SIGTERM) {
		printf("catch signal SIGINT or SIGTERM\nprogram exit abnormally\n");
	}

	if (record_thread_id != NULL) {
		if (*record_thread_id != 0) {
			s32Ret = pthread_kill(*record_thread_id, 0);
			if (0 == s32Ret) {
				pthread_cancel(*record_thread_id);
				pthread_join(*record_thread_id, NULL);
			}
		}
	}

	if (play_thread_id != NULL) {
		if (*play_thread_id != 0) {
			s32Ret = pthread_kill(*play_thread_id, 0);
			if (0 == s32Ret) {
				pthread_cancel(*play_thread_id);
				pthread_join(*play_thread_id, NULL);
			}
		}
	}

	if (play_hdmi_thread_id != NULL) {
		if (*play_hdmi_thread_id != 0) {
			s32Ret = pthread_kill(*play_hdmi_thread_id, 0);
			if (0 == s32Ret) {
				pthread_cancel(*play_hdmi_thread_id);
				pthread_join(*play_hdmi_thread_id, NULL);
			}
		}
	}

	if (aec_record_thread_id != NULL) {
		if (*aec_record_thread_id != 0) {
			s32Ret = pthread_kill(*aec_record_thread_id, 0);
			if (0 == s32Ret) {
				pthread_cancel(*aec_record_thread_id);
				pthread_join(*aec_record_thread_id, NULL);
			}
		}
	}

	if (aec_play_thread_id != NULL) {
		if (*aec_play_thread_id != 0) {
			s32Ret = pthread_kill(*aec_play_thread_id, 0);
			if (0 == s32Ret) {
				pthread_cancel(*aec_play_thread_id);
				pthread_join(*aec_play_thread_id, NULL);
			}
		}
	}

	/* system exit */
	s32Ret = sample_system_exit();
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return;
	}
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
static void *SAMPLE_AUDIO_AencAiProc(void *param)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_U8 * pu8Buf = NULL;
	FILE *codec_file = NULL;
	FILE *pcm_file = NULL;
	IMP_AUDIO_FRAME_S stFrm;
	SAMPLE_AUDIO_THREAD_S *pstTrd = (SAMPLE_AUDIO_THREAD_S *)param;

	prctl(PR_SET_NAME, "imp_AencAiProc", 0, 0, 0);

	pu8Buf = (IMP_U8 *)malloc(pstTrd->s32BufSize);
	if (NULL == pu8Buf) {
		printf("%s : malloc failed!\n", __FUNCTION__);
		return NULL;
	}

	if (g_sample_audio.isCodec) {
		codec_file = SAMPLE_AUDIO_OpenCreatefile(NULL, g_sample_audio.eCodec);
		if (NULL == codec_file) {
			return NULL;
		}
	}

	pcm_file = SAMPLE_AUDIO_OpenCreatefile(NULL, PT_PCM);
	if (NULL == pcm_file) {
		return NULL;
	}

	while (pstTrd->isStart) {
		s32Ret = IMP_AI_GetFrame(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		fwrite(stFrm.pu32VirAddr, 1, stFrm.s32Len, pcm_file);

		/******************************************************
		 * send stream to aenc(g711a, g711u, g726, adpcm, ...)
		 * ***************************************************/
		s32Ret = SAMPLE_AUDIO_AENC_DealStream(pstTrd->s32CodecId, &stFrm, codec_file);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
	}

	free(pu8Buf);
	if (g_sample_audio.isCodec) {
		fclose(codec_file);
	}
	fclose(pcm_file);
	pstTrd->isStart = 0;

	return NULL;
}

static IMP_S32 SAMPLE_AUDIO_CreateThread_AencAi(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	pstTrd->isStart = 1;
	s32Ret = pthread_create(&pstTrd->thread_id, 0, SAMPLE_AUDIO_AencAiProc, pstTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}

	record_thread_id = &pstTrd->thread_id;

	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_DestroyThread_AencAi(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (pstTrd->isStart) {
		pstTrd->isStart = 0;
		pthread_join(pstTrd->thread_id, NULL);
	}

	return s32Ret;
}

/*****************************************************************************
 * function : AI > (Alg) > (Resmp) > (Aenc) > file
 * **************************************************************************/
static IMP_S32 SAMPLE_AUDIO_AencAi()
{
	IMP_AUDIO_IOATTR_S stAttr;
	SAMPLE_AUDIO_THREAD_S stTrd;
	IMP_S32 s32Ret		= IMP_SUCCESS;
	IMP_S32 s32DevId	= SAMPLE_AI_DEVID;
	IMP_S32 s32Chn		= SAMPLE_AUDIO_CHN0;
	IMP_S32 s32AencChn = 1;

	do {
		/********************************
		 * enable ai
		 * *****************************/
		stAttr.eSamplerate 		= g_sample_audio.eAiRate;
		stAttr.eBitwidth   		= AUDIO_BIT_WIDTH_16;
		stAttr.eSoundmode  		= AUDIO_SOUND_MODE_MONO;
		stAttr.s32NumPerFrm 	= g_sample_audio.eAiRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttr.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_AIEnable(s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		/********************************
		 * enable resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_TRUE, s32DevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_ai;
		}

		/*******************************
		 * enable alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_TRUE, s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp;
		}

		/*******************************
		 * set vol
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_TRUE, s32DevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg;
		}

		/*******************************
		 * open aenc codec
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_AencCreate(s32AencChn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg;
		}

		/*******************************
		 * start thread
		 * ****************************/
		stTrd.thread_id = 1;
		stTrd.s32DevId = s32DevId;
		stTrd.s32Chn = s32Chn;
		stTrd.s32CodecId = s32AencChn;
		strncpy(stTrd.pcFileName, g_sample_audio.codecName, strlen(g_sample_audio.codecName));
		stTrd.s32BufSize =  sizeof(IMP_U16) * g_sample_audio.eAiRate * AUDIO_ONEFRAME_TIME / 1000 * AUDIO_SOUND_MODE_MONO;
		s32Ret = SAMPLE_AUDIO_CreateThread_AencAi(&stTrd);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto destroy_aenc;
		}
	} while (0);

	//this sample todo_d
	SAMPLE_AUDIO_TODO_D();

	/*******************************
	 * stop thread
	 * ****************************/
	s32Ret = SAMPLE_AUDIO_DestroyThread_AencAi(&stTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
destroy_aenc:
	s32Ret = SAMPLE_AUDIO_AencDestroy(s32AencChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_TRUE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_TRUE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_ai:
	s32Ret = SAMPLE_AUDIO_AIDisable(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
static void *SAMPLE_AUDIO_AdecAoProc(void *param)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32Size = 0;
	IMP_U8 *pu8Buf = NULL;
	FILE *codec_file = NULL;
	FILE *pcm_file = NULL;

	SAMPLE_AUDIO_THREAD_S *pstTrd = (SAMPLE_AUDIO_THREAD_S *)param;

	prctl(PR_SET_NAME, "imp_AdecAoProc", 0, 0, 0);

	pu8Buf = (IMP_U8 *)malloc(pstTrd->s32BufSize);
	if (NULL == pu8Buf) {
		printf("%s : malloc failed!\n", __FUNCTION__);
		return NULL;
	}

	codec_file = SAMPLE_AUDIO_OpenOnlyfile(NULL, g_sample_audio.eCodec);
	if (NULL == codec_file) {
		return NULL;
	}

	pcm_file = SAMPLE_AUDIO_OpenCreatefile(NULL, PT_PCM);
	if (NULL == pcm_file) {
		return NULL;
	}

	if (g_sample_audio.isCodec) {
		switch (g_sample_audio.eCodec){
			case PT_G726:
				pstTrd->s32BufSize /= 8;
				break;
			case PT_G711A:
			case PT_G711U:
				pstTrd->s32BufSize /= 2;
				break;
			case PT_ADPCM:
				pstTrd->s32BufSize /= 4;
				break;
			default:
				break;
		}
	}

	IMP_AUDIO_FRAME_S stFrm;
	IMP_AUDIO_STREAM_S stStreamIn;
	while (pstTrd->isStart) {
		s32Size = fread(pu8Buf, 1, pstTrd->s32BufSize, codec_file);
		if (s32Size <= 0) {
			//printf("read size:%d\n", s32Size);
			fseek(codec_file, 0, SEEK_SET);//read file again
			continue;
		}

		/******************************************************
		 * send stream to adec(g711a, g711u, g726, adpcm, ...)
		 * ***************************************************/
		stStreamIn.pu8Stream = (IMP_U8 *)pu8Buf;
		stStreamIn.s32Len = s32Size;
		s32Ret = SAMPLE_AUDIO_ADEC_DealStream(pstTrd, &stStreamIn, &stFrm, pcm_file);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		/*****************************************************
		 * adjust vol
		 * **************************************************/
		SAMPLE_AUDIO_AdjustVol();

		s32Ret = SAMPLE_AUDIO_SetVol(0, pstTrd->s32DevId, pstTrd->s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
		//printf("vol:%f, gain:%f\n", g_sample_audio.vol, g_sample_audio.gain);

		/****************************************************
		 * send Frame
		 * *************************************************/
#ifdef SAMPLE_FRAMESIZE_EXT
		s32Ret = IMP_AO_SendFrame_EXT(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#else
		s32Ret = IMP_AO_SendFrame(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#endif

		/*****************************************************
		 *	PAUSE
		 * **************************************************/
		s32Ret = SAMPLE_AUDIO_AOPause(pstTrd->s32DevId, pstTrd->s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
	}

	free(pu8Buf);
	fclose(codec_file);
	fclose(pcm_file);
	pstTrd->isStart = 0;

	return NULL;
}

static IMP_S32 SAMPLE_AUDIO_CreateThread_AdecAo(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	pstTrd->isStart = 1;
	s32Ret = pthread_create(&pstTrd->thread_id, 0, SAMPLE_AUDIO_AdecAoProc, pstTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}

	play_thread_id = &pstTrd->thread_id;
	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_DestroyThread_AdecAo(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (pstTrd->isStart) {
		pstTrd->isStart = 0;
		pthread_join(pstTrd->thread_id, NULL);
	}

	return s32Ret;
}

/*****************************************************************************
 * function : file > (Adec) > (Resmp) > (Alg) > AO
 * **************************************************************************/
static IMP_S32 SAMPLE_AUDIO_AdecAo()
{
	IMP_AUDIO_IOATTR_S stAttr;
	SAMPLE_AUDIO_THREAD_S stTrd;
	IMP_S32 s32Ret		= IMP_SUCCESS;
	IMP_S32 s32DevId	= SAMPLE_AO_DEVID;
	IMP_S32 s32Chn		= SAMPLE_AUDIO_CHN0;
	IMP_S32 s32AdecChn	= 0;

	do {
		/********************************
		 * enable ao
		 * *****************************/
		stAttr.eSamplerate 		= g_sample_audio.eAoRate;
		stAttr.eBitwidth   		= AUDIO_BIT_WIDTH_16;
		stAttr.eSoundmode  		= AUDIO_SOUND_MODE_MONO;
		stAttr.s32NumPerFrm 	= g_sample_audio.eAoRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttr.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_AOEnable(s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		/********************************
		 * enable resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_FALSE, s32DevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_ao;
		}

		/*******************************
		 * enable alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_FALSE, s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp;
		}

		/*******************************
		 * set vol
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, s32DevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg;
		}

		/*******************************
		 * open adec codec
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_AdecCreate(s32AdecChn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg;
		}

		/*******************************
		 * start thread
		 * ****************************/
		stTrd.thread_id = 0;
		stTrd.s32DevId = s32DevId;
		stTrd.s32Chn = s32Chn;
		stTrd.s32CodecId = s32AdecChn;
		strncpy(stTrd.pcFileName, g_sample_audio.codecName, strlen(g_sample_audio.codecName));
		stTrd.s32BufSize =  sizeof(IMP_U16) * g_sample_audio.eStreamRate * AUDIO_ONEFRAME_TIME / 1000 * AUDIO_SOUND_MODE_MONO;
		s32Ret = SAMPLE_AUDIO_CreateThread_AdecAo(&stTrd);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto destroy_adec;
		}
	} while (0);

	//this sample todo_d
	SAMPLE_AUDIO_TODO_D();

	/*******************************
	 * stop thread
	 * ****************************/
	s32Ret = SAMPLE_AUDIO_DestroyThread_AdecAo(&stTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
destroy_adec:
	s32Ret = SAMPLE_AUDIO_AdecDestroy(s32AdecChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_FALSE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_FALSE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_ao:
	s32Ret = SAMPLE_AUDIO_AODisable(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
static void *SAMPLE_AUDIO_AdecHdmiAoAoProc(void *param)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32Size = 0;
	IMP_U8 *pu8Buf = NULL;
	FILE *codec_file = NULL;
	FILE *pcm_file = NULL;

	SAMPLE_AUDIO_THREAD_S *pstTrd = (SAMPLE_AUDIO_THREAD_S *)param;

	prctl(PR_SET_NAME, "imp_AdecHdmiAoAoProc", 0, 0, 0);

	pu8Buf = (IMP_U8 *)malloc(pstTrd->s32BufSize);
	if (NULL == pu8Buf) {
		printf("%s : malloc failed!\n", __FUNCTION__);
		return NULL;
	}

	codec_file = SAMPLE_AUDIO_OpenOnlyfile(NULL, g_sample_audio.eCodec);
	if (NULL == codec_file) {
		return NULL;
	}

	pcm_file = SAMPLE_AUDIO_OpenCreatefile(NULL, PT_PCM);
	if (NULL == pcm_file) {
		return NULL;
	}

	if (g_sample_audio.isCodec) {
		switch (g_sample_audio.eCodec){
			case PT_G726:
				pstTrd->s32BufSize /= 8;
				break;
			case PT_G711A:
			case PT_G711U:
				pstTrd->s32BufSize /= 2;
				break;
			case PT_ADPCM:
				pstTrd->s32BufSize /= 4;
				break;
			default:
				break;
		}
	}

	IMP_AUDIO_FRAME_S stFrm;
	IMP_AUDIO_STREAM_S stStreamIn;
	while (pstTrd->isStart) {
		s32Size = fread(pu8Buf, 1, pstTrd->s32BufSize, codec_file);
		if (s32Size <= 0) {
			//printf("read size:%d\n", s32Size);
			fseek(codec_file, 0, SEEK_SET);//read file again
			continue;
		}

		/******************************************************
		 * send stream to adec(g711a, g711u, g726, adpcm, ...)
		 * ***************************************************/
		stStreamIn.pu8Stream = (IMP_U8 *)pu8Buf;
		stStreamIn.s32Len = s32Size;
		s32Ret = SAMPLE_AUDIO_ADEC_DealStream(pstTrd, &stStreamIn, &stFrm, pcm_file);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		/*****************************************************
		 * adjust both hdmi-ao and ao vol
		 * **************************************************/
		SAMPLE_AUDIO_AdjustVol();
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, SAMPLE_AO_DEVID, pstTrd->s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, SAMPLE_HDMI_AO_DEVID, pstTrd->s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
		//printf("vol:%f, gain:%f\n", g_sample_audio.vol, g_sample_audio.gain);

#ifdef SAMPLE_FRAMESIZE_EXT
		/****************************************************
		 * send ao Frame
		 * *************************************************/
		s32Ret = IMP_AO_SendFrame_EXT(SAMPLE_AO_DEVID, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		/****************************************************
		 * send hdmi-ao Frame
		 * *************************************************/
		s32Ret = IMP_HDMI_AO_SendFrame_EXT(SAMPLE_HDMI_AO_DEVID, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#else
		/****************************************************
		 * send ao Frame
		 * *************************************************/
		s32Ret = IMP_AO_SendFrame(SAMPLE_AO_DEVID, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		/****************************************************
		 * send hdmi-ao Frame
		 * *************************************************/
		s32Ret = IMP_HDMI_AO_SendFrame(SAMPLE_HDMI_AO_DEVID, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#endif
		/*****************************************************
		 *	PAUSE (only pause ao)
		 * **************************************************/
		s32Ret = SAMPLE_AUDIO_AOPause(SAMPLE_HDMI_AO_DEVID, pstTrd->s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
	}

	free(pu8Buf);
	fclose(codec_file);
	fclose(pcm_file);
	pstTrd->isStart = 0;

	return NULL;
}

static IMP_S32 SAMPLE_AUDIO_CreateThread_AdecHdmiAoAo(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	pstTrd->isStart = 1;
	s32Ret = pthread_create(&pstTrd->thread_id, 0, SAMPLE_AUDIO_AdecHdmiAoAoProc, pstTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}

	play_hdmi_thread_id = &pstTrd->thread_id;
	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_DestroyThread_AdecHdmiAoAo(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (pstTrd->isStart) {
		pstTrd->isStart = 0;
		pthread_join(pstTrd->thread_id, NULL);
	}

	return s32Ret;
}

/*****************************************************************************
 * function : file > (Adec) > (Resmp) > (Alg) > AO
 * 							> (Resmp) > (Alg) > HDMI-AO
 * **************************************************************************/
static IMP_S32 SAMPLE_AUDIO_AdecHdmiAoAo()
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32AoDevId		= SAMPLE_AO_DEVID;
	IMP_S32 s32HdmiAoDevId	= SAMPLE_HDMI_AO_DEVID;
	IMP_S32 s32Chn			= SAMPLE_AUDIO_CHN0;
	IMP_S32 s32AdecChn		= 0;
	IMP_AUDIO_IOATTR_S stAttrAo;
	IMP_AUDIO_IOATTR_S stAttrHdmiAo;
	SAMPLE_AUDIO_THREAD_S stTrd;
	IMP_S32 s32VoMod;
	VO_PUB_ATTR_S stVoPubAttr;

	do {
		/*****************************************
		 * init vo
		 * **************************************/
		s32VoMod = SAMPLE_VO_MODULE;
		IMP_VO_Disable(s32VoMod);

		stVoPubAttr.u32BgColor = g_sample_audio.u32BgColor;
		stVoPubAttr.stCanvasSize.u32Width = g_sample_audio.u32CanvasWidth;
		stVoPubAttr.stCanvasSize.u32Height = g_sample_audio.u32CanvasHeight;
		stVoPubAttr.enIntfType = g_sample_audio.enIntfType;
		stVoPubAttr.enIntfSync = g_sample_audio.enIntfSync;
		s32Ret = sample_vo_start_module(s32VoMod, &stVoPubAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = sample_vo_hdmi_start(stVoPubAttr.enIntfSync);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto stop_module;
		}

		/********************************
		 * enable ao
		 * *****************************/
		stAttrAo.eSamplerate 	= g_sample_audio.eAoRate;
		stAttrAo.eBitwidth   	= AUDIO_BIT_WIDTH_16;
		stAttrAo.eSoundmode  	= AUDIO_SOUND_MODE_MONO;
		stAttrAo.s32NumPerFrm 	= g_sample_audio.eAoRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttrAo.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_AOEnable(s32AoDevId, s32Chn, &stAttrAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto stop_hdmi;
		}

		/********************************
		 * enable hdmi-ao
		 * *****************************/
		stAttrHdmiAo.eSamplerate 	= g_sample_audio.eHdmiRate;
		stAttrHdmiAo.eBitwidth   	= AUDIO_BIT_WIDTH_16;
		stAttrHdmiAo.eSoundmode  	= AUDIO_SOUND_MODE_MONO;
		stAttrHdmiAo.s32NumPerFrm 	= g_sample_audio.eHdmiRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttrHdmiAo.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_HdmiAoEnable(s32HdmiAoDevId, s32Chn, &stAttrHdmiAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_ao;
		}

		/********************************
		 * enable ao resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_FALSE, s32AoDevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_hdmi_ao;
		}

		/********************************
		 * enable hdmi-ao resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_FALSE, s32HdmiAoDevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp_ao;
		}

		/*******************************
		 * enable ao alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_FALSE, s32AoDevId, s32Chn, &stAttrAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp_hdmi_ao;
		}

		/*******************************
		 * enable hdmi-ao alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_FALSE, s32HdmiAoDevId, s32Chn, &stAttrHdmiAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ao;
		}

		/*******************************
		 * set ao vol
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, s32AoDevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_hdmi_ao;
		}

		/*******************************
		 * set hdmi-ao vol
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, s32HdmiAoDevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_hdmi_ao;
		}

		/*******************************
		 * open adec codec
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_AdecCreate(s32AdecChn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_hdmi_ao;
		}

		/*******************************
		 * start thread
		 * ****************************/
		stTrd.thread_id = 0;
		stTrd.s32DevId = s32HdmiAoDevId;
		stTrd.s32Chn = s32Chn;
		stTrd.s32CodecId = s32AdecChn;
		strncpy(stTrd.pcFileName, g_sample_audio.codecName, strlen(g_sample_audio.codecName));
		stTrd.s32BufSize = sizeof(IMP_U16) * g_sample_audio.eStreamRate * AUDIO_ONEFRAME_TIME / 1000 * AUDIO_SOUND_MODE_MONO;
		s32Ret = SAMPLE_AUDIO_CreateThread_AdecHdmiAoAo(&stTrd);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto destroy_adec;
		}

	} while (0);

	//this sample todo_d
	SAMPLE_AUDIO_TODO_D();

	/*******************************
	 * stop thread
	 * ****************************/
	s32Ret = SAMPLE_AUDIO_DestroyThread_AdecHdmiAoAo(&stTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
destroy_adec:
	s32Ret = SAMPLE_AUDIO_AdecDestroy(s32AdecChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg_hdmi_ao:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_FALSE, s32HdmiAoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg_ao:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_FALSE, s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp_hdmi_ao:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_FALSE, s32HdmiAoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp_ao:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_FALSE, s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_hdmi_ao:
	s32Ret = SAMPLE_AUDIO_HdmiAoDisable(s32HdmiAoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_ao:
	s32Ret = SAMPLE_AUDIO_AODisable(s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
stop_hdmi:
	sample_vo_hdmi_stop();
stop_module:
	sample_vo_stop_module(s32VoMod);

	return s32Ret;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
static void *SAMPLE_AUDIO_AencAiAecProc(void *param)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_U8 * pu8Buf = NULL;
	FILE *codec_file = NULL;
	FILE *pcm_file = NULL;
	IMP_AUDIO_FRAME_S stFrm;
	SAMPLE_AUDIO_THREAD_S *pstTrd = (SAMPLE_AUDIO_THREAD_S *)param;

	prctl(PR_SET_NAME, "imp_AencAiAecProc", 0, 0, 0);

	pu8Buf = (IMP_U8 *)malloc(pstTrd->s32BufSize);
	if (NULL == pu8Buf) {
		printf("%s : malloc failed!\n", __FUNCTION__);
		return NULL;
	}

	if (g_sample_audio.isCodec) {
		codec_file = SAMPLE_AUDIO_OpenCreatefile(AUDIO_RECORD_FILENAME, g_sample_audio.eCodec);
		if (NULL == codec_file) {
			return NULL;
		}
	}

	pcm_file = SAMPLE_AUDIO_OpenCreatefile(AUDIO_RECORD_FILENAME, PT_PCM);
	if (NULL == pcm_file) {
		return NULL;
	}

	while (pstTrd->isStart) {
		s32Ret = IMP_AI_GetFrame(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		fwrite(stFrm.pu32VirAddr, 1, stFrm.s32Len, pcm_file);
		/******************************************************
		 * send stream to aenc(g711a, g711u, g726, adpcm, ...)
		 * ***************************************************/
		s32Ret = SAMPLE_AUDIO_AENC_DealStream(pstTrd->s32CodecId, &stFrm, codec_file);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
	}
	free(pu8Buf);
	if (g_sample_audio.isCodec) {
		fclose(codec_file);
	}
	fclose(pcm_file);
	pstTrd->isStart = 0;

	return NULL;
}

static void *SAMPLE_AUDIO_AdecAoAecProc(void *param)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32Size = 0;
	IMP_U8 * pu8Buf = NULL;
	FILE *codec_file = NULL;
	FILE *pcm_file = NULL;

	SAMPLE_AUDIO_THREAD_S *pstTrd = (SAMPLE_AUDIO_THREAD_S *)param;

	prctl(PR_SET_NAME, "imp_AdecAoAecProc", 0, 0, 0);

	pu8Buf = (IMP_U8 *)malloc(pstTrd->s32BufSize);
	if (NULL == pu8Buf) {
		printf("%s : malloc failed!\n", __FUNCTION__);
		return NULL;
	}

	codec_file = SAMPLE_AUDIO_OpenOnlyfile(AUDIO_PLAY_FILENAME, g_sample_audio.eCodec);
	if (NULL == codec_file) {
		return NULL;
	}

	if (g_sample_audio.isCodec) {
		pcm_file = SAMPLE_AUDIO_OpenCreatefile(AUDIO_PLAY_FILENAME, PT_PCM);
		if (NULL == pcm_file) {
			fclose(codec_file);
			return NULL;
		}
	}

	if (g_sample_audio.isCodec) {
		switch (g_sample_audio.eCodec){
			case PT_G726:
				pstTrd->s32BufSize /= 8;
				break;
			case PT_G711A:
			case PT_G711U:
				pstTrd->s32BufSize /= 2;
				break;
			case PT_ADPCM:
				pstTrd->s32BufSize /= 4;
				break;
			default:
				break;
		}
	}

	IMP_AUDIO_FRAME_S stFrm;
	IMP_AUDIO_STREAM_S stStreamIn;
	while (pstTrd->isStart) {
		s32Size = fread(pu8Buf, 1, pstTrd->s32BufSize, codec_file);
		if (s32Size <= 0) {
			//printf("read size:%d\n", s32Size);
			fseek(codec_file, 0, SEEK_SET);//read file again
			continue;
		}

		/******************************************************
		 * send stream to adec(g711a, g711u, g726, adpcm, ...)
		 * ***************************************************/
		stStreamIn.pu8Stream = (IMP_U8 *)pu8Buf;
		stStreamIn.s32Len = s32Size;
		s32Ret = SAMPLE_AUDIO_ADEC_DealStream(pstTrd, &stStreamIn, &stFrm, pcm_file);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		/*****************************************************
		 * adjust vol
		 * **************************************************/
		SAMPLE_AUDIO_AdjustVol();

		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, pstTrd->s32DevId, pstTrd->s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
		//printf("vol:%f, gain:%f\n", g_sample_audio.vol, g_sample_audio.gain);

		/****************************************************
		 * send Frame
		 * *************************************************/
#ifdef SAMPLE_FRAMESIZE_EXT
		s32Ret = IMP_AO_SendFrame_EXT(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#else
		s32Ret = IMP_AO_SendFrame(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#endif

		/*****************************************************
		 *	PAUSE
		 * **************************************************/
		s32Ret = SAMPLE_AUDIO_AOPause(pstTrd->s32DevId, pstTrd->s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
	}

	free(pu8Buf);
	fclose(codec_file);
	if (g_sample_audio.isCodec) {
		fclose(pcm_file);
	}
	pstTrd->isStart = 0;

	return NULL;
}

static IMP_S32 SAMPLE_AUDIO_CreateThread_AencAiAec(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	pstTrd->isStart = 1;
	s32Ret = pthread_create(&pstTrd->thread_id, 0, SAMPLE_AUDIO_AencAiAecProc, pstTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}
	aec_record_thread_id = &pstTrd->thread_id;
	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_DestroyThread_AencAiAec(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (pstTrd->isStart) {
		pstTrd->isStart = 0;
		pthread_join(pstTrd->thread_id, NULL);
	}

	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_CreateThread_AdecAoAec(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	pstTrd->isStart = 1;
	s32Ret = pthread_create(&pstTrd->thread_id, 0, SAMPLE_AUDIO_AdecAoAecProc, pstTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}
	aec_play_thread_id = &pstTrd->thread_id;
	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_DestroyThread_AdecAoAec(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (pstTrd->isStart) {
		pstTrd->isStart = 0;
		pthread_join(pstTrd->thread_id, NULL);
	}

	return s32Ret;
}

/*****************************************************************************
 * function : AI(in) > (Alg) > (Resmp) > (Aenc) > file(out)
 * 									||
 * 									\/
 *        near(no echo)    <----AEC(echo)-----       far
 * **************************************************************************/
static IMP_S32 SAMPLE_AUDIO_AiAec()
{
	IMP_AUDIO_IOATTR_S stAttr;
	IMP_AUDIO_IOATTR_S stAttrAo;
	SAMPLE_AUDIO_THREAD_S stTrd;
	SAMPLE_AUDIO_THREAD_S stTrdAo;
	IMP_S32 s32Ret		= IMP_SUCCESS;
	IMP_S32 s32DevId	= SAMPLE_AI_DEVID;
	IMP_S32 s32AoDevId	= SAMPLE_AO_DEVID;
	IMP_S32 s32Chn		= SAMPLE_AUDIO_CHN0;
	IMP_S32 s32AencChn = 1;
	IMP_S32 s32AdecChn = 2;

	do {
		/********************************
		 * enable ai
		 * *****************************/
		stAttr.eSamplerate 		= g_sample_audio.eAiRate;
		stAttr.eBitwidth   		= AUDIO_BIT_WIDTH_16;
		stAttr.eSoundmode  		= AUDIO_SOUND_MODE_MONO;
		stAttr.s32NumPerFrm 	= g_sample_audio.eAiRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttr.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_AIEnable(s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		/********************************
		 * enable ao
		 * *****************************/
		stAttrAo.eSamplerate 	= g_sample_audio.eAoRate;
		stAttrAo.eBitwidth   	= AUDIO_BIT_WIDTH_16;
		stAttrAo.eSoundmode		= AUDIO_SOUND_MODE_MONO;
		stAttrAo.s32NumPerFrm 	= g_sample_audio.eAoRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttrAo.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_AOEnable(s32AoDevId, s32Chn, &stAttrAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_ai;
		}

		/********************************
		 * enable resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_TRUE, s32DevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_ao;
		}

		/********************************
		 * enable resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_FALSE, s32AoDevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp_ai;
		}

		/*******************************
		 * enable aec alg
		 * ****************************/
		s32Ret = IMP_AI_EnableAec(s32DevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp_ao;
		}

		/*******************************
		 * enable alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_TRUE, s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_aec;
		}

		/*******************************
		 * enable alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_FALSE, s32AoDevId, s32Chn, &stAttrAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ai;
		}

		/*******************************
		 * set vol ai
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_TRUE, s32DevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ao;
		}

		/*******************************
		 * set vol ao
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, s32AoDevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ao;
		}

		/*******************************
		 * open aenc codec
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_AencCreate(s32AencChn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ao;
		}

		/*******************************
		 * open adec codec
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_AdecCreate(s32AdecChn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto destroy_aenc;
		}

		/*******************************
		 * start thread
		 * ****************************/
		stTrd.thread_id = 0;
		stTrd.s32DevId = s32DevId;
		stTrd.s32Chn = s32Chn;
		stTrd.s32CodecId = s32AencChn;
		strncpy(stTrd.pcFileName, g_sample_audio.codecName, strlen(g_sample_audio.codecName));
		stTrd.s32BufSize =  sizeof(IMP_U16) * g_sample_audio.eAiRate * AUDIO_ONEFRAME_TIME / 1000 * AUDIO_SOUND_MODE_MONO;
		s32Ret = SAMPLE_AUDIO_CreateThread_AencAiAec(&stTrd);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto destroy_adec;
		}

		/*******************************
		 * start thread
		 * ****************************/
		stTrdAo.thread_id = 1;
		stTrdAo.s32DevId = s32AoDevId;
		stTrdAo.s32Chn = s32Chn;
		stTrdAo.s32CodecId = s32AdecChn;
		strncpy(stTrdAo.pcFileName, g_sample_audio.codecName, strlen(g_sample_audio.codecName));
		stTrdAo.s32BufSize =  sizeof(IMP_U16) * g_sample_audio.eStreamRate * AUDIO_ONEFRAME_TIME / 1000 * AUDIO_SOUND_MODE_MONO;
		s32Ret = SAMPLE_AUDIO_CreateThread_AdecAoAec(&stTrdAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto stop_aencAi;
		}
	} while (0);

	//this sample todo_d
	SAMPLE_AUDIO_TODO_D();

	/*******************************
	 * stop thread
	 * ****************************/
	s32Ret = SAMPLE_AUDIO_DestroyThread_AdecAoAec(&stTrdAo);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
stop_aencAi:
	/*******************************
	 * stop thread
	 * ****************************/
	s32Ret = SAMPLE_AUDIO_DestroyThread_AencAiAec(&stTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
destroy_adec:
	s32Ret = SAMPLE_AUDIO_AdecDestroy(s32AdecChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
destroy_aenc:
	s32Ret = SAMPLE_AUDIO_AencDestroy(s32AencChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg_ai:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_FALSE, s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg_ao:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_TRUE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_aec:
	s32Ret = IMP_AI_DisableAec(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp_ai:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_FALSE, s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp_ao:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_TRUE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_ao:
	s32Ret = SAMPLE_AUDIO_AODisable(s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_ai:
	s32Ret = SAMPLE_AUDIO_AIDisable(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
static void *SAMPLE_AUDIO_AencAiRefProc(void *param)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_U8 * pu8Buf = NULL;
	FILE *codec_file = NULL;
	FILE *pcm_file = NULL;
	FILE *ref_file = NULL;
	IMP_AUDIO_FRAME_S stFrm;
	IMP_AUDIO_FRAME_S stRef;
	SAMPLE_AUDIO_THREAD_S *pstTrd = (SAMPLE_AUDIO_THREAD_S *)param;

	prctl(PR_SET_NAME, "imp_AencAiRefProc", 0, 0, 0);

	pu8Buf = (IMP_U8 *)malloc(pstTrd->s32BufSize);
	if (NULL == pu8Buf) {
		printf("%s : malloc failed!\n", __FUNCTION__);
		return NULL;
	}

	if (g_sample_audio.isCodec) {
		codec_file = SAMPLE_AUDIO_OpenCreatefile(AUDIO_RECORD_FILENAME, g_sample_audio.eCodec);
		if (NULL == codec_file) {
			return NULL;
		}
	}

	pcm_file = SAMPLE_AUDIO_OpenCreatefile(AUDIO_RECORD_FILENAME, PT_PCM);
	if (NULL == pcm_file) {
		return NULL;
	}

	ref_file = SAMPLE_AUDIO_OpenCreatefile(AUDIO_REF_FILENAME, PT_PCM);
	if (NULL == ref_file) {
		return NULL;
	}

	while (pstTrd->isStart) {
		s32Ret = IMP_AI_GetFrameAndRef(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm, &stRef);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		fwrite(stFrm.pu32VirAddr, 1, stFrm.s32Len, pcm_file);
		fwrite(stRef.pu32VirAddr, 1, stRef.s32Len, ref_file);

		/******************************************************
		 * send stream to aenc(g711a, g711u, g726, adpcm, ...)
		 * ***************************************************/
		s32Ret = SAMPLE_AUDIO_AENC_DealStream(pstTrd->s32CodecId, &stFrm, codec_file);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
	}

	free(pu8Buf);
	if (g_sample_audio.isCodec) {
		fclose(codec_file);
	}
	fclose(pcm_file);
	pstTrd->isStart = 0;

	return NULL;
}

static void *SAMPLE_AUDIO_AdecAoRefProc(void *param)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_S32 s32Size = 0;
	IMP_U8 * pu8Buf = NULL;
	FILE *codec_file = NULL;
	FILE *pcm_file = NULL;

	SAMPLE_AUDIO_THREAD_S *pstTrd = (SAMPLE_AUDIO_THREAD_S *)param;

	prctl(PR_SET_NAME, "imp_AdecAoRefProc", 0, 0, 0);

	pu8Buf = (IMP_U8 *)malloc(pstTrd->s32BufSize);
	if (NULL == pu8Buf) {
		printf("%s : malloc failed!\n", __FUNCTION__);
		return NULL;
	}

	codec_file = SAMPLE_AUDIO_OpenOnlyfile(AUDIO_PLAY_FILENAME, g_sample_audio.eCodec);
	if (NULL == codec_file) {
		return NULL;
	}

	if (g_sample_audio.isCodec) {
		pcm_file = SAMPLE_AUDIO_OpenCreatefile(AUDIO_PLAY_FILENAME, PT_PCM);
		if (NULL == pcm_file) {
			fclose(codec_file);
			return NULL;
		}
	}

	if (g_sample_audio.isCodec) {
		switch (g_sample_audio.eCodec){
			case PT_G726:
				pstTrd->s32BufSize /= 8;
				break;
			case PT_G711A:
			case PT_G711U:
				pstTrd->s32BufSize /= 2;
				break;
			case PT_ADPCM:
				pstTrd->s32BufSize /= 4;
				break;
			default:
				break;
		}
	}

	IMP_AUDIO_FRAME_S stFrm;
	IMP_AUDIO_STREAM_S stStreamIn;
	while (pstTrd->isStart) {
		s32Size = fread(pu8Buf, 1, pstTrd->s32BufSize, codec_file);
		if (s32Size <= 0) {
			//printf("read size:%d\n", s32Size);
			fseek(codec_file, 0, SEEK_SET);//read file again
			continue;
		}

		/******************************************************
		 * send stream to adec(g711a, g711u, g726, adpcm, ...)
		 * ***************************************************/
		stStreamIn.pu8Stream = (IMP_U8 *)pu8Buf;
		stStreamIn.s32Len = s32Size;
		s32Ret = SAMPLE_AUDIO_ADEC_DealStream(pstTrd, &stStreamIn, &stFrm, pcm_file);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}

		/*****************************************************
		 * adjust vol
		 * **************************************************/
		SAMPLE_AUDIO_AdjustVol();

		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, pstTrd->s32DevId, pstTrd->s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
		//printf("vol:%f, gain:%f\n", g_sample_audio.vol, g_sample_audio.gain);

		/****************************************************
		 * send Frame
		 * *************************************************/
#ifdef SAMPLE_FRAMESIZE_EXT
		s32Ret = IMP_AO_SendFrame_EXT(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#else
		s32Ret = IMP_AO_SendFrame(pstTrd->s32DevId, pstTrd->s32Chn, &stFrm, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
#endif

		/*****************************************************
		 *	PAUSE
		 * **************************************************/
		s32Ret = SAMPLE_AUDIO_AOPause(pstTrd->s32DevId, pstTrd->s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			break;
		}
	}

	free(pu8Buf);
	fclose(codec_file);
	if (g_sample_audio.isCodec) {
		fclose(pcm_file);
	}
	pstTrd->isStart = 0;

	return NULL;
}
static IMP_S32 SAMPLE_AUDIO_CreateThread_AencAiRef(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	pstTrd->isStart = 1;
	s32Ret = pthread_create(&pstTrd->thread_id, 0, SAMPLE_AUDIO_AencAiRefProc, pstTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}
	ref_record_thread_id = &pstTrd->thread_id;
	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_DestroyThread_AencAiRef(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (pstTrd->isStart) {
		pstTrd->isStart = 0;
		pthread_join(pstTrd->thread_id, NULL);
	}

	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_CreateThread_AdecAoRef(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	pstTrd->isStart = 1;
	s32Ret = pthread_create(&pstTrd->thread_id, 0, SAMPLE_AUDIO_AdecAoRefProc, pstTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}
	ref_play_thread_id = &pstTrd->thread_id;
	return s32Ret;
}

static IMP_S32 SAMPLE_AUDIO_DestroyThread_AdecAoRef(SAMPLE_AUDIO_THREAD_S *pstTrd)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (pstTrd->isStart) {
		pstTrd->isStart = 0;
		pthread_join(pstTrd->thread_id, NULL);
	}

	return s32Ret;
}

/*****************************************************************************
 * function : AI(in) > (Alg) > (Resmp) > (Aenc) > file(out)
 * 									||
 * 									\/
 *        near(no echo)    <----AEC(echo)-----       far
 * **************************************************************************/
static IMP_S32 SAMPLE_AUDIO_AiRef()
{
	IMP_AUDIO_IOATTR_S stAttr;
	IMP_AUDIO_IOATTR_S stAttrAo;
	SAMPLE_AUDIO_THREAD_S stTrd;
	SAMPLE_AUDIO_THREAD_S stTrdAo;
	IMP_S32 s32Ret		= IMP_SUCCESS;
	IMP_S32 s32DevId	= SAMPLE_AI_DEVID;
	IMP_S32 s32AoDevId	= SAMPLE_AO_DEVID;
	IMP_S32 s32Chn		= SAMPLE_AUDIO_CHN0;
	IMP_S32 s32AencChn = 1;
	IMP_S32 s32AdecChn = 2;

	do {
		/********************************
		 * enable ai
		 * *****************************/
		stAttr.eSamplerate 		= g_sample_audio.eAiRate;
		stAttr.eBitwidth   		= AUDIO_BIT_WIDTH_16;
		stAttr.eSoundmode  		= AUDIO_SOUND_MODE_MONO;
		stAttr.s32NumPerFrm 	= g_sample_audio.eAiRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttr.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_AIEnable(s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		/********************************
		 * enable ao
		 * *****************************/
		stAttrAo.eSamplerate 	= g_sample_audio.eAoRate;
		stAttrAo.eBitwidth   	= AUDIO_BIT_WIDTH_16;
		stAttrAo.eSoundmode		= AUDIO_SOUND_MODE_MONO;
		stAttrAo.s32NumPerFrm 	= g_sample_audio.eAoRate * AUDIO_ONEFRAME_TIME_DRIVER / 1000;
		stAttrAo.s16ChnCnt    	= 1;
		s32Ret = SAMPLE_AUDIO_AOEnable(s32AoDevId, s32Chn, &stAttrAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_ai;
		}

		/********************************
		 * enable resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_TRUE, s32DevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_ao;
		}

		/********************************
		 * enable resample
		 * *****************************/
		s32Ret = SAMPLE_AUDIO_EnableResmp(IMP_FALSE, s32AoDevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp_ai;
		}

		/*******************************
		 * enable aec-ref alg
		 * ****************************/
		s32Ret = IMP_AI_EnableAecRefFrame(s32DevId, s32Chn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_resmp_ao;
		}

		/*******************************
		 * enable alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_TRUE, s32DevId, s32Chn, &stAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_aec;
		}

		/*******************************
		 * enable alg
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_EnableAlg(IMP_FALSE, s32AoDevId, s32Chn, &stAttrAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ai;
		}

		/*******************************
		 * set vol
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_TRUE, s32DevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ao;
		}

		/*******************************
		 * set vol
		 * ****************************/
		g_sample_audio.setVol = 1;//temporary setting
		s32Ret = SAMPLE_AUDIO_SetVol(IMP_FALSE, s32AoDevId, s32Chn, &g_sample_audio.vol, &g_sample_audio.gain);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ao;
		}

		/*******************************
		 * open aenc codec
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_AencCreate(s32AencChn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto disable_alg_ao;
		}

		/*******************************
		 * open adec codec
		 * ****************************/
		s32Ret = SAMPLE_AUDIO_AdecCreate(s32AdecChn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto destroy_aenc;
		}

		/*******************************
		 * start thread
		 * ****************************/
		stTrd.thread_id = 0;
		stTrd.s32DevId = s32DevId;
		stTrd.s32Chn = s32Chn;
		stTrd.s32CodecId = s32AencChn;
		strncpy(stTrd.pcFileName, g_sample_audio.codecName, strlen(g_sample_audio.codecName));
		stTrd.s32BufSize =  sizeof(IMP_U16) * g_sample_audio.eAiRate * AUDIO_ONEFRAME_TIME / 1000 * AUDIO_SOUND_MODE_MONO;
		s32Ret = SAMPLE_AUDIO_CreateThread_AencAiRef(&stTrd);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto destroy_adec;
		}

		/*******************************
		 * start thread
		 * ****************************/
		stTrdAo.thread_id = 1;
		stTrdAo.s32DevId = s32AoDevId;
		stTrdAo.s32Chn = s32Chn;
		stTrdAo.s32CodecId = s32AdecChn;
		strncpy(stTrdAo.pcFileName, g_sample_audio.codecName, strlen(g_sample_audio.codecName));
		stTrdAo.s32BufSize =  sizeof(IMP_U16) * g_sample_audio.eStreamRate * AUDIO_ONEFRAME_TIME / 1000 * AUDIO_SOUND_MODE_MONO;
		s32Ret = SAMPLE_AUDIO_CreateThread_AdecAoRef(&stTrdAo);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			goto stop_aencAi;
		}
	} while (0);
	//this sample todo_d
	SAMPLE_AUDIO_TODO_D();

	/*******************************
	 * stop thread
	 * ****************************/
	s32Ret = SAMPLE_AUDIO_DestroyThread_AdecAoRef(&stTrdAo);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
stop_aencAi:
	/*******************************
	 * stop thread
	 * ****************************/
	s32Ret = SAMPLE_AUDIO_DestroyThread_AencAiRef(&stTrd);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
destroy_adec:
	s32Ret = SAMPLE_AUDIO_AdecDestroy(s32AdecChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
destroy_aenc:
	s32Ret = SAMPLE_AUDIO_AencDestroy(s32AencChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg_ai:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_FALSE, s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_alg_ao:
	s32Ret = SAMPLE_AUDIO_DisableAlg(IMP_TRUE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_aec:
	s32Ret = IMP_AI_DisableAecRefFrame(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp_ai:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_FALSE, s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_resmp_ao:
	s32Ret = SAMPLE_AUDIO_DisableResmp(IMP_TRUE, s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_ao:
	s32Ret = SAMPLE_AUDIO_AODisable(s32AoDevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
disable_ai:
	s32Ret = SAMPLE_AUDIO_AIDisable(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}

	return s32Ret;
}



/*****************************************************************************
 * main
 * **************************************************************************/
int main(int argc, char **argv)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_CHAR ch = '\0';
	IMP_S32 exit = 0;

	signal(SIGINT, SAMPLE_AUDIO_HandleSig);
	signal(SIGTERM, SAMPLE_AUDIO_HandleSig);

	SAMPLE_AUDIO_Init();
	/**********************************
	 * parse param
	 * *******************************/
	exit = SAMPLE_AUDIO_ParseParam(argc, argv);
	if (exit)
		return 0;

	/* sample system init */
	s32Ret = sample_system_init();
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return -1;
	}

	/****************************
	 * choose the usage
	 * *************************/
	while (1) {
		SAMPLE_AUDIO_Usage();

again:
		ch = getchar();
		if (10 == ch) {//'10' -> 'LF' Line feed
			goto again;
		}

		switch (ch) {
			case '1':
				/*****************************************************************************
				 * function : AI > (Alg) > (Resmp) > (Aenc) > file
				 * **************************************************************************/
				SAMPLE_AUDIO_AencAi();
				break;
			case '2':
				/*****************************************************************************
				 * function : file > (Adec) > (Resmp) > (Alg) > AO
				 * **************************************************************************/
				SAMPLE_AUDIO_AdecAo();
				break;
			case '3':
				/*****************************************************************************
				 * function : file > (Adec) > (Resmp) > (Alg) > AO
				 * 							> (Resmp) > (Alg) > HDMI-AO
				 * **************************************************************************/
				SAMPLE_AUDIO_AdecHdmiAoAo();
				break;
			case '4':
				/*****************************************************************************
				 * function : AI(in) > (Alg) > (Resmp) > (Aenc) > file(out)
				 * 									||
				 * 									\/
				 *        near(no echo)    <----AEC(echo)-----       far
				 * **************************************************************************/
				SAMPLE_AUDIO_AiAec();
				break;
			case '5':
				/*****************************************************************************
				 * function : AI(in) > (Alg) > (Resmp) > (Aenc) > file(out)
				 * 									||
				 * 									\/
				 *        near(no echo)    <----AEC(echo)-----       far
				 * **************************************************************************/
				SAMPLE_AUDIO_AiRef();
				break;

			case 'q':
				exit = 1;
				break;

			default:
				printf("intput invaild! please try again.\n\n");
				goto again;
		}

		if (exit) {
			break;
		}

	}

	/* sample system exit */
	s32Ret = sample_system_exit();
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return -1;
	}

	return 0;
}



