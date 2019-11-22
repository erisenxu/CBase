/*
 * @(#) HttpHeaderList.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "HttpHeaderList.h"

#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"

#include <string.h>

/**
 * 字段编码
 * @param pstField 要编码的字段
 * @param pstByteArray 保存编码字节流的缓存区数组
 * @return 成功返回0，失败返回错误码
 */
int http_header_list_encode(LPHTTPHEADERLIST pstField, LPBYTEARRAY pstByteArray)
{
    int iRet;
    U16 i;

    if (NULL == pstField || NULL == pstByteArray) return ERROR_INPUT_PARAM_NULL;

    // header
    for (i = 0; i < pstField->nHeaderNumber; i++)
    {
        CHECK_FUNC_RET(http_header_encode(&pstField->astHeader[i], pstByteArray), iRet);
    }

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
int http_header_list_decode(LPHTTPHEADERLIST pstField, const char* szBuf, int iBufLen, int* piReadLen)
{
    char ch1;
    char ch2;
    const char* byMsg = szBuf;
    int iReadLen = 0;
    int iLeftLen = iBufLen;
    int iRet;

    if (NULL == pstField || NULL == szBuf || iBufLen <= 0) return ERROR_INPUT_PARAM_NULL;

    pstField->nHeaderNumber = 0;

    while (iLeftLen > 0)
    {
        ch1 = *byMsg;

        if (iLeftLen < 2)
        {
            break;
        }

        ch2 = *(byMsg + 1);

        if (ch1 == '\r' && ch2 == '\n')
        {
            iLeftLen -= 2;
            break;
        }

        CHECK_FUNC_RET(http_header_decode(&pstField->astHeader[pstField->nHeaderNumber], byMsg, iLeftLen, &iReadLen), iRet);

        byMsg += iReadLen;
        iLeftLen -= iReadLen;

        pstField->nHeaderNumber++;
    }

    if (piReadLen) *piReadLen = iBufLen - iLeftLen;

    return 0;
}

/**
 * 通过消息头名称，查找头字段
 * @param pstField 头列表字段
 * @param szName 头名称
 * @return 若在列表中找到头字段，则返回头字段对象，未找到，返回NULL
 */
LPHTTPHEADER find_http_header_by_name(LPHTTPHEADERLIST pstField, const char* szName)
{
    U16 i;

    if (NULL == pstField || NULL == szName) return NULL;

    for (i = 0; i < pstField->nHeaderNumber; i++)
    {
        if (!strncmp(pstField->astHeader[i].szName, szName, sizeof(pstField->astHeader[i].szName)))
        {
            return &pstField->astHeader[i];
        }
    }

    return NULL;
}

