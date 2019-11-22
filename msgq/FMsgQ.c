/**
 * 共享内存式消息队列
 */

#include "FMsgQ.h"
#include "comm/ErrorCode.h"
#include "comm/BaseFuncDef.h"

#include <string.h>

#define MSG_Q_WR_FLAG_READ_POS  0   /* 从当前读取指针处读取消息 */
#define MSG_Q_WR_FLAG_LOOP_POS  1   /* 需从缓存取重复读取消息 */

#define MAX_MSG_Q_WR_MSG_NUM    1000000000 /* 10亿 */

/**
 * 从消息通道列表中找到通道，并发送消息到通道中
 * @param pstChannelData 通道列表
 * @param dwSrcAddr 消息源地址
 * @param dwDestAddr 消息目的地址
 * @param szBuf 要发送的消息
 * @param dwBufLen 要发送的消息长度
 * @return 若成功，返回0，失败返回错误码
 */
int msgq_send(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwSrcAddr, U32 dwDestAddr, const char* szBuf, U32 dwBufLen)
{
    LPMSGQUEUECHANNELINFO pstChannelInfo;
    LPMSGQUEUEINFO pstMsgQueue;
    char* p_str;        /* 临时变量 */
    U32 dwReadPos;      /* 读数据指针偏移 */
    U8 bFlag;           /* 数据读取flag，如果为1，则要从头开始读取 */
    U32 dwNeedLen;      /* 保存数据需要的总长度 */
    U32 dwReadNum;      /* 读取的数据数量 */
    char* szMsgQueue;   /* 消息队列数据区地址 */

    if (NULL == pstChannelData || NULL == szBuf || 0 == dwBufLen) return ERROR_INPUT_PARAM_NULL;

    pstChannelInfo = msg_queue_find_channel(pstChannelData, dwSrcAddr, dwDestAddr);

    if (NULL == pstChannelInfo) return ERROR_NO_MSG_Q_CHANNEL;

    szMsgQueue = msg_queue_find_queue_address(pstChannelInfo, dwSrcAddr, dwDestAddr);
    if (NULL == szMsgQueue) return ERROR_MSG_Q_NO_INIT;

    /* 消息队列首地址 */
    pstMsgQueue = pstChannelInfo->dwSrcAddress == dwSrcAddr ? &pstChannelInfo->stWriteQueue : &pstChannelInfo->stReadQueue;

    /* 以下数据先备份，避免被读进程覆盖，必要时，这里要加锁，:( */
    dwReadNum = pstMsgQueue->dwReadNum;     /* 重要的数据先备份 */
    dwReadPos = pstMsgQueue->dwReadPos;

    /* 检查消息有没有读取完？ */
    if (pstMsgQueue->dwWritePos == dwReadPos && dwReadNum != pstMsgQueue->dwWriteNum)
    {
        return ERROR_MSG_Q_BUFF_FULL;
    }

    /* 需要的缓存总长度 */
    dwNeedLen = dwBufLen + /*sizeof(dwSrcAddr) + sizeof(dwDestAddr)*/ + sizeof(dwBufLen) + sizeof(bFlag);

    /* 检查消息队列是否有足够的空间存放消息 */
    if (pstMsgQueue->dwWritePos >= dwReadPos)
    {
        U32 dwLeftLen = pstMsgQueue->dwQueueSize - pstMsgQueue->dwWritePos;

        if (dwLeftLen >= dwNeedLen)
        {
            /* 保存数据到内存中 */
            p_str = szMsgQueue + pstMsgQueue->dwWritePos;
            bFlag = MSG_Q_WR_FLAG_READ_POS;

            M_U8_TO_CHAR(p_str, bFlag);
            //M_U32_TO_CHAR(p_str, dwSrcAddr);
            //M_U32_TO_CHAR(p_str, dwDestAddr);
            M_U32_TO_CHAR(p_str, dwBufLen);
            memcpy(p_str, szBuf, dwBufLen);
            /* 更新下写数据的指针偏移 */
            pstMsgQueue->dwWritePos = dwLeftLen == dwNeedLen ? 0 : pstMsgQueue->dwWritePos + dwNeedLen;

            /* 更新下写的消息的数量 */
            pstMsgQueue->dwWriteNum++;
            //if (pstMsgQueue->dwWriteNum >= MAX_MSG_Q_WR_MSG_NUM) pstMsgQueue->dwWriteNum = 0;

            return 0;
        }
        /* 缓冲区长度不够保存数据，先置flag为1，下次读取数据时，要从头开始读取数据 */
        bFlag = MSG_Q_WR_FLAG_LOOP_POS;
        p_str = szMsgQueue + pstMsgQueue->dwWritePos;
        M_U8_TO_CHAR(p_str, bFlag);
        pstMsgQueue->dwWritePos = 0;

        /* 因为重置了dwWritePos，需要再检查消息有没有读取完？ */
        if (pstMsgQueue->dwWritePos == dwReadPos && dwReadNum != pstMsgQueue->dwWriteNum)
        {
            return ERROR_MSG_Q_BUFF_FULL;
        }
    }

    do
    {
        U32 dwLeftLen = dwReadPos - pstMsgQueue->dwWritePos;

        if (dwLeftLen >= dwNeedLen)
        {
            /* 保存数据到内存中 */
            p_str = szMsgQueue + pstMsgQueue->dwWritePos;
            bFlag = MSG_Q_WR_FLAG_READ_POS;

            M_U8_TO_CHAR(p_str, bFlag);
            //M_U32_TO_CHAR(p_str, dwSrcAddr);
            //M_U32_TO_CHAR(p_str, dwDestAddr);
            M_U32_TO_CHAR(p_str, dwBufLen);
            memcpy(p_str, szBuf, dwBufLen);
            /* 更新下写数据的指针偏移 */
            pstMsgQueue->dwWritePos = pstMsgQueue->dwWritePos + dwNeedLen;

            /* 更新下写的消息的数量 */
            pstMsgQueue->dwWriteNum++;
            //if (pstMsgQueue->dwWriteNum >= MAX_MSG_Q_WR_MSG_NUM) pstMsgQueue->dwWriteNum = 0;

            return 0;
        }
        else
        {
            return ERROR_MSG_Q_BUFF_FULL;
        }
    } while(0);

    return 0;
}

/**
 * 从消息通道列表中找到通道，并从通道中提取消息
 * @param pstChannelData 通道列表
 * @param dwSrcAddr 消息源地址
 * @param dwDestAddr 消息目的地址
 * @param pszBuf 用来保存消息的首地址
 * @param pdwMsgLen 函数返回时，填入消息长度
 * @param bRemove 填1将删除消息，填0将不删除消息。注：消息在处理完前，别删除消息，避免发消息的进程覆盖了消息。
 * @return 若成功，返回0，失败返回错误码
 */
static int msgq_do_recv(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwSrcAddr,
                        U32 dwDestAddr, char** pszBuf, U32* pdwMsgLen, U8 bRemove)
{
    LPMSGQUEUECHANNELINFO pstChannelInfo;
    LPMSGQUEUEINFO pstMsgQueue;
    char* p_str;        /* 临时变量 */
    U32 dwWritePos;     /* 写数据指针偏移 */
    U8 bFlag;           /* 数据读取flag，如果为1，则要从头开始读取 */
    U32 dwMsgLen;       /* 消息大小 */
    U32 dwMaxReadPos;   /* 读数据的最大指针偏移 */
    //U32 dwReadSrcAddr;
    //U32 dwReadDestAddr;
    U32 dwMinMsgLen;
    U32 dwWriteNum;     /* 写入的数据数量 */
    char* szMsgQueue;   /* 消息队列数据区地址 */

    if (NULL == pstChannelData) return ERROR_INPUT_PARAM_NULL;

    pstChannelInfo = msg_queue_find_channel(pstChannelData, dwSrcAddr, dwDestAddr);

    if (NULL == pstChannelInfo) return ERROR_NO_MSG_Q_CHANNEL;

    szMsgQueue = msg_queue_find_queue_address(pstChannelInfo, dwSrcAddr, dwDestAddr);
    if (NULL == szMsgQueue) return ERROR_MSG_Q_NO_INIT;

    /* 消息队列首地址 */
    pstMsgQueue = pstChannelInfo->dwSrcAddress == dwSrcAddr ? &pstChannelInfo->stWriteQueue : &pstChannelInfo->stReadQueue;

    /* 以下数据先备份，避免被写进程覆盖，必要时，这里要加锁，:( */
    dwWriteNum = pstMsgQueue->dwWriteNum;	/* 重要的数据先备份 */
    dwWritePos = pstMsgQueue->dwWritePos;

    /* 一般取数据的指针偏移在存数据的指针偏移后面 */
    dwMaxReadPos = dwWritePos;

    /* 如果读数据指针偏移在写数据指针偏移前面，则说明写数据的指针偏移又从内存首地址开始存入数据 */
    if (pstMsgQueue->dwReadPos > dwWritePos)
    {
        /* 检查下Flag是否为1？如果为1，说明下条消息要从头开始读取 */
        p_str = szMsgQueue + pstMsgQueue->dwReadPos;
        M_CHAR_TO_U8(bFlag, p_str);

        if (bFlag == MSG_Q_WR_FLAG_LOOP_POS)
        {
            pstMsgQueue->dwReadPos = 0;
        }
        else
        {
            dwMaxReadPos = pstMsgQueue->dwQueueSize;
        }
    }
    else if (pstMsgQueue->dwReadPos == dwWritePos)
    {
        dwMaxReadPos = pstMsgQueue->dwQueueSize;
    }

    /* 没有消息 */
    if (dwWritePos == pstMsgQueue->dwReadPos && dwWriteNum == pstMsgQueue->dwReadNum)
    {
        if (pdwMsgLen != NULL) *pdwMsgLen = 0;
        return 0;
    }

    /* 检查最小消息 */
    dwMinMsgLen = sizeof(bFlag) + /*sizeof(dwReadSrcAddr) + sizeof(dwReadDestAddr)*/ + sizeof(dwMsgLen);
    if (dwMinMsgLen > dwMaxReadPos - pstMsgQueue->dwReadPos) return ERROR_MSG_Q_HEAD_SHORT;

    /* 进入这里，统一读取消息内容 */
    /* 1. 这里校验一下bFlag, dwSrcAddr, dwDestAddr */
    p_str = szMsgQueue + pstMsgQueue->dwReadPos;
    M_CHAR_TO_U8(bFlag, p_str);
    //M_CHAR_TO_U32(dwReadSrcAddr, p_str);
    //M_CHAR_TO_U32(dwReadDestAddr, p_str);
    /* 读取消息大小 */
    M_CHAR_TO_U32(dwMsgLen, p_str);

    if (MSG_Q_WR_FLAG_READ_POS != bFlag) return ERROR_MSG_Q_HEAD_FLAG;
    //if (dwReadSrcAddr != dwSrcAddr) return ERROR_MSG_Q_HEAD_SRC_ADDR;
    //if (dwReadDestAddr != dwDestAddr) return ERROR_MSG_Q_HEAD_DEST_ADDR;
    if (dwMsgLen > dwMaxReadPos - pstMsgQueue->dwReadPos - dwMinMsgLen) return ERROR_MSG_Q_LEN_SHORT;

    /* 返回内存地址 */
    if (pszBuf != NULL) *pszBuf = p_str;
    if (pdwMsgLen != NULL) *pdwMsgLen = dwMsgLen;
    /* 移动读数据指针偏移 */
    if (bRemove)
    {
        /* 移动读数据指针偏移 */
        pstMsgQueue->dwReadPos += dwMsgLen + dwMinMsgLen;
        if (pstMsgQueue->dwReadPos >= pstMsgQueue->dwQueueSize) pstMsgQueue->dwReadPos = 0;

        /* 更新下读的消息的数量 */
        pstMsgQueue->dwReadNum++;
        //if (pstMsgQueue->dwReadNum >= MAX_MSG_Q_WR_MSG_NUM) pstMsgQueue->dwReadNum = 0;
    }

    return 0;
}

/**
 * 从消息通道列表中找到通道，并从通道中提取消息
 * @param pstChannelData 通道列表
 * @param piSrcAddr 消息源地址，如果填入-1，表示匹配任意源地址。函数返回时，填入消息源地址
 * @param dwDestAddr 消息目的地址
 * @param pszBuf 用来保存消息的首地址
 * @param pdwMsgLen 函数返回时，填入消息长度
 * @return 若成功，返回0，失败返回错误码
 */
int msgq_recv(LPMSGQUEUECHANNELDATA pstChannelData, S32* piSrcAddr, U32 dwDestAddr, char** pszBuf, U32* pdwMsgLen)
{
    U8 i;
    //static U8 bChannelPos = 0;
    int iRet = 0;

    if (NULL == pstChannelData || NULL == piSrcAddr || NULL == pdwMsgLen) return ERROR_INPUT_PARAM_NULL;

    /* 如果是指定源地址 */
    if (*piSrcAddr != -1)
    {
        return msgq_do_recv(pstChannelData, *piSrcAddr, dwDestAddr, pszBuf, pdwMsgLen, 0);
    }

    /* 不指定源地址，轮询找到目的地址 */
    //if (bChannelPos >= pstChannelData->bNum) bChannelPos = 0;

    for (i = 0; i < pstChannelData->bNum; i++)
    {
        /* 如果目的地址相同 */
        if (pstChannelData->astChannelInfo[i].dwDestAddress == dwDestAddr)
        {
            *piSrcAddr = pstChannelData->astChannelInfo[i].dwSrcAddress;
            iRet = msgq_do_recv(pstChannelData, *piSrcAddr, dwDestAddr, pszBuf, pdwMsgLen, 0);
            /* 如果消息长度为0，继续下次轮询 */
            if (iRet == 0 && *pdwMsgLen == 0) continue;
            /* 发生错误，是否也进行下次轮询？纠结 */
            return iRet;
        }
        /* 如果源地址相同 */
        if (pstChannelData->astChannelInfo[i].dwSrcAddress == dwDestAddr)
        {
            *piSrcAddr = pstChannelData->astChannelInfo[i].dwDestAddress;
            iRet = msgq_do_recv(pstChannelData, *piSrcAddr, dwDestAddr, pszBuf, pdwMsgLen, 0);
            /* 如果消息长度为0，继续下次轮询 */
            if (iRet == 0 && *pdwMsgLen == 0) continue;
            /* 发生错误，是否也进行下次轮询？纠结 */
            return iRet;
        }
        /*bChannelPos++;*/
    }

    /* 以上轮询时，因为有continue语句，可能会导致iRet=0， *pdwBufSize=0，这是正确的 */
    return iRet;
}

/**
 * 从消息通道列表中找到通道，并从通道中删除第一个消息
 * @param pstChannelData 通道列表
 * @param dwSrcAddr 消息源地址
 * @param dwDestAddr 消息目的地址
 * @return 若成功，返回0，失败返回错误码
 */
int msgq_pop(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwSrcAddr, U32 dwDestAddr)
{
    return msgq_do_recv(pstChannelData, dwSrcAddr, dwDestAddr, NULL, NULL, 1);
}

/**
 * 从消息通道列表中找到通道，并从通道中提取消息。调用回调函数处理完消息后，再调用msgq_pop将消息删除
 * @param pstChannelData 通道列表
 * @param dwDestAddr 消息目的地址
 * @param pstHandleMsgFunc 处理消息的回调函数
 * @param pstData 调用端传入的数据
 * @param bRemoveOnFail 是否在失败时，也删除消息。bRemoveOnFail=0则不删除
 * @return 成功返回0，失败返回错误码
 */
int msgq_handle(LPMSGQUEUECHANNELDATA pstChannelData, U32 dwDestAddr, MSGQFNHANDLEMSG pstHandleMsgFunc, void* pstData, U8 bRemoveOnFail)
{
    int iRet;
    char* pszBuf = NULL;
    U32 dwMsgLen = 0;
    S32 iSrcAddr = -1;          /* 填-1接收所有消息 */

    if (NULL == pstChannelData) return ERROR_INPUT_PARAM_NULL;

    /* 1. 从Msg-Q接收消息 */
    iRet = msgq_recv(pstChannelData, &iSrcAddr, dwDestAddr, &pszBuf, &dwMsgLen);

    if (iRet != 0)
    {
        return iRet;
    }
    else if (0 == dwMsgLen)
    {
        /* 没有收到消息，直接返回 */
        return 0;
    }

    /* 2. 处理消息 */
    if (pstHandleMsgFunc)
    {
        iRet = pstHandleMsgFunc(pstData, iSrcAddr, dwDestAddr, pszBuf, dwMsgLen);
        if (iRet != 0 && !bRemoveOnFail) return iRet;
    }

    /* 3. 移除消息 */
    msgq_pop(pstChannelData, iSrcAddr, dwDestAddr);

    return iRet;
}

