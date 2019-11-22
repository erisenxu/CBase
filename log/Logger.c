/*
 * @(#) Logger.c Created on 2008-03-27 日志打印相关函数
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "Logger.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"
#include "comm/FileLib.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

LPLOGGER gs_pstLogger = NULL;

/**
 * 根据日志级别描述返回日志级别
 * @param szLevel 级别描述，若不是定义描述，则使用调试级别
 */
static int get_log_level(const char *szLevel)
{
    if (strncasecmp(szLevel, "debug", sizeof("debug")) == 0)
    {
        return LOG_LV_DEBUG;
    }
    else if (strncasecmp(szLevel, "protocol", sizeof("protocol")) == 0)
    {
        return LOG_LV_PROTOCOL;
    }
    else if (strncasecmp(szLevel, "info", sizeof("info")) == 0)
    {
        return LOG_LV_INFO;
    }
    else if (strncasecmp(szLevel, "warn", sizeof("warn")) == 0)
    {
        return LOG_LV_WARN;
    }
    else if (strncasecmp(szLevel, "error", sizeof("error")) == 0)
    {
        return LOG_LV_ERROR;
    }
    else if (strncasecmp(szLevel, "fatal", sizeof("fatal")) == 0)
    {
        return LOG_LV_FATAL;
    }
    else if (strncasecmp(szLevel, "result", sizeof("result")) == 0)
    {
        return LOG_LV_FATAL;
    }
    else if (strncasecmp(szLevel, "print", sizeof("print")) == 0)
    {
        return LOG_LV_PRINT;
    }
    else if (strncasecmp(szLevel, "println", sizeof("println")) == 0)
    {
        return LOG_LV_PRINT_LN;
    }
    else
    {
        /* 默认采用error级别的日志 */
        return LOG_LV_ERROR;
    }
}

/**
 * 返回指定日志级别的级别描述
 */
static const char* get_log_level_name(int nLevel)
{
    switch(nLevel)
    {
    case LOG_LV_DEBUG:
        return "DEBUG";
    case LOG_LV_PROTOCOL:
        return "PROTOCOL";
    case LOG_LV_INFO:
        return "INFO";
    case LOG_LV_WARN:
        return "WARNING";
    case LOG_LV_ERROR:
        return "ERROR";
    case LOG_LV_FATAL:
        return "FATAL ERROR";
    case LOG_LV_RESULT:
        return "RESULT";
    case LOG_LV_PRINT:
        return "PRINT";
    case LOG_LV_PRINT_LN:
        return "PRINTLN";
    }
    return "N/A";
}

/**
 * 实现日志文件之间的切换和备份
 * @param iMaxLogNum 最大日志文件数量
 * @param szLogFileName 日志文件名
 * @return 成功返回0，失败返回错误码
 */
static int do_shift_log_files(int iMaxLogNum, const char *szLogFileName)
{
    char szOldLogFileName[MAX_PATH];
    char szNewLogFileName[MAX_PATH];
    int i;

    if (NULL == szLogFileName) return ERROR_INPUT_PARAM_NULL;

    for (i = iMaxLogNum - 2; i >= 0; i--)
    {
        if (0 == i)
        {
            snprintf(szOldLogFileName, sizeof(szOldLogFileName), "%s", szLogFileName);
        }
        else
        {
            snprintf(szOldLogFileName, sizeof(szOldLogFileName), "%s__bak%d", szLogFileName, i);
        }

        if (access(szOldLogFileName, F_OK) == 0)
        {
            snprintf(szNewLogFileName, sizeof(szNewLogFileName), "%s__bak%d", szLogFileName, i + 1);
            if (rename(szOldLogFileName, szNewLogFileName) < 0)
            {
                return ERROR_FILE_RENAME_FAILURE;
            }
        }
    }

    return 0;
}

/**
 * 判断日志是否达到最大值，若达到，先备份下
 * @param pstLogger 日志对象
 * @return 成功返回0，失败返回错误码
 */
static int shift_log_files(LPLOGGER pstLogger)
{
    struct stat stStat;

    if(stat(pstLogger->szLogFileName, &stStat) < 0) return ERROR_FILE_STAT_FAILURE;

    if (stStat.st_size < pstLogger->iMaxLogSize) return 0;

    return do_shift_log_files(pstLogger->iMaxLogNum, pstLogger->szLogFileName);
}

/**
 * 将日志打印到文件
 * @param iLogLevel 日志打印级别
 * @param pstLogTv 日志打印时间
 * @param szFormat 格式化串
 * @param ap 打印参数列表
 * @return 成功返回0，失败返回错误码
 */
static int logger_write(int iLogLevel, struct timeval* pstLogTv, const char* szFormat, va_list ap)
{
    FILE* pLogFile;
    struct tm* pLogTime;
    time_t tLogTime;

    if (NULL == gs_pstLogger) return ERROR_LOG_NO_INIT;
    if (NULL == szFormat || NULL == pstLogTv) return ERROR_INPUT_PARAM_NULL;

    /* 打开日志文件 */
    pLogFile = fopen(gs_pstLogger->szLogFileName, "a+");

    if (NULL == pLogFile)
    {
        return ERROR_LOG_OPEN_FILE_FAILURE;
    }

    /* 日志打印时间 */
    tLogTime = pstLogTv->tv_sec;
    pLogTime = localtime(&tLogTime);

    /* 打印日志头 */
    fprintf(pLogFile, "[%04d%02d%02d %02d:%02d:%02d.%.6d]<%s> ", pLogTime->tm_year + 1900, pLogTime->tm_mon + 1, pLogTime->tm_mday,
            pLogTime->tm_hour, pLogTime->tm_min, pLogTime->tm_sec, (int)pstLogTv->tv_usec, get_log_level_name(iLogLevel));

    /* 打印日志内容 */
    vfprintf(pLogFile, szFormat, ap);
    fprintf(pLogFile, "\n");

    /* 关闭文件 */
    fclose(pLogFile);

    /* 检查日志是否过大，如果过大，备份起来 */
    shift_log_files(gs_pstLogger);

    return 0;
}

/**
 * 初始化日志结构体对象
 * @param pstLogger 被初始化的日志对象
 * @param szLogPath 日志存放路径
 * @param szBaseFileName 日志文件名
 * @param szLogLevel 日志等级名
 * @param iMaxLogSize 每个日志文件的最大文件大小，单位：byte
 * @param iMaxLogNum 最大日志文件累计数量
 * @return 成功返回0，失败返回错误码
 */
int init_logger(LPLOGGER pstLogger, const char* szLogPath, const char* szBaseFileName, const char* szLogLevel,
                int iMaxLogSize, int iMaxLogNum)
{
    if (NULL == pstLogger || NULL == szLogPath || NULL == szBaseFileName || NULL == szLogLevel) return ERROR_INPUT_PARAM_NULL;

    if (iMaxLogSize <= 0) return ERROR_LOG_MAX_SIZE_INVALID;

    if (iMaxLogNum <= 0) return ERROR_LOG_MAX_FILE_NUM_INVALID;

    gs_pstLogger = pstLogger;

    STRNCPY(gs_pstLogger->szLogPath, sizeof(gs_pstLogger->szLogPath), szLogPath, sizeof(gs_pstLogger->szLogPath));
    STRNCPY(gs_pstLogger->szBaseFileName, sizeof(gs_pstLogger->szBaseFileName), szBaseFileName, sizeof(gs_pstLogger->szBaseFileName));
    snprintf(gs_pstLogger->szLogFileName, sizeof(gs_pstLogger->szLogFileName), "%s/%s.log", szLogPath, szBaseFileName);

    gs_pstLogger->iLogLevel = get_log_level(szLogLevel);
    gs_pstLogger->iMaxLogNum = iMaxLogNum;
    gs_pstLogger->iMaxLogSize = iMaxLogSize;

    /* 初始化日志路径 */
    create_dir(gs_pstLogger->szLogFileName, 0755);

    /* 备份一下旧的日志 */
    shift_log_files(gs_pstLogger);

    return 0;
}

/**
 * 按日志基本打印日志
 * @param iLogLevel 日志级别
 * @param szFormat 格式化串
 * @return 成功返回0，失败返回错误码
 */
int log_info(int iLogLevel, const char* szFormat, ...)
{
    struct timeval stLogTv;
    va_list ap;

    if (NULL == gs_pstLogger) return ERROR_LOG_NO_INIT;

    /* 日志级别低，无须写日志 */
    if (gs_pstLogger->iLogLevel > iLogLevel) return 0;

    if (NULL == szFormat) return ERROR_INPUT_PARAM_NULL;

    va_start(ap, szFormat);
    gettimeofday(&stLogTv, NULL);
    logger_write(iLogLevel, &stLogTv, szFormat, ap);
    va_end(ap);

    return 0;
}

