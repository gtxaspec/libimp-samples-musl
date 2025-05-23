/*
 * sample-Encoder-h264-IVS-move.c
 *
 * Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 */
#include <string.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_ivs.h>
#include <imp/imp_ivs_base_move.h>
#include "sample-common.h"
#define TAG "Sample-Encoder-h264-IVS-base-move"
#define SAD_MODE_SIZE 8  //sadMode为0时表示检测区域大小8*8，这个宏定义为8;
#define DEBUG_PRINT
extern struct chn_conf chn[];
/**
 * @defgroup IMP_IVS
 * @ingroup imp
 * @brief IVS智能分析通用接口API(以下调试内容均可在 Ingenic-SDK-T××/include_cn/imp/imp_ivs_base_move.h　下查看)
 * @section concept 1 相关概念
 * IMP IVS 通过IVS通用接口API调用实例化的IMPIVSInterface以将智能分析算法嵌入到SDK中来分析SDK中的frame图像。
 * @subsection IMPIVSInterface 1.1 IMPIVSInterface
 * IMPIVSInterface 为通用算法接口，具体算法通过实现此接口并将其传给IMP IVS达到在SDK中运行具体算法的目的。
 * 一个channel有且仅为单个算法实例的载体，必须将具体实现的通用算法接口传给具体的channel才能在SDK中运行算法。
 * IMPIVSInterface 成员param为成员函数init的参数。
 * IMP_IVS会在传给成员函数ProcessAsync参数的frame时对其进行外部加锁，ProcessAsync必须在使用完frame后调用IMP_IVS_ReleaseData释放frame，以免死锁。
 * @section ivs_usage 2 使用方法
 * 函数的具体实现见sample示例文件
 *
 * STEP.1 初始化系统，可以直接调用范例中的sample_system_init()函数。
 * 整个应用程序只能初始化系统一次，若之前初始化了，这儿不需要再初始化。
 * @code
 * ret = IMP_ISP_Open(); //打开isp模块
 * ret = IMP_ISP_EnableTuning();	// 使能翻转，调试图像
 *	ret = IMP_ISP_SetCameraInputMode(&mode) //如果有多个sensor(最大支持三摄),设置多摄的模式(单摄请忽略)
 *	ret = IMP_ISP_AddSensor(IMPVI_MAIN, &sensor_info[*]) //添加sensor,在此操作之前sensor驱动已经添加到内核 (IMPVI_MAIN为主摄, IMPVI_SEC为次摄, IMPVI_THR为第三摄)
 *	ret = IMP_ISP_EnableSensor(IMPVI_MAIN, &sensor_info[*])	//使能sensor, 现在sensor开始输出图像 (IMPVI_MAIN为主摄, IMPVI_SEC为次摄, IMPVI_THR为第三摄)
 * @endcode
 *
 * STEP.2 初始化framesource
 * 若算法所使用的framesource通道已创建，直接使用已经创建好的通道即可。
 * 若算法所使用的framesource通道未创建，可以调用范列中的sample_framesource_init(IVS_FS_CHN, &fs_chn_attr)进行创建。
 * @code
 * ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr);	//创建通道
 * ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr);	//设置通道属性
 * @endcode
 *
 * STEP.3 Encoder init编码初始化。
 * @code
 * ret = IMP_Encoder_CreateGroup(chn[i].index);	//创建编码group
 * ret = sample_encoder_init();		//编码初始化
 * @endcode
 *
 * STEP.4 创建ivs具体算法通道组。
 * 多个算法可以共用一个通道组，也可以分别使用通道组，具体件sample_ivs_move_init()
 * @code
 * int sample_ivs_move_init(int grp_num)
 * {
 *  	int ret = 0;
 *
 *		ret = IMP_IVS_CreateGroup(grp_num);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
 *			return -1;
 *		}
 *		return 0;
 * }
 * @endcode
 *
 * STEP.5 绑定算法通道组和framesource通道组
 * @code
 *	bind framesource channel.1-output.1 to ivs group
 *	fs.0 ----------------> encoder.0(main stream)
 *	fs.1 ----------------> ivs----> encoder.1(second stream)
 *
 *	IMPCell framesource_cell = {DEV_ID_FS, IVS_FS_CHN, 0};
 *	IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};
 *	ret = IMP_System_Bind(&framesource_cell, &ivs_cell);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and ivs0 failed\n", IVS_FS_CHN);
 *		return -1;
 *	}
 *	ret = IMP_System_Bind(&ivs_cell, &chn[i].imp_encoder);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",i);
 *		return -1;
 *	}
 * @endcode
 *
 * STEP.6 启动framesource。
 * @code
 *	IMP_FrameSource_SetFrameDepth(0, 0);	//设置最大图像深度，此接口用于设置某一通道缓存的视频图像帧数。当用户设置缓存多帧
 *	视频图像时,用户可以获取到一定数目的连续图像数据。若指定 depth 为 0,表示不需要系统为该通道缓存图像,故用户获取
 *	不到该通道图像数据。系统默认不为通道缓存图像,即 depth 默认为 0。
 *	ret = sample_framesource_streamon(IVS_FS_CHN);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
 *		return -1;
 *	}
 * @endcode
 *
 * STEP.7 启动算法。建议算法通道号和算法编号一致，以便可以直接对应当前操作哪一个算法。
 * @code
 *	ret = sample_ivs_move_start(0, 0, &inteface);
 *	if (ret < 0) {
 *		IMP_LOG_ERR(TAG, "sample_ivs_move_start(0, 0) failed\n");
 *		return -1;
 *	}
 * *interface = IMP_IVS_CreateBaseMoveInterface(&param);	//IMP_IVS_CreateBaseMoveInterface　算法接口
 *  ret = IMP_IVS_CreateChn(chn_num, *interface);	//创建通道
 *  ret = IMP_IVS_RegisterChn(grp_num, chn_num);	//注册通道
 *  ret = IMP_IVS_StartRecvPic(chn_num);	//开始接收图片
 * @endcode
 *
 * STEP.8 获取算法结果
 * Polling结果、获取结果和释放结果必须严格对应，不能中间有中断;
 * 只有Polling结果正确返回，获取到的结果才会被更新，否则获取到的结果无法预知。
 * @code
 *	for (i = 0; i < NR_FRAMES_TO_IVS; i++) {
 *		ret = IMP_IVS_PollingResult(0, IMP_IVS_DEFAULT_TIMEOUTMS);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_PollingResult(%d, %d) failed\n", 0, IMP_IVS_DEFAULT_TIMEOUTMS);
 *			return -1;
 *		}
 *		ret = IMP_IVS_GetResult(0, (void **)&result);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_GetResult(%d) failed\n", 0);
 *			return -1;
 *		}
 *		IMP_LOG_INFO(TAG, "frame[%d], result->ret=%d\n", i, result->ret);
 *
 *		ret = IMP_IVS_ReleaseResult(0, (void *)result);
 *		if (ret < 0) {
 *			IMP_LOG_ERR(TAG, "IMP_IVS_ReleaseResult(%d) failed\n", 0);
 *			return -1;
 *		}
 *	}
 * @endcode
 *
 * STEP.9  get h264 stream
 * @code
 * 获取h264的视频流,生成一个h264的视频文件
 * @encode
 *
 * STEP.10~17 释放资源,关于资源的释放请按照示例代码对应的顺序使用。
 * @code
 *　sample_ivs_move_get_result_stop(ivs_tid);
 *  sample_ivs_move_stop(2, inteface);
 *  sample_framesource_streamoff();
 *  IMP_System_UnBind(&framesource_cell, &ivs_cell);
 *  sample_ivs_move_exit(0);
 *  sample_framesource_exit(IVS_FS_CHN);
 *  sample_system_exit();
 * @endcode
 * @{
 */
static int sample_ivs_move_init(int grp_num)
{
	int ret = 0;
	ret = IMP_IVS_CreateGroup(grp_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
		return -1;
	}
	return 0;
}
static int sample_ivs_move_exit(int grp_num)
{
	int ret = 0;
	ret = IMP_IVS_DestroyGroup(grp_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_DestroyGroup(%d) failed\n", grp_num);
		return -1;
	}
	return 0;
}
static int sample_ivs_move_start(int grp_num, int chn_num, IMPIVSInterface **interface)
{
	int ret = 0;
	IMP_IVS_BaseMoveParam param;
	memset(&param, 0, sizeof(IMP_IVS_BaseMoveParam));
	param.skipFrameCnt = 3;
	param.referenceNum = 4;
	param.sadMode = 0;
	param.sense = 3;
	param.frameInfo.width = FIRST_SENSOR_WIDTH_SECOND;
	param.frameInfo.height = FIRST_SENSOR_HEIGHT_SECOND;
	*interface = IMP_IVS_CreateBaseMoveInterface(&param);
	if (*interface == NULL) {
		IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
		return -1;
	}
	ret = IMP_IVS_CreateChn(chn_num, *interface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_CreateChn(%d) failed\n", chn_num);
		return -1;
	}
	ret = IMP_IVS_RegisterChn(grp_num, chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_RegisterChn(%d, %d) failed\n", grp_num, chn_num);
		return -1;
	}
	ret = IMP_IVS_StartRecvPic(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_StartRecvPic(%d) failed\n", chn_num);
		return -1;
	}
	return 0;
}
static int sample_ivs_move_stop(int chn_num, IMPIVSInterface *interface)
{
	int ret = 0;
	ret = IMP_IVS_StopRecvPic(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_StopRecvPic(%d) failed\n", chn_num);
		return -1;
	}
	sleep(1);
	ret = IMP_IVS_UnRegisterChn(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_UnRegisterChn(%d) failed\n", chn_num);
		return -1;
	}
	ret = IMP_IVS_DestroyChn(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_DestroyChn(%d) failed\n", chn_num);
		return -1;
	}
	IMP_IVS_DestroyBaseMoveInterface(interface);
	return 0;
}
#if 0
static int sample_ivs_set_sense(int chn_num, int sensor)
{
	int ret = 0;
	IMP_IVS_MoveParam param;
	int i = 0;
	ret = IMP_IVS_GetParam(chn_num, &param);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_GetParam(%d) failed\n", chn_num);
		return -1;
	}
	for( i = 0 ; i < param.roiRectCnt ; i++){
	  param.sense[i] = sensor;
	}
	ret = IMP_IVS_SetParam(chn_num, &param);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_SetParam(%d) failed\n", chn_num);
		return -1;
	}
	return 0;
}
#endif
static void *sample_ivs_move_get_result_process(void *arg)
{
	int i = 0, j = 0, ret = 0;
	int chn_num = (int)arg;
	IMP_IVS_BaseMoveOutput *result = NULL;
	for (i = 0; i < NR_FRAMES_TO_SAVE; i++) {
		ret = IMP_IVS_PollingResult(chn_num, IMP_IVS_DEFAULT_TIMEOUTMS);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_IVS_PollingResult(%d, %d) failed\n", chn_num, IMP_IVS_DEFAULT_TIMEOUTMS);
			return (void *)-1;
		}
		ret = IMP_IVS_GetResult(chn_num, (void **)&result);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_IVS_GetResult(%d) failed\n", chn_num);
			return (void *)-1;
		}
#ifdef DEBUG_PRINT
		for(j = 0; j < result->datalen; j ++) {
			printf("%4d ",*(result->data + j));
			if(j%(FIRST_SENSOR_WIDTH_SECOND/SAD_MODE_SIZE) == 0) printf("\n");
		}
#endif
		ret = IMP_IVS_ReleaseResult(chn_num, (void *)result);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_IVS_ReleaseResult(%d) failed\n", chn_num);
			return (void *)-1;
		}
#if 0
		if (i % 20 == 0) {
			ret = sample_ivs_set_sense(chn_num, i % 5);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "sample_ivs_set_sense(%d, %d) failed\n", chn_num, i % 5);
				return (void *)-1;
			}
		}
#endif
	}
	return (void *)0;
}
static int sample_ivs_move_get_result_start(int chn_num, pthread_t *ptid)
{
	if (pthread_create(ptid, NULL, sample_ivs_move_get_result_process, (void *)chn_num) < 0) {
		IMP_LOG_ERR(TAG, "create sample_ivs_move_get_result_process failed\n");
		return -1;
	}
	return 0;
}
static int sample_ivs_move_get_result_stop(pthread_t tid)
{
	pthread_join(tid, NULL);
	return 0;
}
int main(int argc, char *argv[])
{
	int i, ret;
	pthread_t ivs_tid;
	IMPIVSInterface *inteface = NULL;
	chn[0].enable = 0;
	chn[1].enable = 1;
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
	ret = sample_encoder_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}
	/* Step.4 ivs init */
	ret = sample_ivs_move_init(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_move_init(0) failed\n");
		return -1;
	}
    /* step.5 bind */
	/* bind framesource channel.1-output.1 to ivs group */
	/**
	 * fs.0 ----------------> encoder.0(main stream)
	 * fs.1 ----------------> ivs----> encoder.1(second stream)
	 */
	IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};
	IMPCell fs_for_ivs_cell = {DEV_ID_FS, 2, 2};
    for (i = 0; i < FS_CHN_NUM; i++) {
        if (IVS_CHN_ID == i) {
            if (chn[i].enable) {
                ret = IMP_System_Bind(&chn[i].framesource_chn, &ivs_cell);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "Bind FrameSource channel.1 output.1 and ivs0 failed\n");
                    return -1;
                }
                ret = IMP_System_Bind(&ivs_cell, &chn[i].imp_encoder);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",i);
                    return -1;
                }
            }
        } else {
            if (chn[i].enable) {
                ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",i);
                    return -1;
                }
            }
        }
    }
    /* Step.6 framesource Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}
	/* Step.7 ivs move start */
	ret = sample_ivs_move_start(0, 2, &inteface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_move_start(0, 0) failed\n");
		return -1;
	}
	/* Step.8 start to get ivs move result */
	ret = sample_ivs_move_get_result_start(2, &ivs_tid);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_move_get_result_start failed\n");
		return -1;
	}
	/* Step.9 get h264 stream */
	ret = sample_get_video_stream();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get H264 stream failed\n");
		return -1;
	}
	/* Exit sequence as follow */
	/* Step.10 stop to get ivs move result */
	ret = sample_ivs_move_get_result_stop(ivs_tid);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_move_get_result_stop failed\n");
		return -1;
	}
	/* Step.11 ivs move stop */
	ret = sample_ivs_move_stop(2, inteface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_move_stop(0) failed\n");
		return -1;
	}
	/* Step.12 Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}
	/* Step.13 UnBind */
	ret = IMP_System_UnBind(&chn[IVS_CHN_ID].framesource_chn, &ivs_cell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and ivs0 failed\n", IVS_CHN_ID);
		return -1;
	}
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			if(IVS_CHN_ID == i) {
				ret = IMP_System_UnBind(&ivs_cell, &chn[i].imp_encoder);
				if (ret < 0) {
					IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
					return -1;
				}
			}else{
			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
			}
		}
	}
	/* Step.14 ivs exit */
	ret = sample_ivs_move_exit(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ivs mode exit failed\n");
		return -1;
	}
	/* Step.15 Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}
	/* Step.16 FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}
	/* Step.17 System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}
	return 0;
}
