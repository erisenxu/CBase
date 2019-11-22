/**
 * @(#) ByteArray.c 字节数组对象，一般用于消息编码解码
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

#include "ByteArray.h"
#include "comm/BaseFuncDef.h"
#include "comm/ErrorCode.h"

#include <string.h>

/**
 * 向数组中扩展时检查输入参数是否合法
 */
#define BYTE_ARRAY_APPEND_CHECK_PARAM_RET(pstByteArray, nVal) \
    do \
    { \
        if (NULL == pstByteArray || NULL == pstByteArray->pszData) return ERROR_INPUT_PARAM_NULL; \
        if (pstByteArray->dwLen + sizeof(nVal) > pstByteArray->dwSize) return ERROR_APPEND_BUFSIZE_SHORT; \
    } while(0)

/**
 * 将十六进制字符转换成整数
 * @param chHex 被转换的十六进制字符
 */
static U8 hex_char_to_int(char chHex);

/**
 * 将字符添加到数组
 * @param pstByteArray 字节数组对象
 * @param nVal 要添加到数组的字符
 * @return 成功返回0，失败返回错误码
 */
int bytearray_append_u8(LPBYTEARRAY pstByteArray, U8 nVal)
{
    char* s_ptr;

    BYTE_ARRAY_APPEND_CHECK_PARAM_RET(pstByteArray, nVal);

    s_ptr = pstByteArray->pszData + pstByteArray->dwLen;

    M_U8_TO_CHAR(s_ptr, nVal);

    pstByteArray->dwLen += sizeof(nVal);

    return 0;
}

/**
 * 将字符添加到数组
 * @param nVal 要添加到数组的字符
 */
int bytearray_append_s8(LPBYTEARRAY pstByteArray, S8 nVal)
{
    return bytearray_append_u8(pstByteArray, (U8)nVal);
}

/**
 * 将整数添加到数组
 * @param nVal 要添加到数组的整数
 */
int bytearray_append_u16(LPBYTEARRAY pstByteArray, U16 nVal)
{
    char* s_ptr;

    BYTE_ARRAY_APPEND_CHECK_PARAM_RET(pstByteArray, nVal);

    s_ptr = pstByteArray->pszData + pstByteArray->dwLen;

    M_U16_TO_CHAR(s_ptr, nVal);

    pstByteArray->dwLen += sizeof(nVal);

    return 0;
}

/**
 * 将整数添加到数组
 * @param nVal 要添加到数组的整数
 */
int bytearray_append_s16(LPBYTEARRAY pstByteArray, S16 nVal)
{
    return bytearray_append_u16(pstByteArray, (U16)nVal);
}

/**
 * 将整数添加到数组
 * @param nVal 要添加到数组的整数
 */
int bytearray_append_u32(LPBYTEARRAY pstByteArray, U32 nVal)
{
    char* s_ptr;

    BYTE_ARRAY_APPEND_CHECK_PARAM_RET(pstByteArray, nVal);

    s_ptr = pstByteArray->pszData + pstByteArray->dwLen;

    M_U32_TO_CHAR(s_ptr, nVal);

    pstByteArray->dwLen += sizeof(nVal);

    return 0;
}

/**
 * 将整数添加到数组
 * @param nVal 要添加到数组的整数
 */
int bytearray_append_s32(LPBYTEARRAY pstByteArray, S32 nVal)
{
    return bytearray_append_u32(pstByteArray, (U32)nVal);
}

/**
 * 将整数添加到数组
 * @param ulVal 要添加到数组的整数
 */
int bytearray_append_u64(LPBYTEARRAY pstByteArray, U64 ulVal)
{
    char* s_ptr;

    BYTE_ARRAY_APPEND_CHECK_PARAM_RET(pstByteArray, ulVal);

    s_ptr = pstByteArray->pszData + pstByteArray->dwLen;

    M_U64_TO_CHAR(s_ptr, ulVal);
    
    pstByteArray->dwLen += sizeof(ulVal);
    
    return 0;
}

/**
 * 将整数添加到数组
 * @param ulVal 要添加到数组的整数
 */
int bytearray_append_s64(LPBYTEARRAY pstByteArray, S64 ulVal)
{
    return bytearray_append_u64(pstByteArray, (U64)ulVal);
}

/**
 * 将字符串添加到数组
 * @param szVal 要添加到数组的字符串
 * @param nLen 字符串长度
 */
int bytearray_append_string(LPBYTEARRAY pstByteArray, const char* szVal, int nLen)
{
    char* s_ptr;

    /* 如果字符串为空，直接返回 */
    if (nLen <= 0)
    {
        return 0;
    }

    if (NULL == pstByteArray  || NULL == pstByteArray->pszData) return ERROR_INPUT_PARAM_NULL;
    if (pstByteArray->dwLen + nLen > pstByteArray->dwSize) return ERROR_APPEND_BUFSIZE_SHORT;

    s_ptr = pstByteArray->pszData + pstByteArray->dwLen;

    memcpy(s_ptr, szVal, nLen);

    pstByteArray->dwLen += nLen;

    return 0;
}

/**
 * 设置数组的值为字符串指定的值
 * @param szVal 要设置的字符串
 * @param nLen 字符串长度
 */
int bytearray_set_string(LPBYTEARRAY pstByteArray, const char* szVal, int nLen)
{
    bytearray_clear(pstByteArray);

    return bytearray_append_string(pstByteArray, szVal, nLen);
}

/**
 * 清空数组数据
 */
void bytearray_clear(LPBYTEARRAY pstByteArray)
{
    if (NULL == pstByteArray) return;

    pstByteArray->dwLen = 0;
}

/**
 * 返回字符串
 * @return 返回字符串
 */
const char* byte_array_to_string(LPBYTEARRAY pstByteArray)
{
    if (NULL == pstByteArray || NULL == pstByteArray->pszData ||  pstByteArray->dwLen >= pstByteArray->dwSize)
    {
        return NULL;
    }

    pstByteArray->pszData[pstByteArray->dwLen] = 0;

    return pstByteArray->pszData;
}

/**
 * 去掉末尾的给定字符
 */
void bytearray_trim_tail(LPBYTEARRAY pstByteArray, char chTrim)
{
    if (NULL == pstByteArray || NULL == pstByteArray->pszData || pstByteArray->dwLen <= 0) return;

    if (pstByteArray->pszData[pstByteArray->dwLen - 1] == chTrim) pstByteArray->dwLen--;
}

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
                        int nBufLen, int nOffset, int nLength, int nColumn)
{
    const char* szHexDigits = "0123456789abcdef";

    char szText[1024];
    BYTEARRAY stText;
    int nEnd = nLength < 0 ? nOffset + nBufLen : nOffset + nLength;
    int nCols = 0;
    int nHalf;
    int i;
    int j;

    INIT_BYTE_ARRAY(stText, szText, sizeof(szText));

    if (nEnd > nBufLen)
    {
        nEnd = nBufLen;
    }

    if (nColumn <= 0)
    {
        nColumn = 8;
    }

    if (nColumn%2 != 0)
    {
        nColumn++;
    }

    nHalf = nColumn/2;

    bytearray_append_string(pstBuf, "0000   ", strlen("0000   "));

    for (i = nOffset, j = 0; i < nEnd; i++, j++)
    {
        bytearray_append_u8(pstBuf, (U8)(szHexDigits[(szBytes[i] >> 4) & 0xF]));
        bytearray_append_u8(pstBuf, (U8)(szHexDigits[szBytes[i] & 0xF]));
        bytearray_append_string(pstBuf, " ", strlen(" "));

        if (szBytes[i] >= 32 && szBytes[i] < 127)
        {
            bytearray_append_u8(&stText, (U8)szBytes[i]);
        }
        else
        {
            bytearray_append_u8(&stText, (U8)'.');
        }

        if ((j + 1)%nColumn == 0)
        {
            bytearray_append_string(pstBuf, "  ", strlen("  "));
            bytearray_append_string(pstBuf, stText.pszData, stText.dwLen);
            bytearray_append_u8(pstBuf, (U8)'\n');

            nCols += nColumn;
            bytearray_append_u8(pstBuf, (U8)(szHexDigits[(nCols >> 12) & 0xF]));
            bytearray_append_u8(pstBuf, (U8)(szHexDigits[(nCols >> 8) & 0xF]));
            bytearray_append_u8(pstBuf, (U8)(szHexDigits[(nCols >> 4) & 0xF]));
            bytearray_append_u8(pstBuf, (U8)(szHexDigits[nCols & 0xF]));
            bytearray_append_string(pstBuf, "   ", strlen("   "));
            // 清空saText
            stText.dwLen = 0;
            continue;
        }

        if ((j + 1)%nHalf == 0)
        {
            bytearray_append_u8(pstBuf, (U8)' ');
            bytearray_append_u8(&stText, (U8)' ');
        }
    }

    // 检查saText中是否还有数据?
    if (stText.dwLen > 0)
    {
        int nNum = j%nColumn;
        int nSpaces;

        if (nNum < nHalf)
        {
            nSpaces = (nColumn - nNum)*3 + 3;
        }
        else
        {
            nSpaces = (nColumn - nNum)*3 + 2;
        }
        for (i = 0; i < nSpaces; i++)
        {
            bytearray_append_u8(pstBuf, (U8)' ');
        }
        bytearray_append_string(pstBuf, stText.pszData, stText.dwLen);
    }

    // 最后将字节数组变成字符串
    bytearray_append_u8(pstBuf, (U8)0);
}

/**
 * 将字节数组转成字符串，不可见字符以.表示
 * @param pstBuf 用来保存数据的数组
 * @param szBytes 要打印的字节数组
 * @param nBufLen 字节数组szBytes的总长度
 * @param nOffset 要打印的数组的起始字节索引
 * @param nLength 要打印的数组中字节的总长度，若nLength=-1，则转换所有字节
 * @param bAppendZero 是否在字符串末尾添加0
 */
int bytes_to_string(LPBYTEARRAY pstBuf, const char* szBytes, int nBufLen, int nOffset, int nLength, U8 bAppendZero)
{
    int i;
    int iRet;
    int nEnd = nLength < 0 ? nOffset + nBufLen : nOffset + nLength;

    if (nEnd > nBufLen) nEnd = nBufLen;

    for (i = nOffset; i < nEnd; i++)
    {
        if (szBytes[i] >= 32 && szBytes[i] < 127)
        {
            CHECK_FUNC_RET(bytearray_append_u8(pstBuf, (U8)szBytes[i]), iRet);
        }
        else
        {
            CHECK_FUNC_RET(bytearray_append_u8(pstBuf, (U8)'.'), iRet);
        }
    }
    // 最后将字节数组变成字符串
    return bAppendZero ? bytearray_append_u8(pstBuf, (U8)0) : 0;
}

/**
 * 将字节数组转换成十六进制字符串
 * @param pstBuf 用来保存转换后的数据的数组
 * @param szBytes 被转换的字节数组
 * @param nBufLen 字节数组长度
 * @param nOffset 数组的起始字节索引
 * @param bAppendZero 是否在字符串末尾添加0
 */
int bytes_to_hex_string(LPBYTEARRAY pstBuf, const char* szBytes, int nBufLen, int nOffset, U8 bAppendZero)
{
    int iRet = 0;
    int i;

    if (!szBytes || nBufLen <= 0) return iRet;

    const char* szHexDigits = "0123456789abcdef";

    for (i = nOffset; i < nBufLen; i++)
    {
        CHECK_FUNC_RET(bytearray_append_u8(pstBuf, (U8)szHexDigits[(szBytes[i] >> 4) & 0xF]), iRet);
        CHECK_FUNC_RET(bytearray_append_u8(pstBuf, (U8)szHexDigits[szBytes[i] & 0xF]), iRet);
    }

    // 最后将字节数组变成字符串
    return bAppendZero ? bytearray_append_u8(pstBuf, (U8)0) : 0;
}

/**
 * 将十六进制字符串转换成字节数组
 * @param pstBuf 用来保存转换后的数据的数组
 * @param szHexStr 被转换的十六进制字符串数据
 */
int hex_string_to_bytes(LPBYTEARRAY pstBuf, const char* szHexStr)
{
    int iRet = 0;

    if (!szHexStr || !(*szHexStr)) return iRet;

    size_t nLen = strlen(szHexStr);
    char chHi; // 高位
    char chLo; // 低位
    const char* szBuf = szHexStr;

    if (nLen % 2 != 0)
    {
        CHECK_FUNC_RET(bytearray_append_u8(pstBuf, (U8)hex_char_to_int(*szBuf++)), iRet);
    }

    while (*szBuf)
    {
        chHi = *szBuf++;
        chLo = *szBuf++;
        CHECK_FUNC_RET(bytearray_append_u8(pstBuf, (U8)((hex_char_to_int(chHi) << 4) | hex_char_to_int(chLo))), iRet);
    }

    return 0;
}

/**
 * 将十六进制字符转换成整数
 * @param chHex 被转换的十六进制字符
 */
U8 hex_char_to_int(char chHex)
{
    switch (chHex)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return chHex - '0';
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return chHex - 'a' + 10;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return chHex - 'A' + 10;
    default:
        return 0;
    }
}
