/**
 * @(#) FSList.c 实现单向列表
 *
 * @author Erisen Xu
 * @version 1.0
 * @history 2010-11-01 ErisenXu 创建文件
 */

#include "FSList.h"

#include <stdlib.h>

/**
 * 创建单向列表节点对象
 */
static LPFSLISTNODE create_fslist_node(void* pData)
{
    LPFSLISTNODE pstNode;

    pstNode = (LPFSLISTNODE)malloc(sizeof(FSLISTNODE));
    pstNode->pstData = pData;
    pstNode->pstNext = NULL;
    return pstNode;
}

/**
 * 创建单向列表
 */
LPFSLIST create_fslist()
{
    LPFSLIST pstList;

    pstList = (LPFSLIST)malloc(sizeof(FSLIST));

    pstList->pstHead = NULL;
    pstList->pstTail = NULL;

    return pstList;
}

/**
 * 释放单向列表所占内存空间
 * @param pstList 被释放的单向列表指针
 * @param bFreeData 若bFreeData=M_TRUE，将释放列表元素的数据所占用的内存
 */
void free_fslist(LPFSLIST pstList, U8 bFreeData, LPFREENODEFUNC pstFreeFunc)
{
    if (NULL == pstList) return;

    clear_fslist(pstList, bFreeData, pstFreeFunc);

    free(pstList);
}

/**
 * 清空单向列表中的列表元素，并释放列表元素的内存，但不释放单向列表内存
 * @param pstList 被清空的单向列表指针
 * @param bFreeData 若bFreeData=M_TRUE，将释放列表元素的数据所占用的内存
 */
void clear_fslist(LPFSLIST pstList, U8 bFreeData, LPFREENODEFUNC pstFreeFunc)
{
    LPFSLISTNODE pstCurNode;
    LPFSLISTNODE pstNextNode;

    if (NULL == pstList) return;

    pstCurNode = pstList->pstHead;

    if (bFreeData)
    {
        while (pstCurNode)
        {
            pstNextNode = pstCurNode->pstNext;
            if (pstFreeFunc)
            {
                pstFreeFunc(pstCurNode->pstData);
            }
            else
            {
                free(pstCurNode->pstData);
            }
            free(pstCurNode);
            pstCurNode = pstNextNode;
        }
    }
    else
    {
        while (pstCurNode)
        {
            pstNextNode = pstCurNode->pstNext;
            free(pstCurNode);
            pstCurNode = pstNextNode;
        }
    }
    pstList->pstHead = NULL;
    pstList->pstTail = NULL;
}

/**
 * 向单向列表中追加新的节点
 * @param pstList 单向列表指针
 * @param pstData 要添加的单向列表节点数据
 * @return 返回添加的单向列表节点对象
 */
LPFSLISTNODE fslist_append(LPFSLIST pstList, void* pstData)
{
    LPFSLISTNODE pstNewNode;

    pstNewNode = create_fslist_node(pstData);

    fslist_append_node(pstList, pstNewNode);

    return pstNewNode;
}

/**
 * 向单向列表中追加新的节点
 * @param pstList 单向列表指针
 * @param pstNode 要添加的单向列表节点
 */
void fslist_append_node(LPFSLIST pstList, LPFSLISTNODE pstNode)
{
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
}

/**
 * 将列表第一个元素从列表中删除
 * @param pList 单向列表指针
 * @return 返回删除的单向列表节点对象
 */
LPFSLISTNODE fslist_remove_head(LPFSLIST pstList)
{
    LPFSLISTNODE pstHead;

    pstHead = pstList->pstHead;

    if (!pstHead) return pstHead;

    pstList->pstHead = pstHead->pstNext;
    pstHead->pstNext = NULL;

    return pstHead;
}
