/*
 * @(#) TimeLib.h Created on 2016-12-25 时间相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef TIME_LIB_H
#define TIME_LIB_H

#include <time.h>
#include <sys/time.h>

#define MAX_TIME_STRING_LEN     64

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 计算两个时间之间的毫秒差值
 * @param pstTv1 时间1
 * @param pstTv2 时间2
 * @return 返回两个时间之间的毫秒差值
 */
unsigned int ms_pass(struct timeval* pstTv1, struct timeval* pstTv2);

/**
 * 将时间转换为"YYYY-MM-DD HH:mm:SS"的字符串
 * @param nTime 要转换的时间
 * @return 返回转换的时间字符串
 */
const char* time_to_date_string(char* szBuf, int iBufSize, time_t nTime);

/**
 * 将时间戳转换为YYYYMM的整数
 * @param nTime 要转换的时间
 * @return 返回转换的YYYYMM的整数
 */
int time_to_date(time_t nTime);

/**
 * 将时间戳转换为YYYYMMDD的整数
 * @param nTime 要转换的时间
 * @return 返回转换的YYYYMMDD的整数
 */
int time_to_day(time_t nTime);

/**
 * 将时间戳转换为HHmmSS的整数
 * @param nTime 要转换的时间
 * @return 返回转换的HHmmSS的整数
 */
int time_to_second(time_t nTime);

/**
 * 设置当前tick时间
 * @param pstTv tick时间
 */
void set_cur_tick_time(struct timeval* pstTv);

/**
 * 获取当前的tick时间
 * @return 返回当前tick周期更新的时间
 */
time_t get_cur_tick_time(void);

#ifdef __cplusplus
}
#endif

#endif

