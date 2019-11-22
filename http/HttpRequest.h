/*
 * @(#) HttpRequest.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "HttpRequestLine.h"
#include "HttpHeaderList.h"
#include "HttpBody.h"

/**
 * HTTP请求消息结构体
 */
struct tagHttpRequest
{
    HTTPQUESTLINE stReqLine;
    HTTPHEADERLIST stHeaderList;
    HTTPBODY stBody;
};

typedef struct tagHttpRequest  HTTPREQUEST;
typedef struct tagHttpRequest* LPHTTPREQUEST;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 字段编码
 * @param pstField 要编码的字段
 * @param pstByteArray 保存编码字节流的缓存区数组
 * @return 成功返回0，失败返回错误码
 */
int http_request_encode(LPHTTPREQUEST pstField, LPBYTEARRAY pstByteArray);

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @return 成功返回0，失败返回错误码
 */
int http_request_decode(LPHTTPREQUEST pstField, const char* szBuf, int iBufLen);

#ifdef __cplusplus
}
#endif

#endif
