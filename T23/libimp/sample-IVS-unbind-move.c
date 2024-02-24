/*
 * sample-IVS-unbind-move.c
 *
 * Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 */
#include <string.h>
#include <stdlib.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_ivs.h>
#include <imp/imp_ivs_move.h>
#include "sample-common.h"
#define TAG "Sample-IVS-unbind-move"
extern struct chn_conf chn[];

/**
 * @defgroup IMP_IVS
 * @ingroup imp
 * @brief IVS Intelligent Analysis General Interface API (The debugging contents below can be viewed in Ingenic-SDK-Txx/include_cn/imp/imp_ivs_move.h)
 * @section concept 1 
 * IMP IVS uses the IVS general interface API to call the instantiated IMPIVSInterface to embed intelligent analysis algorithms into the SDK for analyzing the frame images in the SDK.
 * @subsection IMPIVSInterface 1.1 IMPIVSInterface
 * IMPIVSInterface is a general algorithm interface. Specific algorithms can be implemented by implementing this interface and passing it to IMP IVS to achieve the purpose of running specific algorithms in the SDK.
 * The member "param" of IMPIVSInterface is the parameter of the member function "init".
 * @section ivs_usage 2 
 * Please refer to the sample file for the specific implementation of the function.
 *
 * STEP.1 To initialize the system, you can directly call the "sample_system_init()" function in the example.
 * The whole application can only initialize the system once. If it has been initialized before, there is no need to initialize it again here.
 * @code
 * ret = IMP_ISP_Open(); //open ISP module
 * ret = IMP_ISP_EnableTuning();	// To enable flipping and debug the image:
 *	ret = IMP_ISP_SetCameraInputMode(&mode) //To set up the multi-sensor mode (please ignore if it is a single sensor) when there are multiple sensors (up to three sensors supported):
 *	ret = IMP_ISP_AddSensor(IMPVI_MAIN, &sensor_info[*]) //To add a sensor (the sensor driver has been added to the kernel before this operation) (IMPVI_MAIN is the main sensor, IMPVI_SEC is the secondary sensor, IMPVI_THR is the third sensor):
 * @endcode
 *
 * STEP.2 Initialize framesource
 * If the framesource channel used by the algorithm has been created, you can directly use the created channel.
 * If the framesource channel used by the algorithm has not been created, you can call "sample_framesource_init(IVS_FS_CHN, &fs_chn_attr)" in the example to create it.
 * @code
 * ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr);	//Create channel
 * ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr);	//Set channel attribute
 * @endcode
 *
 * STEP.3 Initiate framesource。
 * @code
 *	IMP_FrameSource_SetFrameDepth(0, 0);	//To set the maximum image depth, this interface is used to set the number of video frames cached in a channel buffer. 
 * When the user sets multiple frames of video images in the cache, the user can obtain a certain number of continuous image data. 
 * If the depth is specified as 0, it means that the system does not need to cache images for the channel, so the user cannot obtain image data for that channel. 
 * By default, the system does not cache images for the channel, that is, the depth defaults to 0.
 *	ret = sample_framesource_streamon(IVS_FS_CHN);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
 *		return -1;
 *	}
 * @endcode
 *
 * STEP.4 Obtain Algorithm result
 * Polling, getting and releasing results must be strictly corresponding, and cannot be interrupted in the middle.
 * Only when the polling result returns correctly, the obtained result will be updated. Otherwise, the obtained result cannot be predicted.
 *
 * STEP.6~9 To release resources, please use them in the order corresponding to the sample code regarding the release of resources.
 * @code
 *  sample_ivs_move_stop(2, inteface);
 *  sample_framesource_streamoff();
 *  sample_framesource_exit(IVS_FS_CHN);
 *  sample_system_exit();
 * @endcode
 */
static int sample_ivs_move_start(int grp_num, int chn_num, IMPIVSInterface **interface)
{
	IMP_IVS_MoveParam param;
	int i = 0, j = 0;
	memset(&param, 0, sizeof(IMP_IVS_MoveParam));
	param.skipFrameCnt = 1;
	param.frameInfo.width = SENSOR_WIDTH_SECOND;
	param.frameInfo.height = SENSOR_HEIGHT_SECOND;
	param.roiRectCnt = 1;
	for(i=0; i<param.roiRectCnt; i++){
	  param.sense[i] = 4;
	}
	/* printf("param.sense=%d, param.skipFrameCnt=%d, param.frameInfo.width=%d, param.frameInfo.height=%d\n", param.sense, param.skipFrameCnt, param.frameInfo.width, param.frameInfo.height); */
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 2; i++) {
		  if((i==0)&&(j==0)){
			param.roiRect[j * 2 + i].p0.x = i * param.frameInfo.width /* / 2 */;
			param.roiRect[j * 2 + i].p0.y = j * param.frameInfo.height /* / 2 */;
			param.roiRect[j * 2 + i].p1.x = (i + 1) * param.frameInfo.width /* / 2 */ - 1;
			param.roiRect[j * 2 + i].p1.y = (j + 1) * param.frameInfo.height /* / 2 */ - 1;
			printf("(%d,%d) = ((%d,%d)-(%d,%d))\n", i, j, param.roiRect[j * 2 + i].p0.x, param.roiRect[j * 2 + i].p0.y,param.roiRect[j * 2 + i].p1.x, param.roiRect[j * 2 + i].p1.y);
		  }
		  else
		    {
		      	param.roiRect[j * 2 + i].p0.x = param.roiRect[0].p0.x;
			param.roiRect[j * 2 + i].p0.y = param.roiRect[0].p0.y;
			param.roiRect[j * 2 + i].p1.x = param.roiRect[0].p1.x;;
			param.roiRect[j * 2 + i].p1.y = param.roiRect[0].p1.y;;
			printf("(%d,%d) = ((%d,%d)-(%d,%d))\n", i, j, param.roiRect[j * 2 + i].p0.x, param.roiRect[j * 2 + i].p0.y,param.roiRect[j * 2 + i].p1.x, param.roiRect[j * 2 + i].p1.y);
		    }
		}
	}
	*interface = IMP_IVS_CreateMoveInterface(&param);
	if (*interface == NULL) {
		IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
		return -1;
	}
	return 0;
}
static int sample_ivs_move_stop(int chn_num, IMPIVSInterface *interface)
{
	IMP_IVS_DestroyMoveInterface(interface);
	return 0;
}
#if 1
static int sample_ivs_set_sense(int chn_num, int sensor, IMPIVSInterface *interface)
{
	int ret = 0;
	IMP_IVS_MoveParam param;
	int i = 0;

	/* ret = IMP_IVS_GetParam(chn_num, &param); */
    ret = interface->getParam(interface,&param);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "interface->getParam failed ret=%d\n", ret);
		return -1;
	}

    printf("sensor:%d\n",sensor);
	for( i = 0 ; i < param.roiRectCnt ; i++){
	  param.sense[i] = sensor;
	}
	/* ret = IMP_IVS_SetParam(chn_num, &param); */
    ret = interface->setParam(interface,&param);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "interface->setParam failed, ret=%d\n", ret);
		return -1;
	}
	return 0;
}
#endif
int main(int argc, char *argv[])
{
	int i, ret;
	IMPIVSInterface *interface = NULL;
	//IMP_IVS_MoveParam param;
	IMP_IVS_MoveOutput *result = NULL;
	IMPFrameInfo frame;
	unsigned char * g_sub_nv12_buf_move = 0;
	chn[0].enable = 0;
	chn[1].enable = 1;
	int sensor_sub_width = 640;
	int sensor_sub_height = 360;
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

	g_sub_nv12_buf_move = (unsigned char *)malloc(sensor_sub_width * sensor_sub_height * 3 / 2);
	if (g_sub_nv12_buf_move == 0) {
		printf("error(%s,%d): malloc buf failed \n", __func__, __LINE__);
		return -1;
	}
	/* Step.3 framesource Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}
	/* Step.4 ivs move start */
	ret = sample_ivs_move_start(0, 2, &interface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_move_start(0, 0) failed\n");
		return -1;
	}
	if(interface->init && ((ret = interface->init(interface)) < 0)) {
		IMP_LOG_ERR(TAG, "interface->init failed, ret=%d\n", ret);
		return -1;
	}
    ret = sample_ivs_set_sense(2,3,interface);
	/* Step.5 start to get ivs move result */
	for (i = 0; i < NR_FRAMES_TO_SAVE; i++) {

		ret = IMP_FrameSource_SnapFrame(1, PIX_FMT_NV12, sensor_sub_width, sensor_sub_height, g_sub_nv12_buf_move, &frame);
		if (ret < 0) {
			printf("%d get frame failed try again\n", 0);
			usleep(30*1000);
		}
		frame.virAddr = (unsigned int)g_sub_nv12_buf_move;
		if (interface->preProcessSync && ((ret = interface->preProcessSync(interface, &frame)) < 0)) {
			IMP_LOG_ERR(TAG, "interface->preProcessSync failed,ret=%d\n", ret);
			return -1;
		}
		if (interface->processAsync && ((ret = interface->processAsync(interface, &frame)) < 0)) {
			IMP_LOG_ERR(TAG, "interface->processAsync failed,ret=%d\n", ret);
			return -1;
		}
		if (interface->getResult && ((ret = interface->getResult(interface, (void **)&result)) < 0)) {
			IMP_LOG_ERR(TAG, "interface->getResult failed,ret=%d\n", ret);
			return -1;
		}
		IMP_LOG_INFO(TAG, "frame[%d], result->retRoi(%d,%d,%d,%d)\n", i, result->retRoi[0], result->retRoi[1], result->retRoi[2], result->retRoi[3]);
		//release moveresult
		if (interface->releaseResult && ((ret = interface->releaseResult(interface, (void *)result)) < 0)) {
		IMP_LOG_ERR(TAG, "interface->releaseResult failed ret=%d\n", ret);
			return -1;
		}
	}
	if(interface->exit < 0) {
		IMP_LOG_ERR(TAG, "interface->init failed, ret=%d\n", ret);
		return -1;
	}
	free(g_sub_nv12_buf_move);
	/* Step.6 ivs move stop */
	ret = sample_ivs_move_stop(2, interface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_move_stop(0) failed\n");
		return -1;
	}
	/* Step.7 Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}
	/* Step.8 FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}
	/* Step.9 System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}
	return 0;
}
