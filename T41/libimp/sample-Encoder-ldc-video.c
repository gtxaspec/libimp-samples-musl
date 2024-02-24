/*
 * sample-Encoder-ldc-video.c
 * Please note that the version number of the ISP used in this sample is "H20230329a-LDC-Beta".
*/
#include <stdio.h>
#include <stdlib.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"
#include "ldclut.h"

#define TAG "Sample-Encoder-video"

extern struct chn_conf chn[];
static int byGetFd = 0;

int main(int argc, char *argv[])
{
	int i, ret;

    if (argc >= 2) {
        byGetFd = atoi(argv[1]);
    }

#if	1  //Method for using LDC on the main stream and sub-stream.
       //The "ldclut.h" header file contains LDC parameters for different resolutions, as the LDC module relies on these parameters for distortion correction. These parameters can be generated using dynamic debugging tools.
       //Currently, the sample uses a few sets of parameters for testing, as shown in the header file.
	IMPISPLDCInitAttr ldc;
	memset(&ldc, 0, sizeof(IMPISPLDCInitAttr));

	ldc.cattr[0].mode = IMPISP_OPS_MODE_ENABLE;
	ldc.cattr[0].enable = IMPISP_OPS_MODE_ENABLE;
	ldc.cattr[0].prioy = IMPISP_LDC_PRIOY_FIRST;
	memcpy(&ldc.cattr[0].params, &ldc_default_params[13], sizeof(IMPISPLDCParams));

    ldc.cattr[1].mode = IMPISP_OPS_MODE_ENABLE,
	ldc.cattr[1].enable = IMPISP_OPS_MODE_ENABLE;
	ldc.cattr[1].prioy = IMPISP_LDC_PRIOY_FIRST;
	memcpy(&ldc.cattr[1].params, &ldc_default_params[2], sizeof(IMPISPLDCParams));

	IMP_ISP_LDC_INIT(IMPVI_MAIN, &ldc); //Be sure to call it before IMP_ISP_Open. After the call is successful, a set of parameters in ldc_default_params[] will be passed down.
#endif

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

#if 1
	{
		//The IMP_ISP_LDC_INIT initialization can be further configured and accessed through the SET and GET interfaces for LDC-related parameters.
		IMPISPLDCAttr ldc;
		memset(&ldc, 0, sizeof(IMPISPLDCAttr));
		IMP_ISP_LDC_GetAttr(IMPVI_MAIN, &ldc);

		ldc.cattr[0].enable = IMPISP_OPS_MODE_ENABLE;
		ldc.cattr[0].prioy = IMPISP_LDC_PRIOY_FIRST;
		memcpy(&ldc.cattr[0].params, &ldc_default_params[11], sizeof(IMPISPLDCParams));

		ldc.cattr[1].enable = IMPISP_OPS_MODE_ENABLE;
		ldc.cattr[1].prioy = IMPISP_LDC_PRIOY_FIRST;
		memcpy(&ldc.cattr[1].params, &ldc_default_params[2], sizeof(IMPISPLDCParams));
		IMP_ISP_LDC_SetAttr(IMPVI_MAIN, &ldc);
	}
#endif

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

	/* Step.b UnBind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	/* Step.c Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	/* Step.d FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.e System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
