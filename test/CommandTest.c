/*
 * @(#) UriTest.c Created on 2017-04-23
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */


#include "UriTest.h"
#include <stdio.h>
#include <string.h>
#include "comm/CommandParser.h"

/**
 * 命令处理函数
 * @param iArgc 命令参数数量，含命令名，第一个未命令名，其他的为命令参数
 * @param pszArgv 命令参数列表
 * @param pstCallData 调用方数据
 */
int onCommand(int iArgc, char pszArgv[][MAX_CMD_PARAM_LEN], void* pstCallData)
{
    int i;

    for (i = 0; i < iArgc; i++)
    {
        printf("%s ", pszArgv[i]);
    }
    printf("\n");

    return 0;
}

/**
 * 错误处理函数
 * @param iErrCode 错误码
 * @param szErrInfo 错误消息
 * @param szCommand 错误发生的位置
 * @param pstCallData 调用方数据
 */
void onParseError(int iErrCode, const char* szErrInfo, const char* szCommand, void* pstCallData)
{
    printf("%s,%d,'%c' is unexpected,%s\n", szErrInfo, iErrCode, *szCommand, szCommand);
}

void test_parse_string()
{
    COMMANDPARSEHANDLER stHdr;
    int iRet;
    const char* szCommand = "# Just a test!!!\n  setX\n  ps -ef | grep test";

    stHdr.onCommand = onCommand;
    stHdr.onParseError = onParseError;

    iRet = parse_command_string(szCommand, &stHdr, NULL);
    if (iRet != 0)
    {
        printf("parse string error:%d\n", iRet);
    }
}

void test_command_parse(const char* szFileName)
{
    test_parse_string();
    printf("+++++++++++++++++++++++++++++++++\n");

    COMMANDPARSEHANDLER stHdr;
    int iRet;

    stHdr.onCommand = onCommand;
    stHdr.onParseError = onParseError;

    iRet = parse_command_file(szFileName, &stHdr, NULL);
    if (iRet != 0)
    {
        printf("parse string error:%d\n", iRet);
    }
}

