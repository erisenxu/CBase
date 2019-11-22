/**
 * @(#) ByteArray.h 字节数组对象，一般用于消息编码解码
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
 * @history 2010-12-19 ErisenXu 创建文件
 */

#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include "BaseDef.h"

/**
 * 字节数组对象
 */
struct tagByteArray
{
    U32 dwSize;                     /* 数据总大小 */
    U32 dwLen;                      /* 数据总长度 */
    char* pszData;                  /* 数据信息 */
};

typedef struct tagByteArray  BYTEARRAY;
typedef struct tagByteArray* LPBYTEARRAY;

#define INIT_BYTE_ARRAY(ba, szData, dwDataSize) \
    do \
    { \
        ba.dwSize = dwDataSize;\
        ba.pszData = szData; \
        ba.dwLen = 0; \
    } while(0)

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 将字符添加到数组
 * @param pstByteArray 字节数组对象
 * @param nVal 要添加到数组的字符
 * @return 成功返回0，失败返回错误码
 */
int bytearray_append_u8(LPBYTEARRAY pstByteArray, U8 nVal);

/**
 * 将字符添加到数组
 * @param pstByteArray 字节数组对象
 * @param nVal 要添加到数组的字符
 * @return 成功返回0，失败返回错误码
 */
int bytearray_append_s8(LPBYTEARRAY pstByteArray, S8 nVal);

/**
 * 将整数添加到数组
 * @param nVal 要添加到数组的整数
 */
int bytearray_append_u16(LPBYTEARRAY pstByteArray, U16 nVal);

/**
 * 将整数添加到数组
 * @param iVal 要添加到数组的整数
 */
int bytearray_append_s16(LPBYTEARRAY pstByteArray, S16 iVal);

/**
 * 将整数添加到数组
 * @param nVal 要添加到数组的整数
 */
int bytearray_append_u32(LPBYTEARRAY pstByteArray, U32 nVal);

/**
 * 将整数添加到数组
 * @param iVal 要添加到数组的整数
 */
int bytearray_append_s32(LPBYTEARRAY pstByteArray, S32 iVal);

/**
 * 将整数添加到数组
 * @param ulVal 要添加到数组的整数
 */
int bytearray_append_u64(LPBYTEARRAY pstByteArray, U64 ulVal);

/**
 * 将整数添加到数组
 * @param lVal 要添加到数组的整数
 */
int bytearray_append_s64(LPBYTEARRAY pstByteArray, S64 lVal);

/**
 * 将字符串添加到数组
 * @param szVal 要添加到数组的字符串
 * @param nLen 字符串长度
 */
int bytearray_append_string(LPBYTEARRAY pstByteArray, const char* szVal, int nLen);

/**
 * 设置数组的值为字符串指定的值
 * @param szVal 要设置的字符串
 * @param nLen 字符串长度
 */
int bytearray_set_string(LPBYTEARRAY pstByteArray, const char* szVal, int nLen);

/**
 * 清空数组数据
 */
void bytearray_clear(LPBYTEARRAY pstByteArray);

/**
 * 返回字符串
 * @return 返回字符串
 */
const char* byte_array_to_string(LPBYTEARRAY pstByteArray);

/**
 * 去掉末尾的给定字符
 */
void bytearray_trim_tail(LPBYTEARRAY pstByteArray, char chTrim);

/**
 * 将字节数组转换成供打印的十六进制字符串
 *
 * @param pstBuf 用来保存数据的数组
 * @param szBytes 要打印的字节数组
 * @param nBufLen 字节数组szBytes的总长度
 * @param nOffset 要打印的数组的起始字节索引
 * @param nLength 要打印的数组中字节的总长度，若nLength=-1，则转换所有字节
 * @param nColumn 每行打印的列数
 */
void bytes_to_printable(LPBYTEARRAY pstBuf, const char* szBytes,
                        int nBufLen, int nOffset, int nLength, int nColumn);

/**
 * 将字节数组转成字符串，不可见字符以.表示
 * @param pstBuf 用来保存数据的数组
 * @param szBytes 要打印的字节数组
 * @param nBufLen 字节数组szBytes的总长度
 * @param nOffset 要打印的数组的起始字节索引
 * @param nLength 要打印的数组中字节的总长度，若nLength=-1，则转换所有字节
 * @param bAppendZero 是否在字符串末尾添加0
 */
int bytes_to_string(LPBYTEARRAY pstBuf, const char* szBytes, int nBufLen,
                    int nOffset, int nLength, U8 bAppendZero);

/**
 * 将字节数组转换成十六进制字符串
 * @param pstBuf 用来保存转换后的数据的数组
 * @param szBytes 被转换的字节数组
 * @param nBufLen 字节数组长度
 * @param nOffset 数组的起始字节索引
 * @param bAppendZero 是否在字符串末尾添加0
 */
int bytes_to_hex_string(LPBYTEARRAY pstBuf, const char* szBytes, int nBufLen,
                        int nOffset, U8 bAppendZero);

/**
 * 将十六进制字符串转换成字节数组
 * @param pstBuf 用来保存转换后的数据的数组
 * @param szHexStr 被转换的十六进制字符串数据
 */
int hex_string_to_bytes(LPBYTEARRAY pstBuf, const char* szHexStr);

#ifdef __cplusplus
}
#endif

#endif
