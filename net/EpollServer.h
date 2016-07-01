/*
 * @(#) EpollServer.h Created on 2008-03-27
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include "mem/FixMemPool.h"
#include <sys/epoll.h>

#define MAX_CS_FD_NUM       2048

typedef struct tagEpollClient   EPOLLCLIENT;
typedef struct tagEpollClient*  LPEPOLLCLIENT;

typedef struct tagEpollServer   EPOLLSERVER;
typedef struct tagEpollServer*  LPEPOLLSERVER;

/**
 * 处理接收到的epoll消息
 */
typedef int (*LPFUNCHANDLEPOLLMSG)(LPEPOLLSERVER pstEpollServer, LPEPOLLCLIENT pstEpollClient);

/**
 * EPOLL客户数据
 */
struct tagEpollClient
{
    int iSocketFD;                          /* 客户端套接字 */
    U32 dwIP;                               /* 客户端IP */
    U16 nPort;                              /* 客户端端口 */
    //U32 dwMsgLen;                           /* 消息的总长度 */
    U32 dwRecvLen;                          /* 已接收的消息长度 */
    U32 dwClientMemIdx;                     /* 客户端数据内存索引 */
    U32 dwMsgDataMemIdx;                    /* 消息内容内存索引 */
    char* pszRecvMsgData;                   /* 消息内容 */
};

/**
 * EPOLLCLIENT
 */
struct tagEpollServer
{
    int iEpollFD;                                   /* epoll服务描述符 */
    U32 dwMaxMsgLen;                                /* 最大消息长度 */
    FIXMEMPOLL stClientMemPool;                     /* 客户端数据对象内存分配池 */
    FIXMEMPOLL stMsgDataMemPool;                    /* 接收消息的内存分配池 */
    struct epoll_event stEvent;                     /* epoll server事件 */
    struct epoll_event astEvents[MAX_CS_FD_NUM];    /* epoll事件列表 */
    EPOLLCLIENT stEpollInfo;                        /* epoll的数据信息 */
    LPFUNCHANDLEPOLLMSG pstFuncHandleMsg;           /* 处理接收到的epoll消息 */
};

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 初始化epoll服务器
 * @param pstEpollServer 被初始化的epoll服务器
 * @param szIpAddr 服务器绑定的IP地址
 * @param nPort 服务器监听端口
 * @param dwMaxMsgLen 最大消息长度
 * @param szClientMem 客户端数据共享内存地址
 * @param dwClientMemLen 客户端数据共享内存大小
 * @param szMsgDataMem 接收消息的共享内存地址
 * @param dwMsgDataMemLen 接收消息的共享内存大小
 * @param pstFuncHandleMsg 接收消息处理函数
 * @return 成功返回0，失败返回错误码
 */
int init_epoll_server(LPEPOLLSERVER pstEpollServer, const char* szIpAddr, U16 nPort, U32 dwMaxMsgLen,
                      char* szClientMem, U32 dwClientMemLen, char* szMsgDataMem, U32 dwMsgDataMemLen,
                      LPFUNCHANDLEPOLLMSG pstFuncHandleMsg);

/**
 * 释放epoll服务器占用的内存，并关闭套接字
 * @param pstEpollServer epoll服务器对象
 * @return 成功返回0，失败返回错误码
 */
int free_epoll_server(LPEPOLLSERVER pstEpollServer);

/**
 * 处理客户端连接及接收来自客户端的消息
 * @param pstEpollServer epoll服务器对象
 * @param iWaitTime 等待事件的时间，单位：毫秒
 * @return 成功返回0，失败返回错误码
 */
int epoll_server_process(LPEPOLLSERVER pstEpollServer, int iWaitTime);

/**
 * 向客户端发送消息
 * @param pstEpollServer epoll服务器对象
 * @param iClientIndex epoll客户端索引
 * @param iClientFD 客户端socket套接字
 * @param dwClientIP 客户端IP地址
 * @param nClientPort 客户端端口号
 * @param pszMsg 要发送的消息
 * @param dwMsgLen 要发送的消息长度
 * @return 成功返回0，失败返回错误码
 */
int epoll_write_process(LPEPOLLSERVER pstEpollServer, int iClientIndex, int iClientFD, U32 dwClientIP,
                        U16 nClientPort, const char* pszMsg, U32 dwMsgLen);

#ifdef __cplusplus
}
#endif

#endif

