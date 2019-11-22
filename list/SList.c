/*
 * @(#) SList.c Created on 2008-03-27 单向链表
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#include "SList.h"
#include <stdlib.h>

/**
 * 创建单向列表节点对象
 * @param pstData 节点中保存的数据
 * @return 返回创建的单向列表节点对象
 */
static LPSLISTNODE create_slist_node(void* pstData)
{
    LPSLISTNODE pstNode = malloc(sizeof(SLISTNODE));

    if (NULL == pstNode) return NULL;

    pstNode->pstData = pstData;
    pstNode->pstNext = NULL;

    return pstNode;
}

/**
 * 创建单向列表
 */
LPSLIST slist_create(void)
{
    LPSLIST pstList = malloc(sizeof(SLIST));

    if (NULL == pstList) return NULL;

    INIT_SLIST(pstList);

    return pstList;
}

/**
 * 清空单向列表中的列表元素，并释放列表元素的内存，及释放单向列表内存
 * @param pstList 被释放的单向列表指针
 * @param pstFreeFunc 释放节点时调用的功能函数
 */
void slist_free(LPSLIST pstList, FNFREESLISTNODE pstFreeFunc)
{
    if (NULL == pstList) return;

    slist_clear(pstList, pstFreeFunc);

    free(pstList);
}

/**
 * 清空单向列表中的列表元素，并释放列表元素的内存，但不释放单向列表内存
 * @param pstList 被清空的单向列表指针
 * @param pstFreeFunc 释放节点时调用的功能函数
 */
void slist_clear(LPSLIST pstList, FNFREESLISTNODE pstFreeFunc)
{
    LPSLISTNODE pstCurNode;
    LPSLISTNODE pstNextNode;

    if (NULL == pstList) return;

    pstCurNode = pstList->pstHead;

    if (pstFreeFunc)
    {
        while(pstCurNode)
        {
            pstNextNode = pstCurNode->pstNext;
            pstFreeFunc(pstCurNode->pstData);
            free(pstCurNode);
            pstCurNode = pstNextNode;
        }
    }
    else
    {
        while(pstCurNode)
        {
            pstNextNode = pstCurNode->pstNext;
            free(pstCurNode);
            pstCurNode = pstNextNode;
        }
    }

    pstList->pstHead = NULL;
    pstList->pstTail = NULL;
    pstList->ullNumber = 0;
}

/**
 * 向单向列表中追加新的节点
 * @param pstList 单向列表指针
 * @param pstNode 要添加的单向列表节点
 */
void slist_append_node(LPSLIST pstList, LPSLISTNODE pstNode)
{
    if (NULL == pstList || NULL == pstNode) return;

    if (!(pstList->pstHead))
    {
        pstList->pstHead = pstNode;
        pstList->pstTail = pstList->pstHead;
    }
    else if (!(pstList->pstTail))
    {
        pstList->pstTail = pstList->pstHead;
        pstList->pstTail->pstNext = pstNode;
        pstList->pstTail = pstNode;
    }
    else
    {
        pstList->pstTail->pstNext = pstNode;
        pstList->pstTail = pstNode;
    }

    pstList->ullNumber++;
}

/**
 * 向单向列表中追加新的节点
 * @param pstList 单向列表指针
 * @param pstData 要添加的单向列表节点数据
 * @return 返回添加的单向列表节点对象，若分配节点失败，返回NULL
 */
LPSLISTNODE slist_append(LPSLIST pstList, void* pstData)
{
    LPSLISTNODE pstNewNode;

    if (NULL == pstList) return NULL;

    pstNewNode = create_slist_node(pstData);

    if (NULL == pstNewNode) return NULL;

    slist_append_node(pstList, pstNewNode);

    return pstNewNode;
}

/**
 * 将列表第一个元素从列表中删除
 * @param pList 单向列表指针
 * @return 返回删除的单向列表节点对象
 */
LPSLISTNODE slist_remove_head(LPSLIST pstList)
{
    LPSLISTNODE pstHead;

    if (NULL == pstList) return NULL;

    pstHead = pstList->pstHead;

    if (!pstHead) return pstHead;

    pstList->pstHead = pstHead->pstNext;
    pstHead->pstNext = NULL;
    pstList->ullNumber--;

    return pstHead;
}
