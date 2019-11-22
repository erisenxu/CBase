/*
 * @(#) EpollServer.c Created on 2008-03-27
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "EpollServer.h"
#include "net.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"
#include "log/Logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h> /* 使用strerror必须加上这个头文件，否则会导致崩溃，:( */
#include <errno.h>
#include <unistd.h>
#include <time.h>

/**
 * 生成epoll客户端ID
 * @return 返回生成的ID
 */
static U64 gen_epoll_client_id()
{
    U64 ullID;
    U64 ullTime = time(NULL) & 0xFFFFFFFF;
    static U32 dwSequence = 0;

    ullID = ullTime << 32;
    ullID |= ((U64)dwSequence++ & 0xFFFFFFFF);

    return ullID;
}

/**
 * 新增epoll事件
 * @param pstEpollServer epoll服务器
 * @param iSock 套接字
 * @param ptr 指针数据
 * @return 成功返回0，失败返回错误码
 */
static int epoll_sever_add_event(LPEPOLLSERVER pstEpollServer, int iSock, void *ptr)
{
    int iBufSize = 1310720;

    LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == pstEpollServer) return ERROR_INPUT_PARAM_NULL;

    setsockopt(iSock, SOL_SOCKET, SO_SNDBUF, (void*)&iBufSize, sizeof(iBufSize));
    setsockopt(iSock, SOL_SOCKET, SO_RCVBUF, (void*)&iBufSize, sizeof(iBufSize));

    pstEpollServer->stEvent.events = EPOLLIN | EPOLLET;
    pstEpollServer->stEvent.data.ptr = ptr;

    if (epoll_ctl(pstEpollServer->iEpollFD, EPOLL_CTL_ADD, iSock, &pstEpollServer->stEvent) < 0)
    {
        LOG_ERROR("%s:%d, fail to epoll_ctl, errno=%d,%s", __FUNCTION__, __LINE__, errno, strerror(errno));
        return ERROR_EPOLL_CTL_ADD_FAIL;
    }

    return 0;
}

/**
 * 释放epoll客户端占用的内存并关闭客户端套接字
 * @param pstEpollServer Epoll服务器对象
 * @param pstEpollClient Epoll客户端对象
 */
static void free_epoll_client(LPEPOLLSERVER pstEpollServer, LPEPOLLCLIENT pstEpollClient)
{
    LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == pstEpollServer || NULL == pstEpollClient) return;

    if (pstEpollServer->pstFuncOnClientClose) pstEpollServer->pstFuncOnClientClose(pstEpollClient);

    close(pstEpollClient->iSocketFD);
    pstEpollClient->iSocketFD = -1;

#ifdef EPOLL_USE_FIX_MEM_POOL
    fix_mem_pool_free(&pstEpollServer->stMsgDataMemPool, pstEpollClient->dwMsgDataMemIdx);
#else
    mem_pool_free(&pstEpollServer->stMsgDataMemPool, pstEpollClient->pszRecvMsgData);
#endif
    fix_mem_pool_free(&pstEpollServer->stClientMemPool, (int)pstEpollClient->dwClientMemIdx);
}

/**
 * 接受客户端连接
 * @param pstEpollServer epoll服务器对象
 * @return 成功返回0，失败返回错误码
 */
static int epoll_server_accept_process(LPEPOLLSERVER pstEpollServer)
{
    struct sockaddr_in stSin;
    int iSinSize = sizeof(stSin);
    int iClientFD;
    int iRet;
    LPEPOLLCLIENT pstEpollClient;
    int iMemIndex;

    LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == pstEpollServer) return ERROR_INPUT_PARAM_NULL;

    /* 可能有多个客户端连接同时到来 */
    while(1)
    {
        iClientFD = accept(pstEpollServer->stEpollInfo.iSocketFD, (struct sockaddr *)&stSin, (socklen_t*)&iSinSize);

        if (-1 == iClientFD)
        {
            /* 所有的客户端连接都接收到了 */
            if (EAGAIN == errno || EWOULDBLOCK == errno)
            {
                break;
            }
            else
            {
                LOG_WARN("%s:%d, accept client error:%d, %s", __FUNCTION__, __LINE__, errno, strerror(errno));
                break;
            }
        }
        else
        {
            /* 设置为非阻塞式 */
            iRet = socket_set_non_blocking(iClientFD);

            if (iRet != 0)
            {
                close(iClientFD);
                LOG_WARN("%s:%d, client set non blocking error:%d", __FUNCTION__, __LINE__, iRet);
                continue;
            }

            /* 为客户端对象分配内存 */
            pstEpollClient = (LPEPOLLCLIENT)fix_mem_pool_malloc(&pstEpollServer->stClientMemPool, &iMemIndex);

            if (NULL == pstEpollClient)
            {
                close(iClientFD);
                LOG_WARN("%s:%d, malloc for client object error", __FUNCTION__, __LINE__);
                continue;
            }

            pstEpollClient->dwClientMemIdx = (U32)iMemIndex;
            pstEpollClient->bState = 0;
            pstEpollClient->ullClientID = gen_epoll_client_id();

            /* 为接收消息的缓存数组分配内存 */
#ifdef EPOLL_USE_FIX_MEM_POOL
            pstEpollClient->pszRecvMsgData = fix_mem_pool_malloc(&pstEpollServer->stMsgDataMemPool, &iMemIndex);
#else
            pstEpollClient->pszRecvMsgData = mem_pool_malloc(&pstEpollServer->stMsgDataMemPool);
#endif
            if (NULL == pstEpollClient->pszRecvMsgData)
            {
                close(iClientFD);
                fix_mem_pool_free(&pstEpollServer->stClientMemPool, pstEpollClient->dwClientMemIdx);
                LOG_WARN("%s:%d, malloc for client msg data error", __FUNCTION__, __LINE__);
                continue;
            }

            /* 初始化 */
            pstEpollClient->dwIP = stSin.sin_addr.s_addr;
            pstEpollClient->nPort = stSin.sin_port;
            pstEpollClient->iSocketFD = iClientFD;
            pstEpollClient->dwRecvLen = 0;
            //pstEpollClient->dwMsgLen = 0;
#ifdef EPOLL_USE_FIX_MEM_POOL
            pstEpollClient->dwMsgDataMemIdx = iMemIndex;
#endif

            /* 添加epoll监听事件 */
            iRet = epoll_sever_add_event(pstEpollServer, iClientFD, pstEpollClient);

            if (iRet != 0)
            {
                free_epoll_client(pstEpollServer, pstEpollClient);
                LOG_WARN("%s:%d, malloc for client msg data error", __FUNCTION__, __LINE__);
                continue;
            }
        }
    }

    return 0;
}

/**
 * 接收客户端消息
 * @param pstEpollServer Epoll服务器对象
 * @param pstEpollClient Epoll客户端对象
 * @return 成功返回0，失败返回错误码
 */
static int epoll_server_recv_process(LPEPOLLSERVER pstEpollServer, LPEPOLLCLIENT pstEpollClient)
{
    int iReadLen;

    LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == pstEpollServer || NULL == pstEpollClient) return ERROR_INPUT_PARAM_NULL;

    LOG_DEBUG("%s: epoll info, fd:%d. clientIP:%d, clientPort:%d", __FUNCTION__, pstEpollClient->iSocketFD,
        pstEpollClient->dwIP, pstEpollClient->nPort);

    /* 如果接受的消息超过了消息最大长度，还未被pstEpollServer->pstFuncHandleMsg函数处理，将抛弃这个错误消息 */
    if (pstEpollClient->dwRecvLen >= pstEpollServer->dwMaxMsgLen)
    {
        LOG_ERROR("%s:%d, recv message is too long:%d", __FUNCTION__, __LINE__, pstEpollClient->dwRecvLen);
        pstEpollClient->dwRecvLen = 0;
    }

    /* 循环读取数据，直到读完数据 */
    while (1)
    {
        do
        {
            iReadLen = read(pstEpollClient->iSocketFD, pstEpollClient->pszRecvMsgData + pstEpollClient->dwRecvLen,
                pstEpollServer->dwMaxMsgLen - pstEpollClient->dwRecvLen);
        } while (iReadLen < 0 && errno == EINTR);   // 预防读的过程中被其他事件中断

        LOG_DEBUG("%s:%d, read length:%d, errno:%d", __FUNCTION__, __LINE__, iReadLen, errno);

        if (iReadLen == 0)
        {
            // 当客户端关闭了Socket连接
            free_epoll_client(pstEpollServer, pstEpollClient);
            break;
        }
        else if (iReadLen == -1)
        {
            // 当读消息发生了错误
            if (errno == EAGAIN)
            {
                // errno = EAGAIN表示消息已读完
                //if (pstEpollServer->pstFuncHandleMsg != NULL) pstEpollServer->pstFuncHandleMsg(pstEpollServer, pstEpollClient);
                break;
            }
            // 发生了错误，可能是网络不好，此时还是不关闭客户端连接，等下次继续读(TODO:这样做是否比较好?还是关闭客户端比较好？)
            //if (pstEpollServer->pstFuncHandleMsg != NULL) pstEpollServer->pstFuncHandleMsg(pstEpollServer, pstEpollClient);
            break;
        }
        else
        {
            pstEpollClient->dwRecvLen += iReadLen;
            // 这里要处理下这个FuncHandleMsg的返回值，如果有些协议格式是错误的，可以直接
            // 关闭连接(收到了格式不正确的协议，大多情况下可能受到攻击)。如果协议格式错误，需要pstFuncHandleMsg返回特定的错误码
            if (pstEpollServer->pstFuncHandleMsg != NULL)
            {
                if (ERROR_DECODE_INVALID_MSG == pstEpollServer->pstFuncHandleMsg(pstEpollServer, pstEpollClient))
                {
                    // 协议格式错误，关闭socket
                    free_epoll_client(pstEpollServer, pstEpollClient);
                }
            }
        }
    }

    return 0;
}

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
 * @param pstFuncOnClientClose 处理客户端连接关闭事件
 * @return 成功返回0，失败返回错误码
 */
#ifdef EPOLL_USE_FIX_MEM_POOL
int init_epoll_server(LPEPOLLSERVER pstEpollServer, const char* szIpAddr, U16 nPort, U32 dwMaxMsgLen,
                      char* szClientMem, U32 dwClientMemLen, char* szMsgDataMem, U32 dwMsgDataMemLen,
                      LPFUNCHANDLEPOLLMSG pstFuncHandleMsg, LPFUNCONEPOLLCLIENTCLOSE pstFuncOnClientClose)
#else
int init_epoll_server(LPEPOLLSERVER pstEpollServer, const char* szIpAddr, U16 nPort, U32 dwMaxMsgLen,
                      char* szClientMem, U32 dwClientMemLen,
                      LPFUNCHANDLEPOLLMSG pstFuncHandleMsg, LPFUNCONEPOLLCLIENTCLOSE pstFuncOnClientClose)
#endif
{
    int iRet;

    if (NULL == pstEpollServer) return ERROR_INPUT_PARAM_NULL;

    LOG_DEBUG("%s enter.", __FUNCTION__);

    /* 最大消息长度 */
    pstEpollServer->dwMaxMsgLen = dwMaxMsgLen;
    /* 消息处理函数 */
    pstEpollServer->pstFuncHandleMsg = pstFuncHandleMsg;
    pstEpollServer->pstFuncOnClientClose = pstFuncOnClientClose;

    /* 为客户数据分配共享内存 */
    iRet = init_fix_mem_pool(&pstEpollServer->stClientMemPool, szClientMem, dwClientMemLen, sizeof(EPOLLCLIENT));

    if (iRet != 0)
    {
        LOG_ERROR("%s:%d, init mem pool for epoll client fail:%d", __FUNCTION__, __LINE__, iRet);
        return iRet;
    }

    /* 为接收消息分配共享内存 */
#ifdef EPOLL_USE_FIX_MEM_POOL
    iRet = init_fix_mem_pool(&pstEpollServer->stMsgDataMemPool, szMsgDataMem, dwMsgDataMemLen, dwMaxMsgLen);
#else
    iRet = init_mem_pool(&pstEpollServer->stMsgDataMemPool, dwMaxMsgLen);
#endif
    if (iRet != 0)
    {
        LOG_ERROR("%s:%d, init mem pool for ext data fail:%d", __FUNCTION__, __LINE__, iRet);
        return iRet;
    }

    /* 创建非阻塞套接字 */
    iRet = socket_create_tcp_server(&pstEpollServer->stEpollInfo.iSocketFD, szIpAddr, nPort, 1);

    if (iRet != 0)
    {
        LOG_ERROR("%s:%d, create tcp socket for epoll fail:%d", __FUNCTION__, __LINE__, iRet);
        return iRet;
    }

    /* 创建 epoll */
    pstEpollServer->iEpollFD = epoll_create(MAX_CS_FD_NUM);
    if (pstEpollServer->iEpollFD < 0)
    {
        close(pstEpollServer->stEpollInfo.iSocketFD);
        LOG_ERROR("%s:%d, create epoll failure:%s", __FUNCTION__, __LINE__, strerror(errno));
        return ERROR_EPOLL_CREATE_FAIL;
    }

    /* 添加epoll事件 */
    iRet = epoll_sever_add_event(pstEpollServer, pstEpollServer->stEpollInfo.iSocketFD, &pstEpollServer->stEpollInfo);
    if (iRet != 0)
    {
        free_epoll_server(pstEpollServer);
        return iRet;
    }

    return 0;
}

/**
 * 释放epoll服务器占用的内存，并关闭套接字
 * @param pstEpollServer epoll服务器对象
 * @return 成功返回0，失败返回错误码
 */
int free_epoll_server(LPEPOLLSERVER pstEpollServer)
{
    LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == pstEpollServer) return ERROR_INPUT_PARAM_NULL;

    close(pstEpollServer->stEpollInfo.iSocketFD);
    close(pstEpollServer->iEpollFD);

    return 0;
}

/**
 * 处理客户端连接及接收来自客户端的消息
 * @param pstEpollServer epoll服务器对象
 * @param iWaitTime 等待事件的时间，单位：毫秒
 * @return 成功返回0，失败返回错误码
 */
int epoll_server_process(LPEPOLLSERVER pstEpollServer, int iWaitTime)
{
    int iEvtNum;
    int i;
    LPEPOLLCLIENT pstClientInfo;

    //LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == pstEpollServer) return ERROR_INPUT_PARAM_NULL;

    /* 等待epoll事件 */
    iEvtNum = epoll_wait(pstEpollServer->iEpollFD, pstEpollServer->astEvents, MAX_CS_FD_NUM, iWaitTime);

    /* 发生错误 */
    if (iEvtNum < 0)
    {
        LOG_DEBUG("%s:%d, epoll wait failure:%d,%s", __FUNCTION__, __LINE__, errno, strerror(errno));
        return 0;
    }

    /* 处理epoll事件 */
    for (i = 0; i < iEvtNum; i++)
    {
        pstClientInfo = (LPEPOLLCLIENT)pstEpollServer->astEvents[i].data.ptr;

        if (pstClientInfo->iSocketFD == pstEpollServer->stEpollInfo.iSocketFD)
        {
            epoll_server_accept_process(pstEpollServer);
        }
        else
        {
            epoll_server_recv_process(pstEpollServer, pstClientInfo);
        }
    }

    return 0;
}

/**
 * 向客户端发送消息
 * @param pstEpollServer epoll服务器对象
 * @param pstEpollClient epoll客户端对象
 * @param pszMsg 要发送的消息
 * @param dwMsgLen 要发送的消息长度
 * @return 成功写的消息长度，且长度与发送的消息长度相等，失败返回0或负数，错误值在errno中
 */
int epoll_socket_write(int iSockFD, const char* pszMsg, U32 dwMsgLen)
{
    U32 dwSentLen = 0;
    int dwWriteLen;
    const char* pszBuf = pszMsg;

    LOG_DEBUG("%s enter. fd:%d, msglen:%d", __FUNCTION__, iSockFD, dwMsgLen);

    /* 循环调用write，直到写完数据 */
    while (dwSentLen < dwMsgLen)
    {
        do
        {
            dwWriteLen = write(iSockFD, pszBuf, dwMsgLen - dwSentLen);

            LOG_DEBUG("%s. fd:%d, writingLen:%d, wroteLen:%d", __FUNCTION__, iSockFD,
                dwMsgLen - dwSentLen, dwWriteLen);
        } while ((dwWriteLen < 0) && (errno == EINTR));

        if (dwWriteLen <= 0) return dwWriteLen;

        dwSentLen += dwWriteLen;

        pszBuf += dwWriteLen;
    }

    return dwMsgLen;
}

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
int epoll_write_process(LPEPOLLSERVER pstEpollServer, int iClientIndex, U64 ullCilentID, int iClientFD, U32 dwClientIP,
                        U16 nClientPort, const char* pszMsg, U32 dwMsgLen)
{
    int iFree;
    int iSentLen;
    LPEPOLLCLIENT pstEpollClient;

    LOG_DEBUG("%s enter. fd:%d, clientIP:%d, clientPort:%d, msgLen:%d", __FUNCTION__, iClientFD, dwClientIP,
        nClientPort, dwMsgLen);

    /* 检查输入参数合法性 */
    if (NULL == pstEpollServer || iClientIndex < 0 || iClientFD <= 0) return ERROR_INPUT_PARAM_NULL;

    /* 长度是0的消息不发 */
    if (NULL == pszMsg || 0 == dwMsgLen) return 0;

    /* 查找Epoll client */
    pstEpollClient = (LPEPOLLCLIENT)fix_mem_pool_at(&pstEpollServer->stClientMemPool, iClientIndex, &iFree);

    /* 内存不正确？ */
    if (NULL == pstEpollClient)
    {
        return ERROR_MEM_POOL_INDEX_INVALID;
    }

    /* 如果iFree=1表明内存已经释放，返回错误 */
    if (iFree == 1)
    {
        return ERROR_EPOLL_CLIENT_FREE;
    }

    /* 检查Epoll client数据是否正常 */
    if (ullCilentID != pstEpollClient->ullClientID ||
        iClientFD != pstEpollClient->iSocketFD ||
        dwClientIP != pstEpollClient->dwIP ||
        nClientPort != pstEpollClient->nPort)
    {
        return ERROR_EPOLL_CLIENT_UNMATCH;
    }

    iSentLen = epoll_socket_write(iClientFD, pszMsg, dwMsgLen);

    if (iSentLen != dwMsgLen)
    {
        return ERROR_EPOLL_WRITE_ERROR;  // 写错误，这种情况下，调用端可以再重写一次。
    }

    return 0;
}
