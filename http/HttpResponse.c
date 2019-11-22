/*
 * @(#) HttpResponse.c Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "HttpResponse.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"

/**
 * 字段编码
 * @param pstField 要编码的字段
 * @param pstByteArray 保存编码字节流的缓存区数组
 * @return 成功返回0，失败返回错误码
 */
int http_response_encode(LPHTTPRESPONSE pstField, LPBYTEARRAY pstByteArray)
{
    int iRet;

    if (NULL == pstField || NULL == pstByteArray) return ERROR_INPUT_PARAM_NULL;

    // Status line
    CHECK_FUNC_RET(http_status_line_encode(&pstField->stStatusLine, pstByteArray), iRet);

    // header list
    CHECK_FUNC_RET(http_header_list_encode(&pstField->stHeaderList, pstByteArray), iRet);

    // body
    CHECK_FUNC_RET(http_body_encode(&pstField->stBody, pstByteArray), iRet);

    return 0;
}

/**
 * 字段解码
 * @param pstField 要解码的字段
 * @param szBuf 解码读取的字节流
 * @param iBufLen 解码读取的字节流长度
 * @return 成功返回0，失败返回错误码
 */
int http_response_decode(LPHTTPRESPONSE pstField, const char* szBuf, int iBufLen)
{
    int iRet;
    int iReadLen;
    const char* s_ptr = szBuf;
    int iLeftLen = iBufLen;

    if (NULL == pstField || NULL == szBuf || iBufLen <= 0) return ERROR_INPUT_PARAM_NULL;

    // Status line
    CHECK_FUNC_RET(http_status_line_decode(&pstField->stStatusLine, s_ptr, iLeftLen, &iReadLen), iRet);

    s_ptr += iReadLen;
    iLeftLen -= iReadLen;

    // Header list
    CHECK_FUNC_RET(http_header_list_decode(&pstField->stHeaderList, s_ptr, iLeftLen, &iReadLen), iRet);

    s_ptr += iReadLen;
    iLeftLen -= iReadLen;

    // Body
    CHECK_FUNC_RET(http_body_decode(&pstField->stBody, s_ptr, iLeftLen, &iReadLen), iRet);

    return 0;
}

