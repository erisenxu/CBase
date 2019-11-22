/*
 * @(#) Logger.h Created on 2008-03-27 日志打印相关函数
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "comm/BaseDef.h"

/**
 * 日志级别常量定义
 */
enum tagLogLevel
{
    LOG_LV_DEBUG    = 100,  /* debug级别 */
    LOG_LV_PROTOCOL = 200,  /* protocol级别 */
    LOG_LV_INFO     = 300,  /* info级别 */
    LOG_LV_WARN     = 400,  /* warning级别 */
    LOG_LV_ERROR    = 500,  /* error级别 */
    LOG_LV_FATAL    = 600,  /* fatal级别 */
    LOG_LV_RESULT   = 700,  /* result级别 */
    LOG_LV_PRINT    = 800,  /* print级别 */
    LOG_LV_PRINT_LN = 900,  /* print line级别 */
};

/**
 * 日志结构体
 */
struct tagLogger
{
    char szLogPath[MAX_PATH];       /* 日志路径 */
    char szBaseFileName[MAX_PATH];  /* 日志文件基础名 */
    char szLogFileName[MAX_PATH];   /* 日志文件名，包含路径 */
    int iLogLevel;                  /* 日志打印级别 */
    int iMaxLogSize;                /* 每个日志文件的最大文件大小，单位：byte */
    int iMaxLogNum;                 /* 最大日志文件累计数量 */
};

typedef struct tagLogger    LOGGER;
typedef struct tagLogger*   LPLOGGER;

#define LOG_DEBUG(szFormat, args...)            log_info(LOG_LV_DEBUG, szFormat, ##args)
#define LOG_LV_PROTOCOL(szFormat, args...)      log_info(LOG_LV_PROTOCOL, szFormat, ##args)
#define LOG_INFO(szFormat, args...)             log_info(LOG_LV_INFO, szFormat, ##args)
#define LOG_WARN(szFormat, args...)             log_info(LOG_LV_WARN, szFormat, ##args)
#define LOG_ERROR(szFormat, args...)            log_info(LOG_LV_ERROR, szFormat, ##args)
#define LOG_FATAL(szFormat, args...)            log_info(LOG_LV_FATAL, szFormat, ##args)
#define LOG_RESULT(szFormat, args...)           log_info(LOG_LV_RESULT, szFormat, ##args)
#define LOG_PRINT(szFormat, args...)            log_info(LOG_LV_PRINT, szFormat, ##args)
#define LOG_PRINTLN(szFormat, args...)          log_info(LOG_LV_PRINT_LN, szFormat, ##args)

#ifdef __cplusplus
extern "C"
{
#endif

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
                int iMaxLogSize, int iMaxLogNum);

/**
 * 按日志基本打印日志。使用__attribute__声明函数，让编译器对参数进行检查
 * @param iLogLevel 日志级别
 * @param szFormat 格式化串
 * @return 成功返回0，失败返回错误码
 */
int log_info(int iLogLevel, const char* szFormat, ...) __attribute__((format(printf, 2, 3)));

#ifdef __cplusplus
}
#endif

#endif

