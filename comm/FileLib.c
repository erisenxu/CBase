/*
 * @(#) FileLib.c Created on 2008-03-27 文件相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "FileLib.h"
#include "BaseDef.h"
#include "ErrorCode.h"
#include "StrLib.h"

#include <string.h>
#include <sys/stat.h>

/**
 * 循环嵌套创建路径
 * @param szPath 路径名
 * @param iMode 路径权限
 * @return 成功返回0，失败返回错误码
 */
int create_dir(const char* szPath, int iMode)
{
    char szCurPath[MAX_PATH];
    size_t llLen;
    const char* szBuf;
    char* s_ptr;
    struct stat oBuf;
    size_t i;
    int iRet;

    if (NULL == szPath || 0 == *szPath) return ERROR_INPUT_PARAM_NULL;

    /* 路径长度校验 */
    llLen = strlen(szPath);

    if (llLen >= MAX_PATH) return ERROR_PATH_NAME_TOO_LONG;

    memset(szCurPath, 0, sizeof(szCurPath));
    szBuf = szPath;
    s_ptr = szCurPath;

    for (i = 0; i < llLen; i++)
    {
        if (*szCurPath && (*szBuf == '/' || *szBuf == '\\'))
        {
            iRet = stat(szCurPath, &oBuf);
            if (iRet || (oBuf.st_mode & S_IFMT) != S_IFDIR)
            {
                iRet = mkdir(szCurPath, iMode);
                if (iRet != 0) return ERROR_PATH_CREATE_FAILURE;
            }
        }
        *s_ptr++ = *szBuf++;
    }
    return 0;
}

/**
 * 获取文件的key值
 * @param szFileName 文件名
 * @return 成功返回文件的key值，失败返回-1
 */
int file_to_key(const char* szFileName)
{
#if 0
    return ftok(szFileName, 0);
#else
    struct stat stBuf;
    int iRet = stat(szFileName, &stBuf);
    return 0 == iRet ? stBuf.st_ino : -1;
#endif
}

/**
 * 获取文件大小
 * @param szFileName 文件名
 * @return 成功返回文件大小(单位，字节数byte)，失败返回-1
 */
long long get_file_size(const char *szFileName)
{
    struct stat stBuf;
    int iRet = stat(szFileName, &stBuf);
    return 0 == iRet ? stBuf.st_size : -1;
}

/**
 * 判断给定文件名是否是有效文件
 * @param szFileName 文件名
 * @return 若文件名有效且存在，返回1，不是有效文件名返回0
 */
int is_file(const char *szFileName)
{
    struct stat stBuf;
    int iRet = stat(szFileName, &stBuf);
    return 0 == iRet && S_ISREG(stBuf.st_mode);
}

/**
 * 判断给定文件名是否是有效文件路径
 * @param szFileName 文件名
 * @return 若文件路径有效且存在，返回1，不是有效文件路径返回0
 */
int is_dir(const char *szFileName)
{
    struct stat stBuf;
    int iRet = stat(szFileName, &stBuf);
    return 0 == iRet && S_ISDIR(stBuf.st_mode);
}

/**
 * 取文件扩展名
 * @param szFileName 文件名称
 * @return 返回文件名的扩展名，若无扩展名，返回NULL
 */
const char* get_file_extname(const char* szFileName)
{
    int iLastDotPos = -1;
    int iLastPathPos = -1;
    int iFileNameLen = 0;
    
    if (NULL == szFileName) return NULL;
    
    iFileNameLen = (int)strlen(szFileName);
    
    iLastDotPos = last_index_of_string(szFileName, iFileNameLen, ".", 1);
    if (iLastDotPos < 0 || iFileNameLen - 1 == iLastDotPos) return NULL;
    
    iLastPathPos = last_index_of_string(szFileName, iFileNameLen, "/", 1);
    if (iLastPathPos > 0 && iLastDotPos < iLastPathPos) return NULL;
    
    iLastPathPos = last_index_of_string(szFileName, iFileNameLen, "\\", 1);
    if (iLastPathPos > 0 && iLastDotPos < iLastPathPos) return NULL;
    
    return szFileName + iLastDotPos;
}
