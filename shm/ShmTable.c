/*
 * @(#) ShmTable.c Created on 2008-03-27 共享内存式hashtable
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "ShmTable.h"
#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"
#include "log/Logger.h"

#define SHM_TABLE_INVALID_INDEX     -1

/**
 * 释放hash表数据所占用内存
 */
#define FREE_SHM_TABLE_DATA(pstShmTable, pstShmNode) \
    /* 释放内存 */ \
    fix_mem_pool_free(&pstShmTable->stDataMemPool, pstShmNode->iData); \
    /* 指向无效节点，这里很重要 */ \
    pstShmNode->iData = SHM_TABLE_INVALID_INDEX; \
    pstShmNode->iNext = SHM_TABLE_INVALID_INDEX


/**
 * 计算保存数据，hashtable需要的内存大小
 * @param dwDataSize 要保存的单个数据的大小
 * @param dwMaxDataNum 要保存的数据的数量
 * @return 返回hashtable需要的最小内存大小
 */
int calc_shm_table_mem_size(U32 dwDataSize, U32 dwMaxDataNum)
{
    /* hashtable需要的内存大小 = hashtable内存大小 + 节点内存大小*2 + 数据区内存大小 + 2个FixMemPool需要的内存大小 */
    return sizeof(SHMTABLEDATA) + 2 * sizeof(SHMTABLENODE) * dwMaxDataNum +
        dwDataSize * dwMaxDataNum + 2 * sizeof(U32) * dwMaxDataNum;
}

/**
 * 初始化hashtable
 * @param pstShmTable 要初始化的hashtable
 * @param pszMem hashtable的内存地址
 * @param dwMemLen 内存的大小
 * @param dwDataSize 保存在hashtable中数据的长度
 * @param dwMaxDataNum 保存在hashtable中最大数据数量
 * @param pstFuncEqual hashtable比较函数，相同返回0
 * @param pstFuncHash 求hash的函数
 * @return 成功返回0，失败返回错误码
 */
int init_shm_table(LPSHMTABLE pstShmTable, char* pszMem, U32 dwMemLen, U32 dwDataSize,
    U32 dwMaxDataNum, LPFUNCSHMTABLEEQUAL pstFuncEqual, LPFUNCSHMTABLEHASH pstFuncHash)
{
    U32 i;                          /* 临时变量 */
    int iRet;                       /* 返回值 */
    LPSHMTABLENODE pstShmNode;      /* Hash表节点对象 */
    char* pszBase;                  /* 临时指针 */
    U32 dwIndexMemLen;              /* 索引区内存大小 */
    U32 dwIndexMemPoolLen;          /* 索引内存分配区内存大小 */
    U32 dwNeedLen;                  /* Hash表需要的最小内存 */

    LOG_DEBUG("%s: enter", __FUNCTION__);

    if (NULL == pstShmTable || NULL == pszMem || NULL == pstFuncEqual || NULL == pstFuncHash)
    {
        LOG_DEBUG("%s: Init hash table failed: the input parameters are null", __FUNCTION__);
        return ERROR_INPUT_PARAM_NULL;
    }

    /* 检查内存是否足够 */
    dwNeedLen = calc_shm_table_mem_size(dwDataSize, dwMaxDataNum);
    if (dwNeedLen > dwMemLen)
    {
        LOG_ERROR("%s: Init hash table failed: the mem len(%d) is not enough [%d]", __FUNCTION__, dwMemLen, dwNeedLen);
        return ERROR_MEM_LEN_NOT_ENOUGH;
    }

    dwIndexMemLen = dwMaxDataNum * sizeof(*pstShmNode);
    dwIndexMemPoolLen = (sizeof(*pstShmNode) + sizeof(U32))*dwMaxDataNum;

    /* 初始化数据 */
    pstShmTable->pstTableData = (LPSHMTABLEDATA)pszMem;
    pstShmTable->dwMemSize = dwMemLen;
    pstShmTable->dwMaxDataNum = dwMaxDataNum;
    pstShmTable->dwDataSize = dwDataSize;
    pstShmTable->equal = pstFuncEqual;
    pstShmTable->hash = pstFuncHash;
    pstShmTable->stInterator.dwPos = 0;
    pstShmTable->stInterator.pstShmNode = NULL;

    /* 初始化关键数据 */
    pstShmTable->pstTableData->dwNum = 0;
    pstShmTable->pstTableData->szData = pszMem + sizeof(*(pstShmTable->pstTableData));
    pszBase = pstShmTable->pstTableData->szData;

    /* 未分配的节点索引，初始化为-1 */
    for (i = 0; i < dwMaxDataNum; i++)
    {
        pstShmNode = (LPSHMTABLENODE)(pstShmTable->pstTableData->szData + i * sizeof(*pstShmNode));
        pstShmNode->iData = SHM_TABLE_INVALID_INDEX;
        pstShmNode->iNext = SHM_TABLE_INVALID_INDEX;
    }

    /* 初始化内存索引分配区 */
    pszBase += dwIndexMemLen;
    iRet = init_fix_mem_pool(&pstShmTable->stIndexMemPool, pszBase, dwIndexMemPoolLen, sizeof(*pstShmNode));

    if (iRet != 0)
    {
        LOG_ERROR("%s: Init hash table failed: init index mem pool error:%d", __FUNCTION__, iRet);
        return iRet;
    }

    /* 初始化数据索引分配区 */
    pszBase += dwIndexMemPoolLen;
    iRet = init_fix_mem_pool(&pstShmTable->stDataMemPool, pszBase,
        (dwDataSize + sizeof(U32))*dwMaxDataNum, dwDataSize);

    if (iRet != 0)
    {
        LOG_ERROR("%s: Init hash table failed: init data mem pool error:%d", __FUNCTION__, iRet);
        return iRet;
    }

    return 0;
}

/**
 * attach到hashtable的共享内存，不初始化
 * @param pstShmTable 要attach的hashtable
 * @param pszMem hashtable的内存地址
 * @param dwMemLen 内存的大小
 * @param dwDataSize 保存在hashtable中数据的长度
 * @param dwMaxDataNum 保存在hashtable中最大数据数量
 * @param pstFuncEqual hashtable比较函数，相同返回0
 * @param pstFuncHash 求hash的函数
 * @return 成功返回0，失败返回错误码
 */
int attach_shm_table(LPSHMTABLE pstShmTable, char* pszMem, U32 dwMemLen, U32 dwDataSize,
                     U32 dwMaxDataNum, LPFUNCSHMTABLEEQUAL pstFuncEqual, LPFUNCSHMTABLEHASH pstFuncHash)
{
    int iRet;                           /* 返回值 */
    LPSHMTABLENODE pstShmNode;          /* Hash表节点对象 */
    char* pszBase;                      /* 临时指针 */
    U32 dwIndexMemLen;                  /* 索引区内存大小 */
    U32 dwIndexMemPoolLen;              /* 索引内存分配区内存大小 */
    U32 dwNeedLen;                      /* Hash表需要的最小内存 */

    LOG_DEBUG("%s: enter", __FUNCTION__);

    if (NULL == pstShmTable || NULL == pszMem || NULL == pstFuncEqual || NULL == pstFuncHash)
    {
        LOG_DEBUG("%s: Attach hash table failed: the input parameters are null", __FUNCTION__);
        return ERROR_INPUT_PARAM_NULL;
    }

    /* 检查内存是否足够 */
    dwNeedLen = calc_shm_table_mem_size(dwDataSize, dwMaxDataNum);
    if (dwNeedLen > dwMemLen)
    {
        LOG_ERROR("%s: Attach hash table failed: the mem len(%d) is not enough [%d]", __FUNCTION__, dwMemLen, dwNeedLen);
        return ERROR_MEM_LEN_NOT_ENOUGH;
    }

    dwIndexMemLen = dwMaxDataNum * sizeof(*pstShmNode);
    dwIndexMemPoolLen = (sizeof(*pstShmNode) + sizeof(U32))*dwMaxDataNum;

    /* 初始化数据 */
    pstShmTable->pstTableData = (LPSHMTABLEDATA)pszMem;
    pstShmTable->dwMemSize = dwMemLen;
    pstShmTable->dwMaxDataNum = dwMaxDataNum;
    pstShmTable->dwDataSize = dwDataSize;
    pstShmTable->equal = pstFuncEqual;
    pstShmTable->hash = pstFuncHash;
    pstShmTable->stInterator.dwPos = 0;
    pstShmTable->stInterator.pstShmNode = NULL;

    /* 检查下数据 */
    if (pstShmTable->pstTableData->dwNum > pstShmTable->dwMaxDataNum)
    {
        LOG_ERROR("%s: Attach hash table failed: the data num(%d) is much than the max num(%d)", __FUNCTION__,
            pstShmTable->pstTableData->dwNum, pstShmTable->dwMaxDataNum);
        return ERROR_SHM_DATA_ERROR;
    }

    /* 初始化关键数据 */
    pstShmTable->pstTableData->szData = pszMem + sizeof(*(pstShmTable->pstTableData));
    pszBase = pstShmTable->pstTableData->szData;

    /* attach内存索引分配区 */
    pszBase += dwIndexMemLen;
    iRet = attach_fix_mem_pool(&pstShmTable->stIndexMemPool, pszBase, dwIndexMemPoolLen, sizeof(*pstShmNode));

    if (iRet != 0)
    {
        LOG_ERROR("%s: Attach hash table failed: attach index mem pool error:%d", __FUNCTION__, iRet);
        return iRet;
    }

    /* attach数据索引分配区 */
    pszBase += dwIndexMemPoolLen;
    iRet = attach_fix_mem_pool(&pstShmTable->stDataMemPool, pszBase,
        (dwDataSize + sizeof(U32)) * dwMaxDataNum, dwDataSize);

    if (iRet != 0)
    {
        LOG_ERROR("%s: Attach hash table failed: attach data mem pool error:%d", __FUNCTION__, iRet);
        return iRet;
    }

    return 0;
}

/**
 * 添加数据到hashtable中
 * @param pstShmTable hashtable表
 * @param pstData 要添加的数据
 * @return 成功返回要添加的数据的数据区指针，失败返回NULL
 */
void* shm_table_add(LPSHMTABLE pstShmTable, void* pstData)
{
    int iKey;
    char* szData = NULL;
    LPSHMTABLENODE pstShmNode;          /* hash表节点 */
    LPSHMTABLENODE pstShmNewNode;       /* 新分配的hash表节点 */
    int iPoolIndex;                     /* 数据分配索引 */
    int iFree;                          /* 数据是否被释放了内存？ */

    LOG_DEBUG("%s: enter", __FUNCTION__);

    if (NULL == pstShmTable || NULL == pstData || NULL == pstShmTable->pstTableData) return NULL;

    /* 检查数量是否已经超过了最大值？ */
    if (pstShmTable->pstTableData->dwNum >= pstShmTable->dwMaxDataNum)
    {
        LOG_DEBUG("%s: Add data to hash table failed: the table num(%d) is full(%d)", __FUNCTION__,
            pstShmTable->pstTableData->dwNum, pstShmTable->dwMaxDataNum);
        return NULL;
    }

    /* 取hash，找到数据存储的数组索引 */
    iKey = pstShmTable->hash(pstData) % pstShmTable->dwMaxDataNum;

    /* 找到数组存储的节点 */
    pstShmNode = (LPSHMTABLENODE)(pstShmTable->pstTableData->szData + iKey * sizeof(*pstShmNode));

    /* 如果节点是空的 */
    if (SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
    {
        /* 分配数据 */
        szData = fix_mem_pool_malloc(&pstShmTable->stDataMemPool, &iPoolIndex);

        if (NULL == szData)
        {
            LOG_DEBUG("%s: Add data to hash table failed: malloc data failed", __FUNCTION__);
            return NULL;
        }

        /* 拷贝数据？ 是否要外部拷贝性能更高一点呢？ */
        /* memcpy(szData, pstData, pstShmTable->dwDataSize); */
        pstShmNode->iData = iPoolIndex;
        pstShmTable->pstTableData->dwNum++;

        return szData;
    }

    /* 如果节点不为空 */
    while (1)
    {
        /* 判断是否和当前节点相等？ */
        szData = fix_mem_pool_at(&pstShmTable->stDataMemPool, pstShmNode->iData, &iFree);
        if (iFree == 1 || NULL == szData)
        {
            /* 这种情况一般是出现了内存错乱 */
            LOG_ERROR("%s: Add data to hash table failed: mem data error cause the data(%d) is free or invalid",
                __FUNCTION__, pstShmNode->iData);
            return NULL;
        }

        /* 如果当前节点的数据与要添加的数据相等，直接返回本节点的指针 */
        if (pstShmTable->equal(pstData, szData) == 0)
        {
            /* 拷贝数据？ 是否要外部拷贝性能更高一点呢？ */
            /* memcpy(szData, pstData, pstShmTable->dwDataSize); */
            return szData;
        }

        /* 如果当前节点没有后续节点，则添加一个新的节点 */
        if (SHM_TABLE_INVALID_INDEX == pstShmNode->iNext)
        {
            /* 分配索引数据 */
            pstShmNewNode = (LPSHMTABLENODE)fix_mem_pool_malloc(&pstShmTable->stIndexMemPool, &iPoolIndex);
            if (NULL == pstShmNewNode)
            {
                LOG_DEBUG("%s: Add data to hash table failed: malloc index failed", __FUNCTION__);
                return NULL;
            }

            /* 前一个节点，指向后一个节点 */
            pstShmNode->iNext = iPoolIndex;

            /* 分配数据 */
            szData = fix_mem_pool_malloc(&pstShmTable->stDataMemPool, &iPoolIndex);

            if (NULL == szData)
            {
                /* 释放内存 */
                fix_mem_pool_free(&pstShmTable->stIndexMemPool, pstShmNode->iNext);
                pstShmNode->iNext = SHM_TABLE_INVALID_INDEX;
                LOG_DEBUG("%s: Add data to hash table failed: malloc data failed", __FUNCTION__);
                return NULL;
            }

            /* 后一个节点的信息 */
            pstShmNewNode->iData = iPoolIndex;
            pstShmNewNode->iNext = SHM_TABLE_INVALID_INDEX;

            /* 拷贝数据？ 是否要外部拷贝性能更高一点呢？ */
            /* memcpy(szData, pstData, pstShmTable->dwDataSize); */
            pstShmTable->pstTableData->dwNum++;

            return szData;
        }

        pstShmNode = (LPSHMTABLENODE)fix_mem_pool_at(&pstShmTable->stIndexMemPool, pstShmNode->iNext, &iFree);
        if (iFree == 1 || NULL == pstShmNode || SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
        {
            /* 这种情况一般是出现了内存错乱 */
            LOG_ERROR("%s: Add data to hash table failed: mem data error cause the index data is free or invalid",
                __FUNCTION__);
            return NULL;
        }
    }

    return NULL;
}

/**
 * 从hashtable中查找与pstData相等的数据
 * @param pstShmTable hash表
 * @param pstData 要查找的数据
 * @return 找到后返回与pstData相等的数据，没有找到，返回NULL
 */
void* shm_table_get(LPSHMTABLE pstShmTable, void* pstData)
{
    int iKey;
    char* szData = NULL;
    LPSHMTABLENODE pstShmNode;          /* hash表节点 */
    int iFree;                          /* 数据是否被释放了内存？ */

    LOG_DEBUG("%s: enter", __FUNCTION__);

    if (NULL == pstShmTable || NULL == pstData || NULL == pstShmTable->pstTableData) return NULL;

    /* 取hash，找到数据存储的数组索引 */
    iKey = pstShmTable->hash(pstData) % pstShmTable->dwMaxDataNum;

    /* 找到数组存储的节点 */
    pstShmNode = (LPSHMTABLENODE)(pstShmTable->pstTableData->szData + iKey * sizeof(*pstShmNode));

    /* 如果节点是空的 */
    if (SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
    {
        return NULL;
    }

    /* 如果节点不为空 */
    while (1)
    {
        /* 判断是否和当前节点相等？ */
        szData = fix_mem_pool_at(&pstShmTable->stDataMemPool, pstShmNode->iData, &iFree);
        if (iFree == 1 || NULL == szData)
        {
            /* 这种情况一般是出现了内存错乱 */
            LOG_ERROR("%s: Get data from hash table failed: mem data error cause the data(%d) is free or invalid",
                __FUNCTION__, pstShmNode->iData);
            return NULL;
        }

        /* 如果当前节点的数据与要查找的数据相等，直接返回本节点的指针 */
        if (pstShmTable->equal(pstData, szData) == 0)
        {
            return szData;
        }

        /* 如果当前节点没有后续节点，则没有找到 */
        if (SHM_TABLE_INVALID_INDEX == pstShmNode->iNext)
        {
            return NULL;
        }

        pstShmNode = (LPSHMTABLENODE)fix_mem_pool_at(&pstShmTable->stIndexMemPool, pstShmNode->iNext, &iFree);
        if (iFree == 1 || NULL == pstShmNode || SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
        {
            /* 这种情况一般是出现了内存错乱 */
            LOG_ERROR("%s: Get data from hash table failed: mem data error cause the index data is free or invalid",
                __FUNCTION__);
            return NULL;
        }
    }

    return NULL;
}

/**
 * 从hashtable中删除与pstData相等的数据
 * @param pstShmTable hash表
 * @param pstData 要删除的数据，只需要填写pstFuncHash及pstFuncEqual相等的数据即可
 * @return 删除后返回与pstData相等的数据，没有找到要删除的数据，返回NULL
 */
void* shm_table_remove(LPSHMTABLE pstShmTable, void* pstData)
{
    int iKey;
    char* szData = NULL;
    LPSHMTABLENODE pstShmNode;          /* hash表节点 */
    LPSHMTABLENODE pstParentNode;       /* 表节点的父节点 */
    int iFree;                          /* 数据是否被释放了内存？ */

    LOG_DEBUG("%s: enter", __FUNCTION__);

    if (NULL == pstShmTable || NULL == pstData || NULL == pstShmTable->pstTableData) return NULL;

    /* 取hash，找到数据存储的数组索引 */
    iKey = pstShmTable->hash(pstData) % pstShmTable->dwMaxDataNum;

    /* 找到数组存储的节点 */
    pstShmNode = (LPSHMTABLENODE)(pstShmTable->pstTableData->szData + iKey * sizeof(*pstShmNode));

    /* 如果节点是空的 */
    if (SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
    {
        return NULL;
    }

    /* 如果节点不为空 */
    pstParentNode = NULL;
    while (1)
    {
        /* 判断是否和当前节点相等？ */
        szData = fix_mem_pool_at(&pstShmTable->stDataMemPool, pstShmNode->iData, &iFree);
        if (iFree == 1 || NULL == szData)
        {
            /* 这种情况一般是出现了内存错乱 */
            LOG_ERROR("%s: Get data from hash table failed: mem data error cause the data(%d) is free or invalid",
                __FUNCTION__, pstShmNode->iData);
            return NULL;
        }

        /* 如果当前节点的数据与要查找的数据相等，直接返回本节点的指针 */
        if (pstShmTable->equal(pstData, szData) == 0)
        {
            /* 删除节点 */
            if (pstParentNode != NULL)
            {
                /* 释放内存 */
                fix_mem_pool_free(&pstShmTable->stIndexMemPool, pstParentNode->iNext);
                /* 指向下一个节点 */
                pstParentNode->iNext = pstShmNode->iNext;
                /* 释放内存 */
                FREE_SHM_TABLE_DATA(pstShmTable, pstShmNode);
            }
            else if (pstShmNode->iNext != SHM_TABLE_INVALID_INDEX)
            {
                /* 找到当前节点的后一节点 */
                pstParentNode = pstShmNode;
                pstShmNode = (LPSHMTABLENODE)fix_mem_pool_at(&pstShmTable->stIndexMemPool, pstParentNode->iNext, &iFree);
                if (iFree == 1 || NULL == pstShmNode || SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
                {
                    /* 这种情况一般是出现了内存错乱 */
                    LOG_ERROR("%s: Get data from hash table failed: mem data error cause the index data is free or invalid",
                        __FUNCTION__);
                    return NULL;
                }
                /* 释放parent内存 */
                fix_mem_pool_free(&pstShmTable->stIndexMemPool, pstParentNode->iNext);
                FREE_SHM_TABLE_DATA(pstShmTable, pstParentNode);
                /* 指向下一个节点 */
                pstParentNode->iData = pstShmNode->iData;
                pstParentNode->iNext = pstShmNode->iNext;
            }
            else
            {
                /* 释放内存 */
                FREE_SHM_TABLE_DATA(pstShmTable, pstShmNode);
            }
            /* 计数减一 */
            pstShmTable->pstTableData->dwNum--;
            /* 返回被“删除”的数据内存地址 */
            return szData;
        }

        /* 如果当前节点没有后续节点，则没有找到 */
        if (SHM_TABLE_INVALID_INDEX == pstShmNode->iNext)
        {
            return NULL;
        }

        pstParentNode = pstShmNode;
        pstShmNode = (LPSHMTABLENODE)fix_mem_pool_at(&pstShmTable->stIndexMemPool, pstParentNode->iNext, &iFree);
        if (iFree == 1 || NULL == pstShmNode || SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
        {
            /* 这种情况一般是出现了内存错乱 */
            LOG_ERROR("%s: Get data from hash table failed: mem data error cause the index data is free or invalid",
                __FUNCTION__);
            return NULL;
        }
    }

    return NULL;
}

/**
 * 返回hash表中数据的数量
 * @param pstShmTable hash表
 * @return 返回hash表数量
 */
int shm_table_get_data_number(LPSHMTABLE pstShmTable)
{
    return NULL == pstShmTable || NULL == pstShmTable->pstTableData ? 0 : pstShmTable->pstTableData->dwNum;
}

/**
 * 返回hash表可以存储的最大数据数量
 * @param pstShmTable hash表
 * @return 返回hash表可以存储的最大数据数量
 */
int shm_table_get_max_data_number(LPSHMTABLE pstShmTable)
{
    return NULL == pstShmTable ? 0 : pstShmTable->dwMaxDataNum;
}

/**
 * 开始向后遍历hash表
 * @param pstShmTable hash表
 * @param dwPos 遍历hash表的起始位置
 * @return 成功返回0，失败返回错误码
 */
int shm_table_begin_iteration(LPSHMTABLE pstShmTable, U32 dwPos)
{
    if (NULL == pstShmTable) return ERROR_INPUT_PARAM_NULL;
    pstShmTable->stInterator.dwPos = dwPos;
    pstShmTable->stInterator.pstShmNode = NULL;
    return 0;
}

/**
 * 返回遍历hash表获得的当前有效节点数据指针
 * @param pstShmTable hash表
 * @return 若hash表遍历完，返回NULL，没有遍历完返回当前有效节点数据指针
 */
void* shm_table_next(LPSHMTABLE pstShmTable)
{
    U32 i;
    LPSHMTABLENODE pstShmNode;
    char* szData = NULL;
    int iFree;

    if (NULL == pstShmTable || NULL == pstShmTable->pstTableData || 0 == pstShmTable->pstTableData->dwNum) return NULL;

    if (NULL == pstShmTable->stInterator.pstShmNode ||
        SHM_TABLE_INVALID_INDEX == pstShmTable->stInterator.pstShmNode->iData ||    /* 可能在迭代的过程中，数据被删除了 */
        SHM_TABLE_INVALID_INDEX == pstShmTable->stInterator.pstShmNode->iNext)      /* 可能在迭代的过程中，数据被删除了 */
    {
        U32 dwMaxScan = 0;

        /* 当节点被删除时，从下个节点开始遍历 */
        if (pstShmTable->stInterator.pstShmNode != NULL)
        {
            pstShmTable->stInterator.dwPos++;
            pstShmTable->stInterator.pstShmNode = NULL;
        }

        dwMaxScan = pstShmTable->dwMaxDataNum + pstShmTable->stInterator.dwPos;

        for (i = pstShmTable->stInterator.dwPos; i < dwMaxScan; i++)
        {
            U32 j = i % pstShmTable->dwMaxDataNum;

            pstShmNode = (LPSHMTABLENODE)(pstShmTable->pstTableData->szData + j * sizeof(*pstShmNode));

            /* 当找到有效数据 */
            if (pstShmNode->iData != SHM_TABLE_INVALID_INDEX)
            {
                szData = fix_mem_pool_at(&pstShmTable->stDataMemPool, pstShmNode->iData, &iFree);
                if (iFree == 1 || NULL == szData)
                {
                    /* 这种情况一般是出现了内存错乱 */
                    //LOG_ERROR("%s: Get data from hash table failed: mem data error cause the data(%d) is free or invalid",
                    //    __FUNCTION__, pstShmNode->iData);
                    //return NULL;
                    continue;
                }

                /* 更新迭代器 */
                if (pstShmNode->iNext != SHM_TABLE_INVALID_INDEX)
                {
                    pstShmTable->stInterator.dwPos = j;
                    pstShmTable->stInterator.pstShmNode = pstShmNode;
                }
                else
                {
                    pstShmTable->stInterator.dwPos = j + 1;
                }

                return szData;
            }
        }
    }
    else
    {
        /* 查找下一个节点，下一个节点必然是有效数据 */
        pstShmNode = (LPSHMTABLENODE)fix_mem_pool_at(&pstShmTable->stIndexMemPool, pstShmTable->stInterator.pstShmNode->iNext, &iFree);
        if (iFree == 1 || NULL == pstShmNode || SHM_TABLE_INVALID_INDEX == pstShmNode->iData)
        {
            /* 这种情况一般是出现了内存错乱 */
            //LOG_ERROR("%s: Get data from hash table failed: mem data error cause the index data is free or invalid",
            //    __FUNCTION__);
            //return NULL;
            /* 查找下个有效的吧 */
            pstShmTable->stInterator.dwPos++;
            pstShmTable->stInterator.pstShmNode = NULL;
            return shm_table_next(pstShmTable);
        }

        szData = fix_mem_pool_at(&pstShmTable->stDataMemPool, pstShmNode->iData, &iFree);
        if (iFree == 1 || NULL == szData)
        {
            /* 这种情况一般是出现了内存错乱 */
            //LOG_ERROR("%s: Get data from hash table failed: mem data error cause the data(%d) is free or invalid",
            //    __FUNCTION__, pstShmNode->iData);
            //return NULL;
            /* 查找下个有效的吧 */
            pstShmTable->stInterator.dwPos++;
            pstShmTable->stInterator.pstShmNode = NULL;
            return shm_table_next(pstShmTable);
        }

        /* 更新迭代器 */
        if (pstShmNode->iNext != SHM_TABLE_INVALID_INDEX)
        {
            pstShmTable->stInterator.pstShmNode = pstShmNode;
        }
        else
        {
            pstShmTable->stInterator.dwPos++;
            pstShmTable->stInterator.pstShmNode = NULL;
        }

        return szData;
    }

    return NULL;
}

/**
 * 返回遍历hash表获得的当前有效节点数据指针。当遍历到最后节点时，从头开始遍历
 * @param pstShmTable hash表
 * @return 若hash表遍历完，返回NULL，没有遍历完返回当前有效节点数据指针
 */
void* shm_table_next_loop(LPSHMTABLE pstShmTable)
{
    if (NULL == pstShmTable || NULL == pstShmTable->pstTableData || 0 == pstShmTable->pstTableData->dwNum) return NULL;

    if (pstShmTable->stInterator.dwPos >= pstShmTable->dwMaxDataNum)
    {
        pstShmTable->stInterator.dwPos = 0;
        pstShmTable->stInterator.pstShmNode = NULL;
    }

    return shm_table_next(pstShmTable);
}

