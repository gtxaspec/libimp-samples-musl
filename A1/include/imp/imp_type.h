/*
 * IMP type header file.
 *
 * Copyright (C) 2021 Ingenic Semiconductor Co.,Ltd
 */

#ifndef __IMP_TYPE_H__
#define __IMP_TYPE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/
typedef unsigned char           IMP_U8;
typedef unsigned short          IMP_U16;
typedef unsigned int            IMP_U32;
typedef float                   IMP_FLOAT;
typedef double                  IMP_DOUBLE;
typedef signed char             IMP_S8;
typedef short                   IMP_S16;
typedef int                     IMP_S32;

typedef unsigned long long  	IMP_U64;
typedef long long           	IMP_S64;
typedef char                    IMP_CHAR;
#define IMP_VOID                 void

#define IMP_NULL	NULL

typedef enum {
	IMP_FALSE = 0,
	IMP_TRUE  = 1,
} IMP_BOOL;

#define IMP_SUCCESS  0
#define IMP_FAILURE  (-1)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_TYPE_H__ */
