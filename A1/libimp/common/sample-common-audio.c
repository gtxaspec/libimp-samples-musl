#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>

#include <imp/imp_audio.h>
#include <imp/imp_log.h>
#include <imp/imp_utils.h>

#include "imp/imp_audio.h"
#include "sample-common.h"

#define TAG "sample-common-audio"

SAMPLE_AUDIO_S g_sample_audio = {0};

static IMP_VOID SAMPLE_AUDIO_OpenDebug()
{
	system("./impdbg AUDIO_info");
}

static IMP_VOID SAMPLE_AUDIO_SaveFrm()
{
	system("./impdbg AUDIO_info SAVEFRM=100000-Ai");
	system("./impdbg AUDIO_info SAVEFRM=100000-Ao");
}

static IMP_VOID SAMPLE_AUDIO_StopSaveFrm()
{
	system("./impdbg AUDIO_info SAVEFRM=0-Ai");
	system("./impdbg AUDIO_info SAVEFRM=0-Ao");
}

IMP_VOID SAMPLE_AUDIO_TODO_Print()
{
	printf("\n\n/************************************/\n");
	printf("please choose the case which you want to set:\n");
	printf("\tv:\tlower volume 1.5\n");
	printf("\tV:\thigher volume 1.5\n");
	printf("\tg:\tlower gain 0.5\n");
	printf("\tG:\thigher gain 0.5\n");
	printf("\tm:\tset volume mute/noMute\n");
	printf("\tP:\tset frames pause\n");
	printf("\td:\tprint impdbg info\n");
	printf("\ts:\tsave frames impdbg (loop)\n");
	printf("\tS:\tstop save frames impdbg\n");
	printf("\th:\tprint help messages\n");
	printf("\tq:\tquit this audio sample\n\n");
	printf("this sample command >");
}

IMP_S32 SAMPLE_AUDIO_TODO(char ch)
{
	int exit = 0;

	switch (ch) {
		/***********************************
		 * adjust the volume
		 * ********************************/
		case 'v':
			/* lower vol  0.5 */
			g_sample_audio.setVol = -2;
			break;
		case 'V':
			/* higher vol 0.5 */
			g_sample_audio.setVol = -1;
			break;
		case 'g':
			/* lower gain 1.5 */
			g_sample_audio.setVol = 1;
			break;
		case 'G':
			/* higher gain 1.5 */
			g_sample_audio.setVol = 2;
			break;

		case 'm':
			/* set mute*/
			g_sample_audio.isMute = !g_sample_audio.isMute;
			break;

			/*********************************
			 * audio Pause
			 * ******************************/
		case 'P':
			/* set pause*/
			g_sample_audio.isPause = !g_sample_audio.isPause;
			break;

			/*********************************
			 * audio debug
			 * ******************************/
		case 'd':
			/* print audio debug info */
			SAMPLE_AUDIO_OpenDebug();
			break;
		case 's':
			/* save frame for audio debug */
			SAMPLE_AUDIO_SaveFrm();
			break;
		case 'S':
			//close save frame(reserve)
			SAMPLE_AUDIO_StopSaveFrm();
			break;

		case 'h':
			/* print help */
			SAMPLE_AUDIO_TODO_Print();
			break;
			/********************************
			 * quit
			 * *****************************/
		case 'q':
			exit = 1;
			break;

		default:
			printf("intput invaild! please try again.\n\n");
			break;
	}

	return exit;
}

static void show_opt_usage()
{
	printf("Options:\n");
	printf("\t--codec=g726\t\t\tThe codec used.(g711a/g711u/g726/adpcm). \n");
	printf("\t--vol=21.0\t\t\tSet the volume for all devices. \n");
	printf("\t--gain=0.0\t\t\tSet the gain for all devices. \n");
	printf("\t--ai-rate=RATE\t\t\tSet ai sampleRate to record. \n");
	printf("\t--ao-rate=RATE\t\t\tSet ao sampleRate to record. \n");
	printf("\t--hdmi-ao-rate=RATE\t\tSet hdmi-ao sampleRate to record. \n");
	printf("\t--file-rate=RATE\t\tSet in/outfile sampleRate \n");
	printf("\t\t\t\t\t\tRATE=8000/16000/24000/32000/44100.. \n");
	printf("\t--hdmi-vo-bgcolor=0xf1f2f3f4\tSet hdmi-vo background color.\n");
	printf("\t--hdmi-vo-width=1280\t\tSet hdmi-vo width.\n");
	printf("\t--hdmi-vo-height=720\t\tSet hdmi-vo height.\n");
	printf("\t--hdmi-vo-type=1\t\tSet hdmi-vo type. [0:VGA, 1:HDMI, 2:TFT]\n");
	printf("\t--hdmi-vo-sync=2\t\tSet hdmi-vo sync.\n");
	printf("\t\t\tSYNC=\t0\t1080P24;\t1\t1080P25\n");
	printf("\t\t\t\t2\t1080P30;\t3\t1080P60\n");
	printf("\t\t\t\t4\t720P50;\t\t5\t720P60\n");
	printf("\t\t\t\t6\t2560*1440P30;\t7\t2560*1440P60\n");
	printf("\t\t\t\t7\t3840*2160P30;\t8\t3840*2160P50\n");
	printf("\t\t\t\t...\tother reserve.\n");
	printf("\t-d, --debug=save\t\tprint all debug infos,or savefrm \n");
	printf("\t-a, --alg\t\t\tSet to use ns/hpf/agc algorithm \n");
	printf("\t-p, --print\t\t\tPrint all sample-audio params \n");
	printf("\t-h, --help\t\t\tPrint this message and exit. \n");
}


static void print_all_param()
{
	char buf[32] = {0};
	printf("\n\tisAlg:%d\t\t\t\t", g_sample_audio.isAlg);
	printf("\tisMute:%d\n", g_sample_audio.isMute);
	printf("\tisDebug:%d\t\t\t", g_sample_audio.isDebug);
	printf("\tisSaveFrm:%d\n", g_sample_audio.isSaveFrm);
	printf("\tisPause:%d\n", g_sample_audio.isPause);
	printf("\teAiRate:%d\t\t\t", g_sample_audio.eAiRate);
	printf("\teAoRate:%d\n", g_sample_audio.eAoRate);
	printf("\teHdmiRate:%d\t\t\t", g_sample_audio.eHdmiRate);
	printf("\teStreamRate:%d\n", g_sample_audio.eStreamRate);
	printf("\tsetVol:%d\t\t\t", g_sample_audio.setVol);
	printf("\tvol:%f\n", g_sample_audio.vol);
	printf("\tgain:%f\t\t\t", g_sample_audio.gain);
	printf("\tisCodec:%d\n", g_sample_audio.isCodec);

	switch (g_sample_audio.eCodec) {
		case PT_PCM:
			strcpy(buf, "pcm");
			break;
		case PT_G711A:
			strcpy(buf, "g711a");
			break;
		case PT_G711U:
			strcpy(buf, "g711u");
			break;
		case PT_G726:
			strcpy(buf, "g726");
			break;
		case PT_ADPCM:
			strcpy(buf, "adpcm");
			break;
		default:
			strcpy(buf, "unknown");
			break;
	}
	printf("\teCodec:%s\n\n", buf);

	printf("\tu32BgColor:0x%08x\n", g_sample_audio.u32BgColor);
	printf("\tu32CanvasWidth:%d\t\t", g_sample_audio.u32CanvasWidth);
	printf("\tu32CanvasHeight:%d\n", g_sample_audio.u32CanvasHeight);

	memset(buf, '\0', sizeof(buf));
	switch (g_sample_audio.enIntfType) {
		case VO_INTF_VGA:
			strcpy(buf, "VGA");
			break;
		case VO_INTF_HDMI:
			strcpy(buf, "HDMI");
			break;
		case VO_INTF_TFT:
			strcpy(buf, "TFT");
			break;
		default:
			strcpy(buf, "UNKNOW");
			break;
	}
	printf("\tenIntfType:%s\t\t\t", buf);

	memset(buf, '\0', sizeof(buf));
	switch (g_sample_audio.enIntfType) {
		case VO_OUTPUT_1080P24:
			strcpy(buf, "1080P24");
			break;
		case VO_OUTPUT_1080P25:
			strcpy(buf, "1080P25");
			break;
		case VO_OUTPUT_1080P30:
			strcpy(buf, "1080P30");
			break;
		case VO_OUTPUT_1080P60:
			strcpy(buf, "1080P60");
			break;
		case VO_OUTPUT_720P50:
			strcpy(buf, "720P50");
			break;
		case VO_OUTPUT_720P60:
			strcpy(buf, "720P60");
			break;
		case VO_OUTPUT_2560x1440_30:
			strcpy(buf, "2560x1440_30");
			break;
		case VO_OUTPUT_2560x1440_60:
			strcpy(buf, "2560x1440_60");
			break;
		case VO_OUTPUT_3840x2160_30:
			strcpy(buf, "3840x2160_30");
			break;
		case VO_OUTPUT_3840x2160_50:
			strcpy(buf, "3840x2160_50");
			break;
		default:
			strcpy(buf, "UNKNOW");
			break;
	}
	printf("\tenIntfSync:%s\n\n\n", buf);
}

static int check_opt_codec(char *optarg)
{
	int i = 0;

	struct codec_name{
		char *name;
		IMP_AUDIO_PALY_LOAD_TYPE_E eType;
	};

	struct codec_name list[] = {
		{"pcm", PT_PCM},
		{"g711a", PT_G711A},
		{"g711u", PT_G711U},
		{"g726", PT_G726},
		{"adpcm", PT_ADPCM},
	};

	for (i = 0; i < ARRAY_SIZE(list); i++) {
		if (!strncmp(optarg, list[i].name, strlen(list[i].name))) {
			g_sample_audio.isCodec = 1;
			g_sample_audio.eCodec = list[i].eType;
			return 0;
		}
	}
	if (i >= ARRAY_SIZE(list))
		return -1;

	return 0;
}

IMP_S32 SAMPLE_AUDIO_ParseParam(int argc, char **argv)
{
	int c = 0;
	int ret = 0;
	int lopt = 0;
	int opt_index = 0;
	
	enum{
		OPT_CODEC = 1,
		OPT_VOL,
		OPT_GAIN,
		OPT_AI_RATE,
		OPT_AO_RATE,
		OPT_HDMI_AO_RATE,
		OPT_FILE_RATE,
		OPT_HDMI_VO_BGCOLOR,
		OPT_HDMI_VO_WIDTH,
		OPT_HDMI_VO_HEIGHT,
		OPT_HDMI_VO_TYPE,
		OPT_HDMI_VO_SYNC,
	};

	struct option longOpts[] = {
		{"codec", 			optional_argument, 	&lopt, OPT_CODEC},//type of codec"--codec=g726"
		{"vol", 			optional_argument, 	&lopt, OPT_VOL},//vol setting
		{"gain", 			optional_argument, 	&lopt, OPT_GAIN},//gain setting
		{"ai-rate", 		optional_argument, 	&lopt, OPT_AI_RATE},//ai sampleRate
		{"ao-rate", 		optional_argument, 	&lopt, OPT_AO_RATE},//ao sampleRate
		{"hdmi-ao-rate", 	optional_argument, 	&lopt, OPT_HDMI_AO_RATE},//hdmi-ao sampleRate
		{"file-rate", 		optional_argument, 	&lopt, OPT_FILE_RATE},//in/outfile sampleRate
		{"hdmi-vo-bgcolor",	optional_argument,	&lopt, OPT_HDMI_VO_BGCOLOR},//hdmi vo bg color "0xf1f2f3f4"
		{"hdmi-vo-width",	optional_argument,	&lopt, OPT_HDMI_VO_WIDTH},//hdmi vo canvas width
		{"hdmi-vo-height",	optional_argument,	&lopt, OPT_HDMI_VO_HEIGHT},//hdmi vo canvas height
		{"hdmi-vo-type",	optional_argument,	&lopt, OPT_HDMI_VO_TYPE},//hdmi vo type
		{"hdmi-vo-sync",	optional_argument,	&lopt, OPT_HDMI_VO_SYNC},//hdmi vo sync
		{"debug", 			optional_argument, 	NULL, 'd'},//printf debug info,or savefrm
		{"alg", 			no_argument, 		NULL, 'a'},//"--alg" = "--ns" + "--hpf" + "--agc"
		{"print",			no_argument,		NULL, 'p'},//printf all param
		{"help",			no_argument,		NULL, 'h'},//help
		{0,0,0,0},
	};

	while (1) {
		c = getopt_long(argc, argv, "d:aph", longOpts, &opt_index);
		if (c == -1)
			break;

		switch (c) {
			case 0:
				switch (lopt) {
					case OPT_CODEC:
						ret = check_opt_codec(optarg);
						if (ret == -1)
							return ret;
						break;
					case OPT_VOL:
						printf("vol: %f\n", atof(optarg));
						g_sample_audio.vol = atof(optarg);
						break;
					case OPT_GAIN:
						printf("gain: %f\n", atof(optarg));
						g_sample_audio.gain = atof(optarg);
						break;
					case OPT_AI_RATE:
						g_sample_audio.eAiRate = atoi(optarg);
						break;
					case OPT_AO_RATE:
						g_sample_audio.eAoRate = atoi(optarg);
						break;
					case OPT_HDMI_AO_RATE:
						g_sample_audio.eHdmiRate = atoi(optarg);
						break;
					case OPT_FILE_RATE:
						g_sample_audio.eStreamRate = atoi(optarg);
						break;
					case OPT_HDMI_VO_BGCOLOR:
						sscanf(optarg, "%x", &g_sample_audio.u32BgColor);
						break;
					case OPT_HDMI_VO_WIDTH:
						g_sample_audio.u32CanvasWidth = atoi(optarg);
						break;
					case OPT_HDMI_VO_HEIGHT:
						g_sample_audio.u32CanvasHeight = atoi(optarg);
						break;
					case OPT_HDMI_VO_TYPE:
					{
						switch (atoi(optarg)) {
							case 0:
								g_sample_audio.enIntfType = VO_INTF_VGA;
								break;
							case 1:
								g_sample_audio.enIntfType = VO_INTF_HDMI;
								break;
							case 2:
								g_sample_audio.enIntfType = VO_INTF_TFT;
								break;
							default:
								printf("invaild param! please try again!\n");
								return -1;
						}
						break;
					}
					case OPT_HDMI_VO_SYNC:
					{
						int eSync = 0;
						switch (atoi(optarg)) {
							case 0:
								eSync = VO_OUTPUT_1080P24;
								break;
							case 1:
								eSync = VO_OUTPUT_1080P25;
								break;
							case 2:
								eSync = VO_OUTPUT_1080P30;
								break;
							case 3:
								eSync = VO_OUTPUT_1080P60;
								break;
							case 4:
								eSync = VO_OUTPUT_720P50;
								break;
							case 5:
								eSync = VO_OUTPUT_720P60;
								break;
							case 6:
								eSync = VO_OUTPUT_2560x1440_30;
								break;
							case 7:
								eSync = VO_OUTPUT_2560x1440_60;
								break;
							case 8:
								eSync = VO_OUTPUT_3840x2160_30;
								break;
							case 9:
								eSync = VO_OUTPUT_3840x2160_60;
								break;
							default:
								printf("invaild param! please try again!\n");
								return -1;
						}
						g_sample_audio.enIntfSync = eSync;
						break;
					}//END OPT_HDMI_VO_SYNC
				}
				break;
			case 'a':
				g_sample_audio.isAlg = 1;
				break;
			case 'd':
				break;
			case 'h':
				show_opt_usage();
				return 1;
			default:
				printf("invaild param! please try again!\n");
				return -1;
		}
	};

	print_all_param();

	return 0;
}

IMP_VOID SAMPLE_AUDIO_Usage(IMP_VOID)
{
    printf("\n\n/************************************/\n");
    printf("please choose the case which you want to run:\n");
    printf("\t1:  get audio stream from AI, (encode), save file\n");
    printf("\t2:  (loop)read audio stream from file, (decode), send AO\n");
    printf("\t3:  (loop)read audio stream from file, (decode), send AO and HDMI-AO\n");
    printf("\t4:  get audio stream from AI, (encode), both remove the echo(loop)\n");
	printf("\t    from AO and save file\n");
    printf("\t5:  get audio stream from AI, (encode), both remove the echo(loop)\n");
	printf("\t    from AO and save file(captured echo file and no echo file)\n");
    printf("\tq:  quit whole audio sample\n\n");
    printf("sample command >");
}

IMP_VOID SAMPLE_AUDIO_Init()
{
	memset(&g_sample_audio, '\0', sizeof(g_sample_audio));

	g_sample_audio.isAlg		= 0;
	g_sample_audio.isMute		= 0;
	g_sample_audio.isDebug		= 0;
	g_sample_audio.isSaveFrm	= 0;
	g_sample_audio.isPause		= 0;

	g_sample_audio.eAiRate		= AUDIO_SAMPLE_RATE_8000;
	g_sample_audio.eAoRate		= AUDIO_SAMPLE_RATE_8000;
	g_sample_audio.eHdmiRate	= AUDIO_SAMPLE_RATE_32000;
	g_sample_audio.eStreamRate	= AUDIO_SAMPLE_RATE_8000;

	g_sample_audio.setVol		= 0;
	g_sample_audio.vol			= F_CHN_GAIN_DB;

	g_sample_audio.isCodec		= 0;
	g_sample_audio.eCodec		= PT_PCM;

	//hdmi
	g_sample_audio.u32BgColor = SAMPLE_AUDIO_HDMI_BG_COLOR;
	g_sample_audio.u32CanvasWidth = SAMPLE_AUDIO_HDMI_WIDTH;
	g_sample_audio.u32CanvasHeight = SAMPLE_AUDIO_HDMI_HEIGHT;
	g_sample_audio.enIntfType = SAMPLE_AUDIO_HDMI_INTF_TYPE;
	g_sample_audio.enIntfSync = SAMPLE_AUDIO_HDMI_INTF_SYNC;
}

static IMP_CHAR *SAMPLE_AUDIO_Pt2Str(IMP_AUDIO_PALY_LOAD_TYPE_E enType)
{
	if (PT_PCM == enType) {
		return "pcm";
	} else if (PT_G711A == enType) {
		return "g711a";
	} else if (PT_G711U == enType) {
		return "g711u";
	} else if (PT_G726 == enType) {
		return "g726";
	} else if (PT_ADPCM == enType) {
		return "adpcm";
	} else {
		return "data";
	}
}

FILE *SAMPLE_AUDIO_OpenCreatefile(IMP_CHAR *filename, IMP_AUDIO_PALY_LOAD_TYPE_E eType)
{
	FILE *fd;
	IMP_CHAR File_name[128] = {0};

	/* create file for save stream*/
	if (filename) {
		sprintf(File_name, "/tmp/%s.%s", filename, SAMPLE_AUDIO_Pt2Str(eType));
	} else {
		sprintf(File_name, "/tmp/record_file.%s", SAMPLE_AUDIO_Pt2Str(eType));
	}
	fd = fopen(File_name, "w+");
	if (NULL == fd) {
		printf("%s: open file %s failed\n", __FUNCTION__, File_name);
		return NULL;
	}
	printf("\topen stream(%s) file:\"%s\" for aenc ok\n", SAMPLE_AUDIO_Pt2Str(eType), File_name);

	return fd;

}

FILE *SAMPLE_AUDIO_OpenOnlyfile(IMP_CHAR *filename, IMP_AUDIO_PALY_LOAD_TYPE_E eType)
{
	FILE *fd;
	IMP_CHAR File_name[128] = {0};

	/* create file for save stream*/
	if (filename) {
		sprintf(File_name, "/tmp/%s.%s", filename, SAMPLE_AUDIO_Pt2Str(eType));
	} else {
		sprintf(File_name, "/tmp/play_file.%s", SAMPLE_AUDIO_Pt2Str(eType));
	}
	fd = fopen(File_name, "rb");
	if (NULL == fd) {
		printf("%s: open file %s failed\n", __FUNCTION__, File_name);
		return NULL;
	}
	printf("\topen stream(%s) file:\"%s\" for adec ok\n", SAMPLE_AUDIO_Pt2Str(eType), File_name);

	return fd;

}

/*****************************************************************************
 * send stream to adec(g711a, g711u, g726, adpcm, ...)
 * **************************************************************************/
IMP_S32 SAMPLE_AUDIO_ADEC_DealStream(SAMPLE_AUDIO_THREAD_S *pstTrd, IMP_AUDIO_STREAM_S *stStreamIn, IMP_AUDIO_FRAME_S *stFrm, FILE *file)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_AUDIO_STREAM_S stStreamOut;

	if (g_sample_audio.isCodec) {
		s32Ret = IMP_ADEC_SendStream(pstTrd->s32CodecId, stStreamIn, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_ADEC_PollingStream(pstTrd->s32CodecId, 800);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_ADEC_GetStream(pstTrd->s32CodecId, &stStreamOut, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		stFrm->pu32VirAddr = (IMP_U32 *)stStreamOut.pu8Stream;
		stFrm->s32Len      = stStreamOut.s32Len;

		s32Ret = IMP_ADEC_ReleaseStream(pstTrd->s32CodecId, &stStreamOut);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}
	} else {
		stFrm->pu32VirAddr = (IMP_U32 *)stStreamIn->pu8Stream;
		stFrm->s32Len      = stStreamIn->s32Len;
	}

	/*********************************
	 * save stream file before play
	 * ******************************/
	if (file) {
		fwrite(stFrm->pu32VirAddr, 1, stFrm->s32Len, file);
	}

	return s32Ret;
}

/*****************************************************************************
 * send stream to aenc(g711a, g711u, g726, adpcm, ...)
 * **************************************************************************/
IMP_S32 SAMPLE_AUDIO_AENC_DealStream(IMP_S32 s32AencChn, IMP_AUDIO_FRAME_S *stStreamIn, FILE *file)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	IMP_AUDIO_STREAM_S stStreamOut;
	if (g_sample_audio.isCodec) {
		s32Ret = IMP_AENC_SendFrame(s32AencChn, stStreamIn);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_AENC_PollingStream(s32AencChn, 1);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_AENC_GetStream(s32AencChn, &stStreamOut, BLOCK);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_AENC_ReleaseStream(s32AencChn, &stStreamOut);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}
		/*********************************
		 * save stream file before play
		 * ******************************/
		if (file) {
			fwrite(stStreamOut.pu8Stream, 1, stStreamOut.s32Len, file);
		}
	}

	return s32Ret;
}

IMP_VOID SAMPLE_AUDIO_AdjustVol()
{
	if (g_sample_audio.isMute) {
		g_sample_audio.muteStatus = g_sample_audio.vol;
		g_sample_audio.vol = -30.0;
	} else {
		switch (g_sample_audio.setVol) {
			case -2:
				g_sample_audio.vol -= 0.5;
				break;
			case -1:
				g_sample_audio.vol += 0.5;
				break;
			case 0:
				if (g_sample_audio.muteStatus != 0 && g_sample_audio.vol <= -30.0) {
					g_sample_audio.vol = g_sample_audio.muteStatus;
					g_sample_audio.muteStatus = 0;
				}
				break;
			case 1:
				g_sample_audio.gain -= 1.5;
				break;
			case 2:
				g_sample_audio.gain += 1.5;
				break;
			default:
				printf("setvol failed! please ENTER: v/V/g/G.\n");
				break;
		}
		if (g_sample_audio.setVol < 0)
			printf("vol:%f\n", g_sample_audio.vol);
		else if (g_sample_audio.setVol > 0)
			printf("gain:%f\n", g_sample_audio.gain);
	}
}

IMP_S32 sample_hdmi_ao_setvol(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_HDMI_AO_SetVolDb(s32DevId, s32Chn, pfVolDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_SetVolDb err: %x\n", s32Ret);
		return s32Ret;
	}

	*pfVolDb = 0.0;
	s32Ret = IMP_HDMI_AO_GetVolDb(s32DevId, s32Chn,pfVolDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_GetVolDb err: %x\n", s32Ret);
		return s32Ret;
	}
	//IMP_LOG_INFO(TAG, "HDMI Audio Out GetVolDb:%f\n", *pfVolDb);
	return s32Ret;
}

IMP_S32 sample_ai_set_volume(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb, IMP_FLOAT *pfGainDb)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_AI_SetVolDb(s32DevId, s32Chn, pfVolDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_SetVolDb err: %x\n", s32Ret);
		return s32Ret;
	}

	*pfVolDb = 0.0;
	s32Ret = IMP_AI_GetVolDb(s32DevId, s32Chn, pfVolDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_GetVolDb err: %x\n", s32Ret);
		return s32Ret;
	}
	//IMP_LOG_INFO(TAG, "Audio In GetVol VolDb:%f\n", *pfVolDb);

	s32Ret = IMP_AI_SetGainDb(s32DevId, s32Chn, pfGainDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_SetGainDb err: %x\n", s32Ret);
		return s32Ret;
	}

	*pfGainDb = 0.0;
	s32Ret = IMP_AI_GetGainDb(s32DevId, s32Chn, pfGainDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_GetGainDb err: %x\n", s32Ret);
		return s32Ret;
	}
	//IMP_LOG_INFO(TAG, "Audio In Get GainDb:%f\n", *pfGainDb);

	return s32Ret;
}

IMP_S32 sample_ao_set_volume(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb, IMP_FLOAT *pfGainDb)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_AO_SetVolDb(s32DevId, s32Chn, pfVolDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_SetVolDb err: %x\n", s32Ret);
		return s32Ret;
	}

	*pfVolDb = 0.0;
	s32Ret = IMP_AO_GetVolDb(s32DevId, s32Chn, pfVolDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_GetVolDb err: %x\n", s32Ret);
		return s32Ret;
	}
	//IMP_LOG_ERR(TAG, "Audio Out GetVolDb:%f\n", *pfVolDb);

	s32Ret = IMP_AO_SetGainDb(s32DevId, s32Chn, pfGainDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_SetGainDb err: %x\n", s32Ret);
		return s32Ret;
	}

	*pfGainDb = 0.0;
	s32Ret = IMP_AO_GetGainDb(s32DevId, s32Chn, pfGainDb);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_GetGainDb err: %x\n", s32Ret);
		return s32Ret;
	}
	//IMP_LOG_INFO(TAG, "Audio Out Get GainDb:%f\n", *pfGainDb);

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_SetVol(int isAi, IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_FLOAT *pfVolDb, IMP_FLOAT *pfGainDb)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (isAi) {
		s32Ret = IMP_AI_SetVolMute(s32DevId, s32Chn, g_sample_audio.isMute);
	} else {
		if (s32DevId == 0) {
			s32Ret = IMP_AO_SetVolMute(s32DevId, s32Chn, g_sample_audio.isMute);
		} else {
			s32Ret = IMP_HDMI_AO_SetVolMute(s32DevId, s32Chn, g_sample_audio.isMute);
		}
	}

	if (g_sample_audio.setVol) {
		if (isAi) {
			s32Ret = sample_ai_set_volume(s32DevId, s32Chn, pfVolDb, pfGainDb);
		} else {
			if (s32DevId == 0) {
				s32Ret = sample_ao_set_volume(s32DevId, s32Chn, pfVolDb, pfGainDb);
			} else {
				s32Ret = sample_hdmi_ao_setvol(s32DevId, s32Chn, pfVolDb);
			}
		}
	}

	g_sample_audio.setVol = 0;
	return s32Ret;
}

/******************************************
 * create adec channel
 * ***************************************/
IMP_S32 SAMPLE_AUDIO_AdecCreate(IMP_S32 s32AdecChn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (!g_sample_audio.isCodec) {
		return s32Ret;
	}

	IMP_AUDIO_DEC_CHN_ATTR_S stAdecAttr;
	stAdecAttr.eType = g_sample_audio.eCodec;
	stAdecAttr.s32BufSize = 20;

	s32Ret = IMP_ADEC_CreateChn(s32AdecChn, &stAdecAttr);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_ADEC_ClearChnBuf(s32AdecChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_AdecDestroy(IMP_S32 s32AdecChn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (!g_sample_audio.isCodec) {
		return s32Ret;
	}

	s32Ret = IMP_ADEC_DestroyChn(s32AdecChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

/******************************************
 * create aenc channel
 * ***************************************/
IMP_S32 SAMPLE_AUDIO_AencCreate(IMP_S32 s32AencChn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (!g_sample_audio.isCodec) {
		return s32Ret;
	}

	IMP_AUDIO_ENC_CHN_ATTR_S stAencAttr;
	stAencAttr.eType = g_sample_audio.eCodec;
	stAencAttr.s32BufSize = 50;

	s32Ret = IMP_AENC_CreateChn(s32AencChn, &stAencAttr);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_AencDestroy(IMP_S32 s32AencChn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (!g_sample_audio.isCodec) {
		return s32Ret;
	}

	s32Ret = IMP_AENC_DestroyChn(s32AencChn);
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_EnableResmp(IMP_S32 isAi, IMP_S32 s32DevId, IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (isAi) {
		//AI
		if (g_sample_audio.eAiRate != g_sample_audio.eStreamRate) {
			s32Ret = IMP_AI_EnableReSmp(s32DevId, s32Chn, g_sample_audio.eStreamRate);
		}
	} else {
		if (s32DevId == 0) {
			//AO
			if (g_sample_audio.eAoRate != g_sample_audio.eStreamRate) {
				s32Ret = IMP_AO_EnableReSmp(s32DevId, s32Chn, g_sample_audio.eStreamRate);
			}
		} else {
			//HDMI-AO
			if (g_sample_audio.eHdmiRate != g_sample_audio.eStreamRate) {
				s32Ret = IMP_HDMI_AO_EnableReSmp(s32DevId, s32Chn, g_sample_audio.eStreamRate);
			}
		}
	}
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_DisableResmp(IMP_S32 isAi, IMP_S32 s32DevId, IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (isAi) {
		//AI
		s32Ret = IMP_AI_DisableReSmp(s32DevId, s32Chn);
	} else {
		if (s32DevId == 0) {
			//AO
			s32Ret = IMP_AO_DisableReSmp(s32DevId, s32Chn);
		} else {
			//HDMI-AO
			s32Ret = IMP_HDMI_AO_DisableReSmp(s32DevId, s32Chn);
		}
	}
	if (s32Ret != IMP_SUCCESS) {
		SAMPLE_DBG(s32Ret);
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_EnableAlg(int isAi, IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_AUDIO_AGC_CONFIG_S stAgcConfig;
	stAgcConfig.s32TargetLevelDbfs = 15;	/* [0, 31] */
	stAgcConfig.s32CompressionGaindB = 20;	/* [0, 90] */

	if (g_sample_audio.isAlg == 0)
		return s32Ret;

	if (isAi) {
		//AI
		s32Ret = IMP_AI_EnableNs(pstAttr, 3);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_AI_EnableAgc(pstAttr, stAgcConfig);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_AI_EnableHpf(pstAttr);
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}
	} else {
		if (s32DevId == 0) {
			//AO
			s32Ret = IMP_AO_EnableNs(pstAttr, 3);
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_AO_EnableAgc(pstAttr, stAgcConfig);
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_AO_EnableHpf(pstAttr);
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}
		} else {
			//HDMI-AO
			s32Ret = IMP_HDMI_AO_EnableNs(pstAttr, 3);
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_HDMI_AO_EnableAgc(pstAttr, stAgcConfig);
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_HDMI_AO_EnableHpf(pstAttr);
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}
		}
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_DisableAlg(int isAi, IMP_S32 s32DevId, IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (isAi) {
		//AI
		s32Ret = IMP_AI_DisableNs();
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_AI_DisableAgc();
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}

		s32Ret = IMP_AI_DisableHpf();
		if (s32Ret != IMP_SUCCESS) {
			SAMPLE_DBG(s32Ret);
			return s32Ret;
		}
	} else {
		if (s32DevId == 0) {
			//AO
			s32Ret = IMP_AO_DisableNs();
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_AO_DisableAgc();
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_AO_DisableHpf();
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}
		} else {
			//HDMI-AO
			s32Ret = IMP_HDMI_AO_DisableNs();
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_HDMI_AO_DisableAgc();
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}

			s32Ret = IMP_HDMI_AO_DisableHpf();
			if (s32Ret != IMP_SUCCESS) {
				SAMPLE_DBG(s32Ret);
				return s32Ret;
			}
		}
	}

	return s32Ret;
}

/************************************************************
 * Pause : only support Ao or Hdmi-Ao
 * *********************************************************/
IMP_S32 SAMPLE_AUDIO_AOPause(IMP_S32 s32DevId, IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	if (g_sample_audio.isPause) {
		printf("\nPlease input 'P' key to continue.  >");
		while (1) {
			if (g_sample_audio.isPause == 0)
				break;
			else
				sleep(1);
		};

		if (s32DevId == 0)
			s32Ret = IMP_AO_ClearChnBuf(s32DevId, s32Chn);
		else
			s32Ret = IMP_HDMI_AO_ClearChnBuf(s32DevId, s32Chn);
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_AOEnable(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_AO_SetPubAttr(s32DevId, pstAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_SetPubAttr err: %x\n", s32Ret);
		return s32Ret;
	}

	memset(pstAttr, 0x0, sizeof(IMP_AUDIO_IOATTR_S));
	s32Ret = IMP_AO_GetPubAttr(s32DevId, pstAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_GetPubAttr err: %x\n", s32Ret);
		return s32Ret;
	}

	if (pstAttr->eSamplerate != g_sample_audio.eAoRate) {
		printf("set sampleRate failed!\n");
		return IMP_FAILURE;
	} else {
		g_sample_audio.eAoRate = pstAttr->eSamplerate;
	}

	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   eSamplerate  :%d\n", pstAttr->eSamplerate);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   eBitwidth    :%d\n", pstAttr->eBitwidth);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   eSoundmode   :%d\n", pstAttr->eSoundmode);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   s32NumPerFrm :%d\n", pstAttr->s32NumPerFrm);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   s16ChnCnt    :%d\n", pstAttr->s16ChnCnt);

	s32Ret = IMP_AO_Enable(s32DevId);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_Enable err: %x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_AO_EnableChn(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_EnableChn err: %x\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_AODisable(IMP_S32 s32DevId, IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_AO_DisableChn(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_DisableChn err: %x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_AO_Disable(s32DevId);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AO_Disable err: %x\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_HdmiAoEnable(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	s32Ret = IMP_HDMI_AO_SetPubAttr(s32DevId, pstAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_SetPubAttr err: %x\n", s32Ret);
		return s32Ret;
	}

	memset(pstAttr, 0x0, sizeof(IMP_AUDIO_IOATTR_S));
	s32Ret = IMP_HDMI_AO_GetPubAttr(s32DevId, pstAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_GetPubAttr err: %x\n", s32Ret);
		return s32Ret;
	}

	IMP_LOG_INFO(TAG, "Hdmi Audio Out GetPubAttr   eSamplerate  :%d\n", pstAttr->eSamplerate);
	IMP_LOG_INFO(TAG, "Hdmi Audio Out GetPubAttr   eBitwidth    :%d\n", pstAttr->eBitwidth);
	IMP_LOG_INFO(TAG, "Hdmi Audio Out GetPubAttr   eSoundmode   :%d\n", pstAttr->eSoundmode);
	IMP_LOG_INFO(TAG, "Hdmi Audio Out GetPubAttr   s32NumPerFrm :%d\n", pstAttr->s32NumPerFrm);
	IMP_LOG_INFO(TAG, "Hdmi Audio Out GetPubAttr   s16ChnCnt    :%d\n", pstAttr->s16ChnCnt);

	s32Ret = IMP_HDMI_AO_Enable(s32DevId);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_Enable err: %x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_HDMI_AO_EnableChn(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_EnableChn err: %x\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_HdmiAoDisable(IMP_S32 s32DevId,IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	s32Ret = IMP_HDMI_AO_DisableChn(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_DisableChn err: %x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_HDMI_AO_Disable(s32DevId);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_HDMI_AO_Disable err: %x\n", s32Ret);
		return s32Ret;
	}
	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_AIEnable(IMP_S32 s32DevId, IMP_S32 s32Chn, IMP_AUDIO_IOATTR_S *pstAttr)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_AUDIO_CHN_PARAM_S stChnParam;

	s32Ret = IMP_AI_SetPubAttr(s32DevId, pstAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_SetPubAttr err: %x\n", s32Ret);
		return s32Ret;
	}

	memset(pstAttr, 0x0, sizeof(IMP_AUDIO_IOATTR_S));
	s32Ret = IMP_AI_GetPubAttr(s32DevId, pstAttr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_GetPubAttr err: %x\n", s32Ret);
		return s32Ret;
	}

	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  eSamplerate  : %d\n", pstAttr->eSamplerate);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  eBitwidth    : %d\n", pstAttr->eBitwidth);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  eSoundmode   : %d\n", pstAttr->eSoundmode);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  s32NumPerFrm : %d\n", pstAttr->s32NumPerFrm);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  s16ChnCnt    : %d\n", pstAttr->s16ChnCnt);

	s32Ret = IMP_AI_Enable(s32DevId);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_Enable err: %x\n", s32Ret);
		return s32Ret;
	}

	stChnParam.eAecChn = AUDIO_AEC_CHANNEL_FIRST_LEFT;
	s32Ret = IMP_AI_SetChnParam(s32DevId, s32Chn, &stChnParam);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_SetChnParam err: %x\n", s32Ret);
		return s32Ret;
	}

	memset(&stChnParam, 0x0, sizeof(IMP_AUDIO_CHN_PARAM_S));
	s32Ret = IMP_AI_GetChnParam(s32DevId, s32Chn, &stChnParam);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_GetChnParam err: %x\n", s32Ret);
		return s32Ret;
	}
	IMP_LOG_INFO(TAG, "eAecChn: %d\n", stChnParam.eAecChn);

	s32Ret = IMP_AI_EnableChn(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_EnableChn err: %x\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}

IMP_S32 SAMPLE_AUDIO_AIDisable(IMP_S32 s32DevId, IMP_S32 s32Chn)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	s32Ret = IMP_AI_DisableChn(s32DevId, s32Chn);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_DisableChn err: %x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_AI_Disable(s32DevId);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_AI_Disable err: %x\n", s32Ret);
		return s32Ret;
	}

	return s32Ret;
}



