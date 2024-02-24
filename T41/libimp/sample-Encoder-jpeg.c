/*
 * sample-Encoder-jpeg.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 *
 * All API specific descriptions called in this file can be viewed in the header files under proj/sdk-lv3/include/api/cn/imp/ directory.
 *
 * Step.1 System init 
 *		@code
 *			memset(&sensor_info, 0, sizeof(sensor_info));
 *			if(SENSOR_NUM == IMPISP_TOTAL_ONE){
 *				memcpy(&sensor_info[0], &Def_Sensor_Info[0], sizeof(IMPSensorInfo));
 *			} else if(SENSOR_NUM == IMPISP_TOTAL_TWO){
 *				memcpy(&sensor_info[0], &Def_Sensor_Info[0], sizeof(IMPSensorInfo) * 2);
 *			}else if(SENSOR_NUM ==IMPISP_TOTAL_THR){
 *				memcpy(&sensor_info[0], &Def_Sensor_Info[0], sizeof(IMPSensorInfo) * 3)
 *			} //Copy the contents of the corresponding Def_Sensor_Info to sensor_info according to the number of sensors.
 *
 *			ret = IMP_ISP_Open() //Open the ISP module.
 *			ret = IMP_ISP_SetCameraInputMode(&mode) //If there are multiple sensors (up to three cameras), set the multi-camera mode (ignore for single camera).
 *			ret = IMP_ISP_AddSensor(IMPVI_MAIN, &sensor_info[*]) //Add the sensor. Before this operation, the sensor driver has been added to the kernel. (IMPVI_MAIN is the main camera, IMPVI_SEC is the secondary camera, IMPVI_THR is the third camera).
 *			ret = IMP_ISP_EnableSensor(IMPVI_MAIN, &sensor_info[*])	//Enable the sensor, and the sensor now starts outputting images. (IMPVI_MAIN is the main camera, IMPVI_SEC is the secondary camera, IMPVI_THR is the third camera).
 *			ret = IMP_System_Init() //Initialize the system.
 *			ret = IMP_ISP_EnableTuning() //Enable ISP tuning, and then you can call the ISP debugging interface.
 *		@endcode
 * Step.2 FrameSource init Framesource Initialization
 *		@code
 *			ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr) //Create the channel.
 *			ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr) //Set the relevant attributes of the channel, including: image width, image height, image format, channel output frame rate, cache buffer number, cropping and scaling properties.
 *		@endcode
 * Step.3 Encoder initialization:
 *		@code
 *			ret = IMP_Encoder_CreateGroup(chn[i].index) //Create the encoding group.
 *			ret = IMP_Encoder_SetDefaultParam(&channel_attr, IMP_ENC_PROFILE_JPEG, IMP_ENC_RC_MODE_FIXQP, imp_chn_attr_tmp->picWidth, imp_chn_attr_tmp->picHeight,
 *											imp_chn_attr_tmp->outFrmRateNum, imp_chn_attr_tmp->outFrmRateDen, 0, 0, 25, 0) //Set the default properties of the encoding channel.
 *			ret = IMP_Encoder_CreateChn(4 + chn[i].index, &channel_attr) //Create the encoding channel.
 *			ret = IMP_Encoder_RegisterChn(i, 4 + chn[i].index) //Register the encoding channel to the group.
 *		@endcode
 * Step.4 Bind the FrameSource channel and the encoding channel:
 *		@code
 *			ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder)	//Bind the FrameSource channel and the encoding channel. When the binding is successful, the data produced by the FrameSource can be automatically transmitted to the encoding channel.
 *		@endcode
 * Step.5 Stream On Enable the FrameSource channel and start outputting images:
 *		@code
 *			ret = IMP_FrameSource_EnableChn(chn[i].index) //Enable the channel, and the channel starts outputting images.
 *		@endcode
 * Step.6 Get Snap Get the JPEG encoded image:
 *		@code
 *			ret = IMP_Encoder_StartRecvPic(4 + chn[i].index) //The encoding channel starts receiving images.
 *			ret = IMP_Encoder_PollingStream(4 + chn[i].index, 1000) //Poll the stream in the encoding channel.
 *			ret = IMP_Encoder_GetStream(4 + chn[i].index, &stream, 1) //Get the encoded stream.
 *			ret = save_stream(snap_fd, &stream) //Save the result to the opened file descriptor.
 *			ret = IMP_Encoder_ReleaseStream(4 + chn[i].index, &stream) //Release the encoded stream, which is used in pairs with IMP_Encoder_GetStream.
 *			ret = IMP_Encoder_StopRecvPic(4 + chn[i].index) //Stop receiving images in the encoding channel.
 *		@endcode
 * Step.7 Stream Off: Disable the FrameSource channel and stop outputting images:
 *		@code
 *			ret = IMP_FrameSource_DisableChn(chn[i].index) //Disable the channel, and the channel stops outputting images.
 *		@endcode
 * Step.8 UnBind: Unbind the FrameSource and the encoding channel:
 *		@code
 *			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder) //Unbind the FrameSource and the encoding channel.
 *		@endcode
 * Step.9 Encoder exit: Deinitialize the encoder:
 *		@code
 *			ret = IMP_Encoder_Query(chnNum, &chn_stat) //Query the encoding channel status.
 *			ret = IMP_Encoder_UnRegisterChn(chnNum) //If the encoding channel has been registered, unregister it from the group.
 *			ret = IMP_Encoder_DestroyChn(chnNum) //Destroy the encoding channel.
 *		@endcode
 * Step.10 FrameSource exit: Deinitialize the FrameSource:
 *		@code
 *			ret = IMP_FrameSource_DestroyChn(chn[i].index) //Destroy the channel
 *		@endcode
 * Step.11 System exit: Deinitialize the system:
 *		@code
 *			ret = IMP_ISP_DisableTuning() // Disable ISP tuning.
 *			ret = IMP_System_Exit() //Deinitialize the system.
 *			ret = IMP_ISP_DisableSensor(IMPVI_MAIN, &sensor_info[*]) // Disable the sensor, and the sensor stops outputting images. (IMPVI_MAIN is the main camera, IMPVI_SEC is the secondary camera, IMPVI_THR is the third camera).
 *			ret = IMP_ISP_DelSensor(IMPVI_MAIN, &sensor_info[*]) //Delete the sensor. (IMPVI_MAIN is the main camera, IMPVI_SEC is the secondary camera, IMPVI_THR is the third camera).
 *			ret = IMP_ISP_Close() //Close the ISP module.
 *		@endcode
 * */

#include <stdio.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Encoder-jpeg"

extern struct chn_conf chn[];

int main(int argc, char *argv[])
{
	int i, ret;

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
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", i);
				return -1;
			}
		}
	}

	ret = sample_jpeg_init();
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

	/* Step.5 Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

	/* Step.6 Get Snap */
	ret = sample_get_jpeg_snap(NR_FRAMES_TO_SAVE / 10);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get H264 stream failed\n");
		return -1;
	}
	/* drop several pictures of invalid data */
	/* sleep(SLEEP_TIME); */
	/* Exit sequence as follow... */
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
	ret = sample_jpeg_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder jpeg exit failed\n");
		return -1;
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
