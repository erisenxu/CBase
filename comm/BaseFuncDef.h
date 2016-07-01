/*
 * @(#) BaseFuncDef.h Created on 2014-03-27
 *
 * Copyright (c) 2014-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef BASE_FUNC_DEF_H
#define BASE_FUNC_DEF_H

/**
 * 从缓冲区中读取4个字节数据到指定变量中
 * @param nValue 存放读取的数据的变量，类型：U64
 * @param pszBuf 源缓冲区指针变量，类型：char*
 */
#define M_CHAR_TO_U64(nValue, pszBuf) \
    nValue = ((U64)(*pszBuf++ & 0xFF) << 56); \
    nValue |= ((U64)(*pszBuf++ & 0xFF) << 48); \
    nValue |= ((U64)(*pszBuf++ & 0xFF) << 40); \
    nValue |= ((U64)(*pszBuf++ & 0xFF) << 32); \
    nValue |= ((U64)(*pszBuf++ & 0xFF) << 24); \
    nValue |= ((U64)(*pszBuf++ & 0xFF) << 16); \
    nValue |= ((U64)(*pszBuf++ & 0xFF) << 8); \
    nValue |= (*pszBuf++ & 0xFF)

/**
 * 从缓冲区中读取4个字节数据到指定变量中
 * @param nValue 存放读取的数据的变量，类型：U32
 * @param pszBuf 源缓冲区指针变量，类型：char*
 */
#define M_CHAR_TO_U32(nValue, pszBuf) \
    nValue = ((*pszBuf++ & 0xFF) << 24); \
    nValue |= ((*pszBuf++ & 0xFF) << 16); \
    nValue |= ((*pszBuf++ & 0xFF) << 8); \
    nValue |= (*pszBuf++ & 0xFF)

/**
 * 从缓冲区中读取2个字节数据到指定变量中
 * @param nValue 存放读取的数据的变量，类型：U16
 * @param pszBuf 源缓冲区指针变量，类型：char*
 */
#define M_CHAR_TO_U16(nValue, pszBuf) \
    nValue = ((*pszBuf++ & 0xFF) << 8); \
    nValue |= (*pszBuf++ & 0xFF)

/**
 * 从缓冲区中读取1个字节数据到指定变量中
 * @param bValue 存放读取的数据的变量，类型：U8
 * @param pszBuf 源缓冲区指针变量，类型：char*
 */
#define M_CHAR_TO_U8(bValue, pszBuf) \
    bValue = (U8)(*pszBuf++)

/**
 * 写1个字节数据到缓冲区中
 * @param pszBuf 目标缓冲区指针变量，类型：char*
 * @param bValue 存放读取的数据的变量，类型：U8
 */
#define M_U8_TO_CHAR(pszBuf, bValue) \
    *pszBuf++ = (U8)(bValue)

/**
 * 写2个字节数据到缓冲区中
 * @param pszBuf 目标缓冲区指针变量，类型：char*
 * @param nValue 存放读取的数据的变量，类型：U16
 */
#define M_U16_TO_CHAR(pszBuf, nValue) \
    *pszBuf++ = (U8)(((nValue) >> 8) & 0xFF); \
    *pszBuf++ = (U8)((nValue) & 0xFF)

/**
 * 写4个字节数据到缓冲区中
 * @param pszBuf 目标缓冲区指针变量，类型：char*
 * @param dwValue 存放读取的数据的变量，类型：U32
 */
#define M_U32_TO_CHAR(pszBuf, dwValue) \
    *pszBuf++ = (U8)(((dwValue) >> 24) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 16) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 8) & 0xFF); \
    *pszBuf++ = (U8)((dwValue) & 0xFF)

/**
 * 写4个字节数据到缓冲区中
 * @param pszBuf 目标缓冲区指针变量，类型：char*
 * @param dwValue 存放读取的数据的变量，类型：U64
 */
#define M_U64_TO_CHAR(pszBuf, dwValue) \
    *pszBuf++ = (U8)(((dwValue) >> 56) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 48) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 40) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 32) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 24) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 16) & 0xFF); \
    *pszBuf++ = (U8)(((dwValue) >> 8) & 0xFF); \
    *pszBuf++ = (U8)((dwValue) & 0xFF)

#define CHECK_FUNC_RET(func, iRet) \
    do \
    { \
        iRet = func; \
        if (iRet != 0) return iRet; \
    } while(0)

#define CHECK_FUNC_RET2(func, iRet) \
    do \
    { \
        iRet = func; \
        if (iRet != 0) return; \
    } while(0)

#ifndef WIN32
#define STRNCPY(szDest, nDestSize, szSrc, nMaxCount)  strncpy(szDest, szSrc, nDestSize)
#else
#define STRNCPY(szDest, nDestSize, szSrc, nMaxCount)  strncpy_s(szDest, nDestSize, szSrc, nMaxCount)
#endif

#define UNUSED(val)

#define CLOSE_SOCKET(s) \
    if (s != INADDR_NONE) close(s)

#endif
