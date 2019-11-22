/*
 * @(#) HttpHeader.c Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "HttpHeader.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * 字段编码
 * @param pstField 要编码的字段
 * @param pstByteArray 保存编码字节流的缓存区数组
 * @return 成功返回0，失败返回错误码
 */
int http_header_encode(LPHTTPHEADER pstField, LPBYTEARRAY pstByteArray)
{
    int iRet;
    char szHdr[MAX_HTTP_HDR_LEN];

    if (NULL == pstField || NULL == pstByteArray) return ERROR_INPUT_PARAM_NULL;

    SNPRINTF(szHdr, sizeof(szHdr), "%s:%s\r\n", pstField->szName, pstField->szValue);

    CHECK_FUNC_RET(bytearray_append_string(pstByteArray, szHdr, (int)strlen(szHdr)), iRet);

    return 0;
}

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @param piReadLen 从字节流中解码字段读取的长度
 * @return 成功返回0，失败返回错误码
 */
int http_header_decode(LPHTTPHEADER pstField, const char* szBuf, int iBufLen, int* piReadLen)
{
    // The format of the HTTP header is:
    // Message-header = Field-name ":" [Field-value]
    int i = 0;
    U8 bSeenEnd = 0;
    U8 bState = 0;
    char ch;
    const char* s_ptr;
    char* s_value;
    int iValLen = 0;

    if (NULL == pstField || NULL == szBuf || iBufLen <= 0) return ERROR_INPUT_PARAM_NULL;

    s_ptr = szBuf;
    s_value = pstField->szName;

    for (i = 0; (i < iBufLen && !bSeenEnd); i++)
    {
        ch = *s_ptr++;

        switch (bState)
        {
        case 0:
            if (ch == ':')
            {
                bState = 1;
                *s_value = 0;
                s_value = pstField->szValue;
                iValLen = 0;
                break;
            }
            if (!isspace(ch) || iValLen > 0)
            {
                if (iValLen >= MAX_HTTP_HDR_NAME_LEN) return ERROR_DECODE_FIELD_LONG;
                *s_value++ = ch;
                iValLen++;
            }
            break;
        case 1:
            if (ch == '\r')
            {
                bState = 2;
                *s_value = 0;
                break;
            }
            if (!isspace(ch) || iValLen > 0)
            {
                if (iValLen >= MAX_HTTP_HDR_VALUE_LEN) return ERROR_DECODE_FIELD_LONG;
                *s_value++ = ch;
                iValLen++;
            }
            break;
        case 2:
            if (ch != '\n') return ERROR_DECODE_INVALID_MSG;
            bSeenEnd = 1;
            break;
        }
    }

    if (!bSeenEnd) return ERROR_DECODE_INVALID_MSG;

    if (piReadLen) *piReadLen = i;

    return 0;
}

