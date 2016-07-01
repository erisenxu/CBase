/*
 * @(#) ShmMgr.h Created on 2008-03-27 共享内存创建与管理 - 将一块内存分成不同的部分，分配给不同的模块使用
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef SHM_MGR_H
#define SHM_MGR_H

#include <stdlib.h>
#include "comm/BaseDef.h"

#define MAX_SHM_MODULE_NAME_LEN     32
#define MAX_SHM_MODULE_NUM          256

/**
 * 给模块分配内存时调用的初始化函数，由各个模块提供
 * @param pstMem 分配给模块的内存地址
 * @param bFresh 是否是新创建，bFresh=1标识新创建，需要全新初始化，bFresh=0延续以前的设置，在reload时，如果isCanReLoad=0，会强制这个标记为0
 *               当chIsFresh=0时，一般是用在kill程序，重新设置本地的全局指针等
 */
typedef int (*LPFUNCSHMMODULEINITFUNC)(void* pstMem, U8 bFresh);

/**
 * 取模块的共享内存大小
 */
typedef size_t (*LPFUNCGETSHMMODULESIZE)();

/**
 * 共享内存每个模块的内存信息
 */
struct tagShmModuleInfo
{
    char szModuleName[MAX_SHM_MODULE_NAME_LEN];         /* 模块名，可用来检索模块 */
    size_t llBaseAddr;                                  /* 本模块内存相对于共享内存首地址的偏移 */
    size_t llSize;                                      /* 本模块共享内存大小 */
    U8 bReload;                                         /* 是否能动态更新，=1是，=0否，如果可以，将调用InitFunc再次初始化 */
    U8 bNeedMemset;                                     /* 是否需要在刚分配时memset，=1是，=0否 */
    LPFUNCGETSHMMODULESIZE getModuleShmSize;            /* 获取本模块占用共享内存大小的函数，若为NULL，则模块共享内存使用llSize指定的固定大小 */
    LPFUNCSHMMODULEINITFUNC initModule;                 /* 初始化函数，建议各应用在这个函数中必须自己保存一下首地址，避免调用get_module_baseaddr_by_name()来获取 */
};

typedef struct tagShmModuleInfo    SHMMODULEINFO;
typedef struct tagShmModuleInfo*   LPSHMMODULEINFO;

/**
 * 共享内存模块管理
 */
struct tagShmModuleMgr
{
    char szShmMgrName[MAX_SHM_MODULE_NAME_LEN];         /* 内存模块管理器名 */
    U16 nModuleNum;                                     /* 内存模块数量 */
    SHMMODULEINFO astModules[MAX_SHM_MODULE_NUM];       /* 内存模块数组 */
    char* pszShmAddr;                                   /* 共享内存首地址 */
};

typedef struct tagShmModuleMgr     SHMMODULEMGR;
typedef struct tagShmModuleMgr*    LPSHMMODULEMGR;

/**
 * 共享内存头部信息，用来记录共享内存的一些信息
 */
struct tagShmModuleMgrHead
{
    S8 chFlag;                                          /* 是否初始化标记，0未初始化，1初始化过了 */
    int iProcID;                                        /* 使用shmaddr的共享内存id，如果不匹配，无法连接上，防止多进程配置错了，公用相同的shm */
    size_t llSize;                                      /* 共享内存的大小 */
};

typedef struct tagShmModuleMgrHead     SHMMODULEMGRHEAD;
typedef struct tagShmModuleMgrHead*    LPSHMMODULEMGRHEAD;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 将各模块绑定到共享内存
 * @param pstModuleMgr 内存模块管理器对象
 * @param iProcID 程序id，一般可以用程序的msgq-id作为程序id
 * @param szShmPath 共享内存地址
 * @param chOnlyAttach chOnlyAttach=1，仅绑定到共享内存
 * @param bReload 是否动态更新(不停止进程更新内存数据)
 * @param pchFresh 若共享内存是新创建的，返回1，若是已存在的，返回0
 * @return 成功返回共享内存首地址，失败返回NULL
 */
char* shm_module_attach(LPSHMMODULEMGR pstModuleMgr, int iProcID, const char* szShmPath, char chOnlyAttach, U8 bReload, char* pchFresh);

/**
 * 初始化各内存模块的内存
 * @param pstModuleMgr 内存模块管理器对象
 * @param iProcID 程序id，一般可以用程序的msgq-id作为程序id
 * @param bFresh 是否是新创建，bFresh=1标识新创建，需要全新初始化，bFresh=0延续以前的设置
 * @param bReload 是否能动态更新(不停止进程更新内存数据)
 * @return 成功返回0，失败返回错误码
 */
int init_shm_modules(LPSHMMODULEMGR pstModuleMgr, int iProcID, U8 bFresh, U8 bReload);

/**
 * 根据模块名获得模块数据区首地址
 * @param pstModuleMgr 内存模块管理器对象
 * @param szModuleName 模块名称
 * @return 成功返回模块数据区首地址，失败返回NULL
 */
char* get_shm_module_addr_by_name(LPSHMMODULEMGR pstModuleMgr, const char* szModuleName);

#ifdef __cplusplus
}
#endif

#endif

