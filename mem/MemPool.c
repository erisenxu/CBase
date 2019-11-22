/*
 * @(#) MemPool.c Created on 2008-03-27 内存池，大小不固定，从堆上分配内存
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "MemPool.h"
#include <stdlib.h>
#include "comm/ErrorCode.h"
#include "log/Logger.h"

/**
 * 内存池增长尺寸
 */
#define MEM_POOL_EXTEND_SIZE 5

/**
 * 预分配内存
 */
static void mem_pool_append(LPMEMPOOL pstMemPool)
{
    U32 i;
    void* pstMem;

    if (NULL == pstMemPool) return;

    for (i = 0; i < MEM_POOL_EXTEND_SIZE; i++)
    {
        pstMem = malloc(pstMemPool->dwBlockSize);

        if (NULL == pstMem) continue;

        slist_append(pstMemPool->pstFreeBlocks, pstMem);
    }
}

/**
 * 初始化内存池
 * @param pstMemPool 被初始化的内存池
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回0，失败返回错误码
 */
int init_mem_pool(LPMEMPOOL pstMemPool, U32 dwBlockSize)
{
    if (NULL == pstMemPool || 0 == dwBlockSize) return ERROR_INPUT_PARAM_NULL;

    pstMemPool->dwBlockSize = dwBlockSize;
    pstMemPool->pstFreeBlocks = slist_create();

    if (NULL == pstMemPool->pstFreeBlocks) return ERROR_MEM_POOL_MEM_ALLOCATE;

    pstMemPool->pstFreeNodes = slist_create();

    /* 预分配内存 */
    mem_pool_append(pstMemPool);

    return 0;
}

/**
 * 创建内存池
 * @param pstMemPool 被初始化的内存池
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回创建的内存池，失败返回NULL
 */
LPMEMPOOL create_mem_pool(U32 dwBlockSize)
{
    LPMEMPOOL pstMemPool;
    int iRet;

    if (0 == dwBlockSize) return NULL;

    pstMemPool = malloc(sizeof(MEMPOOL));

    if (NULL == pstMemPool) return NULL;

    iRet = init_mem_pool(pstMemPool, dwBlockSize);

    if (iRet != 0)
    {
        free(pstMemPool);
        return NULL;
    }

    return pstMemPool;
}

/**
 * 释放内存池所占内存空间
 * @param pstMemPool 要释放的内存池
 */
void free_mem_pool(LPMEMPOOL pstMemPool)
{
    if (NULL == pstMemPool) return;

    clear_mem_pool(pstMemPool);

    free(pstMemPool);
}

/**
 * 清理内存池所分配内存空间，但不释放内存池对象所占空间
 * @param pstMemPool 要清空的内存池
 */
void clear_mem_pool(LPMEMPOOL pstMemPool)
{
    if (NULL == pstMemPool) return;

    slist_free(pstMemPool->pstFreeBlocks, free);
    slist_free(pstMemPool->pstFreeNodes, free);
}

/**
 * 从内存池中分配内存
 * @param pstMemPool 内存池对象
 * @return 返回分配的内存，若分配不成功，返回NULL
 */
void* mem_pool_malloc(LPMEMPOOL pstMemPool)
{
    LPSLISTNODE pstHeadNode;
    void* pstMem;

    if (NULL == pstMemPool) return NULL;

    pstHeadNode = pstMemPool->pstFreeBlocks->pstHead;

    if (NULL == pstHeadNode)
    {
        mem_pool_append(pstMemPool);
        pstHeadNode = pstMemPool->pstFreeBlocks->pstHead;
    }

    if (NULL == pstHeadNode) return NULL;

    pstMem = pstHeadNode->pstData;

    /* 将内存从空闲列表中移除 */
    slist_remove_head(pstMemPool->pstFreeBlocks);

    /* 释放节点内存 */
    pstHeadNode->pstData = NULL;

    if (pstMemPool->pstFreeNodes)
    {
        slist_append_node(pstMemPool->pstFreeNodes, pstHeadNode);
    }
    else
    {
        free(pstHeadNode);
    }

    LOG_DEBUG("%s, pool free size:%llu, malloc size:%llu", __FUNCTION__, pstMemPool->pstFreeBlocks->ullNumber,
              pstMemPool->pstFreeNodes ? pstMemPool->pstFreeNodes->ullNumber : 0);

    return pstMem;
}

/**
 * 将内存释放给内存池
 * @param pstPool 内存池对象
 * @param pstMem 要释放的内存
 */
void mem_pool_free(LPMEMPOOL pstMemPool, void* pstMem)
{
    LPSLISTNODE pstFreeNode = NULL;

    if (NULL == pstMemPool || NULL == pstMem) return;

    if (pstMemPool->pstFreeNodes)
    {
        pstFreeNode = slist_remove_head(pstMemPool->pstFreeNodes);
    }

    if (pstFreeNode)
    {
        pstFreeNode->pstData = pstMem;
        slist_append_node(pstMemPool->pstFreeBlocks, pstFreeNode);
    }
    else
    {
        slist_append(pstMemPool->pstFreeBlocks, pstMem);
    }

    LOG_DEBUG("%s, pool free size:%llu, malloc size:%llu", __FUNCTION__, pstMemPool->pstFreeBlocks->ullNumber,
              pstMemPool->pstFreeNodes ? pstMemPool->pstFreeNodes->ullNumber : 0);
}
