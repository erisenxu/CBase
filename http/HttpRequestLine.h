/*
 * @(#) HttpRequestLine.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_REQUEST_LINE_H
#define HTTP_REQUEST_LINE_H

#include "comm/ByteArray.h"
#include "HttpConstants.h"

/**
 * HTTP请求消息请求行结构体
 */
struct tagHttpQuestLine
{
    char szMethod[MAX_HTTP_METHOD_LEN];         /* 请求名，GET/POST/PUT, etc */
    char szUrl[MAX_HTTP_URL_LEN];               /* 请求地址 */
    char szProtocol[MAX_HTTP_PROTOCOL_LEN];     /* 协议名，恒定为HTTP */
    char szVersion[MAX_HTTP_VERSION_LEN];       /* 协议版本，1.1 */
};

typedef struct tagHttpQuestLine  HTTPQUESTLINE;
typedef struct tagHttpQuestLine* LPHTTPQUESTLINE;

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
int http_request_line_encode(LPHTTPQUESTLINE pstField, LPBYTEARRAY pstByteArray);

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @param piReadLen 从字节流中解码字段读取的长度
 * @return 成功返回0，失败返回错误码
 */
int http_request_line_decode(LPHTTPQUESTLINE pstField, const char* szBuf, int iBufLen, int* piReadLen);

#ifdef __cplusplus
}
#endif

#endif

