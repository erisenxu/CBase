/*
 * @(#) MathLib.h Created on 2016-12-10 数学相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 生成整数iStart与iEnd之间的随机数
 * @param iStart 起始整数
 * @param iEnd 结束整数
 * @return 返回生成的随机数
 */
int rand_range(int iStart, int iEnd);

/**
 * 生成一个随机的长整数
 * @return 返回生成的随机长整数
 */
unsigned long long rand_ulong();

/**
 * 使用经典洗牌算法，随机排序数组
 * @param pArray 需要随机排序的数组
 * @param iSize 数组元素所占用内存大小
 * @param iArrLen 数组元素长度
 * @param pSwap 临时变量，所占内存大小>=iSize
 */
void rand_set_array(void* pArray, int iSize, int iArrLen, void* pSwap);

/**
 * 使用经典洗牌算法，随机排序整数数组
 * @param pArray 需要随机排序的数组
 * @param iArrLen 数组元素长度
 */
void rand_set_array_i(int* pArray, int iArrLen);

/**
 * 使用经典洗牌算法，随机排序short整数数组
 * @param pArray 需要随机排序的数组
 * @param iArrLen 数组元素长度
 */
void rand_set_array_n(short* pArray, int iArrLen);

/**
 * 使用经典洗牌算法，随机排序cjar整数数组
 * @param pArray 需要随机排序的数组
 * @param iArrLen 数组元素长度
 */
void rand_set_array_ub(unsigned char* pArray, int iArrLen);

#ifdef __cplusplus
}
#endif

#endif

