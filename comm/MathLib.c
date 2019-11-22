/*
 * @(#) MathLib.c Created on 2016-12-10 数学相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "MathLib.h"

#include <stdlib.h>
#include <string.h>

/**
 * 生成整数iStart与iEnd之间的随机数
 * @param iStart 起始整数
 * @param iEnd 结束整数
 * @return 返回生成的随机数
 */
int rand_range(int iStart, int iEnd)
{
    if (iStart == iEnd) return iStart;

    return iStart > iEnd ? iEnd + random()%(iStart - iEnd) : iStart + random()%(iEnd - iStart);
}


/**
 * 生成一个随机的长整数
 * @return 返回生成的随机长整数
 */
unsigned long long rand_ulong()
{
    unsigned long long r1 = random();
    unsigned long long r2 = random();

    return r1 | (r2 << 32);
}

/**
 * 使用经典洗牌算法，随机排序数组
 * @param pArray 需要随机排序的数组
 * @param iSize 数组元素所占用内存大小
 * @param iArrLen 数组元素长度
 * @param pSwap 临时变量，所占内存大小>=iSize
 */
void rand_set_array(void* pArray, int iSize, int iArrLen, void* pSwap)
{
    int i;
    int iPos;

    for (i = iArrLen - 1; i >= 0; i--)
    {
        iPos = rand_range(0, i);

        if (iPos != i)
        {
            memcpy(pSwap, pArray + iSize * i, iSize);
            memcpy(pArray + iSize * i, pArray + iSize * iPos, iSize);
            memcpy(pArray + iSize * iPos, pSwap, iSize);
        }
    }
}

/**
 * 使用经典洗牌算法，随机排序整数数组
 * @param pArray 需要随机排序的数组
 * @param iArrLen 数组元素长度
 */
void rand_set_array_i(int* pArray, int iArrLen)
{
    int i;
    int iPos;
    int iTmp;

    for (i = iArrLen - 1; i >= 0; i--)
    {
        iPos = rand_range(0, i);

        if (iPos != i)
        {
            iTmp = pArray[i];
            pArray[i] = pArray[iPos];
            pArray[iPos] = iTmp;
        }
    }
}

/**
 * 使用经典洗牌算法，随机排序short整数数组
 * @param pArray 需要随机排序的数组
 * @param iArrLen 数组元素长度
 */
void rand_set_array_n(short* pArray, int iArrLen)
{
    int i;
    int iPos;
    short nTmp;

    for (i = iArrLen - 1; i >= 0; i--)
    {
        iPos = rand_range(0, i);

        if (iPos != i)
        {
            nTmp = pArray[i];
            pArray[i] = pArray[iPos];
            pArray[iPos] = nTmp;
        }
    }
}

/**
 * 使用经典洗牌算法，随机排序cjar整数数组
 * @param pArray 需要随机排序的数组
 * @param iArrLen 数组元素长度
 */
void rand_set_array_ub(unsigned char* pArray, int iArrLen)
{
    int i;
    int iPos;
    unsigned char nTmp;

    for (i = iArrLen - 1; i >= 0; i--)
    {
        iPos = rand_range(0, i);

        if (iPos != i)
        {
            nTmp = pArray[i];
            pArray[i] = pArray[iPos];
            pArray[iPos] = nTmp;
        }
    }
}

