/*
 * @(#) ShmTableTest.c Created on 2008-03-27 共享内存式hashtable
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "shm/ShmTable.h"
#include "ShmTableTest.h"
#include "comm/ErrorCode.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN    32
#define MAX_DATA_NUM    32

struct tagShtData
{
    int iID;
    char szName[MAX_NAME_LEN];
    int iMoney;
    U8 bStatus;
    char szNickName[MAX_NAME_LEN];
};

typedef struct tagShtData   SHTDATA;
typedef struct tagShtData*  LPSHTDATA;

int sht_data_equal(void* pstData1, void* pstData2)
{
    if (pstData1 == pstData2) return 0;

    if (NULL == pstData1 || NULL == pstData2) return -1;

    return ((LPSHTDATA)pstData1)->iID - ((LPSHTDATA)pstData2)->iID;
}

U32 sht_data_hash(void* pstData)
{
    if (NULL == pstData) return 0;

    return ((LPSHTDATA)pstData)->iID % 32;
}

void print_sht_data(LPSHTDATA pstData)
{
    if (NULL == pstData) return;

    printf("id=%d,name=%s,nickname=%s,money=%d,status=%d\n", pstData->iID, pstData->szName,
           pstData->szNickName, pstData->iMoney, pstData->bStatus);
}

void print_shm_table(LPSHMTABLE pstTable, int iPos)
{
    if (NULL == pstTable || NULL == pstTable->pstTableData) return;
    if (iPos >= 0) shm_table_begin_iteration(pstTable, iPos);
    printf("table number:%d\n", pstTable->pstTableData->dwNum);

    LPSHTDATA pstData = (LPSHTDATA)shm_table_next(pstTable);
    while (pstData != NULL)
    {
        print_sht_data(pstData);
        pstData = (LPSHTDATA)shm_table_next(pstTable);
    }
}

int get_shm_table_size()
{
    return calc_shm_table_mem_size(sizeof(SHTDATA), MAX_DATA_NUM);
}

int attach_shm_table_by_id(LPSHMTABLE pstShmTable, int iQueueID)
{
    void* p;
    int iShmID;
    int iSize = get_shm_table_size();

    /* 创建共享内存 */
    iShmID = shmget(iQueueID, iSize, SHM_DFT_ACCESS);

    if (iShmID < 0)
    {
        return ERROR_SHM_GET_FAILURE;
    }

    /* 共享内存首地址 */
    p = shmat(iShmID, NULL, 0);

    if (p == (void*)-1) return ERROR_SHM_ATTACH_FAILURE;

    return attach_shm_table(pstShmTable, (char*)p, iSize, sizeof(SHTDATA), MAX_DATA_NUM, sht_data_equal, sht_data_hash);
}

int test_init_shm_table(int iQueueID)
{
    void* p;
    int iRet;
    struct shmid_ds stBuf;
    int iShmID;
    SHMTABLE stShmTable;

    /* 创建共享内存 */
    iShmID = shmget(iQueueID, get_shm_table_size(), SHM_DFT_ACCESS | IPC_CREAT);

    if (iShmID < 0)
    {
        return ERROR_SHM_GET_FAILURE;
    }

    /* 共享内存首地址 */
    p = shmat(iShmID, NULL, 0);

    if (p == (void*)-1) return ERROR_SHM_ATTACH_FAILURE;

    /* 获取共享内存 */
    iRet = shmctl(iShmID, IPC_STAT, &stBuf);
    if (iRet < 0) return ERROR_SHM_CTL_STAT_FAILURE;

    /* 检查分配的共享内存大小 */
    /*if (stBuf.shm_segsz != sizeof(MSGQUEUECHANNELDATA)) return ERROR_SHM_ALLOCATE_FAILURE;*/
    //if (stBuf.shm_segsz < sizeof(MSGQUEUECHANNELDATA)) return ERROR_SHM_ALLOCATE_FAILURE;
    iRet = init_shm_table(&stShmTable, (char*)p, stBuf.shm_segsz, sizeof(SHTDATA), MAX_DATA_NUM, sht_data_equal, sht_data_hash);

    return iRet;
}

int test_shm_table_set(int iQueueID, int iID)
{
    SHMTABLE stShmTable;
    SHTDATA stShmData;
    LPSHTDATA pstShmData;

    int iRet = attach_shm_table_by_id(&stShmTable, iQueueID);

    if (iRet != 0) return iRet;

    // 添加数据
    stShmData.iID = iID;

    pstShmData = shm_table_add(&stShmTable, &stShmData);

    if (NULL == pstShmData) return 1;

    pstShmData->iID = stShmData.iID;
    sprintf(pstShmData->szName, "xy_%d", iID);
    pstShmData->iMoney = 1000 + iID;
    pstShmData->bStatus = iID;
    sprintf(pstShmData->szNickName, "xy_%d", iID);;

    print_shm_table(&stShmTable, 0);

    return 0;
}

int test_shm_table_get(int iQueueID, int iID)
{
    SHMTABLE stShmTable;
    SHTDATA stShmData;
    LPSHTDATA pstShmData;

    int iRet = attach_shm_table_by_id(&stShmTable, iQueueID);

    if (iRet != 0) return iRet;

    // 删除数据
    stShmData.iID = iID;
    pstShmData = shm_table_get(&stShmTable, &stShmData);

    if (pstShmData != NULL)
    {
        printf("get:");
        print_sht_data(pstShmData);
    }

    return 0;
}

int test_shm_table_remove(int iQueueID, int iID)
{
    SHMTABLE stShmTable;
    SHTDATA stShmData;
    LPSHTDATA pstShmData;

    int iRet = attach_shm_table_by_id(&stShmTable, iQueueID);

    if (iRet != 0) return iRet;

    // 删除数据
    stShmData.iID = iID;
    pstShmData = shm_table_remove(&stShmTable, &stShmData);

    if (pstShmData != NULL)
    {
        printf("delete:");
        print_sht_data(pstShmData);
    }

    print_shm_table(&stShmTable, 0);

    return 0;
}

int test_shm_table_print(int iQueueID)
{
    SHMTABLE stShmTable;

    int iRet = attach_shm_table_by_id(&stShmTable, iQueueID);

    if (iRet != 0) return iRet;

    print_shm_table(&stShmTable, 0);

    return 0;
}

/**
 * 测试在迭代过程中删除元素
 */
int test_shm_table_iteration_on_delete(int iQueueID, int iPos)
{
    SHMTABLE stShmTable;
    LPSHTDATA pstShmData;
    SHTDATA stShmData;

    int iRet = attach_shm_table_by_id(&stShmTable, iQueueID);

    if (iRet != 0) return iRet;

    //print_shm_table(&stShmTable, 0);

    /* 添加id为123, 27, 155, 187的节点，这些节点的hash=27，保证它们在一个链上，遍历过程中，删除123，155 */

    iRet = shm_table_begin_iteration(&stShmTable, iPos);

    if (iRet != 0) return iRet;

    /* 先打印下iPos后续所有节点，方便对比 */
    print_shm_table(&stShmTable, 0);

    /* 再从iPos节点开始遍历 */
    iRet = shm_table_begin_iteration(&stShmTable, iPos);

    if (iRet != 0) return iRet;

    /* 之前设置的iPos(比如设置iPos=27)，保证本次遍历到123这个节点 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    // 删除数据
    stShmData.iID = 123;
    pstShmData = shm_table_remove(&stShmTable, &stShmData);

    if (pstShmData != NULL)
    {
        printf("delete:");
        print_sht_data(pstShmData);
    }

    /* 再遍历 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    // 删除数据
    stShmData.iID = 155;
    pstShmData = shm_table_remove(&stShmTable, &stShmData);

    if (pstShmData != NULL)
    {
        printf("delete:");
        print_sht_data(pstShmData);
    }

    /* 再遍历 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    // 删除数据
    stShmData.iID = 187;
    pstShmData = shm_table_remove(&stShmTable, &stShmData);

    if (pstShmData != NULL)
    {
        printf("delete:");
        print_sht_data(pstShmData);
    }

    /* 再遍历 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    /* 再遍历 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    /* 再遍历 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    /* 再遍历 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    /* 打印当前遍历后的节点 */
    print_shm_table(&stShmTable, -1);

    /* 再遍历 */
    pstShmData = shm_table_next_loop(&stShmTable);
    if (pstShmData != NULL)
    {
        printf("Iterator:");
        print_sht_data(pstShmData);
    }

    return 0;
}

void test_shm_table(int iType, int iID)
{
    int iRet = -1;
    int iQueueID = 9400;

    switch (iType)
    {
    case 'i':
        iRet = test_init_shm_table(iQueueID);
        break;
    case 's':
        iRet = test_shm_table_set(iQueueID, iID);
        break;
    case 'g':
        iRet = test_shm_table_get(iQueueID, iID);
        break;
    case 'r':
        iRet = test_shm_table_remove(iQueueID, iID);
        break;
    case 'p':
        iRet = test_shm_table_print(iQueueID);
        break;
    case 't':
        iRet = test_shm_table_iteration_on_delete(iQueueID, iID);
        break;
    }

    if (iRet != 0)
    {
        printf("failed:%d\n", iRet);
    }
}

