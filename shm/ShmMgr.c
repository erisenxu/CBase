/*
 * @(#) ShmMgr.c Created on 2008-03-27 共享内存创建与管理 - 将一块内存分成不同的部分，分配给不同的模块使用
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "ShmMgr.h"
#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"
#include "comm/FileLib.h"
#include "log/Logger.h"

#include <string.h> /* 使用strerror必须加上这个头文件，否则会导致崩溃，:( */
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/**
 * 初始化内存模块的数量
 * @param pstModuleMgr 内存模块管理器对象
 */
static void init_shm_module_num(LPSHMMODULEMGR pstModuleMgr)
{
    int i = 0;

    if (NULL == pstModuleMgr) return;

    for (i = 0; i < MAX_SHM_MODULE_NUM; i++)
    {
        if (pstModuleMgr->astModules[i].szModuleName[0] == 0)
        {
            break;
        }
    }
    pstModuleMgr->nModuleNum = i;
    return;
}

/**
 * 检查内存模块中是否存在重复的模块名
 * @param pstModuleMgr 内存模块管理器对象
 * @return 若不存在重复的模块名，返回0，否则，返回错误码
 */
static int check_shm_module_name(LPSHMMODULEMGR pstModuleMgr)
{
    int i, j;

    if (NULL == pstModuleMgr) return ERROR_INPUT_PARAM_NULL;

    for (i = 0; i < pstModuleMgr->nModuleNum - 1; ++i)
    {
        for(j = i + 1; j < pstModuleMgr->nModuleNum; ++j)
        {
            if(!strcasecmp(pstModuleMgr->astModules[i].szModuleName, pstModuleMgr->astModules[j].szModuleName))
            {
                LOG_WARN("%s: SHM module name are duplicate on [%s] .", __FUNCTION__, pstModuleMgr->astModules[i].szModuleName);
                return ERROR_SHM_MODULE_NAME_REPEAT;
            }
        }
    }
    return 0;
}

#if 0
/**
 * 获取shm调用的错误码描述，其实可以使用strerror(errno)，但是系统描述不清晰
 */
static char* shm_errno(int iErr)
{
    switch(iErr)
    {
    case EACCES: /*13*/
        return "Shm exists, but operation permission limit";
    case EEXIST: /*17*/
        return "Shm exists, but IPC_CREAT and IPC_EXCL are both set";
    case EINVAL: /*22*/
        return "Shm is to be created and Size is Invalid, or Shm exist and Size is Less than input";
    case ENOENT: /*2*/
        return "Shm does not exist, and IPC_CREAT is 0";
    case ENOSPC: /*28*/
        return "Shm is to be created, but system limit";
    default:
        return "Unkonw\0";
    }
}
#endif

/**
 * 获取共享内存
 * @param iKey 共享内存key
 * @param iSize 共享内存大小
 * @param iFlag 创建共享内存flag
 * @return 成功返回获取的共享内存地址，失败返回NULL
 */
static void* get_shm(int iKey,  size_t llSize, int iFlag)
{
    int iShm;
    void *pvShm;

    /* 连接或创建共享内存 */
    iShm = shmget(iKey, llSize, iFlag);
    if (iShm < 0)
    {
        LOG_WARN("shmget fail[eno=%d,%s]", errno, strerror(errno));
        return NULL;
    }

    pvShm = shmat(iShm, NULL, 0);
    if ((void*)-1 == pvShm)
    {
        LOG_WARN("shmat fail[eno=%d,%s]", errno, strerror(errno));
        return NULL;
    }

    return pvShm;
}

/**
 * 根据路径名获取共享内存地址，如果内存已存在，则返回已存在的内存地址；如果不存在且Flag设置了IPC_CREAT标志，则试图创建共享内存
 * @param szPath 路径名
 * @param size 共享内存大小
 * @param iFlag Flag
 * @param pchFresh 若共享内存是新建的，返回1，已经存在的内存返回0
 * @return 成功返回获得的内存地址，失败返回NULL
 */
static void* get_shm_by_path(const char *szPath,  size_t llSize, int iFlag, char* pchFresh)
{
    int iKey;
    void* pvShm;

    iKey = file_to_key(szPath);
    if (iKey < 0)
    {
        LOG_ERROR("%s:path_to_key fail", __FUNCTION__);
        return NULL;
    }

    LOG_WARN("%s: Key is [%d]", __FUNCTION__, iKey);

    pvShm = get_shm(iKey, llSize, iFlag & (~IPC_CREAT));
    if (!pvShm)
    {
        if (iFlag & IPC_CREAT)
        {
            pvShm = get_shm(iKey, llSize, iFlag);
            if (!pvShm)
            {
                LOG_ERROR("%s:get_shm fail with IPC_CREAT", __FUNCTION__);
                return NULL;
            }
            if (pchFresh) *pchFresh = 1;
            return pvShm;
        }
        LOG_ERROR("%s:get_shm fail without IPC_CREAT", __FUNCTION__);
        return NULL;
    }

    if (pchFresh) *pchFresh = 0;
    return pvShm;
}

/**
 * 活动内存模块占用内存大小
 * @param pstModule 内存模块
 * @return 返回内存模块占用大小
 */
static size_t get_shm_module_size(LPSHMMODULEINFO pstModule)
{
    size_t llSize;
    if (NULL == pstModule) return 0;

    llSize = (NULL == pstModule->getModuleShmSize) ? pstModule->llSize : pstModule->getModuleShmSize();

    return llSize < 0 ? 0 : llSize;
}

//共享内存的结构划分: 上次是否成功标记 + 总大小 + {模块名 + 实际数据} * Entries的数量
/**
 * 计算共享内存的总大小
 * @param pstModuleMgr 内存模块管理器对象
 * @return 返回内存总大小
 */
static size_t get_shm_module_total_size(LPSHMMODULEMGR pstModuleMgr)
{
    size_t llSize;
    int i;

    if (NULL == pstModuleMgr) return 0;

    llSize = sizeof(SHMMODULEMGRHEAD); /* 头信息的长度 */

    if (pstModuleMgr->nModuleNum < 0)
    {
        return llSize;
    }

    llSize += (pstModuleMgr->nModuleNum * MAX_SHM_MODULE_NAME_LEN);  /* 每个共享内存entry的开头32个字节时modulename，用来做校验用 */

    for (i = 0; i < pstModuleMgr->nModuleNum; i++)
    {
        llSize += get_shm_module_size(&(pstModuleMgr->astModules[i])); /* 模块大小 */
    }

    return llSize;
}

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
char* shm_module_attach(LPSHMMODULEMGR pstModuleMgr, int iProcID, const char* szShmPath, char chOnlyAttach, U8 bReload, char* pchFresh)
{
    int iRet;
    size_t llShmSize;
    char* pszMemAddr;
    char chFresh;

    if (NULL == pstModuleMgr || NULL == szShmPath) return NULL;

    /* 1. 初始化内存模块个数 */
    init_shm_module_num(pstModuleMgr);

    /* 2. 检查内存模块是否存在重复的模块名 */
    iRet = check_shm_module_name(pstModuleMgr);

    if (iRet != 0)
    {
        LOG_WARN("%s: SHM module name are duplicated:%s.", __FUNCTION__, szShmPath);
        return NULL;
    }

    /* 3. 计算共享内存总大小 */
    llShmSize = get_shm_module_total_size(pstModuleMgr);
    LOG_WARN("%s: try to attach or create shm, path:%s, total size:%zu.", __FUNCTION__, szShmPath, llShmSize);

    /* 4. 申请共享内存 */
    pszMemAddr = get_shm_by_path(szShmPath, llShmSize, 0664 | IPC_CREAT, &chFresh);

    if (NULL == pszMemAddr)
    {
        LOG_ERROR("%s: failed to attach shm:%s.", __FUNCTION__, szShmPath);
        return pszMemAddr;
    }

    /* 5. 保存共享内存首地址 */
    pstModuleMgr->pszShmAddr = pszMemAddr;

    /* 6. 重要的信息打印下 */
    LOG_WARN("%s: success attach to shm, totalsize=%zu, chFresh=%d.", __FUNCTION__, llShmSize, chFresh);

    if (pchFresh != NULL) *pchFresh = chFresh;

    /* 7. 初始化各模块内存 */
    if (0 == chOnlyAttach)
    {
        iRet = init_shm_modules(pstModuleMgr, iProcID, chFresh, bReload);
        if (iRet != 0)
        {
            return NULL;
        }
    }

    return pszMemAddr;
}

/**
 * 初始化各内存模块的内存
 * @param pstModuleMgr 内存模块管理器对象
 * @param iProcID 共享内存id
 * @param bFresh 是否是新创建，bFresh=1标识新创建，需要全新初始化，bFresh=0延续以前的设置
 * @param bReload 是否能动态更新(不停止进程更新内存数据)
 * @return 成功返回0，失败返回错误码
 */
int init_shm_modules(LPSHMMODULEMGR pstModuleMgr, int iProcID, U8 bFresh, U8 bReload)
{
    size_t llShmSize;
    int i;
    int iRet;
    char* pszShmAddr;

    if (NULL == pstModuleMgr) return ERROR_INPUT_PARAM_NULL;

    /* 1. 共享内存总大小 */
    llShmSize = get_shm_module_total_size(pstModuleMgr);

    /* 2. 如果非新建内存，检查一下共享内存头部数据是否正确 */
    if (0 == bFresh)
    {
        SHMMODULEMGRHEAD stShmHead;

        memcpy(&stShmHead, pstModuleMgr->pszShmAddr, sizeof(stShmHead));

        /* 打印头部信息 */
        LOG_WARN("===== %s(bFresh=%d):ShmHead(Flag:%d, ProcID:%d, size:%zu) =====",
                 __FUNCTION__, bFresh, stShmHead.chFlag, stShmHead.iProcID, stShmHead.llSize);

        /* 检查头部大小 */
        if (stShmHead.llSize != llShmSize)
        {
            LOG_ERROR("%s, attach shm and init shm modules error, the shm size(%zu) is not match size(%zu) saved in shm head", __FUNCTION__,
                      llShmSize, stShmHead.llSize);
            return ERROR_SHM_MODULE_HEAD_SIZE_VERIFY;
        }

        /* 检查共享内存id是否匹配 */
        if (stShmHead.iProcID != iProcID)
        {
            LOG_ERROR("%s, attach shm and init shm modules error, the procID(%d) is not match the procID(%d) saved in shm head", __FUNCTION__,
                      iProcID, stShmHead.iProcID);
            return ERROR_SHM_MODULE_HEAD_PROC_VERIFY;
        }

        /* 初始化未完成，继续初始化 */
        if (0 == stShmHead.chFlag)
        {
            LOG_WARN("%s, attach exist shm, which is not finished to init its module, so init it as fresh",
                     __FUNCTION__);
            bFresh = 1;
        }
    }

    /* 3. 新申请的内存，执行各模块的init函数对各个模块内存进行初始化 */
    if (1 == bFresh)
    {
        size_t llModuleSize; /* 模块内存大小 */
        LPSHMMODULEMGRHEAD pstShmHead = (LPSHMMODULEMGRHEAD)pstModuleMgr->pszShmAddr;

        /* 共享内存首地址 */
        pszShmAddr = pstModuleMgr->pszShmAddr;

        /* 记录头部信息 */
        pstShmHead->chFlag = 0;
        pstShmHead->iProcID = iProcID;
        pstShmHead->llSize = llShmSize;

        /* 打印头部信息 */
        LOG_WARN("===== %s(bFresh=%d):ShmHead(Flag:%d, ProcID:%d, size:%zu) =====",
                 __FUNCTION__, bFresh, pstShmHead->chFlag, pstShmHead->iProcID, pstShmHead->llSize);

        /* 指针迁移 */
        pszShmAddr += sizeof(*pstShmHead);

        /* 初始化模块内存 */
        for (i = 0; i < pstModuleMgr->nModuleNum; i++)
        {
            LOG_WARN("=====%s: Begin to init shm for module name(%s), module addr(%p) =====", __FUNCTION__,
                     pstModuleMgr->astModules[i].szModuleName, pszShmAddr);

            llModuleSize = get_shm_module_size(&pstModuleMgr->astModules[i]);

            /* a. 初始化数据区域 */
            if (pstModuleMgr->astModules[i].bNeedMemset)
            {
                memset(pszShmAddr + MAX_SHM_MODULE_NAME_LEN, 0, llModuleSize);
            }

            /* b. 调用模块的初始化函数初始化 */
            if (pstModuleMgr->astModules[i].initModule != NULL)
            {
                iRet = pstModuleMgr->astModules[i].initModule(pszShmAddr + MAX_SHM_MODULE_NAME_LEN, 1);
                if (iRet != 0)
                {
                    LOG_ERROR("%s, init shm for module(%s) failed:%d", __FUNCTION__, pstModuleMgr->astModules[i].szModuleName, iRet);
                    return iRet;
                }
            }

            /* c. 将模块名称保存模块内存头部，注意要在初始化成功后才保存 */
            memcpy(pszShmAddr, pstModuleMgr->astModules[i].szModuleName, MAX_SHM_MODULE_NAME_LEN);

            /* d. 模块地址偏移 */
            pstModuleMgr->astModules[i].llBaseAddr = pszShmAddr - pstModuleMgr->pszShmAddr;

            /* e. 指针迁移，指向下一个模块内存 */
            pszShmAddr += (MAX_SHM_MODULE_NAME_LEN + llModuleSize);

            LOG_WARN("=====%s: End to init shm for module name(%s), module size(%zu), next module addr(%p) =====",
                     __FUNCTION__, pstModuleMgr->astModules[i].szModuleName, llModuleSize, pszShmAddr);
        }
    }
    else
    {
        /* 4. 如果是老的内存，检查下各个模块的大小是否匹配，模块名称是否匹配 */
        pszShmAddr = pstModuleMgr->pszShmAddr + sizeof(SHMMODULEMGRHEAD);

        for (i = 0; i < pstModuleMgr->nModuleNum; i++)
        {
            LOG_WARN("=====%s: Begin to check shm for module name(%s), module addr(%p) =====", __FUNCTION__,
                     pstModuleMgr->astModules[i].szModuleName, pszShmAddr);

            /* a. 检查模块名称是否相同 */
            if (strncasecmp(pstModuleMgr->astModules[i].szModuleName, pszShmAddr, MAX_SHM_MODULE_NAME_LEN) != 0)
            {
                LOG_ERROR("%s, check shm for module(%s) failed, module name is dismatched", __FUNCTION__, pstModuleMgr->astModules[i].szModuleName);
                return ERROR_SHM_MODULE_NAME_VERIFY;
            }

            /* b. 是否重新加载 */
            if (pstModuleMgr->astModules[i].initModule != NULL)
            {
                char bCurReload = bReload;

                /* 若模块不能动态加载，设置为不可加载 */
                if (!pstModuleMgr->astModules[i].bReload) bCurReload = 0;

                iRet = pstModuleMgr->astModules[i].initModule(pszShmAddr + MAX_SHM_MODULE_NAME_LEN, bCurReload);
                if (iRet != 0)
                {
                    LOG_ERROR("%s, init old shm for module(%s) failed:%d", __FUNCTION__, pstModuleMgr->astModules[i].szModuleName, iRet);
                    return iRet;
                }
            }

            /* c. 指向下一个模块 */
            pszShmAddr += (MAX_SHM_MODULE_NAME_LEN + get_shm_module_size(&pstModuleMgr->astModules[i]));

            LOG_WARN("=====%s: End to check shm for module name(%s), next module addr(%p) =====", __FUNCTION__,
                     pstModuleMgr->astModules[i].szModuleName, pszShmAddr);
        }
    }

    /* 5. 如果初始化模块内存成功，标记下共享内存头部 */
    ((LPSHMMODULEMGRHEAD)pstModuleMgr->pszShmAddr)->chFlag = 1;

    return 0;
}

/**
 * 根据模块名获得模块数据区首地址
 * @param pstModuleMgr 内存模块管理器对象
 * @param szModuleName 模块名称
 * @return 成功返回模块数据区首地址，失败返回NULL
 */
char* get_shm_module_addr_by_name(LPSHMMODULEMGR pstModuleMgr, const char* szModuleName)
{
    char* pszShmAddr;
    int i;

    if (NULL == pstModuleMgr || NULL == szModuleName) return NULL;

    for (i = 0; i < pstModuleMgr->nModuleNum; i++)
    {
        pszShmAddr = pstModuleMgr->pszShmAddr + pstModuleMgr->astModules[i].llBaseAddr;

        if (strncasecmp(pszShmAddr, szModuleName, MAX_SHM_MODULE_NAME_LEN) == 0)
        {
            return pszShmAddr + MAX_SHM_MODULE_NAME_LEN;
        }
    }

    return NULL;
}

