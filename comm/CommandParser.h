/*
 * @(#) CommandParser.h Created on 2018-08-05 命令解析
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#define MAX_CMD_PARAM_NUM   64
#define MAX_CMD_PARAM_LEN   256

/**
 * 命令处理函数
 * @param iArgc 命令参数数量，含命令名，第一个未命令名，其他的为命令参数
 * @param pszArgv 命令参数列表
 * @param pstCallData 调用方数据
 */
typedef int (*FNCOMMANDPARSEHANDLER) (int iArgc, char pszArgv[][MAX_CMD_PARAM_LEN], void* pstCallData);

/**
 * 错误处理函数
 * @param iErrCode 错误码
 * @param szErrInfo 错误消息
 * @param szCommand 错误发生的位置
 * @param pstCallData 调用方数据
 */
typedef void (*FNCOMMANDPARSEERRORHANDLER) (int iErrCode, const char* szErrInfo, const char* szCommand, void* pstCallData);

/**
 * 命令解析处理器
 */
struct tagCommandParseHandler
{
    FNCOMMANDPARSEHANDLER onCommand;            /* 命令处理函数 */
    FNCOMMANDPARSEERRORHANDLER onParseError;    /* 解析错误处理函数 */
};

typedef struct tagCommandParseHandler   COMMANDPARSEHANDLER;
typedef struct tagCommandParseHandler*  LPCOMMANDPARSEHANDLER;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 解析命令字符串
 * @param szCommand 被解析的字符串
 * @param pstParseHandler 解析处理器
 * @param pstCallData 调用方数据
 * @return 成功返回0，失败返回错误码
 */
int parse_command_string(const char* szCommand, LPCOMMANDPARSEHANDLER pstParseHandler, void* pstCallData);

/**
 * 解析命令文件
 * @param szFileName 被解析的命令文件名称
 * @param pstParseHandler 解析处理器
 * @param pstCallData 调用方数据
 * @return 成功返回0，失败返回错误码
 */
int parse_command_file(const char* szFileName, LPCOMMANDPARSEHANDLER pstParseHandler, void* pstCallData);

#ifdef __cplusplus
}
#endif

#endif /* COMMAND_PARSER_H */
