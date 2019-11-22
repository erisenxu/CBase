/**
 * 共享内存式消息队列
 */

#ifndef FMSG_Q_H
#define FMSG_Q_H

#include "FMsgChannel.h"

/**
 * 处理消息回调函数
 * @param pstData 调用端传入的数据
 * @param iSrcAddr 消息源地址
 * @param dwDestAddr 消息目的地址
 * @param pszBuf 消息队列中接收到的消息
 * @param dwMsgLen 消息长度
 * @return 成功返回0，失败返回错误码
 */
typedef int (*MSGQFNHANDLEMSG)(void* pstData, S32 iSrcAddr, U32 dwDestAddr, char* pszBuf, U32 dwMsgLen);

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 从消息通道列表中找到通道，并发送消息到通道中
 * @param pstChannelData 通道列表
 * @param dwSrcAddr 消息源地址
 * @param dwDestAddr 消息目的地址
 * @param szBuf 要发送的消息
 * @param dwBufLen 要发送的消息长度
 * @return 若成功，返回0，失败返回错误码
 */
int msgq_send(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwSrcAddr, U32 dwDestAddr, const char* szBuf, U32 dwBufLen);

/**
 * 从消息通道列表中找到通道，并从通道中提取消息。调用端处理完消息后，需调用msgq_pop将消息删除
 * @param pstChannelData 通道列表
 * @param piSrcAddr 消息源地址，如果填入-1，表示匹配任意源地址。函数返回时，填入消息源地址
 * @param dwDestAddr 消息目的地址
 * @param pszBuf 用来保存消息的首地址
 * @param pdwMsgLen 函数返回时，填入消息长度
 * @return 若成功，返回0，失败返回错误码
 */
int msgq_recv(LPMSGQUEUECHANNELDATA pstChannelData, S32* piSrcAddr, U32 dwDestAddr, char** pszBuf, U32* pdwMsgLen);

/**
 * 从消息通道列表中找到通道，并从通道中删除第一个消息
 * @param pstChannelData 通道列表
 * @param dwSrcAddr 消息源地址
 * @param dwDestAddr 消息目的地址
 * @return 若成功，返回0，失败返回错误码
 */
int msgq_pop(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwSrcAddr, U32 dwDestAddr);

/**
 * 从消息通道列表中找到通道，并从通道中提取消息。调用回调函数处理完消息后，再调用msgq_pop将消息删除
 * @param pstChannelData 通道列表
 * @param dwDestAddr 消息目的地址
 * @param pstHandleMsgFunc 处理消息的回调函数
 * @param pstData 调用端传入的数据
 * @param bRemoveOnFail 是否在失败时，也删除消息。bRemoveOnFail=0则不删除
 * @return 成功返回0，失败返回错误码
 */
int msgq_handle(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwDestAddr, MSGQFNHANDLEMSG pstHandleMsgFunc, void* pstData, U8 bRemoveOnFail);

#ifdef __cplusplus
}
#endif

#endif

