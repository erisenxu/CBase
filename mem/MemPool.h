/*
 * @(#) MemPool.h Created on 2008-03-27 内存池，大小不固定，从堆上分配内存
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef MEM_POOL_H
#define MEM_POOL_H

#include "list/SList.h"
#include "comm/BaseDef.h"

/**
 * 内存池结构体
 */
struct tagMemPool
{
    U32 dwBlockSize;        /* 内存块大小 */
    LPSLIST pstFreeBlocks;  /* 空闲内存列表 */
    LPSLIST pstFreeNodes;   /* 空闲内存列表 */
};

typedef struct tagMemPool    MEMPOOL;
typedef struct tagMemPool*   LPMEMPOOL;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 初始化内存池
 * @param pstMemPool 被初始化的内存池
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回0，失败返回错误码
 */
int init_mem_pool(LPMEMPOOL pstMemPool, U32 dwBlockSize);

/**
 * 创建内存池
 * @param pstMemPool 被初始化的内存池
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回创建的内存池，失败返回NULL
 */
LPMEMPOOL create_mem_pool(U32 dwBlockSize);

/**
 * 释放内存池所占内存空间
 * @param pstMemPool 要释放的内存池
 */
void free_mem_pool(LPMEMPOOL pstMemPool);

/**
 * 清理内存池所分配内存空间，但不释放内存池对象所占空间
 * @param pstMemPool 要清空的内存池
 */
void clear_mem_pool(LPMEMPOOL pstMemPool);

/**
 * 从内存池中分配内存
 * @param pstMemPool 内存池对象
 * @return 返回分配的内存，若分配不成功，返回NULL
 */
void* mem_pool_malloc(LPMEMPOOL pstMemPool);

/**
 * 将内存释放给内存池
 * @param pstPool 内存池对象
 * @param pstMem 要释放的内存
 */
void mem_pool_free(LPMEMPOOL pstMemPool, void* pstMem);

#ifdef __cplusplus
}
#endif

#endif
