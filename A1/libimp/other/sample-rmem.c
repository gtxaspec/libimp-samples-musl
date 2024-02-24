#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_log.h>

#define TAG "sample-rmem"

/*
   sample-rmem是用户使用媒体内存的方法。
 */
int main(int argc,char *argv[])
{
	IMP_S32 s32Ret = IMP_SUCCESS;
	IMP_U32 u32PoolId = 0;
	IMP_U32 u32BlockSize = 0;
	IMP_U32 u32BlockNum = 0;
	IMP_VOID *pBlockVirAddr = NULL;
	IMP_U32 u32BlockPhyAddr = 0;
	char s8PoolName[16] = {0};
	char s8BlockName[16] = {0};
	IMP_SYS_CONF_S stSysConf;

    /************************************************
    step1:  init system
    *************************************************/
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

    /************************************************
    step2:  get rmem
    *************************************************/
	u32BlockSize = 1920 * 1080 * 3 / 2;
	u32BlockNum = 1;
	sprintf(s8PoolName, "test-pool");
	s32Ret = IMP_System_CreatPool(&u32PoolId, u32BlockSize, u32BlockNum, s8PoolName);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_CreatPool failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	sprintf(s8BlockName, "test-block");
	pBlockVirAddr = IMP_System_GetBlock(u32PoolId, s8PoolName, u32BlockSize, s8BlockName);
	if (!pBlockVirAddr) {
		IMP_LOG_ERR(TAG, "IMP_System_GetBlock failed!\n");
		return -1;
	}

	memset(pBlockVirAddr, 0x0, u32BlockSize);

	u32BlockPhyAddr = IMP_System_Block2PhyAddr(pBlockVirAddr);
	if (!u32BlockPhyAddr) {
		IMP_LOG_ERR(TAG, "IMP_System_Block2PhyAddr failed!\n");
		return -1;
	}

    /************************************************
    step3:  release rmem
    *************************************************/
	s32Ret = IMP_System_ReleaseBlock(u32PoolId, pBlockVirAddr);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_ReleaseBlock failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = IMP_System_DestroyPool(u32PoolId, s8PoolName);
	if (s32Ret != IMP_SUCCESS) {
		IMP_LOG_ERR(TAG, "IMP_System_DestroyPool failed! ret = 0x%08x\n", s32Ret);
		return s32Ret;
	}

    /************************************************
    step4:  system exit
    *************************************************/
	IMP_System_Exit();
	return s32Ret;
}
