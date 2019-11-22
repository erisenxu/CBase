/*
 * @(#) HttpStatusLine.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_STATUS_LINE_H
#define HTTP_STATUS_LINE_H

#include "comm/ByteArray.h"
#include "HttpConstants.h"

/**
 * HTTP响应消息状态行结构体
 */
struct tagHttpStatusLine
{
    char szProtocol[MAX_HTTP_PROTOCOL_LEN];     /* 协议名，恒定为HTTP */
    char szVersion[MAX_HTTP_VERSION_LEN];       /* 协议版本，1.1 */
    char szStatus[MAX_HTTP_STATUS_LEN];         /* 状态码 100/200/400, etc */
    char szReason[MAX_HTTP_REASON_LEN];         /* 状态信息 */
};

typedef struct tagHttpStatusLine  HTTPSTATUSLINE;
typedef struct tagHttpStatusLine* LPHTTPSTATUSLINE;

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
int http_status_line_encode(LPHTTPSTATUSLINE pstField, LPBYTEARRAY pstByteArray);

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @param piReadLen 从字节流中解码字段读取的长度
 * @return 成功返回0，失败返回错误码
 */
int http_status_line_decode(LPHTTPSTATUSLINE pstField, const char* szBuf, int iBufLen, int* piReadLen);

#ifdef __cplusplus
}
#endif

#endif
