/**
 * 共享内存式消息队列 - 通道信息
 * 特别注意：因消息队列实现方式中采用了栈中的静态变量，所以一个进程只可以绑定一个channel！！！
 */

#ifndef FMSG_CHANNEL_H
#define FMSG_CHANNEL_H

#include "comm/BaseDef.h"

#define MAX_MSG_CHANNEL_NUM 64  /* 最大通道数量 */

/**
 * 消息通道地址
 */
struct tagMsgQueueAddress
{
    U32 dwSrcAddress;           /* 通道源地址 */
    U32 dwDestAddress;          /* 通道目的地址 */
};

typedef struct tagMsgQueueAddress   MSGQUEUEADDRESS;
typedef struct tagMsgQueueAddress*  LPMSGQUEUEADDRESS;

/**
 * 消息队列信息
 */
struct tagMsgQueueInfo
{
    /* 读写信息 */
    U32 dwWritePos;         /* 写数据指针偏移 */
    U32 dwWriteNum;         /* 写入的数据包总数量 */
    U32 dwReadPos;          /* 读数据指针偏移 */
    U32 dwReadNum;          /* 读出的数据包总数量 */
    /* 大小信息 */
    U32 dwQueueSize;        /* 消息队列共享内存总大小 */
};

typedef struct tagMsgQueueInfo   MSGQUEUEINFO;
typedef struct tagMsgQueueInfo*  LPMSGQUEUEINFO;

/**
 * 消息队列通道信息
 */
struct tagMsgQueueChannelInfo
{
    S32 iShmID;                 /* 通道共享内存ID */
    /* 地址信息 */
    U32 dwSrcAddress;           /* 通道源地址 */
    U32 dwDestAddress;          /* 通道目的地址 */
    /* 读写信息 */
    MSGQUEUEINFO stWriteQueue;  /* 通道源存消息队列信息 */
    MSGQUEUEINFO stReadQueue;   /* 通道源读消息队列信息 */
    /* 索引信息 */
    U8 bIndex;                  /* 通道地址索引 */
};

typedef struct tagMsgQueueChannelInfo   MSGQUEUECHANNELINFO;
typedef struct tagMsgQueueChannelInfo*  LPMSGQUEUECHANNELINFO;

/**
 * 消息队列通道信息
 */
struct tagMsgQueueChannelData
{
    U8 bNum;                                                    /* 通道数量 */
    MSGQUEUECHANNELINFO astChannelInfo[MAX_MSG_CHANNEL_NUM];    /* 通道信息 */
};

typedef struct tagMsgQueueChannelData   MSGQUEUECHANNELDATA;
typedef struct tagMsgQueueChannelData*  LPMSGQUEUECHANNELDATA;

/**
 * 消息队列地址信息，因有指针，只可以保存在缓存中
 */
struct tagMsgQueueCacheInfo
{
    S32 iShmID;                     /* 通道共享内存ID */
    char* pszWriteMsgQueue;         /* 通道源存消息队列首地址 */
    char* pszReadMsgQueue;          /* 通道源读消息队列首地址 */
};

typedef struct tagMsgQueueCacheInfo   MSGQUEUECACHEINFO;
typedef struct tagMsgQueueCacheInfo*  LPMSGQUEUECACHEINFO;

/**
 * 消息队列地址信息，因有指针，只可以保存在缓存中
 */
struct tagMsgQueueCacheData
{
    U8 bNum;                                                /* 通道数量 */
    MSGQUEUECACHEINFO astCacheInfo[MAX_MSG_CHANNEL_NUM];    /* 通道消息对了地址信息 */
};

typedef struct tagMsgQueueCacheData   MSGQUEUECACHEDATA;
typedef struct tagMsgQueueCacheData*  LPMSGQUEUECACHEDATA;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 通过共享内存名称，为消息队列通道数据列表创建共享内存
 * @param szName 共享内存名称，若为NULL，则采用当前时间作为名称创建共享内存
 * @param piQueueID 返回共享内存的key
 * @param piShmID 返回共享内存的shmid
 * @return 成功返回0，失败返回错误码
 */
int msg_queue_init_channel(const char* szName, int* piQueueID, int* piShmID);

/**
 * 通过指定的ID，为消息队列通道数据列表创建共享内存
 * @param iQueueID 指定的共享内存的key
 * @param piShmID 返回共享内存的shmid
 * @return 成功返回0，失败返回错误码
 */
int msg_queue_init_channel_by_id(int iQueueID, int* piShmID);

/**
 * 根据共享内存的key，获得消息队列通道数据列表共享内存地址，并返回
 * @param iQueueID 消息队列通道数据列表共享内存key
 * @param piRet 保存错误码的指针
 * @return 成功返回消息队列通道数据列表共享内存地址，失败返回NULL，并将错误码保存在piRet中
 */
LPMSGQUEUECHANNELDATA msg_queue_get_channel_by_id(int iQueueID, int* piRet);

/**
 * 根据地址查找消息通道
 * @param pstChannelData 通道数据列表
 * @param dwAddr1 通道地址
 * @param dwAddr2 通道地址
 * @return 返回查找到的通道对象。若未找到，返回0
 */
LPMSGQUEUECHANNELINFO msg_queue_find_channel(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwAddr1, U32 dwAddr2);

/**
 * 创建消息通道
 * @param pstChannelData 通道数据列表
 * @param dwAddr1 通道地址1
 * @param dwAddr2 通道地址2
 * @param dwWriteQueueSize 通道地址1存消息队列大小
 * @param dwReadQueueSize 通道地址1读消息队列大小
 * @return 若成功返回0，失败返回各种错误码
 */
int msg_queue_create_channel(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwAddr1, U32 dwAddr2, U32 dwWriteQueueSize, U32 dwReadQueueSize);

/**
 * 返回通道数据存储地址
 * @param pstChannelInfo 通道信息
 * @param dwSrcAddress 源地址
 * @param dwDestAddress 目的地址
 * @return 成功后，返回通道数据存储地址，失败返回NULL
 */
char* msg_queue_find_queue_address(LPMSGQUEUECHANNELINFO pstChannelInfo, U32 dwSrcAddress, U32 dwDestAddress);

#ifdef __cplusplus
}
#endif

#endif

