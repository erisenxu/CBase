/*
 * @(#) JsonParser.c Created on 2008-03-27 Json解析器
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "JsonParser.h"

#include "comm/ErrorCode.h"
#include "comm/BaseDef.h"
#include "comm/BaseFuncDef.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> /* 使用strerror必须加上这个头文件，否则会导致崩溃，:( */
#include <errno.h>
#include <ctype.h>

#define MAX_JSON_NAME_LEN       10240       /* 最大Json名及值长度 */
#define MAX_JSON_OBJECT_NUMBER  256         /* Json对象最大嵌套数量 */

/**
 * 解析器上下文相关对象
 */
typedef struct tagJsonParseContext 	JSONPARSECONTEXT;
typedef struct tagJsonParseContext* LPJSONPARSECONTEXT;

/**
 * 状态处理函数
 * @param pstJsonParseCtx 解析器上下文相关对象
 * @return 成功返回0，失败返回错误码
 */
typedef int (*FNJSONPARSESTATEHANDLER) (LPJSONPARSECONTEXT pstJsonParseCtx);

/**
 * json元素信息
 */
struct tagJsonObjInfo
{
    char szObjName[MAX_JSON_NAME_LEN];      /* json元素名称 */
    unsigned char chJsonType;               /* json元素类型 */
    unsigned char chItemType;               /* json子元素类型 */
};

typedef struct tagJsonObjInfo   JSONOBJINFO;
typedef struct tagJsonObjInfo*  LPJSONOBJINFO;

/**
 * 上下文对象
 */
struct tagJsonParseContext
{
    const char* szJsonBuf;                              /* 当前处理的json字符串 */
    void* pstCallData;                                  /* 调用方数据 */
    JSONOBJINFO stObjInfo[MAX_JSON_OBJECT_NUMBER];      /* json元素信息 */
    char szCurrentName[MAX_JSON_NAME_LEN];              /* 当前处理的json元素名 */
    S32 iObjNameIndex;                                  /* json元素信息数组stObjInfo读写索引 */
    U32 dwCurrentNameIndex;                             /* 当前处理的json元素名数组写索引 */
    FNJSONPARSESTATEHANDLER fnParseState;               /* 当前状态处理函数 */
    LPJSONPARSEHANDLER pstParseHandler;                 /* 解析处理器 */
};

// 各种解析状态处理函数
static int onParseStateInit(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjNameD(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjNameS(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjName(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjNameDSEnd(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjNameEnd(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjValueD(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjValueS(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjValue(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjValueO(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjValueDSEnd(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjValueEnd(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateObjValueOEnd(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateArray(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateArrayItemD(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateArrayItemS(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateArrayItem(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateArrayItemDSEnd(LPJSONPARSECONTEXT pstJsonParseCtx);
static int onParseStateArrayItemEnd(LPJSONPARSECONTEXT pstJsonParseCtx);

/**
 * 处理解析错误
 * @param pstCtx 上下文相关对象
 * @param iErrCode 错误码
 * @param szErrInfo 错误消息
 */
static void handleParseError(LPJSONPARSECONTEXT pstCtx, int iErrCode, const char* szErrInfo)
{
    if (pstCtx->pstParseHandler && pstCtx->pstParseHandler->onParseError)
    {
        pstCtx->pstParseHandler->onParseError(iErrCode, szErrInfo, pstCtx->szJsonBuf, pstCtx->pstCallData);
    }
}

/**
 * 处理json子元素/子对象开始
 * @param pstCtx 上下文相关对象
 * @param szName 子元素/子对象名称
 * @param chJsonType 子元素/子对象类型
 * @return 成功返回0，失败返回错误码
 */
static int handleStartElement(LPJSONPARSECONTEXT pstCtx, const char* szName, char chJsonType, char chItemType)
{
    if (NULL == pstCtx || NULL == szName) return ERROR_INPUT_PARAM_NULL;

    if (pstCtx->iObjNameIndex + 1 >= MAX_JSON_OBJECT_NUMBER)
    {
        handleParseError(pstCtx, ERROR_JSON_PARSE_TOO_MANY_OBJ, "too many json object");
        return ERROR_JSON_PARSE_TOO_MANY_OBJ;
    }

    if (pstCtx->pstParseHandler && pstCtx->pstParseHandler->onStartElement)
    {
        int iRet = pstCtx->pstParseHandler->onStartElement(szName, pstCtx->pstCallData);
        if (iRet != 0) return iRet;
    }
    /* 入栈 */
    STRNCPY(pstCtx->stObjInfo[++pstCtx->iObjNameIndex].szObjName, sizeof(pstCtx->szCurrentName), szName, sizeof(pstCtx->szCurrentName));

    pstCtx->stObjInfo[pstCtx->iObjNameIndex].chJsonType = chJsonType;
    pstCtx->stObjInfo[pstCtx->iObjNameIndex].chItemType = chItemType;

    return 0;
}

/**
 * 处理json子元素/子对象开始
 * @param pstCtx 上下文相关对象
 * @return 成功返回0，失败返回错误码
 */
static int handleStartElementNoPush(LPJSONPARSECONTEXT pstCtx)
{
    if (NULL == pstCtx) return ERROR_INPUT_PARAM_NULL;

    if (pstCtx->iObjNameIndex < 0 || pstCtx->iObjNameIndex >= MAX_JSON_OBJECT_NUMBER)
    {
        handleParseError(pstCtx, ERROR_JSON_FORMAT_ERROR, "json format error");
        return ERROR_JSON_FORMAT_ERROR;
    }

    if (pstCtx->pstParseHandler && pstCtx->pstParseHandler->onStartElement)
    {
        int iRet = pstCtx->pstParseHandler->onStartElement(pstCtx->stObjInfo[pstCtx->iObjNameIndex].szObjName, pstCtx->pstCallData);
        if (iRet != 0) return iRet;
    }

    return 0;
}

/**
 * 处理子元素/子对象解析结束
 * @param pstCtx 上下文相关对象
 * @param szValue 子元素/子对象的值
 * @param bPop 是否将子元素/子对象出栈
 * @return 成功返回0，失败返回错误码
 */
static int handleEndElement(LPJSONPARSECONTEXT pstCtx, const char* szValue, U8 bPop)
{
    if (NULL == pstCtx || NULL == szValue) return ERROR_INPUT_PARAM_NULL;

    if (pstCtx->iObjNameIndex < 0)
    {
        handleParseError(pstCtx, ERROR_JSON_FORMAT_ERROR, "format error");
        return ERROR_JSON_FORMAT_ERROR;
    }

    if (pstCtx->pstParseHandler && pstCtx->pstParseHandler->onEndElement)
    {
        int iRet = pstCtx->pstParseHandler->onEndElement(pstCtx->stObjInfo[pstCtx->iObjNameIndex].szObjName, szValue,
            pstCtx->stObjInfo[pstCtx->iObjNameIndex].chJsonType, pstCtx->stObjInfo[pstCtx->iObjNameIndex].chItemType,
            pstCtx->pstCallData);
        if (iRet != 0) return iRet;
    }
    /* pop from list */
    if (1 == bPop)
    {
        if (pstCtx->iObjNameIndex >= 0)
        {
            pstCtx->iObjNameIndex--;
        }
        else
        {
            handleParseError(pstCtx, ERROR_JSON_FORMAT_ERROR, "format error");
            return ERROR_JSON_FORMAT_ERROR;
        }
    }

    return 0;
}

#define APPEND_CURRENT_ELEMENT_NAME(pstCtx) \
    if (pstCtx->dwCurrentNameIndex + 1 >= sizeof(pstCtx->szCurrentName)) \
    { \
        handleParseError(pstCtx, ERROR_JSON_OBJECT_TOO_LONG, "json object name or value is too long"); \
        return ERROR_JSON_OBJECT_TOO_LONG; \
    } \
    pstCtx->szCurrentName[pstCtx->dwCurrentNameIndex++] = *pstCtx->szJsonBuf

// 状态处理函数
int onParseStateInit(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case '\"':
        pstJsonParseCtx->fnParseState = onParseStateObjNameD;
        break;
    case '\'':
        pstJsonParseCtx->fnParseState = onParseStateObjNameS;
        break;
    case '{':
        pstJsonParseCtx->fnParseState = onParseStateObjValueEnd;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleStartElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, kJsonValueTypeObject, 0), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        break;
    case '[':
        pstJsonParseCtx->fnParseState = onParseStateArray;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleStartElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, kJsonValueTypeArray, 0), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
            pstJsonParseCtx->fnParseState = onParseStateObjName;
        }
        break;
    }
    return 0;
}

int onParseStateObjNameD(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == '\"')
    {
        pstJsonParseCtx->fnParseState = onParseStateObjNameDSEnd;
    }
    else
    {
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
    }
    return 0;
}

int onParseStateObjNameS(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == '\'')
    {
        pstJsonParseCtx->fnParseState = onParseStateObjNameDSEnd;
    }
    else
    {
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
    }
    return 0;
}

int onParseStateObjName(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == ':')
    {
        int iRet;

        pstJsonParseCtx->fnParseState = onParseStateObjNameEnd;
        /* 去掉空格 */
        while (pstJsonParseCtx->dwCurrentNameIndex > 0 && isspace(*(pstJsonParseCtx->szCurrentName + pstJsonParseCtx->dwCurrentNameIndex - 1))) pstJsonParseCtx->dwCurrentNameIndex--;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        /* NOTE: 这里可以检查一下对象名称中是否包含非法字符，比如空格，短杠等 */
        CHECK_FUNC_RET(handleStartElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 0, 0), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
    }
    else
    {
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
    }
    return 0;
}

int onParseStateObjNameDSEnd(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == ':')
    {
        int iRet;

        pstJsonParseCtx->fnParseState = onParseStateObjNameEnd;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleStartElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 0, 0), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
    }
    else if (!isspace(*pstJsonParseCtx->szJsonBuf))
    {
        handleParseError(pstJsonParseCtx, ERROR_JSON_NAME_FORMAT, "json format error: encounter invalid character after object name");
        return ERROR_JSON_NAME_FORMAT;
    }
    return 0;
}

int onParseStateObjNameEnd(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case '\"':
        pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType = kJsonValueTypeString;
        pstJsonParseCtx->fnParseState = onParseStateObjValueD;
        break;
    case '\'':
        pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType = kJsonValueTypeString;
        pstJsonParseCtx->fnParseState = onParseStateObjValueS;
        break;
    case '{':
        pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType = kJsonValueTypeObject;
        pstJsonParseCtx->fnParseState = onParseStateObjValueO;
        break;
    case '[':
        pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType = kJsonValueTypeArray;
        pstJsonParseCtx->fnParseState = onParseStateArray;
        break;
    case ',':
        // Number的值出现空值
        pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType = kJsonValueTypeNumber;
        pstJsonParseCtx->fnParseState = onParseStateObjValueEnd;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType = kJsonValueTypeNumber;
            APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
            pstJsonParseCtx->fnParseState = onParseStateObjValue;
        }
        break;
    }
    return 0;
}

int onParseStateObjValueD(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == '\"')
    {
        pstJsonParseCtx->fnParseState = onParseStateObjValueDSEnd;
    }
    else
    {
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
    }
    return 0;
}

int onParseStateObjValueS(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == '\'')
    {
        pstJsonParseCtx->fnParseState = onParseStateObjValueDSEnd;
    }
    else
    {
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
    }
    return 0;
}

int onParseStateObjValueO(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case '\"':
        pstJsonParseCtx->fnParseState = onParseStateObjNameD;
        break;
    case '\'':
        pstJsonParseCtx->fnParseState = onParseStateObjNameS;
        break;
    case '{':
    case '[':
    case ',':
        handleParseError(pstJsonParseCtx, ERROR_JSON_OBJECT_FORMAT, "json format error: encounter invalid character in object");
        return ERROR_JSON_OBJECT_FORMAT;
    case '}':
        pstJsonParseCtx->fnParseState = onParseStateObjValueOEnd;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
            pstJsonParseCtx->fnParseState = onParseStateObjName;
        }
        break;
    }
    return 0;
}

int onParseStateObjValue(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case ',':
    case '}':
        /* 去掉空格 */
        while (pstJsonParseCtx->dwCurrentNameIndex > 0 && isspace(*(pstJsonParseCtx->szCurrentName + pstJsonParseCtx->dwCurrentNameIndex - 1))) pstJsonParseCtx->dwCurrentNameIndex--;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        /* NOTE: 这里可以检查一下值中是否包含非数字字符 */
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 1), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        /* 如果是对象结束，连续再pop一次 */
        if ('}' == *pstJsonParseCtx->szJsonBuf)
        {
            pstJsonParseCtx->fnParseState = onParseStateObjValueOEnd;
            CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        }
        else
        {
            pstJsonParseCtx->fnParseState = onParseStateObjValueEnd;
        }
        break;
    default:
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
        break;
    }
    return 0;
}

int onParseStateObjValueDSEnd(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case ',':
    case '}':
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 1), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        /* 如果是对象结束，连续再pop一次 */
        if ('}' == *pstJsonParseCtx->szJsonBuf)
        {
            pstJsonParseCtx->fnParseState = onParseStateObjValueOEnd;
            CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        }
        else
        {
            pstJsonParseCtx->fnParseState = onParseStateObjValueEnd;
        }
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            handleParseError(pstJsonParseCtx, ERROR_JSON_OBJECT_FORMAT, "json format error: encounter invalid character in object");
            return ERROR_JSON_OBJECT_FORMAT;
        }
        break;
    }
    return 0;
}

int onParseStateObjValueEnd(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case '\"':
        pstJsonParseCtx->fnParseState = onParseStateObjNameD;
        break;
    case '\'':
        pstJsonParseCtx->fnParseState = onParseStateObjNameS;
        break;
    case '{':
    case '[':
    case ',':
        handleParseError(pstJsonParseCtx, ERROR_JSON_OBJECT_FORMAT, "json format error: encounter invalid character in object");
        return ERROR_JSON_OBJECT_FORMAT;
    case '}':
        pstJsonParseCtx->fnParseState = onParseStateObjValueOEnd;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
            pstJsonParseCtx->fnParseState = onParseStateObjName;
        }
        break;
    }
    return 0;
}

int onParseStateObjValueOEnd(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case ',':
        if (pstJsonParseCtx->iObjNameIndex >= 0 && kJsonValueTypeArray == pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType)
        {
            pstJsonParseCtx->fnParseState = onParseStateArray;
        }
        else
        {
            pstJsonParseCtx->fnParseState = onParseStateObjValueEnd;
        }
        break;
    case '}':
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        break;
    case ']':
        if (pstJsonParseCtx->iObjNameIndex >= 0 && kJsonValueTypeArray == pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType)
        {
            pstJsonParseCtx->fnParseState = onParseStateArrayItemEnd;
            CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        }
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            handleParseError(pstJsonParseCtx, ERROR_JSON_OBJECT_FORMAT, "json format error: encounter invalid character in object");
            return ERROR_JSON_OBJECT_FORMAT;
        }
        break;
    }
    return 0;
}

int onParseStateArray(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case '\"':
        pstJsonParseCtx->fnParseState = onParseStateArrayItemD;
        pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chItemType = kJsonValueTypeString;
        break;
    case '\'':
        pstJsonParseCtx->fnParseState = onParseStateArrayItemS;
        pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chItemType = kJsonValueTypeString;
        break;
    case '{':
        //CHECK_FUNC_RET(handleStartElement(pstJsonParseCtx, "", kJsonValueTypeArray, kJsonValueTypeObject), iRet);
        CHECK_FUNC_RET(handleStartElement(pstJsonParseCtx, pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].szObjName, kJsonValueTypeArray, kJsonValueTypeObject), iRet);
        //pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chItemType = kJsonValueTypeObject;
        pstJsonParseCtx->fnParseState = onParseStateObjValueEnd;
        break;
    case '[':
        CHECK_FUNC_RET(handleStartElement(pstJsonParseCtx, "", kJsonValueTypeArray, kJsonValueTypeArray), iRet);
        break;
    case ']':
        pstJsonParseCtx->fnParseState = onParseStateArrayItemEnd;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        break;
    case ',':
        /* 这里不做任何事情，忽略空的数组元素 */
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
            pstJsonParseCtx->fnParseState = onParseStateArrayItem;
            pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chItemType = kJsonValueTypeNumber;
        }
        break;
    }
    return 0;
}

int onParseStateArrayItemD(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == '\"')
    {
        pstJsonParseCtx->fnParseState = onParseStateArrayItemDSEnd;
    }
    else
    {
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
    }
    return 0;
}

int onParseStateArrayItemS(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    if (*pstJsonParseCtx->szJsonBuf == '\'')
    {
        pstJsonParseCtx->fnParseState = onParseStateArrayItemDSEnd;
    }
    else
    {
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
    }
    return 0;
}

int onParseStateArrayItem(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case ',':
        pstJsonParseCtx->fnParseState = onParseStateArray;
        /* 去掉空格 */
        while (pstJsonParseCtx->dwCurrentNameIndex > 0 && isspace(*(pstJsonParseCtx->szCurrentName + pstJsonParseCtx->dwCurrentNameIndex - 1))) pstJsonParseCtx->dwCurrentNameIndex--;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 0), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        // startElement
        CHECK_FUNC_RET(handleStartElementNoPush(pstJsonParseCtx), iRet);
        break;
    case ']':
        pstJsonParseCtx->fnParseState = onParseStateArrayItemEnd;
        /* 去掉空格 */
        while (pstJsonParseCtx->dwCurrentNameIndex > 0 && isspace(*(pstJsonParseCtx->szCurrentName + pstJsonParseCtx->dwCurrentNameIndex - 1))) pstJsonParseCtx->dwCurrentNameIndex--;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 1), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        break;
    default:
        APPEND_CURRENT_ELEMENT_NAME(pstJsonParseCtx);
        break;
    }
    return 0;
}

int onParseStateArrayItemDSEnd(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case ',':
        pstJsonParseCtx->fnParseState = onParseStateArray;
        /* 去掉空格 */
        while (pstJsonParseCtx->dwCurrentNameIndex > 0 && isspace(*(pstJsonParseCtx->szCurrentName + pstJsonParseCtx->dwCurrentNameIndex - 1))) pstJsonParseCtx->dwCurrentNameIndex--;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 0), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        // startElement
        CHECK_FUNC_RET(handleStartElementNoPush(pstJsonParseCtx), iRet);
        break;
    case ']':
        pstJsonParseCtx->fnParseState = onParseStateArrayItemEnd;
        /* 去掉空格 */
        while (pstJsonParseCtx->dwCurrentNameIndex > 0 && isspace(*(pstJsonParseCtx->szCurrentName + pstJsonParseCtx->dwCurrentNameIndex - 1))) pstJsonParseCtx->dwCurrentNameIndex--;
        pstJsonParseCtx->szCurrentName[pstJsonParseCtx->dwCurrentNameIndex] = 0;
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, pstJsonParseCtx->szCurrentName, 1), iRet);
        pstJsonParseCtx->dwCurrentNameIndex = 0;
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            handleParseError(pstJsonParseCtx, ERROR_JSON_ARRAY_FORMAT, "json format error: encounter invalid character in array");
            return ERROR_JSON_ARRAY_FORMAT;
        }
        break;
    }
    return 0;
}

int onParseStateArrayItemEnd(LPJSONPARSECONTEXT pstJsonParseCtx)
{
    int iRet;

    switch (*pstJsonParseCtx->szJsonBuf)
    {
    case ',':
        if (pstJsonParseCtx->iObjNameIndex >= 0 && kJsonValueTypeArray == pstJsonParseCtx->stObjInfo[pstJsonParseCtx->iObjNameIndex].chJsonType)
        {
            pstJsonParseCtx->fnParseState = onParseStateArray;
        }
        else
        {
            pstJsonParseCtx->fnParseState = onParseStateInit;
        }
        break;
    case ']':
    case '}':
        CHECK_FUNC_RET(handleEndElement(pstJsonParseCtx, "", 1), iRet);
        break;
    default:
        if (!isspace(*pstJsonParseCtx->szJsonBuf))
        {
            handleParseError(pstJsonParseCtx, ERROR_JSON_ARRAY_FORMAT, "json format error: encounter invalid character in array");
            return ERROR_JSON_ARRAY_FORMAT;
        }
        break;
    }

    return 0;
}

/**
 * 解析json字符串
 * @param szJson 被解析的字符串
 * @param pstParseHandler 解析处理器
 * @return 成功返回0，失败返回错误码
 */
int parse_json_string(const char* szJson, LPJSONPARSEHANDLER pstParseHandler, void* pstCallData)
{
    JSONPARSECONTEXT stJsonParseContext;
    int iRet = 0;

    if (NULL == szJson || NULL == pstParseHandler) return ERROR_INPUT_PARAM_NULL;

    stJsonParseContext.dwCurrentNameIndex = 0;
    stJsonParseContext.iObjNameIndex = -1;
    stJsonParseContext.fnParseState = onParseStateInit;
    stJsonParseContext.szJsonBuf = szJson;
    stJsonParseContext.pstParseHandler = pstParseHandler;
    stJsonParseContext.pstCallData = pstCallData;

    while (*stJsonParseContext.szJsonBuf)
    {
        iRet = stJsonParseContext.fnParseState(&stJsonParseContext);
        if (iRet != 0) return iRet;
        stJsonParseContext.szJsonBuf++;
    }

    if (stJsonParseContext.iObjNameIndex != -1)
    {
        if (pstParseHandler->onParseError)
        {
            pstParseHandler->onParseError(ERROR_JSON_FORMAT_ERROR, "json format error", "", pstCallData);
        }
        return ERROR_JSON_FORMAT_ERROR;
    }

    return 0;
}

/**
 * 解析json文件
 * @param szFileName 被解析的json
 * @param pstParseHandler 解析处理器
 * @return 成功返回0，失败返回错误码
 */
int parse_json_file(const char* szFileName, LPJSONPARSEHANDLER pstParseHandler, void* pstCallData)
{
    JSONPARSECONTEXT stJsonParseContext;
    int iRet = 0;
    char szJson[8192];
    int iLen = 0;
    int fd;

    if (NULL == szFileName || NULL == pstParseHandler) return ERROR_INPUT_PARAM_NULL;

    stJsonParseContext.dwCurrentNameIndex = 0;
    stJsonParseContext.iObjNameIndex = -1;
    stJsonParseContext.fnParseState = onParseStateInit;
    //stJsonParseContext.szJsonBuf = szJson;
    stJsonParseContext.pstParseHandler = pstParseHandler;
    stJsonParseContext.pstCallData = pstCallData;

    /* open file */
    fd = open(szFileName, O_RDONLY, 0666);
    if (fd < 0)
    {
        if (pstParseHandler->onParseError)
        {
            char szMsg[2048];
            SNPRINTF(szMsg, sizeof(szMsg), "open file error:%d,%s", errno, strerror(errno));
            pstParseHandler->onParseError(ERROR_FILE_OPEN_FAILURE, szMsg, "", pstCallData);
        }
        return ERROR_FILE_OPEN_FAILURE;
    }

    while (1)
    {
        iLen = read(fd, szJson, sizeof(szJson) - 1);

        /* 读完文件 */
        if (0 == iLen) break;

        /* 读文件错误 */
        if (iLen < 0)
        {
            if (pstParseHandler->onParseError)
            {
                char szMsg[2048];
                SNPRINTF(szMsg, sizeof(szMsg), "read file error:%d,%s", errno, strerror(errno));
                pstParseHandler->onParseError(ERROR_FILE_OPEN_FAILURE, szMsg, "", pstCallData);
            }

            close(fd);
            return ERROR_FILE_READ_FAILURE;
        }

        /* 处理读取数据 */
        szJson[iLen] = 0;
        stJsonParseContext.szJsonBuf = szJson;
        while (*stJsonParseContext.szJsonBuf)
        {
            iRet = stJsonParseContext.fnParseState(&stJsonParseContext);
            if (iRet != 0)
            {
                close(fd);
                return iRet;
            }
            stJsonParseContext.szJsonBuf++;
        }
    }

    /* close file */
    close(fd);

    if (stJsonParseContext.iObjNameIndex != -1)
    {
        if (pstParseHandler->onParseError)
        {
            pstParseHandler->onParseError(ERROR_JSON_FORMAT_ERROR, "json format error", "", pstCallData);
        }
        return ERROR_JSON_FORMAT_ERROR;
    }

    return 0;
}

