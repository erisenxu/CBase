/*
 * @(#) FixMemPool.h Created on 2008-03-27 共享内存式 - 固定大小的内存池
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef FIX_MEM_POOL_H
#define FIX_MEM_POOL_H

#include "comm/BaseDef.h"

/**
 * 内存池结构体
 */
struct tagFixMemPool
{
    U32 dwBlockSize;        /* 内存块大小 */
    U32 dwFreeNum;          /* 空闲内存数量 */
    U32 dwBlockNum;         /* 最大内存块数量 */
    U32* piBlock;           /* 内存块 */
    char* pszBlockMem;      /* 内存块指针 */
    //char* pszMemHead;
};

typedef struct tagFixMemPool    FIXMEMPOOL;
typedef struct tagFixMemPool*   LPFIXMEMPOOL;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 使用共享内存初始化内存池
 * @param pstFixMemPool 被初始化的内存池
 * @param pszMem 共享内存
 * @param dwMemLen 共享内存大小
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回0，失败返回错误码
 */
int init_fix_mem_pool(LPFIXMEMPOOL pstFixMemPool, char* pszMem, U32 dwMemLen, U32 dwBlockSize);

/**
 * 内存池attach到共享内存，但不修改内存池中的初始值
 * @param pstFixMemPool 被attach的内存池
 * @param pszMem 共享内存
 * @param dwMemLen 共享内存大小
 * @param dwBlockSize 每块内存的大小
 * @return 成功返回0，失败返回错误码
 */
int attach_fix_mem_pool(LPFIXMEMPOOL pstFixMemPool, char* pszMem, U32 dwMemLen, U32 dwBlockSize);

/**
 * 从共享内存池中分配内存
 * @param pstFixMemPool 共享内存池对象
 * @param piIndex 成功时，返回分配的内存块的索引
 * @return 成功返回分配的内存，如果内存不足返回NULL
 */
char* fix_mem_pool_malloc(LPFIXMEMPOOL pstFixMemPool, int* piIndex);

/**
 * 释放从共享内存池中分配的内存
 * @param pstFixMemPool 共享内存池对象
 * @param iBlockIndex 要释放的内存块索引
 * @return 成功返回0，失败返回错误码
 */
int fix_mem_pool_free(LPFIXMEMPOOL pstFixMemPool, int iBlockIndex);

/**
 * 获取内存池指定索引处的内存
 * @param pstFixMemPool 共享内存池对象
 * @param iBlockIndex 要释放的内存块索引
 * @param piFree 若内存被占用，返回0，未被占用返回1。若函数调用失败，piFree无意义
 * @return 成功返回索引处的内存，失败NULL
 */
char* fix_mem_pool_at(LPFIXMEMPOOL pstFixMemPool, int iBlockIndex, int* piFree);

#ifdef __cplusplus
}
#endif

#endif

