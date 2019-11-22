/*
 * @(#) HttpHeader.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include "comm/ByteArray.h"
#include "HttpConstants.h"

/**
 * HTTP消息头部结构体
 */
struct tagHttpHeader
{
    char szName[MAX_HTTP_HDR_NAME_LEN];         /* 消息头名 */
    char szValue[MAX_HTTP_HDR_VALUE_LEN];       /* 消息头的值 */
};

typedef struct tagHttpHeader  HTTPHEADER;
typedef struct tagHttpHeader* LPHTTPHEADER;

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
int http_header_encode(LPHTTPHEADER pstField, LPBYTEARRAY pstByteArray);

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @param piReadLen 从字节流中解码字段读取的长度
 * @return 成功返回0，失败返回错误码
 */
int http_header_decode(LPHTTPHEADER pstField, const char* szBuf, int iBufLen, int* piReadLen);

#ifdef __cplusplus
}
#endif

#endif
