/*
 * @(#) HttpBody.c Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "HttpBody.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"

#include <stdio.h>
#include <string.h>

/**
 * 字段编码
 * @param pstField 要编码的字段
 * @param pstByteArray 保存编码字节流的缓存区数组
 * @return 成功返回0，失败返回错误码
 */
int http_body_encode(LPHTTPBODY pstField, LPBYTEARRAY pstByteArray)
{
    int iRet;

    if (NULL == pstField || NULL == pstByteArray) return ERROR_INPUT_PARAM_NULL;

    // body
    CHECK_FUNC_RET(bytearray_append_string(pstByteArray, "\r\n", (int)strlen("\r\n")), iRet);

    CHECK_FUNC_RET(bytearray_append_string(pstByteArray, pstField->szBody, pstField->nBodyLen), iRet);

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
int http_body_decode(LPHTTPBODY pstField, const char* szBuf, int iBufLen, int* piReadLen)
{
    if (NULL == pstField) return ERROR_INPUT_PARAM_NULL;

    if (NULL == szBuf || iBufLen <= 0)
    {
        pstField->nBodyLen = 0;
        return 0;
    }

    pstField->nBodyLen = iBufLen >= MAX_HTTP_BODY_LEN ? MAX_HTTP_BODY_LEN - 1 : iBufLen;

    memcpy(pstField->szBody, szBuf, pstField->nBodyLen);
    pstField->szBody[pstField->nBodyLen] = 0;

    return 0;
}
