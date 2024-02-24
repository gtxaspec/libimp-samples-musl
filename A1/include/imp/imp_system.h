/*
 * IMP System header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_SYSTEM_H__
#define __IMP_SYSTEM_H__

#include "imp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * IMP系统模块头文件
 */

/**
 * @defgroup imp IMP(Ingenic Media Platform)
 */

/**
 * @defgroup IMP_System
 * @ingroup imp
 * @brief 系统控制模块，包括IMP的基础功能以及模块间绑定的相关功能
 * @section concept 1 相关概念
 * 系统控制主要实现连接各模块，定义数据流的功能。以下是一些重要概念：
 *
 * @{
 */

/**
 * IMP系统版本号定义.
 */
typedef struct {
	char cVersion[64];	/**< IMP系统版本号 */
} IMP_VERSION_S;

#define IMP_MAX_SCALER_TABLE_NUM 16
/*
 * IMP系统静态参数，必须在系统初始化之前设置。
 * u32AlignWidth 为整个系统中使用图像的stride字节对齐数，必须是16的整数倍;系统默认是16.
* u32Scaler 为系统中缩放模块预先设置缩放因子，每个因子对应一组缩放曲线常数表. 如果使用系默认值，请把第一个因子设为0
 * */
typedef struct {
	/*
	 *  stride of picture buffer must be aligned with this value.
	 *	it must be multiple of 16.
	 * */
	IMP_U32	u32AlignWidth;
    /*
     * SDK中缩放因子表,每个因子对应一张缩放参数表，缩放表会占用rmem内存.
     * 参数意思：(srcsize * 10 ) / dstsize； 0 表示结束.SDK会根据实际的缩放倍数选择最合适的参数表.
     * {3,5,7,9,11,12,0}
     * */
    IMP_U32 u32Scaler[IMP_MAX_SCALER_TABLE_NUM];
} IMP_SYS_CONF_S;

/**
 * @fn IMP_S32 IMP_System_SetConf(const IMP_SYS_CONF_S *pstSysConf)
 *
 * 设置IMP系统静态属性.
 *
 * @param[in] pstSysConf 系统静态参数结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 此API调用后会设置系统静态参数.
 *
 * @attention 在IMP的任何操作之前必须在系统初始化之前调用，否则返回错误.
 */
IMP_S32 IMP_System_SetConf(const IMP_SYS_CONF_S *pstSysConf);

/**
 * @fn IMP_S32 IMP_System_GetConf(IMP_SYS_CONF_S *pstSysConf)
 *
 * 获取IMP系统静态属性.
 *
 * @param[in] pstSysConf 系统静态参数结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 获取系统静态参数.
 *
 * @attention
 */
IMP_S32 IMP_System_GetConf(IMP_SYS_CONF_S *pstSysConf);


/**
 * @fn int IMP_System_Init(void)
 *
 * IMP系统初始化.
 *
 * @param 无.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 此API调用后会初始化基础的数据结构.
 *
 * @attention 在IMP的任何操作之前必须先调用此接口进行初始化.
 */
IMP_S32 IMP_System_Init(void);

/**
 * @fn int IMP_System_Exit(void)
 *
 * IMP系统去初始化.
 *
 * @param 无.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 此函数调用后会释放IMP所有的内存以及句柄，并关闭硬件单元.
 *
 * @attention 在调用此API后，若要再次使用IMP则需重新进行IMP系统初始化.
 */
IMP_S32 IMP_System_Exit(void);

/**
 * @fn int IMP_System_Bind(IMP_CELL_S *srcCell, IMP_CELL_S *dstCell)
 *
 * 绑定源Cell和目的Cell.
 *
 * @param[in] srcCell 源Cell指针.
 * @param[in] dstCell 目的Cell指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 根据module， device和channel概念，同一个硬件模块可以虚拟出多个分时复用
 * 设备，每个虚拟设备包括多个通道。绑定是通道和通道之间关系.
 * @remarks 绑定关系成功后，源Cell(Output)产生的数据会自动传送到目的Cell.
 *
 * @attention 无。
 */
IMP_S32 IMP_System_Bind(IMP_CELL_S *srcCell, IMP_CELL_S *dstCell);

/**
 * @fn int IMP_System_UnBind(IMP_CELL_S *srcCell, IMP_CELL_S *dstCell)
 *
 * 解除源Cell和目的Cell的绑定.
 *
 * @param[in] srcCell 源Cell指针.
 * @param[in] dstCell 目的Cell指针.
 *
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无。
 */
IMP_S32 IMP_System_UnBind(IMP_CELL_S *srcCell, IMP_CELL_S *dstCell);

/**
 * @fn int IMP_System_GetBindbyDest(IMP_CELL_S *dstCell, IMP_CELL_S *srcCell)
 *
 * 获取绑定在目的Cell的源Cell信息.
 *
 * @param[in] dstCell 目的Cell指针.
 * @param[out] srcCell 源Cell指针.
 *
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无。
 */
IMP_S32 IMP_System_GetBindbyDest(IMP_CELL_S *dstCell, IMP_CELL_S *srcCell);

/**
 * @fn int IMP_System_GetVersion(IMP_VERSION_S *pstVersion)
 *
 * 获取IMP系统版本号.
 *
 * @param[out] pstVersion IMP系统版本号结构体指针.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 无.
 */
IMP_S32 IMP_System_GetVersion(IMP_VERSION_S *pstVersion);


/**
 * @fn IMP_S32 IMP_System_GetCurPts(IMP_U64 *pu64CurPts)
 *
 * 获得IMP系统的当前时间戳，单位为微秒。
 *
 * @param[in] pu64CurPts 时间戳指针
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 系统初始化后时间戳自动被初始化。系统去初始化后时间戳失效。
 *
 * @attention 无。
 */
IMP_S32 IMP_System_GetCurPts(IMP_U64 *pu64CurPts);

/**
 * @fn IMP_S32 IMP_System_InitPtsBase(IMP_U64 u64Ptsbase)
 *
 * 设置IMP系统的时间戳，单位为微秒。
 *
 * @param[in] u64Ptsbase 基础时间。
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无。
 *
 * @attention 会强制修改系统时间戳,建议在媒体业务没有启动时候调用.
 */
IMP_S32 IMP_System_InitPtsBase(IMP_U64 u64Ptsbase);

/**
 * @fn IMP_S32 IMP_System_SyncPts(IMP_U64 u64Ptsbase)
 *
 * 设置IMP系统的时间戳，单位为微秒。
 *
 * @param[in] u64Ptsbase 基础时间。
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无。
 *
 * @attention 该函数只会微调系统时间戳，不会导致系统时间戳出现回退现象.
 */
IMP_S32 IMP_System_SyncPts(IMP_U64 u64Ptsbase);

/**
 * @fn uint32_t IMP_System_ReadReg32(uint32_t u32Addr)
 *
 * 读取32位寄存器的值。
 *
 * @param[in] regAddr 寄存器的物理地址。
 *
 * @retval 寄存器的值（32位）
 *
 * @remarks 无。
 *
 * @attention 无。
 */
IMP_U32 IMP_System_ReadReg32(IMP_U32 regAddr);

/**
 * @fn void IMP_System_WriteReg32(uint32_t regAddr, uint32_t value)
 *
 * 向32位寄存器中写值。
 *
 * @param[in] regAddr 寄存器的物理地址。
 * @param[in] value 要写入的值。
 *
 * @retval 无
 *
 * @remarks 无。
 *
 * @attention 在不明确寄存器的含义之前请谨慎调用此API，否则可能会导致系统错误。
 */
void IMP_System_WriteReg32(IMP_U32 regAddr, IMP_U32 value);

/**
 * @fn const char* IMP_System_GetCPUInfo(void)
 *
 * 获取CPU型号信息.
 *
 * @param 无.
 *
 * @retval CPU型号字符串.
 *
 * @remarks 返回值是CPU型号类型的字符串,例如"A1-N"及"A1-X".
 *
 * @attention 无.
 */
const IMP_CHAR* IMP_System_GetCPUInfo(IMP_VOID);

/**
 * @fn IMP_S32 IMP_System_GetChipId(IMP_U32 *pu32ChipId)
 *
 * 获取CPU CHIP ID信息,该信息长度96bit.
 *
 * @param[in] pu32ChipId CPUID存放地址，该地址空间最少96bit.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 芯片ID 为96bit数据，所以参数空间必须最少12bytes，否则会出现冲内存问题.
 */
IMP_S32 IMP_System_GetChipId(IMP_U32 *pu32ChipId);

/**
 * @fn IMP_S32 IMP_System_CloseFd(IMP_VOID)
 *
 * 关闭所有SDK打开的日志，系统和内存的Fd
 *
 * @param 无.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks 无.
 *
 * @attention 该接口建议在所有模块被禁用后调用.
 */
IMP_S32 IMP_System_CloseFd(IMP_VOID);


/**
 * @fn IMP_S32 IMP_System_CreatPool(IMP_U32 *pu32PoolId, IMP_U32 u32BlkSize, IMP_U32 u32BlkCnt, const char *pcPoolName);
 *
 * 在Rmem上创建mempool
 *
 * @param[in] pu32PoolId 	申请的缓存池ID存放地址.
 * @param[in] u32BlkSize	申请缓存池中每个缓存块大小,单位 bytes.
 * @param[in] u32BlkSize	申请缓存池中缓存块的个数.
 * @param[in] pcPoolName	缓存池名字,不能为空.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks
 * 视频内存池主要向媒体业务提供大块物理连续内存管理功能,负责内存的分配和回收,
 * 充分发挥内存缓存池的作用,让物理内存资源在各个媒体处理模块中合理使用.
 * 视频内存池建立在原保留内存RMEM（不由内核建立页表）的基础上进行大块内存管理,
 * 每申请一个内存池都是物理连续的内存，之后在该内存池上申请内存同样是申请物理连续的内存.
 * 若使用内存池，必须在系统初始化之前配置内存池大小，根据业务的不同,
 * 内存池申请的大小和数量不同。
 *
 * @attention pcPoolName 不能为空，建议所有pool名字不能重复.
 */
IMP_S32 IMP_System_CreatPool(IMP_U32 *pu32PoolId, IMP_U32 u32BlkSize, IMP_U32 u32BlkCnt, const char *pcPoolName);

/**
 * @fn IMP_S32 IMP_System_CreatPool_Align(IMP_U32 *pu32PoolId, IMP_U32 u32BlkSize, IMP_U32 u32BlkCnt, const char *pcPoolName,IMP_S32 s32AlignWidth);
 *
 * 在Rmem上创建mempool
 *
 * @param[in] pu32PoolId 	申请的缓存池ID存放地址.
 * @param[in] u32BlkSize	申请缓存池中每个缓存块大小,单位 bytes.
 * @param[in] u32BlkSize	申请缓存池中缓存块的个数.
 * @param[in] pcPoolName	缓存池名字,不能为空.
 * @param[in] s32AlignWidth	内存池字节对齐数.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks
 * 视频内存池主要向媒体业务提供大块物理连续内存管理功能,负责内存的分配和回收,
 * 充分发挥内存缓存池的作用,让物理内存资源在各个媒体处理模块中合理使用.
 * 视频内存池建立在原保留内存RMEM（不由内核建立页表）的基础上进行大块内存管理,
 * 每申请一个内存池都是物理连续的内存，之后在该内存池上申请内存同样是申请物理连续的内存.
 * 若使用内存池，必须在系统初始化之前配置内存池大小，根据业务的不同,
 * 内存池申请的大小和数量不同。
 *
 * @attention pcPoolName 不能为空，建议所有pool名字不能重复.
 */
IMP_S32 IMP_System_CreatPool_Align(IMP_U32 *pu32PoolId, IMP_U32 u32BlkSize, IMP_U32 u32BlkCnt, const char *pcPoolName,IMP_S32 s32AlignWidth);

/**
 * @fn IMP_S32 IMP_System_DestroyPool(IMP_U32 u32PoolId, const char *pcPoolName);
 *
 * 销毁Rmem上对应的 mempool
 *
 * @param[in] u32PoolId 	需要销毁的poolid，优先进行匹配，如果匹配成功，则忽略pcName.
 * @param[in] pcPoolName	需要销毁的pool名称, 在u32PoolId 无效时，进行匹配.
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks
 * 销毁创建的mempool，优先进行u32PoolId进行匹配，如果匹配成功，则直接销毁；忽略pcPoolName.
 * 如果u32PoolId 匹配失败，则使用pcPoolName进行匹配，匹配成功，则销毁。
 * memepool 销毁以后，从其内部申请的block都将释放，如果在违规使用会出现无法预知错误.
 *
 * @attention 确保u32PoolId有效的前提下，pcPoolName 可以设置为NULL. 建议在释放所有block以后调用.
 */
IMP_S32 IMP_System_DestroyPool(IMP_U32 u32PoolId, const char *pcPoolName);

/**
 * @fn IMP_VOID *IMP_System_GetBlock(IMP_U32 u32PoolId, const char *pcPoolName, IMP_U32 u32Size, const char *pcBlkName);
 *
 * 从对应u32PoolId 或 pcPoolName中获取block对应地址空间
 *
 * @param[in] u32PoolId 	需要指定poolid，优先进行匹配，如果匹配成功，则忽略pciPoolName.
 * @param[in] pcName		pool的名称, 在u32PoolId 无效时，进行匹配，如果匹配成功，则从对应的pool中分配block.
 * @param[in] Size		 	申请block大小.
 * @param[in] pcBlkName		申请block名字，不能为空.
 *
 *
 * @retval 成功返回正确block空间虚拟起始地址
 * @retval 失败返回 IMP_NULL
 *
 * @remarks
 *
 * @attention 确保u32PoolId有效的前提下，pcName 可以设置为NULL.
 */
IMP_VOID *IMP_System_GetBlock(IMP_U32 u32PoolId, const char *pcPoolName, IMP_U32 u32Size, const char *pcBlkName);

/**
 * @fn IMP_S32 IMP_System_ReleaseBlock(IMP_U32 u32PoolId, IMP_VOID *pBlockVaddr);
 *
 * 从对应u32PoolId中释放对应虚拟起始地址的block空间.
 *
 * @param[in] u32PoolId 	需要指定poolid.
 * @param[in] pBlockVaddr	需要释放的blcok虚拟地址，不能为空.
 *
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks
 *
 * @attention 无.
 */
IMP_S32 IMP_System_ReleaseBlock(IMP_U32 u32PoolId, IMP_VOID *pBlockVaddr);

/**
 * @fn IMP_U32 IMP_System_Block2PhyAddr(IMP_VOID *pBlockVaddr);
 *
 * 用户态获取一个缓存块的物理地址
 *
 * @param[in] pBlockVaddr	需要转换的blcok虚拟地址
 *
 *
 * @retval 成功返回正确block空间物理地址
 * @retval 失败返回0
 *
 * @remarks
 *
 * @attention 无.
 */
IMP_U32 IMP_System_Block2PhyAddr(IMP_VOID *pBlockVaddr);

/**
 * @fn IMP_U32 IMP_System_Block2PoolId(IMP_VOID *pBlockVaddr);
 *
 * 用户态获取一个缓存块的所在缓存池的ID
 *
 * @param[in] pBlockVaddr	缓存块的虚拟地址
 *
 *
 * @retval 成功返回正确缓存池ID
 * @retval 失败返回0
 *
 * @remarks
 *
 * @attention 无.
 */
IMP_U32 IMP_System_Block2PoolId(IMP_VOID *pBlockVaddr);

/**
 * @fn IMP_S32 IMP_System_FlushCache(IMP_VOID *pBlockVaddr, IMP_U32 u32Size);
 *
 * 用户刷新cache里的内容到内存并使cache里的内容无效.
 *
 * @param[in] pBlockVaddr	缓存块的虚拟地址
 * @param[in] u32Size		缓存块的大小
 *
 *
 * @retval IMP_SUCCESS 成功
 * @retval 非IMP_SUCCESS 失败，具体原因参见错误码说明.
 *
 * @remarks
 *
 * @attention 无.
 */
IMP_S32 IMP_System_FlushCache(IMP_VOID *pBlockVaddr, IMP_U32 u32Size);

/**
 * @fn IMP_CHAR *IMP_System_PixfmtToString(PIXEL_FORMAT_E Pixfmt);
 *
 * 获取对应图像格式的字符串表示.
 *
 * @param[in] Pixfmt	图像格式
 *
 *
 * @retval 成功返回字符串起始地址
 * @retval 失败返回 IMP_NULL
 *
 * @remarks
 *
 * @attention 无.
 */
IMP_CHAR *IMP_System_PixfmtToString(PIXEL_FORMAT_E Pixfmt);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_SYSTEM_H__ */
