/*
 * @(#) net.c Created on 2008-03-27
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "net.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"
#include "log/Logger.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>  /* For Linux, For unix is <xti.h>. This is for TCP_NODELAY */

#include <errno.h>
#include <string.h> /* 使用strerror必须加上这个头文件，否则会导致崩溃，:( */

/**
 * 将套接字设置为非阻塞式
 * @param fd 套接字
 * @return 成功返回0，失败返回错误码
 */
int socket_set_non_blocking(int fd)
{
    int iFlags;
    int iRet;

    LOG_DEBUG("%s enter.", __FUNCTION__);

    iFlags = fcntl(fd, F_GETFL, 0);

    if (iFlags < 0)
    {
        LOG_ERROR("%s:%d, fail to get flag for socket:%d, error info:%s", __FUNCTION__, __LINE__, fd, strerror(errno));
        return ERROR_SOCKET_GET_FLAG;
    }

    iFlags |= O_NONBLOCK;

    iRet = fcntl(fd, F_SETFL, iFlags);

    if (iRet < 0)
    {
        LOG_ERROR("%s:%d, fail to set flag for socket:%d, error info:%s", __FUNCTION__, __LINE__, fd, strerror(errno));
        return ERROR_SOCKET_SET_FLAG;
    }

    return 0;
}

/**
 * 创建tcp服务器，并绑定IP、端口并开始监听
 * @param pifd 创建成功，返回套接字
 * @param szIpAddr 绑定的IP地址
 * @param nPort 监听端口
 * @param bNonBlock 填1创建非阻塞套接字，填0创建阻塞式套接字
 * @return 成功返回0，失败返回错误码
 */
int socket_create_tcp_server(int* pifd, const char* szIpAddr, U16 nPort, U8 bNonBlock)
{
    int iRet;
    struct sockaddr_in stsin;
    int iOptval = 1;

    LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == pifd) return ERROR_INPUT_PARAM_NULL;

    /* 创建套件字， 采用IPV4(AF_INET) */
    *pifd = socket(AF_INET, SOCK_STREAM, 0);

    if (*pifd < 0)
    {
        LOG_ERROR("%s:%d, create socket error:%s", __FUNCTION__, __LINE__, strerror(errno));
        return ERROR_SOCKET_INVOKE;
    }

    /* 设置端口重用 */
    iRet = setsockopt(*pifd, SOL_SOCKET, SO_REUSEADDR, &iOptval, sizeof(iOptval));
    if (iRet < 0)
    {
        LOG_ERROR("%s:%d, set socket reuse error:%s", __FUNCTION__, __LINE__, strerror(errno));
        close(*pifd);
        return ERROR_SOCKET_SET_REUSE;
    }

    /* 绑定套接字 */
    memset(&stsin, 0, sizeof(stsin));
    stsin.sin_family = AF_INET;
    stsin.sin_port = htons(nPort);
    stsin.sin_addr.s_addr = NULL == szIpAddr ? htonl(INADDR_ANY) : inet_addr(szIpAddr);

    iRet = bind(*pifd, (struct sockaddr*)&stsin, sizeof(stsin));
    if (iRet < 0)
    {
        LOG_ERROR("%s:%d, bind socket error:%s", __FUNCTION__, __LINE__, strerror(errno));
        close(*pifd);
        return ERROR_SOCKET_BIND;
    }

    /* 设置为非阻塞 */
    if (bNonBlock)
    {
        iRet = socket_set_non_blocking(*pifd);
        if (iRet != 0)
        {
            LOG_ERROR("%s:%d, set socket non blocking error:%d", __FUNCTION__, __LINE__, iRet);
            close(*pifd);
            return iRet;
        }
    }

    /* 开始监听 */
    iRet = listen(*pifd, SOMAXCONN);
    if (iRet < 0)
    {
        LOG_ERROR("%s:%d, socket listen error:%s", __FUNCTION__, __LINE__, strerror(errno));
        close(*pifd);
        return ERROR_SOCKET_LISTEN;
    }

    return iRet;
}

/**
 * 将ip转换为ip地址
 * @param iAddr 要转换的ip
 * @param szBuf 保存ip地址的缓存区
 * @param dwBufSize 缓存区大小
 * @return 成功转换的ip地址
 */
char* to_safe_address(S32 iAddr, char* szBuf, U32 dwBufSize)
{
    if (NULL == szBuf || 0 == dwBufSize) return szBuf;

    STRNCPY(szBuf, dwBufSize, inet_ntoa(*((struct in_addr *)&iAddr)), dwBufSize);

    return szBuf;
}

