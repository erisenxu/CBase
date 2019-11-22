/*
 * @(#) CommandParser.c Created on 2018-08-05 命令解析
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "CommandParser.h"
#include "BaseDef.h"
#include "BaseFuncDef.h"
#include "ErrorCode.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> /* 使用strerror必须加上这个头文件，否则会导致崩溃，:( */
#include <errno.h>
#include <ctype.h>

/**
 * 解析器上下文相关对象
 */
typedef struct tagCommandParseContext   COMMANDPARSECONTEXT;
typedef struct tagCommandParseContext*  LPCOMMANDPARSECONTEXT;

/**
 * 状态处理函数
 * @param pstParseCtx 解析器上下文相关对象
 * @return 成功返回0，失败返回错误码
 */
typedef int (*FNCOMMANDPARSESTATEHANDLER) (LPCOMMANDPARSECONTEXT pstParseCtx);

/**
 * 上下文对象
 */
struct tagCommandParseContext
{
    const char* szCommandBuf;                           /* 当前处理的命令字符串 */
    void* pstCallData;                                  /* 调用方数据 */
    char szArgv[MAX_CMD_PARAM_NUM][MAX_CMD_PARAM_LEN];  /* 命令参数列表 */
    int iArgc;                                          /* 命令参数个数 */
    int iParamIndex;                                    /* 命令参数数组写索引 */
    FNCOMMANDPARSESTATEHANDLER fnParseState;            /* 当前状态处理函数 */
    LPCOMMANDPARSEHANDLER pstParseHandler;              /* 解析处理器 */
};

/**
 * 处理解析错误
 * @param pstCtx 上下文相关对象
 * @param iErrCode 错误码
 * @param szErrInfo 错误消息
 */
static void handleParseError(LPCOMMANDPARSECONTEXT pstCtx, int iErrCode, const char* szErrInfo)
{
    if (pstCtx->pstParseHandler && pstCtx->pstParseHandler->onParseError)
    {
        pstCtx->pstParseHandler->onParseError(iErrCode, szErrInfo, pstCtx->szCommandBuf, pstCtx->pstCallData);
    }
}

/**
 * 处理argv解析结束
 * @param pstCtx 上下文相关对象
 * @return 成功返回0，失败返回错误码
 */
static int handleArgvParseEnd(LPCOMMANDPARSECONTEXT pstCtx)
{
    if (NULL == pstCtx) return ERROR_INPUT_PARAM_NULL;

    pstCtx->szArgv[pstCtx->iArgc][pstCtx->iParamIndex] = 0;
    pstCtx->iArgc++;

    if (pstCtx->iArgc > 0 && pstCtx->pstParseHandler && pstCtx->pstParseHandler->onCommand)
    {
        int iRet = pstCtx->pstParseHandler->onCommand(pstCtx->iArgc, pstCtx->szArgv, pstCtx->pstCallData);
        if (iRet != 0) return iRet;
    }

    pstCtx->iArgc = 0;
    pstCtx->iParamIndex = 0;

    return 0;
}

#define APPEND_ARG_VALUE(pstCtx) \
    if (pstCtx->iArgc + 1 >= MAX_CMD_PARAM_NUM) \
    { \
        handleParseError(pstCtx, ERROR_ARG_VALUE_TOO_MANY, "too many command arg values"); \
        return ERROR_ARG_VALUE_TOO_MANY; \
    } \
    pstCtx->szArgv[pstCtx->iArgc][pstCtx->iParamIndex] = 0; \
    pstCtx->iParamIndex = 0; \
    pstCtx->iArgc++

#define APPEND_CURRENT_ARG_VALUE(pstCtx) \
    if (pstCtx->iParamIndex + 1 >= sizeof(pstCtx->szArgv[0])) \
    { \
        handleParseError(pstCtx, ERROR_ARG_VALUE_TOO_LONG, "command arg value is too long"); \
        return ERROR_ARG_VALUE_TOO_LONG; \
    } \
    pstCtx->szArgv[pstCtx->iArgc][pstCtx->iParamIndex++] = *pstCtx->szCommandBuf

// 状态处理函数
static int onParseStateInit(LPCOMMANDPARSECONTEXT pstParseCtx);
static int onParseStateComment(LPCOMMANDPARSECONTEXT pstParseCtx);
static int onParseStateArgv(LPCOMMANDPARSECONTEXT pstParseCtx);
static int onParseStateArgvNext(LPCOMMANDPARSECONTEXT pstParseCtx);

// 状态处理函数
int onParseStateInit(LPCOMMANDPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szCommandBuf)
    {
    case '#':
        pstParseCtx->fnParseState = onParseStateComment;
        break;
    default:
        if (!isspace(*pstParseCtx->szCommandBuf))
        {
            APPEND_CURRENT_ARG_VALUE(pstParseCtx);
            pstParseCtx->fnParseState = onParseStateArgv;
        }
        break;
    }
    return 0;
}

int onParseStateComment(LPCOMMANDPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szCommandBuf)
    {
    case '\n':
        pstParseCtx->fnParseState = onParseStateInit;
        break;
    default:
        break;
    }
    return 0;
}

int onParseStateArgv(LPCOMMANDPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szCommandBuf)
    {
    case '\n':
        pstParseCtx->fnParseState = onParseStateInit;
        return handleArgvParseEnd(pstParseCtx);
    default:
        if (!isspace(*pstParseCtx->szCommandBuf))
        {
            APPEND_CURRENT_ARG_VALUE(pstParseCtx);
        }
        else
        {
            APPEND_ARG_VALUE(pstParseCtx);
            pstParseCtx->fnParseState = onParseStateArgvNext;
        }
        break;
    }
    return 0;
}

// 状态处理函数
int onParseStateArgvNext(LPCOMMANDPARSECONTEXT pstParseCtx)
{
    switch (*pstParseCtx->szCommandBuf)
    {
    case '\n':
        pstParseCtx->fnParseState = onParseStateInit;
        return handleArgvParseEnd(pstParseCtx);
    default:
        if (!isspace(*pstParseCtx->szCommandBuf))
        {
            APPEND_CURRENT_ARG_VALUE(pstParseCtx);
            pstParseCtx->fnParseState = onParseStateArgv;
        }
        break;
    }
    return 0;
}

/**
 * 解析命令字符串
 * @param szCommand 被解析的字符串
 * @param pstParseHandler 解析处理器
 * @param pstCallData 调用方数据
 * @return 成功返回0，失败返回错误码
 */
int parse_command_string(const char* szCommand, LPCOMMANDPARSEHANDLER pstParseHandler, void* pstCallData)
{
    COMMANDPARSECONTEXT stParseContext;
    int iRet = 0;

    if (NULL == szCommand || NULL == pstParseHandler) return ERROR_INPUT_PARAM_NULL;

    stParseContext.szCommandBuf = szCommand;
    stParseContext.iArgc = 0;
    stParseContext.iParamIndex = 0;
    stParseContext.pstParseHandler = pstParseHandler;
    stParseContext.pstCallData = pstCallData;
    stParseContext.fnParseState = onParseStateInit;

    while (*stParseContext.szCommandBuf)
    {
        iRet = stParseContext.fnParseState(&stParseContext);
        if (iRet != 0) return iRet;
        stParseContext.szCommandBuf++;
    }

    if (stParseContext.fnParseState == onParseStateArgvNext || stParseContext.fnParseState == onParseStateArgv)
    {
        return handleArgvParseEnd(&stParseContext);
    }

    return 0;
}

/**
 * 解析命令文件
 * @param szFileName 被解析的命令文件名称
 * @param pstParseHandler 解析处理器
 * @param pstCallData 调用方数据
 * @return 成功返回0，失败返回错误码
 */
int parse_command_file(const char* szFileName, LPCOMMANDPARSEHANDLER pstParseHandler, void* pstCallData)
{
    COMMANDPARSECONTEXT stParseContext;
    int iRet = 0;
    char szCmd[8192];
    int iLen = 0;
    int fd;

    if (NULL == szFileName || NULL == pstParseHandler) return ERROR_INPUT_PARAM_NULL;

    stParseContext.iArgc = 0;
    stParseContext.iParamIndex = 0;
    stParseContext.fnParseState = onParseStateInit;
    stParseContext.pstParseHandler = pstParseHandler;
    stParseContext.pstCallData = pstCallData;

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
        iLen = read(fd, szCmd, sizeof(szCmd) - 1);

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
        szCmd[iLen] = 0;
        stParseContext.szCommandBuf = szCmd;
        while (*stParseContext.szCommandBuf)
        {
            iRet = stParseContext.fnParseState(&stParseContext);
            if (iRet != 0)
            {
                close(fd);
                return iRet;
            }
            stParseContext.szCommandBuf++;
        }
    }

    /* close file */
    close(fd);

    if (stParseContext.fnParseState == onParseStateArgvNext || stParseContext.fnParseState == onParseStateArgv)
    {
        return handleArgvParseEnd(&stParseContext);
    }

    return 0;
}
