/**
 * @(#) Base64.c Base64加密解密算法实现
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
 * @history 2017-04-23 ErisenXu 创建文件
 */

#include "Base64.h"

#include <string.h>

static char get_base64_char(int index)
{
    const char szBase64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (index >= 0 && index < 64)
        return szBase64Table[index];

    return '=';
}

// 从双字中取单字节
#define B0(a) (a & 0xFF)
#define B1(a) (a >> 8 & 0xFF)
#define B2(a) (a >> 16 & 0xFF)
#define B3(a) (a >> 24 & 0xFF)

/**
 * Base64 Encode
 *
 * @param base64code The output buffer used to store the encoded base64 string
 * @param src The input string
 * @param srcLen The length of the input string
 * @return Returns the length of the encoded base64 string
 */
int base64_encode(char* base64code, const char* src, int srcLen)
{
    int i;

    if (srcLen == 0) srcLen = (int)strlen(src);

    int len = 0;
    unsigned char* psrc = (unsigned char*)src;
    char * p64 = base64code;
    for (i = 0; i < srcLen - 3; i += 3)
    {
        unsigned long ulTmp = *(unsigned long*)psrc;
        register int b0 = get_base64_char((B0(ulTmp) >> 2) & 0x3F);
        register int b1 = get_base64_char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
        register int b2 = get_base64_char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F);
        register int b3 = get_base64_char((B2(ulTmp) << 2 >> 2) & 0x3F);
        *((unsigned long*)p64) = b0 | b1 << 8 | b2 << 16 | b3 << 24;
        len += 4;
        p64  += 4;
        psrc += 3;
    }

    // 处理最后余下的不足3字节的数据
    if (i < srcLen)
    {
        int rest = srcLen - i;
        unsigned long ulTmp = 0;
        int j;
        for (j = 0; j < rest; ++j)
        {
            *(((unsigned char*)&ulTmp) + j) = *psrc++;
        }

        p64[0] = get_base64_char((B0(ulTmp) >> 2) & 0x3F);
        p64[1] = get_base64_char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
        p64[2] = rest > 1 ? get_base64_char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F) : '=';
        p64[3] = rest > 2 ? get_base64_char((B2(ulTmp) << 2 >> 2) & 0x3F) : '=';
        p64 += 4;
        len += 4;
    }

    *p64 = '\0';

    return len;
}

static int get_base64_index(char ch)
{
    int index = -1;
    if (ch >= 'A' && ch <= 'Z')
    {
        index = ch - 'A';
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        index = ch - 'a' + 26;
    }
    else if (ch >= '0' && ch <= '9')
    {
        index = ch - '0' + 52;
    }
    else if (ch == '+')
    {
        index = 62;
    }
    else if (ch == '/')
    {
        index = 63;
    }

    return index;
}

/**
 * Base64 decode
 *
 * @param buf The output buffer used to store the decoded string
 * @param base64code The base64 string
 * @param srcLen The length of the base64 string
 * @return Returns the length of the decoded string
 */
int base64_decode(char* buf, const char* base64code, int srcLen)
{
    int i;

    if (srcLen == 0) srcLen = (int)strlen(base64code);

    int len = 0;
    unsigned char* psrc = (unsigned char*)base64code;
    char * pbuf = buf;
    for (i = 0; i < srcLen - 4; i += 4)
    {
        unsigned long ulTmp = *(unsigned long*)psrc;

        register int b0 = (get_base64_index((char)B0(ulTmp)) << 2 | get_base64_index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
        register int b1 = (get_base64_index((char)B1(ulTmp)) << 4 | get_base64_index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
        register int b2 = (get_base64_index((char)B2(ulTmp)) << 6 | get_base64_index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;

        *((unsigned long*)pbuf) = b0 | b1 << 8 | b2 << 16;
        psrc  += 4;
        pbuf += 3;
        len += 3;
    }

    // 处理最后余下的不足4字节的饿数据
    if (i < srcLen)
    {
        int rest = srcLen - i;
        unsigned long ulTmp = 0;
        int j;
        for (j = 0; j < rest; ++j)
        {
            *(((unsigned char*)&ulTmp) + j) = *psrc++;
        }

        register int b0 = (get_base64_index((char)B0(ulTmp)) << 2 | get_base64_index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
        *pbuf++ = b0;
        len++;

        if ('=' != B1(ulTmp) && '=' != B2(ulTmp))
        {
            register int b1 = (get_base64_index((char)B1(ulTmp)) << 4 | get_base64_index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
            *pbuf++ = b1;
            len++;
        }

        if ('=' != B2(ulTmp) && '=' != B3(ulTmp))
        {
            register int b2 = (get_base64_index((char)B2(ulTmp)) << 6 | get_base64_index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;
            *pbuf++ = b2;
            len++;
        }
    }

    *pbuf = '\0';

    return len;
}

