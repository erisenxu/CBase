/**
 * @(#) Hash.c Hash算法实现
 *
 * Copyright (c) 2014-2016 Erisen Xu (@itfriday)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author Erisen Xu
 * @version 1.0
 * @history 2017-04-13 ErisenXu 创建文件
 */

#include "Hash.h"

/**
 * 字串time33哈希函数，又叫 DJBX33A，Bernstein's hash
 */
U32 hash_code_time33(const char* szStr, S32 iLen)
{
    U32 dwHash = 5381;

    if (iLen > 0)
    {
        int i;
        for (i = 0; i < iLen; i++)
        {
            dwHash += (dwHash << 5) + (U32)szStr[i];
        }
    }
    else
    {
        const char* s_ptr = szStr;

        while (*s_ptr)
        {
            dwHash += (dwHash << 5) + (U32)(*s_ptr);
            s_ptr++;
        }
    }

    return dwHash;
}

/**
 * FNV1_32_HASH哈希函数，可用于一致性hash
 */
U32 hash_code_fnv1_32(const char* szStr, S32 iLen)
{
    int p = 16777619;
    int iHash = 2166136261l;

    if (iLen > 0)
    {
        int i;
        for (i = 0; i < iLen; i++)
        {
            iHash = (iHash ^ szStr[i]) * p;
        }
    }
    else
    {
        const char* s_ptr = szStr;

        while (*s_ptr)
        {
            iHash = (iHash ^ (*s_ptr)) * p;
            s_ptr++;
        }
    }

    iHash += iHash;
    iHash ^= iHash >> 7;
    iHash += iHash;
    iHash ^= iHash >> 17;
    iHash += iHash;

    return iHash >= 0 ? iHash : -iHash;
}
