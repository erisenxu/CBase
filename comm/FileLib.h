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

/**
 * 获取文件大小
 * @param szFileName 文件名
 * @return 成功返回文件大小(单位，字节数byte)，失败返回-1
 */
long long get_file_size(const char *szFileName);

/**
 * 判断给定文件名是否是有效文件
 * @param szFileName 文件名
 * @return 若文件名有效且存在，返回1，不是有效文件名返回0
 */
int is_file(const char *szFileName);

/**
 * 判断给定文件名是否是有效文件路径
 * @param szFileName 文件名
 * @return 若文件路径有效且存在，返回1，不是有效文件路径返回0
 */
int is_dir(const char *szFileName);

/**
 * 取文件扩展名
 * @param szFileName 文件名称
 * @return 返回文件名的扩展名，若无扩展名，返回NULL
 */
const char* get_file_extname(const char* szFileName);

#ifdef __cplusplus
}
#endif

#endif

