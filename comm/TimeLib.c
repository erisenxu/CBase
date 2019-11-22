/*
 * @(#) TimeLib.c Created on 2016-12-25 时间相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "TimeLib.h"
#include "BaseFuncDef.h"

#include <stdlib.h>
#include <sys/time.h>

#define MAX_SEC_TO_MS 1000000  /* 当秒转换成毫秒时，最大值 */

/**
 * 当前一次Tick周期开始时更新timeval
 */
static struct timeval g_stCurTickTimeVal;

/**
 * 计算两个时间之间的毫秒差值
 * @param pstTv1 时间1
 * @param pstTv2 时间2
 * @return 返回两个时间之间的毫秒差值
 */
unsigned int ms_pass(struct timeval* pstTv1, struct timeval* pstTv2)
{
    int iSec = pstTv1->tv_sec - pstTv2->tv_sec;

    if (iSec < -MAX_SEC_TO_MS)
    {
        iSec =  -MAX_SEC_TO_MS;
    }
    else if(iSec > MAX_SEC_TO_MS)
    {
        iSec = MAX_SEC_TO_MS; /* 防溢出 */
    }

    return iSec * 1000 + (pstTv1->tv_usec - pstTv2->tv_usec)/1000;
}

/**
 * 将时间转换为"YYYY-MM-DD HH:mm:SS"的字符串
 * @param nTime 要转换的时间
 * @return 返回转换的时间字符串
 */
const char* time_to_date_string(char* szBuf, int iBufSize, time_t nTime)
{
    struct tm oTm;

    localtime_r(&nTime, &oTm);

    SNPRINTF(szBuf, iBufSize, "%04d-%02d-%02d %02d:%02d:%02d",
             oTm.tm_year + 1900, oTm.tm_mon + 1, oTm.tm_mday,
             oTm.tm_hour, oTm.tm_min, oTm.tm_sec);

    return szBuf;
}

/**
 * 将时间戳转换为YYYYMM的整数
 * @param nTime 要转换的时间
 * @return 返回转换的YYYYMM的整数
 */
int time_to_date(time_t nTime)
{
    struct tm oTm;

    localtime_r(&nTime, &oTm);

    return (oTm.tm_year + 1900) * 100 + oTm.tm_mon + 1;
}

/**
 * 将时间戳转换为YYYYMMDD的整数
 * @param nTime 要转换的时间
 * @return 返回转换的YYYYMMDD的整数
 */
int time_to_day(time_t nTime)
{
    struct tm oTm;

    localtime_r(&nTime, &oTm);

    return (oTm.tm_year + 1900) * 10000 + (oTm.tm_mon + 1)*100 + oTm.tm_mday;
}

/**
 * 将时间戳转换为HHmmSS的整数
 * @param nTime 要转换的时间
 * @return 返回转换的HHmmSS的整数
 */
int time_to_second(time_t nTime)
{
    struct tm oTm;

    localtime_r(&nTime, &oTm);

    return oTm.tm_hour * 10000 + oTm.tm_min * 100 + oTm.tm_sec;
}

/**
 * 设置当前tick时间
 * @param pstTv tick时间
 */
void set_cur_tick_time(struct timeval* pstTv)
{
    if (NULL == pstTv)
    {
        gettimeofday(&g_stCurTickTimeVal, NULL);
    }
    else
    {
        g_stCurTickTimeVal.tv_sec = pstTv->tv_sec;
        g_stCurTickTimeVal.tv_usec = pstTv->tv_usec;
    }
}

/**
 * 获取当前的tick时间
 * @return 返回当前tick周期更新的时间
 */
time_t get_cur_tick_time(void)
{
    return g_stCurTickTimeVal.tv_sec;
}

