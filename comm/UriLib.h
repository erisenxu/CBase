/*
 * @(#) UriLib.h Created on 2008-03-27 URI相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef URI_LIB_H
#define URI_LIB_H

#include "BaseDef.h"

#define MAX_URI_PROTOCOL_NAME_LEN   16
#define MAX_URI_HOST_LEN            128
#define MAX_URI_PATH_LEN            256
#define MAX_URI_LEN                 512

/**
 * URI信息
 */
struct tagUriInfo
{
    U16 nPort;                                           /* 端口 */
    char szProtocolName[MAX_URI_PROTOCOL_NAME_LEN];      /* 协议名 */
    char szHost[MAX_URI_HOST_LEN];                       /* host */
    char szPath[MAX_URI_PATH_LEN];                       /* 路径 */
};

typedef struct tagUriInfo   URIINFO;
typedef struct tagUriInfo*  LPURIINFO;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 解析uri
 * @param szURI 路径名
 * @param pstUriInfo URI信息
 * @return 成功返回0，失败返回错误码
 */
int uri_parse(const char* szURI, LPURIINFO pstUriInfo);

#ifdef __cplusplus
}
#endif

#endif

