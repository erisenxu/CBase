/*
 * @(#) FixMemPool.c Created on 2008-03-27 共享内存式 - 固定大小的内存池
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "FixMemPool.h"

#include "comm/ErrorCode.h"

#define BLOCK_USEMASK           0x80000000
#define MAX_FIX_MEM_BLOCK_NUM   0x7FFFFFFF      /* 共享内存可以分配的最大数量 */

#include <stdlib.h>

/**
 * 使用共享内存初始化内存池
 * @param pstFixMemPool 被初始化的内存池
 * @param pszMem 共享内存
 * @param dwMemLen 共享内存大小
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回0，失败返回错误码
 */
int init_fix_mem_pool(LPFIXMEMPOLL pstFixMemPool, char* pszMem, U32 dwMemLen, U32 dwBlockSize)
{
    U32 i;

    if (NULL == pstFixMemPool || NULL == pszMem) return ERROR_INPUT_PARAM_NULL;

    if (0 == dwBlockSize) return ERROR_MEM_POOL_BLOCK_SIZE_ZERO;

    if (dwMemLen < dwBlockSize + sizeof(U32)) return ERROR_MEM_POOL_MEM_LEN_SHORT;

    pstFixMemPool->dwBlockNum = dwMemLen/(dwBlockSize + sizeof(U32));

    if (pstFixMemPool->dwBlockNum > MAX_FIX_MEM_BLOCK_NUM) return ERROR_MEM_POOL_BLOCK_NUM_LARGE;

    /* 为内存池分配内存 */
    pstFixMemPool->piBlock = (int*)pszMem;

    pstFixMemPool->dwFreeNum = pstFixMemPool->dwBlockNum;
    pstFixMemPool->pszBlockMem = pszMem + pstFixMemPool->dwBlockNum * sizeof(U32);
    pstFixMemPool->dwBlockSize = dwBlockSize;

    /* 分割共享内存 */
    for (i = 0; i < pstFixMemPool->dwBlockNum; i++)
    {
        *(pstFixMemPool->piBlock + i) = i;
    }

    return 0;
}

/**
 * 内存池attach到共享内存，但不修改内存池中的初始值
 * @param pstFixMemPool 被attach的内存池
 * @param pszMem 共享内存
 * @param dwMemLen 共享内存大小
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回0，失败返回错误码
 */
int attach_fix_mem_pool(LPFIXMEMPOLL pstFixMemPool, char* pszMem, U32 dwMemLen, U32 dwBlockSize)
{
    U32 i;

    if (NULL == pstFixMemPool || NULL == pszMem) return ERROR_INPUT_PARAM_NULL;

    if (0 == dwBlockSize) return ERROR_MEM_POOL_BLOCK_SIZE_ZERO;

    if (dwMemLen < dwBlockSize + sizeof(U32)) return ERROR_MEM_POOL_MEM_LEN_SHORT;

    pstFixMemPool->dwBlockNum = dwMemLen/(dwBlockSize + sizeof(U32));

    if (pstFixMemPool->dwBlockNum > MAX_FIX_MEM_BLOCK_NUM) return ERROR_MEM_POOL_BLOCK_NUM_LARGE;

    /* 为内存池分配内存 */
    pstFixMemPool->piBlock = (int*)pszMem;

    pstFixMemPool->pszBlockMem = pszMem + pstFixMemPool->dwBlockNum * sizeof(U32);
    pstFixMemPool->dwBlockSize = dwBlockSize;

    /* 计算空闲内存数量 */
    pstFixMemPool->dwFreeNum = 0;

    for (i = 0; i < pstFixMemPool->dwBlockNum; i++)
    {
        if (((*(pstFixMemPool->piBlock + i)) & BLOCK_USEMASK) == 0)
        {
            pstFixMemPool->dwFreeNum++;
        }
    }

    return 0;
}

/**
 * 从共享内存池中分配内存
 * @param pstFixMemPool 共享内存池对象
 * @param piIndex 成功时，返回分配的内存块的索引
 * @return 成功返回分配的内存，如果内存不足返回NULL
 */
char* fix_mem_pool_malloc(LPFIXMEMPOLL pstFixMemPool, int* piIndex)
{
    int iIndex;

    if (NULL == pstFixMemPool || NULL == pstFixMemPool->pszBlockMem || NULL == pstFixMemPool->piBlock) return NULL;

    while(pstFixMemPool->dwFreeNum > 0)
    {
        pstFixMemPool->dwFreeNum--;
        iIndex = (*(pstFixMemPool->piBlock + pstFixMemPool->dwFreeNum)) & (~(BLOCK_USEMASK));

        /* 检查index处的内存是否被占用，如果没占用，跳出循环 */
        if (((*(pstFixMemPool->piBlock + iIndex)) & BLOCK_USEMASK) == 0)
        {
            /* 设置为被占用状态 */
            *(pstFixMemPool->piBlock + iIndex) = (*(pstFixMemPool->piBlock + iIndex)) | BLOCK_USEMASK;
            if (piIndex != NULL) *piIndex = iIndex;

            return pstFixMemPool->pszBlockMem + pstFixMemPool->dwBlockSize * iIndex;
        }
    }

    return NULL;
}

/**
 * 释放从共享内存池中分配的内存
 * @param pstFixMemPool 共享内存池对象
 * @param iBlockIndex 要释放的内存快索引
 * @return 成功返回0，失败返回错误码
 */
int fix_mem_pool_free(LPFIXMEMPOLL pstFixMemPool, int iBlockIndex)
{
    if (NULL == pstFixMemPool || iBlockIndex < 0) return ERROR_INPUT_PARAM_NULL;

    if (iBlockIndex >= pstFixMemPool->dwBlockNum) return ERROR_ARRAY_LEN_NOT_ENOUGH;

    /* 检查是否已经释放了内存，如果已经释放，不必重复释放 */
    if (((*(pstFixMemPool->piBlock + iBlockIndex)) & BLOCK_USEMASK) == 0)
    {
        return 0;
    }

    if (pstFixMemPool->dwFreeNum >= pstFixMemPool->dwBlockNum) return ERROR_ARRAY_LEN_NOT_ENOUGH;

    /* 设置为非占用状态 */
    *(pstFixMemPool->piBlock + iBlockIndex) = (*(pstFixMemPool->piBlock + iBlockIndex)) & (~(BLOCK_USEMASK));

    /* 释放内存 */
    if (((*(pstFixMemPool->piBlock + pstFixMemPool->dwFreeNum)) & BLOCK_USEMASK) == 0)
    {
        *(pstFixMemPool->piBlock + pstFixMemPool->dwFreeNum) = iBlockIndex;
    }
    else
    {
        *(pstFixMemPool->piBlock + pstFixMemPool->dwFreeNum) = iBlockIndex | BLOCK_USEMASK;
    }

    pstFixMemPool->dwFreeNum++;

    return 0;
}

/**
 * 获取内存池指定索引处的内存
 * @param pstFixMemPool 共享内存池对象
 * @param iBlockIndex 要释放的内存块索引
 * @param piFree 若内存被占用，返回0，未被占用返回1。若函数调用失败，piFree无意义
 * @return 成功返回索引处的内存，失败NULL
 */
char* fix_mem_pool_at(LPFIXMEMPOLL pstFixMemPool, int iBlockIndex, int* piFree)
{
    if (NULL == pstFixMemPool || iBlockIndex < 0 || iBlockIndex >= pstFixMemPool->dwBlockNum) return NULL;

    if (piFree != NULL)
    {
        *piFree = ((*(pstFixMemPool->piBlock + iBlockIndex)) & BLOCK_USEMASK) == 0 ? 1 : 0;
    }

    return pstFixMemPool->pszBlockMem + pstFixMemPool->dwBlockSize * iBlockIndex;
}

