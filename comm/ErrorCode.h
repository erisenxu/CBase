/*
 * @(#) ErrorCode.h Created on 2014-03-27
 *
 * Copyright (c) 2014-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef ERROR_CODE_H
#define ERROR_CODE_H

/* 普通错误码-1~-1000 */
#define ERROR_INPUT_PARAM_NULL                      -1      /* 输入参数不合法，为NULL */
#define ERROR_ARRAY_LEN_NOT_ENOUGH                  -2      /* 数组长度不够 */
#define ERROR_GET_CURRENT_PATH                      -3      /* 取当前目录时，发生错误 */
#define ERROR_FILE_OPEN_FAILURE                     -4      /* 打开文件时，发生错误 */
#define ERROR_FILE_TOKEN_FAILURE                    -5      /* 获取文件的token信息失败(调用ftok失败) */
#define ERROR_FILE_RENAME_FAILURE                   -6      /* 文件更名失败 */
#define ERROR_FILE_STAT_FAILURE                     -7      /* 获取文件信息失败 */
#define ERROR_PATH_NAME_TOO_LONG                    -8      /* 路径名太长 */
#define ERROR_PATH_CREATE_FAILURE                   -9      /* 创建路径失败 */
#define ERROR_MEM_LEN_NOT_ENOUGH                    -10     /* 内存长度不够 */

/* 编解码错误码-1001~-1100 */
#define ERROR_ENCODE_BUFSIZE_SHORT                  -1001   /* 消息编码时缓存太小 */
#define ERROR_DECODE_BUFSIZE_SHORT                  -1002   /* 消息解码时缓存太小 */
#define ERROR_APPEND_BUFSIZE_SHORT                  -1003   /* 向数组中扩展字符时，数组长度太小 */
#define ERROR_ARRAY_SUB_FIELD_INFO_NULL             -1004   /* 数组字段的子字段信息为NULL */
#define ERROR_DECODE_ARRAY_LONG                     -1005   /* 数组解码时，数组个数太大 */
#define ERROR_DECODE_INVALID_MSG                    -1006   /* 消息解码到不合法的协议 */

/* 共享内存错误码-1101~-1200 */
#define ERROR_SHM_GET_FAILURE                       -1101   /* 创建共享内存失败 */
#define ERROR_SHM_ATTACH_FAILURE                    -1102   /* 附接到共享内存失败 */
#define ERROR_SHM_CTL_STAT_FAILURE                  -1103   /* 获取共享内存信息失败 */
#define ERROR_SHM_ALLOCATE_FAILURE                  -1104   /* 共享内存分配内存失败 */
#define ERROR_SHM_MODULE_NAME_REPEAT                -1105   /* 内存模块名重复 */
#define ERROR_SHM_MODULE_HEAD_SIZE_VERIFY           -1106   /* 模块内存头部保存的内存大小校验失败 */
#define ERROR_SHM_MODULE_HEAD_PROC_VERIFY           -1107   /* 模块内存头部保存的ProcID校验失败 */
#define ERROR_SHM_MODULE_NAME_VERIFY                -1108   /* 模块内存保存的名称校验失败 */
#define ERROR_SHM_DATA_ERROR                        -1109   /* 共享内存中的数据不正确 */

/* 共享内存消息队列-1201~-1300*/
#define ERROR_NO_MSG_Q_CHANNEL                      -1201   /* 无法找到消息队列 */
#define ERROR_MSG_Q_NO_INIT                         -1202   /* 消息队列未正常初始化 */
#define ERROR_MSG_Q_BUFF_FULL                       -1203   /* 消息队列长度不足 */
#define ERROR_MSG_Q_HEAD_SHORT                      -1204   /* 消息队列中保存消息头的长度不足 */
#define ERROR_MSG_Q_HEAD_FLAG                       -1205   /* 消息队列中消息头的FLAG不正确 */
#define ERROR_MSG_Q_HEAD_SRC_ADDR                   -1206   /* 消息队列中消息头的源地址不正确 */
#define ERROR_MSG_Q_HEAD_DEST_ADDR                  -1207   /* 消息队列中消息头的目的地址不正确 */
#define ERROR_MSG_Q_LEN_SHORT                       -1208   /* 消息队列中消息长度不正确 */
#define ERROR_MSG_Q_BUF_LEN_SHORT                   -1209   /* 从消息队列读取消息时，保存读取的消息的缓存长度不够 */
#define ERROR_MSG_Q_CHANNEL_SHM_CTL                 -1210   /* 消息队列中，读取通道共享内存失败 */
#define ERROR_MSG_Q_CHANNEL_NUM_BOUND               -1211   /* 消息队列的通道数量越界 */

/* 网络错误码-1301~-1400 */
#define ERROR_SOCKET_GET_FLAG                       -1301   /* socket调用fcntl获取标志失败 */
#define ERROR_SOCKET_SET_FLAG                       -1302   /* socket调用fcntl设置标志失败 */
#define ERROR_SOCKET_INVOKE                         -1303   /* 调用socket创建套接字失败 */
#define ERROR_SOCKET_BIND                           -1304   /* 绑定套接字失败 */
#define ERROR_SOCKET_LISTEN                         -1305   /* 套接字监听失败 */
#define ERROR_SOCKET_SET_REUSE                      -1306   /* 设置套接字重用失败 */
#define ERROR_SOCKET_CONNECT                        -1307   /* 连接服务器失败 */
#define ERROR_EPOLL_CREATE_FAIL                     -1308   /* 创建epoll失败 */
#define ERROR_EPOLL_CTL_ADD_FAIL                    -1309   /* epoll添加套接字监听失败 */
#define ERROR_EPOLL_CLIENT_FREE                     -1310   /* epoll客户端对象已被释放，可能是客户端关闭了连接 */
#define ERROR_EPOLL_CLIENT_UNMATCH                  -1311   /* epoll客户端数据与内存索引不匹配，可能是客户端关闭了连接，而内存被其他连接使用 */
#define ERROR_EPOLL_WRITE_ERROR                     -1312   /* epoll发送消息给客户端失败 */

/* 内存池错误码-1401~-1500 */
#define ERROR_MEM_POOL_BLOCK_SIZE_ZERO              -1401   /* 内存池块大小为0 */
#define ERROR_MEM_POOL_MEM_LEN_SHORT                -1402   /* 内存池分配的内存太小，比块小 */
#define ERROR_MEM_POOL_MEM_ALLOCATE                 -1403   /* 为内存池分配内存失败 */
#define ERROR_MEM_POOL_INDEX_INVALID                -1404   /* 内存地址索引不正确 */
#define ERROR_MEM_POOL_BLOCK_NUM_LARGE              -1405   /* 内存池块数量过大 */

/* XML解析错误码段-1501~-1600 */
#define ERROR_XML_LOAD_FIELD_NULL                   -1501   /* XML-Loader的当前字段对象为NULL */
#define ERROR_XML_LOAD_GET_SUB_FIELD_FUNC_NULL      -1502   /* GetSubFieldByName未定义 */
#define ERROR_XML_LOAD_NO_SUB_FIELD                 -1503   /* 未找到给定名字的子字段 */
#define ERROR_XML_LOAD_PARSE                        -1504   /* 解析xml出错 */
#define ERROR_XML_LOAD_ERROR                        -1505   /* 解析出现致命错误 */
#define ERROR_XML_LOAD_FATAL_ERROR                  -1506   /* 解析出现致命错误 */
#define ERROR_XML_LOAD_VALUE_TOO_LONG               -1507   /* 字段的值太长 */

/* 日志处理错误码段-1601~-1700 */
#define ERROR_LOG_MAX_SIZE_INVALID                  -1601   /* 日志最大文件长度不正确 */
#define ERROR_LOG_MAX_FILE_NUM_INVALID              -1602   /* 日志最大文件数量不正确 */
#define ERROR_LOG_OPEN_FILE_FAILURE                 -1603   /* 打开日志文件出错 */
#define ERROR_LOG_NO_INIT                           -1604   /* 日志对象未初始化 */

/* 命令行错误码-1701~-1800 */
#define ERROR_CMDLINE_NO_CFG_FILE                   -1701    /* 命令行参数缺少配置文件项 */
#define ERROR_CMDLINE_UNKNOWN                       -1702    /* 未知命令行参数 */
#define ERROR_CMDLINE_READ_CFG_FILE                 -1703    /* 读取配置文件出错 */
#define ERROR_CMDLINE_NO_MSG_Q_ID                   -1704    /* 命令行参数缺少消息通道ID */
#define ERROR_CMDLINE_NO_ID                         -1705    /* 命令行参数缺少id配置项 */

#endif
