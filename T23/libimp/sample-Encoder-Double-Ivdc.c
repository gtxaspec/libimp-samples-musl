/*
 * sample-Encoder-h264-jpeg-double.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include "sample-common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TAG "Sample-Encoder-Double-Ivdc"
static int  gconf_i2c_addr = 0x37;
static int  gconf_i2c_addr1 = 0x3f;
static int  gconf_FPS_Num = 15;
static int  gconf_FPS_Den = 1;
static int  gconf_nrvbs = 2;
static int  gconf_Main_VideoWidth = 1920;
static int  gconf_Main_VideoHeight = 1080;
static int  gconf_Second_VideoWidth = 720;
static int  gconf_Second_VideoHeight = 576;
static int  gconf_videoBitrate = 2000;
static int  gconf_direct_mode = 1;
static IMPSensorInfo sensor_info;
static IMPSensorInfo sensor_info1;
static char gconf_Sensor_Name[16] = "gc2053";
static char gconf_Sensor_Name1[16] = "gc2053s1";
static int ImpSystemInit()
{
	int ret = 0;

	memset(&sensor_info, 0, sizeof(IMPSensorInfo));
	memset(&sensor_info1, 0, sizeof(IMPSensorInfo));

	strcpy(sensor_info.name, gconf_Sensor_Name);
	sensor_info.cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
	strcpy(sensor_info.i2c.type, gconf_Sensor_Name);
	sensor_info.i2c.addr = gconf_i2c_addr;
	sensor_info.i2c.i2c_adapter_id = 0;

	strcpy(sensor_info1.name, gconf_Sensor_Name1);
	sensor_info1.cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
	sensor_info1.sensor_id = 1;
	strcpy(sensor_info1.i2c.type, gconf_Sensor_Name1);
	sensor_info1.i2c.addr = gconf_i2c_addr1;
	sensor_info1.i2c.i2c_adapter_id = 0;

	IMP_LOG_DBG(TAG, "ImpSystemInit start\n");

	if(IMP_ISP_Open()){
		IMP_LOG_ERR(TAG, "failed to EmuISPOpen\n");
		return -1;
	}

	ret = IMP_ISP_AddSensor(&sensor_info);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_AddSensor(&sensor_info1);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to AddSensor1\n");
		return -1;
	}

	ret = IMP_ISP_EnableSensor();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to EnableSensor\n");
		return -1;
	}

	IMP_System_Init();

	ret = IMP_ISP_EnableTuning();
	if(ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_ISP_EnableTuning error\n");
		return -1;
	}

	IMPISPAEMin ae_min;
	ae_min.min_again = 2048;
	ret += IMP_ISP_Tuning_SetAeMin(&ae_min);
	if (ret < 0){
		IMP_LOG_ERR(TAG, "failed to set ae min parameters\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, "ImpSystemInit success\n");

	return 0;
}

static int Framesource_init()
{
	int i = 0;
	int nr_chn = 2;
	int ret = -1;
	IMPFSChnAttr imp_chn_attr[6];

	memset(imp_chn_attr, 0, sizeof(imp_chn_attr));
	imp_chn_attr[0].pixFmt = PIX_FMT_NV12;
	imp_chn_attr[0].outFrmRateNum = gconf_FPS_Num;
	imp_chn_attr[0].outFrmRateDen = gconf_FPS_Den;
	imp_chn_attr[0].nrVBs = gconf_nrvbs;
	imp_chn_attr[0].type = FS_PHY_CHANNEL;
	imp_chn_attr[0].crop.enable = 0;
	imp_chn_attr[0].scaler.enable = 0;
	imp_chn_attr[0].scaler.outwidth = gconf_Main_VideoWidth;
	imp_chn_attr[0].scaler.outheight = gconf_Main_VideoHeight;
	imp_chn_attr[0].picWidth = gconf_Main_VideoWidth;
	imp_chn_attr[0].picHeight = gconf_Main_VideoHeight;

	imp_chn_attr[1].pixFmt = PIX_FMT_NV12;
	imp_chn_attr[1].outFrmRateNum = gconf_FPS_Num;
	imp_chn_attr[1].outFrmRateDen = gconf_FPS_Den;
	imp_chn_attr[1].nrVBs = gconf_nrvbs;
	imp_chn_attr[1].type = FS_PHY_CHANNEL;
	imp_chn_attr[1].crop.enable = 0;
	imp_chn_attr[1].scaler.enable = 1;
	imp_chn_attr[1].scaler.outwidth = gconf_Second_VideoWidth;
	imp_chn_attr[1].scaler.outheight = gconf_Second_VideoHeight;
	imp_chn_attr[1].picWidth = gconf_Second_VideoWidth;
	imp_chn_attr[1].picHeight = gconf_Second_VideoHeight;

	memcpy(&imp_chn_attr[2], &imp_chn_attr[1], sizeof(IMPFSChnAttr));
	memcpy(&imp_chn_attr[3], &imp_chn_attr[0], sizeof(IMPFSChnAttr));
	memcpy(&imp_chn_attr[4], &imp_chn_attr[1], sizeof(IMPFSChnAttr));
	memcpy(&imp_chn_attr[5], &imp_chn_attr[2], sizeof(IMPFSChnAttr));

	for (i = 0; i < nr_chn; i++) {
		/* create channel i*/
		ret = IMP_FrameSource_CreateChn(i, imp_chn_attr+i);
		if(ret < 0){
			IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error!\n", i);
			return -1;
		}

		ret = IMP_FrameSource_SetChnAttr(i, imp_chn_attr+i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(%d) error: %d\n", ret, i);
			return -1;
		}
	}
	for (i = 3; i < nr_chn+3; i++) {
		/* create channel i*/
		ret = IMP_FrameSource_CreateChn(i, imp_chn_attr+i);
		if(ret < 0){
			IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error!\n", i);
			return -1;
		}

		ret = IMP_FrameSource_SetChnAttr(i, imp_chn_attr+i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(%d) error: %d\n", ret, i);
			return -1;
		}
	}
	return 0;
}

static int encoder_param_defalt(IMPEncoderCHNAttr *chnAttr, IMPPayloadType encType, IMPEncoderRcMode rcMode,
		int w, int h, int outFrmRateNum, int outFrmRateDen, int outBitRate)
{
	IMPEncoderAttr *enc_attr;
	IMPEncoderRcAttr *rc_attr;

	if ((encType < PT_JPEG) || (encType > PT_H265)) {
		IMP_LOG_ERR(TAG, "unsupported encode type:%d, we only support h264, h265 and jpeg type\n", encType);
		return -1;
	}

	memset(chnAttr, 0, sizeof(IMPEncoderCHNAttr));
	enc_attr = &chnAttr->encAttr;
	enc_attr->enType = encType;
	enc_attr->bufSize = 0;
	enc_attr->profile   = 1;
	enc_attr->picWidth  = w;
	enc_attr->picHeight = h;

	if (encType == PT_H264) {
		enc_attr->profile = -1;
		rc_attr = &chnAttr->rcAttr;
		rc_attr->outFrmRate.frmRateNum = outFrmRateNum;
		rc_attr->outFrmRate.frmRateDen = outFrmRateDen;
		rc_attr->maxGop = 2 * rc_attr->outFrmRate.frmRateNum / rc_attr->outFrmRate.frmRateDen;
		if (rcMode == ENC_RC_MODE_CBR) {
			rc_attr->attrRcMode.rcMode = ENC_RC_MODE_CBR;
			rc_attr->attrRcMode.attrH264Cbr.outBitRate = outBitRate;
			rc_attr->attrRcMode.attrH264Cbr.maxQp = 45;
			rc_attr->attrRcMode.attrH264Cbr.minQp = 15;
			rc_attr->attrRcMode.attrH264Cbr.iBiasLvl = 0;
			rc_attr->attrRcMode.attrH264Cbr.frmQPStep = 3;
			rc_attr->attrRcMode.attrH264Cbr.gopQPStep = 15;
			rc_attr->attrRcMode.attrH264Cbr.adaptiveMode = false;
			rc_attr->attrRcMode.attrH264Cbr.gopRelation = false;

			rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
			rc_attr->attrHSkip.hSkipAttr.m = 0;
			rc_attr->attrHSkip.hSkipAttr.n = 0;
			rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
			rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
			rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
			rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
		} else if (rcMode == ENC_RC_MODE_VBR) {
			rc_attr->attrRcMode.rcMode = ENC_RC_MODE_VBR;
			rc_attr->attrRcMode.attrH264Vbr.maxQp = 45;
			rc_attr->attrRcMode.attrH264Vbr.minQp = 15;
			rc_attr->attrRcMode.attrH264Vbr.staticTime = 2;
			rc_attr->attrRcMode.attrH264Vbr.maxBitRate = outBitRate;
			rc_attr->attrRcMode.attrH264Vbr.iBiasLvl = 0;
			rc_attr->attrRcMode.attrH264Vbr.changePos = 80;
			rc_attr->attrRcMode.attrH264Vbr.qualityLvl = 2;
			rc_attr->attrRcMode.attrH264Vbr.frmQPStep = 3;
			rc_attr->attrRcMode.attrH264Vbr.gopQPStep = 15;
			rc_attr->attrRcMode.attrH264Vbr.gopRelation = false;

			rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
			rc_attr->attrHSkip.hSkipAttr.m = 0;
			rc_attr->attrHSkip.hSkipAttr.n = 0;
			rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
			rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
			rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
			rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
		} else if (rcMode == ENC_RC_MODE_SMART) {
			rc_attr->attrRcMode.rcMode = ENC_RC_MODE_SMART;
			rc_attr->attrRcMode.attrH264Smart.maxQp = 45;
			rc_attr->attrRcMode.attrH264Smart.minQp = 15;
			rc_attr->attrRcMode.attrH264Smart.staticTime = 2;
			rc_attr->attrRcMode.attrH264Smart.maxBitRate = outBitRate;
			rc_attr->attrRcMode.attrH264Smart.iBiasLvl = 0;
			rc_attr->attrRcMode.attrH264Smart.changePos = 80;
			rc_attr->attrRcMode.attrH264Smart.qualityLvl = 2;
			rc_attr->attrRcMode.attrH264Smart.frmQPStep = 3;
			rc_attr->attrRcMode.attrH264Smart.gopQPStep = 15;
			rc_attr->attrRcMode.attrH264Smart.gopRelation = false;

			rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
			rc_attr->attrHSkip.hSkipAttr.m = rc_attr->maxGop - 1;
			rc_attr->attrHSkip.hSkipAttr.n = 1;
			rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 6;
			rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
			rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
			rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
		} else { /* fixQp */
			rc_attr->attrRcMode.rcMode = ENC_RC_MODE_FIXQP;
			rc_attr->attrRcMode.attrH264FixQp.qp = 35;

			rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
			rc_attr->attrHSkip.hSkipAttr.m = 0;
			rc_attr->attrHSkip.hSkipAttr.n = 0;
			rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
			rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
			rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
			rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
		}
	}

    return 0;
}

static int Encoder_init()
{
	int ret, i, enc_chnCnt = 6, enc_grpCnt = 4;
	int chnNum = 0, grpNum = 0;
	IMPEncoderCHNAttr chnAttr;

	for (i = 0; i < enc_grpCnt; i++) {
		ret = IMP_Encoder_CreateGroup(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", i);
			return -1;
		}
	}

	for (i = 0; i < enc_chnCnt; i++) {
		switch (i) {
			case 0:
				encoder_param_defalt(&chnAttr, PT_H264, ENC_RC_MODE_SMART, gconf_Main_VideoWidth,
						gconf_Main_VideoHeight, gconf_FPS_Num, gconf_FPS_Den, gconf_videoBitrate);
				chnNum = grpNum = 0;
				chnAttr.bEnableIvdc = gconf_direct_mode ? true : false;
				break;
			case 1:
				encoder_param_defalt(&chnAttr, PT_H264, ENC_RC_MODE_SMART, gconf_Main_VideoWidth,
						gconf_Main_VideoHeight, gconf_FPS_Num, gconf_FPS_Den, gconf_videoBitrate);
				chnNum = grpNum = 1;
				chnAttr.bEnableIvdc = gconf_direct_mode ? true : false;
				break;
			case 2: /* main-0 jpeg*/
				encoder_param_defalt(&chnAttr, PT_JPEG, ENC_RC_MODE_SMART, gconf_Main_VideoWidth,
						gconf_Main_VideoHeight, gconf_FPS_Num, gconf_FPS_Den, gconf_videoBitrate);
				chnNum = 2;
				grpNum = 0;
				chnAttr.bEnableIvdc = gconf_direct_mode ? true : false;
				break;
			case 3: /* main-1 h264 or h265 */
				encoder_param_defalt(&chnAttr, PT_JPEG, ENC_RC_MODE_SMART, gconf_Main_VideoWidth,
						gconf_Main_VideoHeight, gconf_FPS_Num, gconf_FPS_Den, gconf_videoBitrate);
				chnNum = 3;
				grpNum = 1;
				chnAttr.bEnableIvdc = gconf_direct_mode ? true : false;
				break;
			case 4:
				encoder_param_defalt(&chnAttr, PT_H264, ENC_RC_MODE_SMART, gconf_Second_VideoWidth,
						gconf_Second_VideoHeight, gconf_FPS_Num, gconf_FPS_Den, gconf_videoBitrate);
				chnNum = 4;
				grpNum = 2;
				break;
			case 5:
				encoder_param_defalt(&chnAttr, PT_H264, ENC_RC_MODE_SMART, gconf_Second_VideoWidth,
						gconf_Second_VideoHeight, gconf_FPS_Num, gconf_FPS_Den, gconf_videoBitrate);
				chnNum = 5;
				grpNum = 3;
				break;
			default:
				IMP_LOG_ERR(TAG, "unsupported encChnNum:%d\n", i);
				return -1;
		}

		/* Create Channel */
		ret = IMP_Encoder_CreateChn(chnNum, &chnAttr);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error: %d\n", chnNum, ret);
			return -1;
		}

		/* Resigter Channel */
		ret = IMP_Encoder_RegisterChn(grpNum, chnNum);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(%d, %d) error: %d\n", grpNum, chnNum, ret);
			return -1;
		}
	}

	return 0;

}

static int Framesource_streamon()
{
	int i, ret;
	int nr_chn = 2;
	for (i = 0; i < nr_chn; i++) {
		ret = IMP_FrameSource_EnableChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, i);
			return -1;
		}
	}

	for (i = 3; i < nr_chn+3; i++) {
		ret = IMP_FrameSource_EnableChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, i);
			return -1;
		}
	}
	return 0;
}

static int Framesource_streamoff()
{
	int i, ret;
	int nr_chn = 2;
	for (i = 0; i < nr_chn; i++) {
		ret = IMP_FrameSource_DisableChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, i);
			return -1;
		}
	}

	for (i = 3; i < nr_chn+3; i++) {
		ret = IMP_FrameSource_DisableChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, i);
			return -1;
		}
	}
	return 0;
}

static int save_stream(int fd, IMPEncoderStream *stream)
{
	int ret, i, nr_pack = stream->packCount;

	for (i = 0; i < nr_pack; i++) {
		ret = write(fd, (void *)stream->pack[i].virAddr,
				stream->pack[i].length);
		if (ret != stream->pack[i].length) {
			IMP_LOG_ERR(TAG, "stream write ret(%d) != stream->pack[%d].length(%d)\n", ret, i, stream->pack[i].length);
			return -1;
		}
	}
	return 0;
}

static void *get_stream_thread(void *arg)
{
	int chnNum, ret, i;
	char stream_path[64];
    int stream_fd = -1;

	chnNum = (int)arg;

	if ((chnNum == 2) || (chnNum == 3)) {
		for (i = 0; i < NR_FRAMES_TO_SAVE / 50; i ++) {
			sleep(1);
			ret = IMP_Encoder_StartRecvPic(chnNum);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
				return ((void *)-1);
			}

			sprintf(stream_path, "%s/stream-%d-%d.%s", STREAM_FILE_PATH_PREFIX, chnNum, i, "jpeg");

			stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
			if (stream_fd < 0) {
				IMP_LOG_ERR(TAG, "open jpeg_fd failed\n");
				return ((void *)-1);
			}

			/* Polling JPEG Snap, set timeout as 1000msec */
			ret = IMP_Encoder_PollingStream(chnNum, 1000);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "chnNum:%d, Polling stream timeout\n", chnNum);
				return ((void *)-1);
			}

			IMPEncoderStream stream;
			/* Get JPEG Snap */
			ret = IMP_Encoder_GetStream(chnNum, &stream, 1);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream() failed\n");
				return ((void *)-1);
			}

			ret = save_stream(stream_fd, &stream);
			if (ret < 0) {
				close(stream_fd);
				return ((void *)-1);
			}

			IMP_Encoder_ReleaseStream(chnNum, &stream);

			close(stream_fd);

			ret = IMP_Encoder_StopRecvPic(chnNum);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic() failed\n");
				return ((void *)-1);
			}
		}
	} else {
		ret = IMP_Encoder_StartRecvPic(chnNum);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
			return ((void *)-1);
		}

		sprintf(stream_path, "%s/stream-%d.%s", STREAM_FILE_PATH_PREFIX, chnNum, "h264");
		stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
		if (stream_fd < 0) {
			IMP_LOG_ERR(TAG, "open h264_fd failed\n");
			return ((void *)-1);
		}

		for (i = 0; i < NR_FRAMES_TO_SAVE; i++) {
			ret = IMP_Encoder_PollingStream(chnNum, 1000);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_PollingStream(%d) timeout\n", chnNum);
				continue;
			}

			IMPEncoderStream stream;
			/* Get H264 or H265 Stream */
			ret = IMP_Encoder_GetStream(chnNum, &stream, 1);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream(%d) failed\n", chnNum);
				return ((void *)-1);
			}

			ret = save_stream(stream_fd, &stream);
			if (ret < 0) {
				close(stream_fd);
				return ((void *)ret);
			}
			IMP_Encoder_ReleaseStream(chnNum, &stream);
		}

		close(stream_fd);

		ret = IMP_Encoder_StopRecvPic(chnNum);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic(%d) failed\n", chnNum);
			return ((void *)-1);
		}
	}

	return ((void *)0);
}

static int Encoder_exit()
{
	int i = 0, ret;
	int enc_chnCnt = 6, enc_grpCnt = 4;

	for (i = 0; i < enc_chnCnt; i++) {
		ret = IMP_Encoder_UnRegisterChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_UnRegisterChn(%d) error: %d\n", i, ret);
			return -1;
		}

		ret = IMP_Encoder_DestroyChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyChn(%d) error: %d\n", i, ret);
			return -1;
		}
	}

	for (i = 0; i < enc_grpCnt; i++) {
		ret = IMP_Encoder_DestroyGroup(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyGroup(%d) error: %d\n", i, ret);
			return -1;
		}
	}

	return 0;
}

static int Framesource_exit()
{
	int ret,i;
	int nr_chn = 2;

	for (i = 0; i < nr_chn; i++) {
		ret = IMP_FrameSource_DestroyChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn(%d) error: %d\n", i, ret);
			return -1;
		}
	}
	for (i = 3; i < nr_chn+3; i++) {
		ret = IMP_FrameSource_DestroyChn(i);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn(%d) error: %d\n", i, ret);
			return -1;
		}
	}

	return 0;
}

static int ImpSystemExit()
{
	int ret = 0;

	IMP_LOG_DBG(TAG, "ImpSystemExit start\n");

	IMP_System_Exit();

	ret = IMP_ISP_DisableSensor();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_ISP_DelSensor(&sensor_info);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to DelSensor\n");
		return -1;
	}

	ret = IMP_ISP_DelSensor(&sensor_info1);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to DelSensor1\n");
		return -1;
	}

	ret = IMP_ISP_DisableTuning();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_ISP_DisableTuning failed\n");
		return -1;
	}

	if(IMP_ISP_Close()){
		IMP_LOG_ERR(TAG, "failed to open ISP\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, " sample_system_exit success\n");
	return 0;
}

/* sample-Encoder-Double-Ivdc.c:双摄双直通用例
 * direct_mode=2为双摄双直通模式。
 * Framesource0只用于主摄主码流，Framesource3只用于次摄主码流。
 * 编码Grouop0和编码Grouop1只用于双摄双直通绑定fs0和fs3。
 * 编码通道0(h264)和编码通道2(jpeg)只能注册在编码Group0。
 * 编码通道1(h264)和编码通道3(jpeg)只能注册在编码Group1。
 * 主摄主码流直通：                |--enc_chn 0 用于h264
 * FrameSource0 -- Encoder_Group0->
 *                                 |--enc_chn 2 用于jpeg
 *
 * 次摄主码流直通：                |--enc_chn 1 用于h264
 * FrameSource3 -- Encoder_Group1->
 *                                 |--enc_chn 3 用于jpeg
 *
 * 主摄次码流非直通：
 * FrameSource1 -- Encoder_Group2->|--enc_chn 4 用于主摄次码流h264
 *
 * 次摄次码流非直通：
 * FrameSource4 -- Encoder_Group3->|--enc_chn 5 用于次摄次码流h264
 *
 * 以上是sample的Framesource，编码group，编码chn绑定关系图。
 *
 * */

int main(int argc, char *argv[])
{
	int i, ret;
	int arg = 0;

	/* Step.1 System init */
	ret = ImpSystemInit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpSystemInit() failed\n");
		return -1;
	}

	/* Step.2 FrameSource init */
	ret = Framesource_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "framesource_init() failed\n");
		return -1;
	}

	/* Step.3 Encoder init */
	ret = Encoder_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "encoder_init failed\n");
		return -1;
	}

	IMPCell framesource_chn0 = {DEV_ID_FS, 0, 0};
	IMPCell framesource_chn3 = {DEV_ID_FS, 3, 0};
	IMPCell framesource_chn1 = {DEV_ID_FS, 1, 0};
	IMPCell framesource_chn4 = {DEV_ID_FS, 4, 0};
	IMPCell encoder0 = {DEV_ID_ENC, 0, 0};
	IMPCell encoder1 = {DEV_ID_ENC, 1, 0};
	IMPCell encoder2 = {DEV_ID_ENC, 2, 0};
	IMPCell encoder3 = {DEV_ID_ENC, 3, 0};
	/* Step.4 Bind */
	ret = IMP_System_Bind(&framesource_chn0, &encoder0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind(&framesource_chn0, &encoder0) failed...\n");
		return ret;
	}

	ret = IMP_System_Bind(&framesource_chn3, &encoder1);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind(&framesource_chn3, &encoder1) failed...\n");
		return ret;
	}

	ret = IMP_System_Bind(&framesource_chn1, &encoder2);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind(&framesource_chn1, &encoder2) failed...\n");
		return ret;
	}

	ret = IMP_System_Bind(&framesource_chn4, &encoder3);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Bind(&framesource_chn4, &encoder3) failed...\n");
		return ret;
	}


	/* Step.5 Stream On */
	ret = Framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "framesource_streamon failed\n");
		return -1;
	}

	/* Step.6 Get stream and Snap */
	pthread_t tid[6];
	for (i = 0; i < 6; i++) {
		arg = i;
		ret = pthread_create(&tid[i], NULL, get_stream_thread, (void *)arg);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "Create index(%d) get_stream_thread failed\n", i);
		}
	}

	for (i = 0; i < 6; i++) {
		pthread_join(tid[i], NULL);
	}

	/* Step.7 Streamoff */
	ret = Framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	/* Step.8 Unbind */
	ret = IMP_System_UnBind(&framesource_chn0, &encoder0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_UnBind(&framesource_chn0, &encoder0) failed...\n");
		return ret;
	}

	ret = IMP_System_UnBind(&framesource_chn3, &encoder1);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_UnBind(&framesource_chn3, &encoder1) failed...\n");
		return ret;
	}

	ret = IMP_System_UnBind(&framesource_chn1, &encoder2);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_UnBind(&framesource_chn1, &encoder2) failed...\n");
		return ret;
	}

	ret = IMP_System_UnBind(&framesource_chn4, &encoder3);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_UnBind(&framesource_chn4, &encoder3) failed...\n");
		return ret;
	}


	/* Step.9 Encoder exit */
	ret = Encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	/* Step.10 FrameSource exit */
	ret = Framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.11 System exit */
	ret = ImpSystemExit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpSystemExit() failed\n");
		return -1;
	}

	return 0;
}
