/**
 * 共享内存式消息队列 - 通道信息
 */

#include "FMsgChannel.h"
#include "comm/ErrorCode.h"
#include "comm/BaseDef.h"
#include "comm/FileLib.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static MSGQUEUECACHEDATA gs_stMsgQueueCacheData;

/**
 * 为消息队列通道数据列表创建共享内存
 * @param szName 共享内存名称，若为NULL，则采用当前时间作为名称创建共享内存
 * @param piQueueID 返回共享内存的key
 * @param piShmID 返回共享内存的shmid
 * @return 成功返回0，失败返回错误码
 */
int msg_queue_init_channel(const char* szName, int* piQueueID, int* piShmID)
{
    char szCurPath[MAX_PATH];
    char szFileName[MAX_PATH];
    char* pszCurPath;
    int fd;

    if (NULL == piQueueID || NULL == piShmID) return ERROR_INPUT_PARAM_NULL;

    pszCurPath = getcwd(szCurPath, sizeof(szCurPath));

    if (NULL == pszCurPath)
    {
        return ERROR_GET_CURRENT_PATH;
    }

    /* 创建文件 */
    if (NULL == szName)
    {
        snprintf(szFileName, sizeof(szFileName), "%s/.msgq.%d", pszCurPath, time(NULL));
    }
    else
    {
        snprintf(szFileName, sizeof(szFileName), "%s/.msgq.%s", pszCurPath, szName);
    }

    fd = open(szFileName, O_CREAT | O_RDWR, 0666);

    if (fd < 0)
    {
        return ERROR_FILE_OPEN_FAILURE;
    }

    /* 关闭文件 */
    close(fd);

    /* 获得文件的key值 */
    *piQueueID = file_to_key(szFileName);

    if (*piQueueID < 0)
    {
        return ERROR_FILE_TOKEN_FAILURE;
    }

    return msg_queue_init_channel_by_id(*piQueueID, piShmID);
}

/**
 * 通过指定的ID，为消息队列通道数据列表创建共享内存
 * @param iQueueID 指定的共享内存的key
 * @param piShmID 返回共享内存的shmid
 * @return 成功返回0，失败返回错误码
 */
int msg_queue_init_channel_by_id(int iQueueID, int* piShmID)
{
    void* p;
    int iRet;
    struct shmid_ds stBuf;

    if (NULL == piShmID) return ERROR_INPUT_PARAM_NULL;

    /* 创建共享内存 */
    *piShmID = shmget(iQueueID, sizeof(MSGQUEUECHANNELDATA), SHM_DFT_ACCESS | IPC_CREAT);

    if (*piShmID < 0)
    {
        return ERROR_SHM_GET_FAILURE;
    }

    /* 共享内存首地址 */
    p = shmat(*piShmID, NULL, 0);

    if (p == (void*)-1) return ERROR_SHM_ATTACH_FAILURE;

    /* 获取共享内存 */
    iRet = shmctl(*piShmID, IPC_STAT, &stBuf);
    if (iRet < 0) return ERROR_SHM_CTL_STAT_FAILURE;

    /* 检查分配的共享内存大小 */
    /*if (stBuf.shm_segsz != sizeof(MSGQUEUECHANNELDATA)) return ERROR_SHM_ALLOCATE_FAILURE;*/
    if (stBuf.shm_segsz < sizeof(MSGQUEUECHANNELDATA)) return ERROR_SHM_ALLOCATE_FAILURE;

    /* 初始化内存 */
    memset(p, stBuf.shm_segsz, 0);

    return 0;
}

/**
 * 根据共享内存的key，获得消息队列通道数据列表共享内存地址，并返回
 * @param iQueueID 消息队列通道数据列表共享内存key
 * @param piRet 保存错误码的指针
 * @return 成功返回消息队列通道数据列表共享内存地址，失败返回NULL，并将错误码保存在piRet中
 */
LPMSGQUEUECHANNELDATA msg_queue_get_channel_by_id(int iQueueID, int* piRet)
{
    int iShmID;
    void* p;
    int iRet;
    U8 i;
    LPMSGQUEUECHANNELDATA pstChannelData;
    struct shmid_ds stBuf;

    /* 附接到共享内存 */
    iShmID = shmget(iQueueID, 0, SHM_DFT_ACCESS);

    if (iShmID < 0)
    {
        if (NULL != piRet) *piRet = ERROR_SHM_GET_FAILURE;
        return NULL;
    }

    /* 共享内存首地址 */
    p = shmat(iShmID, NULL, 0);

    if (p == (void*)-1)
    {
        if (NULL != piRet) *piRet = ERROR_SHM_ATTACH_FAILURE;
        return NULL;
    }

    /* 获取共享内存 */
    iRet = shmctl(iShmID, IPC_STAT, &stBuf);
    if (iRet < 0)
    {
        if (NULL != piRet) *piRet = ERROR_SHM_CTL_STAT_FAILURE;
        return NULL;
    }

    /* 检查分配的共享内存大小 */
    /*if (stBuf.shm_segsz != sizeof(MSGQUEUECHANNELDATA)) return ERROR_SHM_ALLOCATE_FAILURE;*/
    if (stBuf.shm_segsz < sizeof(MSGQUEUECHANNELDATA))
    {
        if (NULL != piRet) *piRet = ERROR_SHM_ALLOCATE_FAILURE;
        return NULL;
    }

    pstChannelData = (LPMSGQUEUECHANNELDATA)p;

    /* 数据是否正确？ */
    if (pstChannelData->bNum > MAX_MSG_CHANNEL_NUM)
    {
        if (NULL != piRet) *piRet = ERROR_MSG_Q_CHANNEL_NUM_BOUND;
        return NULL;
    }

    /* 更新下通道指针(缓存数据，方便使用) */
    gs_stMsgQueueCacheData.bNum = pstChannelData->bNum;
    for (i = 0; i < pstChannelData->bNum; i++)
    {
        /* attach到共享内存 */
        p = shmat(pstChannelData->astChannelInfo[i].iShmID, NULL, 0);
        if (p == (void*)-1)
        {
            if (NULL != piRet) *piRet = ERROR_SHM_ATTACH_FAILURE;
            return NULL;
        }

        /* 检查共享内存大小 */
        iRet = shmctl(pstChannelData->astChannelInfo[i].iShmID, IPC_STAT, &stBuf);
        if (iRet < 0)
        {
            if (NULL != piRet) *piRet = ERROR_SHM_CTL_STAT_FAILURE;
            return NULL;
        }

        pstChannelData->astChannelInfo[i].bIndex = i;
        gs_stMsgQueueCacheData.astCacheInfo[i].iShmID = pstChannelData->astChannelInfo[i].iShmID;
        gs_stMsgQueueCacheData.astCacheInfo[i].pszWriteMsgQueue = (char*)p;
        gs_stMsgQueueCacheData.astCacheInfo[i].pszReadMsgQueue = ((char*)p) +
            pstChannelData->astChannelInfo[i].stWriteQueue.dwQueueSize;
    }

    if (NULL != piRet) *piRet = 0;
    return pstChannelData;
}

/**
 * 根据地址查找消息通道
 * @param pstChannelData 通道数据列表
 * @param dwAddr1 通道地址
 * @param dwAddr2 通道地址
 * @return 返回查找到的通道对象
 */
LPMSGQUEUECHANNELINFO msg_queue_find_channel(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwAddr1, U32 dwAddr2)
{
    U8 i;
    if (NULL == pstChannelData) return NULL;

    for (i = 0; i < pstChannelData->bNum; i++)
    {
        if ((pstChannelData->astChannelInfo[i].dwSrcAddress == dwAddr1 && pstChannelData->astChannelInfo[i].dwDestAddress == dwAddr2) ||
            (pstChannelData->astChannelInfo[i].dwSrcAddress == dwAddr2 && pstChannelData->astChannelInfo[i].dwDestAddress == dwAddr1))
        {
            return &pstChannelData->astChannelInfo[i];
        }
    }

    return NULL;
}

/**
 * 创建消息通道
 * @param pstChannelData 通道数据列表
 * @param dwAddr1 通道地址1
 * @param dwAddr2 通道地址2
 * @param dwWriteQueueSize 通道地址1存消息队列大小
 * @param dwReadQueueSize 通道地址1读消息队列大小
 * @return 若成功返回0，失败返回各种错误码
 */
int msg_queue_create_channel(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwAddr1, U32 dwAddr2, U32 dwWriteQueueSize, U32 dwReadQueueSize)
{
    int iShmID;
    void* p;
    struct shmid_ds stBuf;
    int iRet;

    if (NULL == pstChannelData) return ERROR_INPUT_PARAM_NULL;

    if (pstChannelData->bNum >= MAX_MSG_CHANNEL_NUM) return ERROR_ARRAY_LEN_NOT_ENOUGH;

    /* 创建共享内存 */
    iShmID = shmget(0, dwReadQueueSize + dwWriteQueueSize, SHM_DFT_ACCESS | IPC_CREAT);

    if (iShmID < 0) return ERROR_SHM_GET_FAILURE;

    /* 共享内存首地址 */
    p = shmat(iShmID, NULL, 0);

    if (p == (void*)-1) return ERROR_SHM_ATTACH_FAILURE;

    /* 获取共享内存 */
    iRet = shmctl(iShmID, IPC_STAT, &stBuf);
    if (iRet < 0) return ERROR_SHM_CTL_STAT_FAILURE;

    /* 检查分配的共享内存大小 */
    /*if (stBuf.shm_segsz != dwReadQueueSize + dwWriteQueueSize) return ERROR_SHM_ALLOCATE_FAILURE;*/
    if (stBuf.shm_segsz < dwReadQueueSize + dwWriteQueueSize) return ERROR_SHM_ALLOCATE_FAILURE;

    /* 初始化内存 */
    memset(p, stBuf.shm_segsz, 0);
    memset(&pstChannelData->astChannelInfo[pstChannelData->bNum], 0, sizeof(pstChannelData->astChannelInfo[0]));

    pstChannelData->astChannelInfo[pstChannelData->bNum].iShmID = iShmID;
    pstChannelData->astChannelInfo[pstChannelData->bNum].dwSrcAddress = dwAddr1;
    pstChannelData->astChannelInfo[pstChannelData->bNum].dwDestAddress = dwAddr2;
    pstChannelData->astChannelInfo[pstChannelData->bNum].stWriteQueue.dwQueueSize = dwWriteQueueSize;
    pstChannelData->astChannelInfo[pstChannelData->bNum].stReadQueue.dwQueueSize = dwReadQueueSize;

    /* 缓存数据 */
    gs_stMsgQueueCacheData.astCacheInfo[pstChannelData->bNum].iShmID = iShmID;
    gs_stMsgQueueCacheData.astCacheInfo[pstChannelData->bNum].pszWriteMsgQueue = (char*)p;
    gs_stMsgQueueCacheData.astCacheInfo[pstChannelData->bNum].pszReadMsgQueue = ((char*)p) + dwWriteQueueSize;
    pstChannelData->astChannelInfo[pstChannelData->bNum].bIndex = pstChannelData->bNum;

    pstChannelData->bNum++;
    gs_stMsgQueueCacheData.bNum = pstChannelData->bNum;

    return 0;
}

/**
 * 返回通道数据存储地址
 * @param pstChannelInfo 通道信息
 * @param dwSrcAddress 源地址
 * @param dwDestAddress 目的地址
 * @return 成功后，返回通道数据存储地址，失败返回NULL
 */
char* msg_queue_find_queue_address(LPMSGQUEUECHANNELINFO pstChannelInfo, U32 dwSrcAddress, U32 dwDestAddress)
{
    char* szQueue = NULL;
    LPMSGQUEUECACHEINFO pstCacheInfo = NULL;

    if (NULL == pstChannelInfo) return szQueue;

    do
    {
        U8 i;

        if (pstChannelInfo->bIndex < gs_stMsgQueueCacheData.bNum)
        {
            if (pstChannelInfo->iShmID == gs_stMsgQueueCacheData.astCacheInfo[pstChannelInfo->bIndex].iShmID)
            {
                /* 找到后，不继续找了 */
                pstCacheInfo = &gs_stMsgQueueCacheData.astCacheInfo[pstChannelInfo->bIndex];
                break;
            }
        }

        /* 没有找到，遍历所有 */
        for (i = 0; i < gs_stMsgQueueCacheData.bNum; i++)
        {
            if (pstChannelInfo->iShmID == gs_stMsgQueueCacheData.astCacheInfo[i].iShmID)
            {
                pstCacheInfo = &gs_stMsgQueueCacheData.astCacheInfo[i];
                break;
            }
        }
    } while(0);

    if (pstCacheInfo != NULL)
    {
        if (pstChannelInfo->dwSrcAddress == dwSrcAddress)
        {
            szQueue = pstCacheInfo->pszWriteMsgQueue;
        }
        else if (pstChannelInfo->dwSrcAddress == dwDestAddress)
        {
            szQueue = pstCacheInfo->pszReadMsgQueue;
        }
    }

    return szQueue;
}

