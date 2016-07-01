/*
 * @(#) net.h Created on 2008-03-27
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef M_NET_H
#define M_NET_H

#include "comm/BaseDef.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 将套接字设置为非阻塞式
 * @param fd 套接字
 * @return 成功返回0，失败返回错误码
 */
int socket_set_non_blocking(int fd);

/**
 * 创建tcp服务器，并绑定IP、端口开始监听并开始监听
 * @param pifd 创建成功，返回套接字
 * @param szIpAddr 绑定的IP地址
 * @param nPort 监听端口
 * @param bNonBlock 填1创建非阻塞套接字，填0创建阻塞式套接字
 * @return 成功返回0，失败返回错误码
 */
int socket_create_tcp_server(int* pifd, const char* szIpAddr, U16 nPort, U8 bNonBlock);

/**
 * 将ip转换为ip地址
 * @param iAddr 要转换的ip
 * @param szBuf 保存ip地址的缓存区
 * @param dwBufSize 缓存区大小
 * @return 成功转换的ip地址
 */
char* to_safe_address(S32 iAddr, char* szBuf, U32 dwBufSize);

#ifdef __cplusplus
}
#endif

#endif

