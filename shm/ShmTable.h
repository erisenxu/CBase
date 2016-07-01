/*
 * @(#) ShmTable.h Created on 2008-03-27 共享内存式hashtable
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef SHM_TABLE_H
#define SHM_TABLE_H

#include "mem/FixMemPool.h"

/**
 * hashtable比较函数，相同返回0
 */
typedef int (*LPFUNCSHMTABLEEQUAL)(void* pstData1, void* pstData2);

/**
 * 求hash的函数
 */
typedef U32 (*LPFUNCSHMTABLEHASH)(void* pstData);

/**
 * hashtable节点结构体
 */
struct tagShmTableNode
{
    int iData;      /* 共享内存数据索引 */
    int iNext;      /* 下一个数据存放的共享数据索引 */
};

typedef struct tagShmTableNode  SHMTABLENODE;
typedef struct tagShmTableNode* LPSHMTABLENODE;

/**
 * hashtable数据信息
 */
struct tagShmTableData
{
    U32 dwNum;              /* 当前存储的数据的数量 */
    char* szData;           /* 数据区，共享内存地址 */
};

typedef struct tagShmTableData  SHMTABLEDATA;
typedef struct tagShmTableData* LPSHMTABLEDATA;

/**
 * hashtable迭代信息
 */
struct tagShmTableInterator
{
    U32 dwPos;                      /* 当前迭代所在的数组位置 */
    LPSHMTABLENODE pstShmNode;      /* 当前迭代所在的节点 */
};

typedef struct tagShmTableInterator  SHMTABLEINTERATOR;
typedef struct tagShmTableInterator* LPSHMTABLEINTERATOR;

/**
 * hashtable结构体
 */
struct tagShmTable
{
    U32 dwMemSize;                      /* 数据区大小 */
    U32 dwDataSize;                     /* 每个数据大小 */
    U32 dwMaxDataNum;                   /* 数据最大数量 */
    LPSHMTABLEDATA pstTableData;        /* 数据区，共享内存地址 */
    FIXMEMPOLL stIndexMemPool;          /* 数据索引内存分配区 */
    FIXMEMPOLL stDataMemPool;           /* 数据内存分配区 */
    SHMTABLEINTERATOR stInterator;      /* 迭代器 */
    LPFUNCSHMTABLEEQUAL equal;          /* 数据相对函数 */
    LPFUNCSHMTABLEHASH hash;            /* 数据hash算法函数 */
};

typedef struct tagShmTable     SHMTABLE;
typedef struct tagShmTable*    LPSHMTABLE;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 计算保存数据，hashtable需要的内存大小
 * @param dwDataSize 要保存的单个数据的大小
 * @param dwMaxDataNum 要保存的数据的数量
 * @return 返回hashtable需要的最小内存大小
 */
int calc_shm_table_mem_size(U32 dwDataSize, U32 dwMaxDataNum);

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
    U32 dwMaxDataNum, LPFUNCSHMTABLEEQUAL pstFuncEqual, LPFUNCSHMTABLEHASH pstFuncHash);

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
    U32 dwMaxDataNum, LPFUNCSHMTABLEEQUAL pstFuncEqual, LPFUNCSHMTABLEHASH pstFuncHash);

/**
 * 添加数据到hashtable中。注意本函数只返回添加后的指针，不会将pstData的数据拷贝到指针中
 * @param pstShmTable hashtable表
 * @param pstData 要添加的数据，只需要填写pstFuncHash及pstFuncEqual相等的数据即可
 * @return 成功返回要添加的数据的数据区指针，失败返回NULL。如果hashtable中已经有了与pstData相等的数据，则返回hashtable中保存的那份数据
 */
void* shm_table_add(LPSHMTABLE pstShmTable, void* pstData);

/**
 * 从hashtable中查找与pstData相等的数据
 * @param pstShmTable hash表
 * @param pstData 要查找的数据，只需要填写pstFuncHash及pstFuncEqual相等的数据即可
 * @return 找到后返回与pstData相等的数据，没有找到，返回NULL
 */
void* shm_table_get(LPSHMTABLE pstShmTable, void* pstData);

/**
 * 从hashtable中删除与pstData相等的数据
 * @param pstShmTable hash表
 * @param pstData 要删除的数据，只需要填写pstFuncHash及pstFuncEqual相等的数据即可
 * @return 删除后返回与pstData相等的数据，没有找到要删除的数据，返回NULL
 */
void* shm_table_remove(LPSHMTABLE pstShmTable, void* pstData);

/**
 * 返回hash表中数据的数量
 * @param pstShmTable hash表
 * @return 返回hash表数量
 */
int shm_table_get_data_number(LPSHMTABLE pstShmTable);

/**
 * 返回hash表可以存储的最大数据数量
 * @param pstShmTable hash表
 * @return 返回hash表可以存储的最大数据数量
 */
int shm_table_get_max_data_number(LPSHMTABLE pstShmTable);

/**
 * 开始向后遍历hash表
 * @param pstShmTable hash表
 * @param dwPos 遍历hash表的起始位置
 * @return 成功返回0，失败返回错误码
 */
int shm_table_begin_iteration(LPSHMTABLE pstShmTable, U32 dwPos);

/**
 * 返回遍历hash表获得的当前有效节点数据指针
 * @param pstShmTable hash表
 * @return 若hash表遍历完，返回NULL，没有遍历完返回当前有效节点数据指针
 */
void* shm_table_next(LPSHMTABLE pstShmTable);

/**
 * 返回遍历hash表获得的当前有效节点数据指针。当遍历到最后节点时，从头开始遍历
 * @param pstShmTable hash表
 * @return 若hash表遍历完，返回NULL，没有遍历完返回当前有效节点数据指针
 */
void* shm_table_next_loop(LPSHMTABLE pstShmTable);

#ifdef __cplusplus
}
#endif

#endif

