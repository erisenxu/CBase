/*
 * @(#) HttpClient.h Created on 2008-03-27
 *
 * 一个简易的Http客户端的实现，使用的场合有限。
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "http/HttpRequest.h"
#include "http/HttpResponse.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
                      LPHTTPRESPONSE pstHttpResp, U8 chProtocolType);

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
                          LPHTTPRESPONSE pstHttpResp, U8 chProtocolType);

#ifdef __cplusplus
}
#endif

#endif

