/*
 * @(#) HttpHeaderList.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef HTTP_HEADER_LIST_H
#define HTTP_HEADER_LIST_H

#include "HttpHeader.h"

/**
 * HTTP消息头部列表结构体
 */
struct tagHttpHeaderList
{
    U16 nHeaderNumber;
    HTTPHEADER astHeader[MAX_HTTP_HDR_NUM];
};

typedef struct tagHttpHeaderList  HTTPHEADERLIST;
typedef struct tagHttpHeaderList* LPHTTPHEADERLIST;

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
int http_header_list_encode(LPHTTPHEADERLIST pstField, LPBYTEARRAY pstByteArray);

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @param piReadLen 从字节流中解码字段读取的长度
 * @return 成功返回0，失败返回错误码
 */
int http_header_list_decode(LPHTTPHEADERLIST pstField, const char* szBuf, int iBufLen, int* piReadLen);

/**
 * 通过消息头名称，查找头字段
 * @param pstField 头列表字段
 * @param szName 头名称
 * @return 若在列表中找到头字段，则返回头字段对象，未找到，返回NULL
 */
LPHTTPHEADER find_http_header_by_name(LPHTTPHEADERLIST pstField, const char* szName);

#ifdef __cplusplus
}
#endif

#endif
