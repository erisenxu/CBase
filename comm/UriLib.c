/*
 * @(#) UriLib.c Created on 2008-03-27 URI相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "UriLib.h"
#include "BaseFuncDef.h"
#include "ErrorCode.h"

#include <string.h>
#include <strings.h>

#define URI_PARSE_END       1

/**
 * URI解析器上下文相关对象
 */
typedef struct tagUriParseContext   URIPARSECONTEXT;
typedef struct tagUriParseContext*  LPURIPARSECONTEXT;

/**
 * 状态处理函数
 * @param pstUriParseCtx 解析器上下文相关对象
 * @return 成功返回0，失败返回错误码
 */
typedef int (*FNURIPARSESTATEHANDLER) (LPURIPARSECONTEXT pstUriParseCtx);

/**
 * 上下文对象
 */
struct tagUriParseContext
{
    const char* szUriBuf;                               /* 当前处理的uri字符串 */
    char szCurrentValue[MAX_URI_LEN];                   /* 当前处理的值 */
    U32 dwCurrentValueIndex;                            /* 当前处理的值的写索引 */
    LPURIINFO pstUriInfo;                               /* URI信息 */
    FNURIPARSESTATEHANDLER fnParseState;                /* 当前状态处理函数 */
};

#define APPEND_CURRENT_URI_VALUE(pstCtx) \
    if (pstCtx->dwCurrentValueIndex + 1 >= sizeof(pstCtx->szCurrentValue)) \
    { \
        return ERROR_URI_TOO_LONG; \
    } \
    pstCtx->szCurrentValue[pstCtx->dwCurrentValueIndex++] = *pstCtx->szUriBuf

// 各种解析状态处理函数
static int onParseStateInit(LPURIPARSECONTEXT pstParseCtx);
static int onParseStatePath(LPURIPARSECONTEXT pstParseCtx);
static int onParseStateProtocolEnd(LPURIPARSECONTEXT pstParseCtx);
static int onParseStateHostStart(LPURIPARSECONTEXT pstParseCtx);
static int onParseStateHost(LPURIPARSECONTEXT pstParseCtx);
static int onParseStatePort(LPURIPARSECONTEXT pstParseCtx);

int onParseStateInit(LPURIPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szUriBuf)
    {
    case '/':
        pstParseCtx->fnParseState = onParseStatePath;
        APPEND_CURRENT_URI_VALUE(pstParseCtx);
        break;
    case ':':
        pstParseCtx->fnParseState = onParseStateProtocolEnd;
        break;
    case '?':
    case '#':
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        MSTRNCPY(pstParseCtx->pstUriInfo->szPath, pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        /* 解析完成 */
        return URI_PARSE_END;
    default:
        APPEND_CURRENT_URI_VALUE(pstParseCtx);
        break;
    }
    return 0;
}

int onParseStateProtocolEnd(LPURIPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szUriBuf)
    {
    case '/':
        pstParseCtx->fnParseState = onParseStateHostStart;
        break;
    default:
        return ERROR_URI_FORMAT_ERROR;
    }
    return 0;
}

int onParseStateHostStart(LPURIPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szUriBuf)
    {
    case '/':
        pstParseCtx->fnParseState = onParseStateHost;
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        MSTRNCPY(pstParseCtx->pstUriInfo->szProtocolName, pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        break;
    default:
        return ERROR_URI_FORMAT_ERROR;
    }
    return 0;
}

int onParseStateHost(LPURIPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szUriBuf)
    {
    case '/':
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        MSTRNCPY(pstParseCtx->pstUriInfo->szHost, pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        /* change state */
        pstParseCtx->fnParseState = onParseStatePath;
        APPEND_CURRENT_URI_VALUE(pstParseCtx);
        break;
    case ':':
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        MSTRNCPY(pstParseCtx->pstUriInfo->szHost, pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        /* change state */
        pstParseCtx->fnParseState = onParseStatePort;
        break;
    case '?':
    case '#':
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        MSTRNCPY(pstParseCtx->pstUriInfo->szHost, pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        /* 解析完成 */
        return URI_PARSE_END;
    default:
        APPEND_CURRENT_URI_VALUE(pstParseCtx);
        break;
    }
    return 0;
}

int onParseStatePort(LPURIPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szUriBuf)
    {
    case '/':
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        pstParseCtx->pstUriInfo->nPort = atoi(pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        /* change state */
        pstParseCtx->fnParseState = onParseStatePath;
        APPEND_CURRENT_URI_VALUE(pstParseCtx);
        break;
    case '?':
    case '#':
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        pstParseCtx->pstUriInfo->nPort = atoi(pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        /* 解析完成 */
        return URI_PARSE_END;
    default:
        APPEND_CURRENT_URI_VALUE(pstParseCtx);
        break;
    }
    return 0;
}

int onParseStatePath(LPURIPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szUriBuf)
    {
    case '?':
    case '#':
        pstParseCtx->szCurrentValue[pstParseCtx->dwCurrentValueIndex] = 0;
        MSTRNCPY(pstParseCtx->pstUriInfo->szPath, pstParseCtx->szCurrentValue);
        pstParseCtx->dwCurrentValueIndex = 0;
        /* 解析完成 */
        return URI_PARSE_END;
    default:
        APPEND_CURRENT_URI_VALUE(pstParseCtx);
        break;
    }
    return 0;
}

/**
 * 解析uri
 * @param szURI 路径名
 * @param pstUriInfo URI信息
 * @return 成功返回0，失败返回错误码
 */
int uri_parse(const char* szURI, LPURIINFO pstUriInfo)
{
    URIPARSECONTEXT stParseCtx;
    int iRet;

    if (NULL == szURI || NULL == pstUriInfo) return ERROR_INPUT_PARAM_NULL;

    stParseCtx.szUriBuf = szURI;
    stParseCtx.fnParseState = onParseStateInit;
    stParseCtx.dwCurrentValueIndex = 0;
    stParseCtx.pstUriInfo = pstUriInfo;

    stParseCtx.pstUriInfo->nPort = 0;
    *stParseCtx.pstUriInfo->szPath = 0;
    *stParseCtx.pstUriInfo->szHost = 0;
    *stParseCtx.pstUriInfo->szProtocolName = 0;

    while (*stParseCtx.szUriBuf)
    {
        iRet = stParseCtx.fnParseState(&stParseCtx);
        if (iRet == URI_PARSE_END) break;
        if (iRet != 0) return iRet;
        stParseCtx.szUriBuf++;
    }

    /* 处理状态转换完后的情况 */
    if (stParseCtx.dwCurrentValueIndex > 0)
    {
        if (onParseStateInit == stParseCtx.fnParseState ||
            onParseStatePath == stParseCtx.fnParseState)
        {
            stParseCtx.szCurrentValue[stParseCtx.dwCurrentValueIndex] = 0;
            MSTRNCPY(stParseCtx.pstUriInfo->szPath, stParseCtx.szCurrentValue);
        }
        else if (onParseStateHost == stParseCtx.fnParseState)
        {
            stParseCtx.szCurrentValue[stParseCtx.dwCurrentValueIndex] = 0;
            MSTRNCPY(stParseCtx.pstUriInfo->szHost, stParseCtx.szCurrentValue);
        }
        else if (onParseStatePort == stParseCtx.fnParseState)
        {
            stParseCtx.szCurrentValue[stParseCtx.dwCurrentValueIndex] = 0;
            stParseCtx.pstUriInfo->nPort = atoi(stParseCtx.szCurrentValue);
        }
        else
        {
            return ERROR_URI_FORMAT_ERROR;
        }
    }

    if (0 == stParseCtx.pstUriInfo->nPort)
    {
        stParseCtx.pstUriInfo->nPort = strncasecmp(stParseCtx.pstUriInfo->szProtocolName, "https", sizeof(stParseCtx.pstUriInfo->szProtocolName)) == 0 ? 443 : 80;
    }

    return 0;
}

