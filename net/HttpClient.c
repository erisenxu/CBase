/*
 * @(#) HttpClient.c Created on 2008-03-27
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "HttpClient.h"
#include "log/Logger.h"
#include "net.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <errno.h>
#include <string.h> /* 使用strerror必须加上这个头文件，否则会导致崩溃，:( */
#include <time.h>
#include <unistd.h>

#define MAX_SSL_ERR_LEN     512

#define LOG_SSL_ERR(msg) \
    char szErrInfo[MAX_SSL_ERR_LEN]; \
    ERR_error_string_n(ERR_get_error(), szErrInfo, sizeof(szErrInfo)); \
    LOG_ERROR(msg, __FUNCTION__, szErrInfo)

/**
 * 接收HTTP响应，并处理响应
 * @param iSocket TCP套接字
 * @param iTimeout 等待超时时间
 * @param pstHttpResp 接收到的响应消息
 * @param pstSsl SSL对象，如果不为NULL，则是普通套接字
 * @return 成功返回0，失败返回错误码
 */
static int receive_http_response(int iSocket, int iTimeout, LPHTTPRESPONSE pstHttpResp, SSL* pstSsl);

/**
 * 解析http协议的长度
 * @param szHttpMsg http消息
 * @param iMsgLen 包长度
 * @return 返回http协议的总长度，若包不全，返回-1
 */
static int http_msg_parse_length(const char* szHttpMsg, int iMsgLen);

/**
 * 初始化SSL context
 */
static SSL_CTX* init_ssl_context()
{
    // 加载SSL错误信息
    SSL_load_error_strings();
    // 初始化
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    // 创建context，使用SSL V2/V3
    return SSL_CTX_new(SSLv23_client_method());
}

/**
 * 初始化SSL context
 */
static SSL* init_ssl_connect(SSL_CTX* pstSslCtx, int iSocket)
{
    int iRet;

    SSL* pstSsl = SSL_new(pstSslCtx);

    if (NULL == pstSsl)
    {
        // 打印错误信息
        LOG_SSL_ERR("%s:Error to create ssl: %s");
        return NULL;
    }

    // 将SSL与TCP SOCKET 连接
    if (!SSL_set_fd(pstSsl, iSocket))
    {
        // 打印错误信息
        LOG_SSL_ERR("%s:Error to bind ssl with socket: %s");
        // 关闭SSL套接字 
        SSL_shutdown(pstSsl);
        // 释放SSL套接字 
        SSL_free(pstSsl);
        return NULL;
    }

    // 建立SSL连接
    iRet = SSL_connect(pstSsl);
    if (iRet < 0)
    {
        // 打印错误信息
        LOG_SSL_ERR("%s:Error to establish ssl connection: %s");
        // 关闭SSL套接字 
        SSL_shutdown(pstSsl);
        // 释放SSL套接字 
        SSL_free(pstSsl);
        return NULL;
    }

    return pstSsl;
}

/**
 * 发送HTTP请求
 * @param szHost 服务器地址
 * @param nPort 服务器监听端口
 * @param pstHttpReq 请求数据
 * @param iTimeout 等待超时时间，若iTimeout=-1，则一直等待，直到数据接收完
 * @param pstHttpResp 接收到的响应消息
 * @param chProtocolType 协议类型，0：HTTP，1：HTTPS
 * @return 成功返回0，失败返回错误码
 */
int send_http_request(const char* szHost, U16 nPort, LPHTTPREQUEST pstHttpReq, int iTimeout,
                      LPHTTPRESPONSE pstHttpResp, U8 chProtocolType)
{
    int iRet;
    char szHttpMsg[MAX_HTTP_MSG_LEN];
    BYTEARRAY baHttpReq;

    LOG_DEBUG("%s enter.", __FUNCTION__);

    if (NULL == szHost || NULL == pstHttpReq || NULL == pstHttpResp) return ERROR_INPUT_PARAM_NULL;

    /* 编码HTTP Request */
    INIT_BYTE_ARRAY(baHttpReq, szHttpMsg, sizeof(szHttpMsg));

    iRet = http_request_encode(pstHttpReq, &baHttpReq);

    if (iRet != 0)
    {
        LOG_ERROR("%s:Error to encode http request: %d", __FUNCTION__, iRet);
        return iRet;
    }

    return send_http_request_msg(szHost, nPort, baHttpReq.pszData, baHttpReq.dwLen, iTimeout,
                                 pstHttpResp, chProtocolType);
}

/**
 * 发送HTTP请求
 * @param szHost 服务器地址
 * @param nPort 服务器监听端口
 * @param pstHttpReq 请求数据
 * @param iTimeout 等待超时时间，若iTimeout=-1，则一直等待，直到数据接收完
 * @param pstHttpResp 接收到的响应消息
 * @param chProtocolType 协议类型，0：HTTP，1：HTTPS
 * @return 成功返回0，失败返回错误码
 */
int send_http_request_msg(const char* szHost, U16 nPort, const char* szHttpReq, U32 dwMsgLen, int iTimeout,
                          LPHTTPRESPONSE pstHttpResp, U8 chProtocolType)
{
    int iRet;
    int iSocket = -1;

    if (NULL == szHost || NULL == szHttpReq || NULL == pstHttpResp) return ERROR_INPUT_PARAM_NULL;

    /* 创建socket，并连接到服务器 */
    iRet = socket_connect_to_tcp_server(&iSocket, szHost, nPort);

    if (iRet != 0 && iSocket > 0)
    {
        LOG_ERROR("%s:Error to connect to host (%s): %d",
                  __FUNCTION__, szHost, iRet);
        return iRet;
    }

    if (0 == chProtocolType)
    {
        /* 向服务器发送http请求消息 */
        iRet = send(iSocket, szHttpReq, dwMsgLen, 0);
        if (iRet < 0)
        {
            LOG_ERROR("%s:Error to send http request to host (%s): %d, %s",
                    __FUNCTION__, szHost, iRet, strerror(errno));
            CLOSE_SOCKET(iSocket);
            return iRet;
        }

        /* 接收来自http的响应消息 */
        iRet = receive_http_response(iSocket, iTimeout, pstHttpResp, NULL);
        CLOSE_SOCKET(iSocket);
        return iRet;
    }
    else
    {
        /* 初始化SSL */
        SSL_CTX* pstSslCtx = init_ssl_context();
        if (NULL == pstSslCtx)
        {
            LOG_SSL_ERR("%s:Error to init ssl context: %s");
            CLOSE_SOCKET(iSocket);
            return ERROR_SSL_CONTEXT_INIT;
        }

        /* 建立SSL连接 */
        SSL* pstSsl = init_ssl_connect(pstSslCtx, iSocket);
        if (NULL == pstSsl)
        {
            SSL_CTX_free(pstSslCtx);
            CLOSE_SOCKET(iSocket);
            return ERROR_SSL_CONNECT_INIT;
        }

        /* 发送消息 */
        iRet = SSL_write(pstSsl, szHttpReq, dwMsgLen);
        if (iRet < 0)
        {
            LOG_SSL_ERR("%s:Error to write ssl message: %s");
            // 关闭SSL套接字 
            SSL_shutdown(pstSsl);
            // 释放SSL套接字 
            SSL_free(pstSsl);
            // 释放SSL context
            SSL_CTX_free(pstSslCtx);
            // 关闭套接字
            CLOSE_SOCKET(iSocket);
            return ERROR_SSL_WRITE;
        }

        /* 接收来自http的响应消息 */
        iRet = receive_http_response(iSocket, iTimeout, pstHttpResp, pstSsl);

        // 关闭SSL套接字 
        SSL_shutdown(pstSsl);
        // 释放SSL套接字 
        SSL_free(pstSsl);
        // 释放SSL context
        SSL_CTX_free(pstSslCtx);
        // 关闭套接字
        CLOSE_SOCKET(iSocket);

        return iRet;
    }
}

/**
 * 接收HTTP响应，并处理响应
 * @param iSocket TCP套接字
 * @param iTimeout 等待超时时间
 * @param pstHttpResp 接收到的响应消息
 * @param pstSsl SSL对象，如果不为NULL，则是普通套接字
 * @return 成功返回0，失败返回错误码
 */
int receive_http_response(int iSocket, int iTimeout, LPHTTPRESPONSE pstHttpResp, SSL* pstSsl)
{
    struct timeval oTimeVal;
    fd_set oReadfds;
    int iRet = 0;
    int iReadLen = 0;
    int iMsgLen = -1;
    char szHttpMsg[MAX_HTTP_MSG_LEN];
    int iMaxLen = sizeof(szHttpMsg);
    char* pszRecvMsg = szHttpMsg;
    U32 dwStartTime = time(NULL);

    /* 接收来自http的响应消息 */
    while (1)
    {
        oTimeVal.tv_sec = 0;
        oTimeVal.tv_usec = 5000;

        FD_ZERO(&oReadfds);
        FD_SET(iSocket, &oReadfds);

        iRet = select(iSocket + 1, &oReadfds, NULL, NULL, &oTimeVal);

        if (iRet != SOCKET_ERROR && FD_ISSET(iSocket, &oReadfds))
        {
            iRet = NULL == pstSsl ? recv(iSocket, pszRecvMsg, iMaxLen, 0) : SSL_read(pstSsl, pszRecvMsg, iMaxLen);

            if (iRet <= 0)
            {
                return ERROR_SOCKET_PEER_CLOSE;
            }

            pszRecvMsg += iRet;
            /* 总读取长度 */
            iReadLen += iRet;
            iMaxLen -= iRet;

            /* 提取消息长度 */
            if (iMsgLen == -1)
            {
                iMsgLen = http_msg_parse_length(szHttpMsg, iReadLen);
            }

            /* 如果消息已经接收完 */
            if (iReadLen >= iMsgLen && iMsgLen != -1)
            {
                /* 处理消息 */
                iRet = http_response_decode(pstHttpResp, szHttpMsg, iReadLen);
                if (iRet != 0) return iRet;
                /* 处理消息 */
                return 0;
            }

            /* 如果消息太长 */
            if (iMaxLen <= 0)
            {
                return ERROR_APPEND_BUFSIZE_SHORT;
            }
        }

        /* 判断是否超时 */
        if (iTimeout > 0)
        {
            if (time(NULL) - dwStartTime > iTimeout)
            {
                return ERROR_SOCKET_TIMEOUT;
            }
        }
    }

    return 0;
}

/**
 * 解析http协议的长度
 * @param szHttpMsg http消息
 * @param iMsgLen 包长度
 * @return 返回http协议的总长度，若包不全，返回-1
 */
int http_msg_parse_length(const char* szHttpMsg, int iMsgLen)
{
    const char* s_ptr;
    const char* s_end;
    int iBodyLen;

    if (szHttpMsg == NULL || iMsgLen <= 0) return -1;

    /* 是否看到了长度字段？ */
    s_ptr = strstr(szHttpMsg, HTTP_HDR_LENGTH_NAME);

    if (s_ptr != NULL)
    {
        s_ptr = strstr(s_ptr, ":");

        /* 消息没有收全，返回-1继续收取 */
        if (s_ptr == NULL) return -1;

        /* 消息没有收全，返回-1继续收取 */
        s_end = strstr(s_ptr, "\r\n");
        if (s_end == NULL) return -1;

        iBodyLen = atoi(s_ptr + strlen(":"));

        /* 是否看到了body? */
        s_ptr = strstr(s_ptr, "\r\n\r\n");
        /* 消息没有收全，返回-1继续收取 */
        if (s_ptr == NULL) return -1;
        /* 返回总长度 */
        return s_ptr - szHttpMsg + strlen("\r\n\r\n") + iBodyLen;
    }

    /* 当没有长度字段时，是否看到了body? */
    s_ptr = strstr(szHttpMsg, "\r\n\r\n");
    /* 消息没有收全，返回-1继续收取 */
    if (s_ptr == NULL) return -1;
    /* 有body，但是没有长度字段，那么认为整个消息都收完了，返回总长度 */
    return iMsgLen;
}
