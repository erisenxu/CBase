/*
 * @(#) HttpResponse.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "HttpStatusLine.h"
#include "HttpHeaderList.h"
#include "HttpBody.h"

/**
 * HTTP响应消息结构体
 */
struct tagHttpResponse
{
    HTTPSTATUSLINE stStatusLine;
    HTTPHEADERLIST stHeaderList;
    HTTPBODY stBody;
};

typedef struct tagHttpResponse  HTTPRESPONSE;
typedef struct tagHttpResponse* LPHTTPRESPONSE;

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
int http_response_encode(LPHTTPRESPONSE pstField, LPBYTEARRAY pstByteArray);

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @return 成功返回0，失败返回错误码
 */
int http_response_decode(LPHTTPRESPONSE pstField, const char* szBuf, int iBufLen);

#ifdef __cplusplus
}
#endif

#endif

