/*
 * sample-common.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "sample-common.h"

#define TAG "Sample-Common"

IMP_S32 sample_system_init(void)
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_SYS_CONF_S stSysConf;
	memset(&stSysConf, 0x0, sizeof(stSysConf));
	IMP_System_Exit();

	stSysConf.u32AlignWidth = 16;
	s32Ret = IMP_System_SetConf(&stSysConf);
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "IMP_System_SetConf failed\n");
		return s32Ret;
	}

	s32Ret = IMP_System_Init();
	if(s32Ret != IMP_SUCCESS){
		IMP_LOG_ERR(TAG, "IMP_System_Init failed\n");
		return s32Ret;
	}

	IMP_LOG_DBG(TAG, "ImpSystemInit success\n");

	return s32Ret;
}

IMP_S32 sample_system_exit(void)
{
	IMP_S32 s32Ret = IMP_SUCCESS;

	IMP_LOG_DBG(TAG, "sample_system_exit start\n");

	IMP_System_Exit();

	IMP_LOG_DBG(TAG, " sample_system_exit success\n");

	return s32Ret;
}
