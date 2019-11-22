/*
 * @(#) JsonParser.h Created on 2008-03-27 Json解析器
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

/**
 * 子元素开始处理
 * @param szName 子元素名称
 * @param pstCallData 调用方数据
 */
typedef int (*FNJSONPARSESTARTELEMENTHANDLER) (const char* szName, void* pstCallData);

/**
 * 子元素结束处理
 * @param szName 子元素名称
 * @param szValue 子元素值
 * @param chJsonType 子元素类型
 * @param chItemType 子元素类型
 * @param pstCallData 调用方数据
 */
typedef int (*FNJSONPARSEENDELEMENTHANDLER) (const char* szName, const char* szValue, char chJsonType, char chItemType, void* pstCallData);

/**
 * 错误处理函数
 * @param iErrCode 错误码
 * @param szErrInfo 错误消息
 * @param szJson 错误发生的位置
 * @param pstCallData 调用方数据
 */
typedef void (*FNJSONPARSEERRORHANDLER) (int iErrCode, const char* szErrInfo, const char* szJson, void* pstCallData);

/**
 * json值类型
 */
enum tagJsonValueType
{
    kJsonValueTypeNumber = 1,
    kJsonValueTypeString,
    kJsonValueTypeArray,
    kJsonValueTypeObject
};

/**
 * json解析处理器
 */
struct tagJsonParseHandler
{
    FNJSONPARSESTARTELEMENTHANDLER onStartElement;      /* 子元素开始处理函数 */
    FNJSONPARSEENDELEMENTHANDLER onEndElement;          /* 子元素结束处理函数 */
    FNJSONPARSEERRORHANDLER onParseError;               /* 解析错误处理函数 */
};

typedef struct tagJsonParseHandler   JSONPARSEHANDLER;
typedef struct tagJsonParseHandler*  LPJSONPARSEHANDLER;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 解析json字符串
 * @param szJson 被解析的字符串
 * @param pstParseHandler 解析处理器
 * @param pstCallData 调用方数据
 * @return 成功返回0，失败返回错误码
 */
int parse_json_string(const char* szJson, LPJSONPARSEHANDLER pstParseHandler, void* pstCallData);

/**
 * 解析json文件
 * @param szFileName 被解析的json
 * @param pstParseHandler 解析处理器
 * @param pstCallData 调用方数据
 * @return 成功返回0，失败返回错误码
 */
int parse_json_file(const char* szFileName, LPJSONPARSEHANDLER pstParseHandler, void* pstCallData);

#ifdef __cplusplus
}
#endif

#endif

