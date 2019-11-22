/*
 * @(#) HttpBody.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_BODY_H
#define HTTP_BODY_H

#include "comm/ByteArray.h"
#include "HttpConstants.h"

/**
 * HTTP消息内容结构体
 */
struct tagHttpBody
{
    U16 nBodyLen;
    char szBody[MAX_HTTP_BODY_LEN];
};

typedef struct tagHttpBody  HTTPBODY;
typedef struct tagHttpBody* LPHTTPBODY;

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
int http_body_encode(LPHTTPBODY pstField, LPBYTEARRAY pstByteArray);

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @param piReadLen 从字节流中解码字段读取的长度
 * @return 成功返回0，失败返回错误码
 */
int http_body_decode(LPHTTPBODY pstField, const char* szBuf, int iBufLen, int* piReadLen);

#ifdef __cplusplus
}
#endif

#endif
