/*
 * @(#) StrLib.c Created on 2008-03-27 字符串相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "StrLib.h"

#include <string.h>

/**
 * 查找字符串
 * @param szVal 要查找的字符串
 * @param nValLen 字符串长度
 * @return 返回第一次找到的字符串位置，若未找到，返回-1
 */
int index_of_string(const char* szStr, int nStrLen, const char* szVal, int nValLen)
{
    int i = 0;

    if (NULL == szStr || nStrLen <= 0 || NULL == szVal || nValLen <= 0) return -1;

    for (i = 0; i < nStrLen; i++)
    {
        if (nStrLen - i < nValLen) return -1;

        if (memcmp(szStr + i, szVal, nValLen) == 0) return i;
    }

    return -1;
}

/**
 * 查找字符串
 * @param szVal 要查找的字符串
 * @param nValLen 字符串长度
 * @return 返回最后出现字符串位置，若未找到，返回-1
 */
int last_index_of_string(const char* szStr, int nStrLen, const char* szVal, int nValLen)
{
    int i = 0;

    if (NULL == szStr || nStrLen <= 0 || NULL == szVal || nValLen <= 0) return -1;
    
    for (i = nStrLen - nValLen; i >= 0; i--)
    {
        if (memcmp(szStr + i, szVal, nValLen) == 0) return i;
    }

    return -1;
}

/**
 * 判断字符串是否以另一个字符串开始
 * @param szVal 要查找的字符串
 * @param nValLen 字符串长度
 * @return 若字符串以另一个字符串开始，则返回1，否则返回0
 */
unsigned char string_start_with(const char* szStr, int nStrLen, const char* szVal, int nValLen)
{
    if (NULL == szStr || NULL == szVal || nStrLen < nValLen) return 0;

    if (memcmp(szStr, szVal, nValLen) == 0) return 1;

    return 0;
}

/**
 * 判断字符串是否以另一个字符串结尾
 * @param szVal 要查找的字符串
 * @param nValLen 字符串长度
 * @return 若字符串以另一个字符串结尾，则返回1，否则返回0
 */
unsigned char string_end_with(const char* szStr, int nStrLen, const char* szVal, int nValLen)
{
    if (NULL == szStr || NULL == szVal || nStrLen < nValLen) return 0;

    if (memcmp(szStr + (nStrLen - nValLen), szVal, nValLen) == 0) return 1;

    return 0;
}
