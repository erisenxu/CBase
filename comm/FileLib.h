/*
 * @(#) FileLib.h Created on 2008-03-27 文件相关API封装
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef FILE_LIB_H
#define FILE_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 循环嵌套创建路径
 * @param szPath 路径名
 * @param iMode 路径权限
 * @return 成功返回0，失败返回错误码
 */
int create_dir(const char* szPath, int iMode);

/**
 * 获取文件的key值
 * @param szFileName 文件名
 * @return 成功返回文件的key值，失败返回-1
 */
int file_to_key(const char* szFileName);

#ifdef __cplusplus
}
#endif

#endif

