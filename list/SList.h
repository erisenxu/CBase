/*
 * @(#) SList.h Created on 2008-03-27 单向链表
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */

#ifndef SLIST_H
#define SLIST_H

#include "comm/BaseDef.h"

typedef struct tagSListNode     SLISTNODE;
typedef struct tagSListNode*    LPSLISTNODE;

/**
 * 单向列表类的节点
 */
struct tagSListNode
{
    LPSLISTNODE pstNext;
    void* pstData;
};

/**
 * 单向列表类
 */
struct tagSList
{
    LPSLISTNODE pstHead;
    LPSLISTNODE pstTail;
    U64 ullNumber;
};

typedef struct tagSList         SLIST;
typedef struct tagSList*        LPSLIST;

/**
 * 释放节点内存的函数
 */
typedef void (*FNFREESLISTNODE) (void*);

/**
 * 初始化单向列表
 */
#define INIT_SLIST(pstList) \
    do \
    { \
        pstList->pstHead = NULL; \
        pstList->pstTail = NULL; \
        pstList->ullNumber = 0; \
    } while(0)

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 创建单向列表
 * @return 返回创建的列表，如果分配内存失败，返回NULL
 */
LPSLIST slist_create(void);

/**
 * 清空单向列表中的列表元素，并释放列表元素的内存，及释放单向列表内存
 * @param pstList 被释放的单向列表指针
 * @param pstFreeFunc 释放节点时调用的功能函数
 */
void slist_free(LPSLIST pstList, FNFREESLISTNODE pstFreeFunc);

/**
 * 清空单向列表中的列表元素，并释放列表元素的内存，但不释放单向列表内存
 * @param pstList 被清空的单向列表指针
 * @param pstFreeFunc 释放节点时调用的功能函数
 */
void slist_clear(LPSLIST pstList, FNFREESLISTNODE pstFreeFunc);

/**
 * 向单向列表中追加新的节点
 * @param pstList 单向列表指针
 * @param pstNode 要添加的单向列表节点
 */
void slist_append_node(LPSLIST pstList, LPSLISTNODE pstNode);

/**
 * 向单向列表中追加新的节点
 * @param pstList 单向列表指针
 * @param pstData 要添加的单向列表节点数据
 * @return 返回添加的单向列表节点对象，若分配节点失败，返回NULL
 */
LPSLISTNODE slist_append(LPSLIST pstList, void* pstData);

/**
 * 将列表第一个元素从列表中删除
 * @param pList 单向列表指针
 * @return 返回删除的单向列表节点对象
 */
LPSLISTNODE slist_remove_head(LPSLIST pstList);

#ifdef __cplusplus
}
#endif

#endif
