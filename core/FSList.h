/**
 * @(#) FSList.h 实现单向列表
 *
 * @author Erisen Xu
 * @version 1.0
 * @history 2010-11-01 ErisenXu 创建文件
 */

#ifndef FSLIST_H
#define FSLIST_H

#include "comm/BaseDef.h"

typedef struct tagSListNode     FSLISTNODE;
typedef struct tagSListNode*    LPFSLISTNODE;

/**
 * 单向列表类的节点
 */
struct tagSListNode
{
    LPFSLISTNODE pstNext;
    void* pstData;
};

/**
 * 单向列表类
 */
struct tagSList
{
    LPFSLISTNODE pstHead;
    LPFSLISTNODE pstTail;
};

typedef struct tagSList     FSLIST;
typedef struct tagSList*    LPFSLIST;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 释放节点内存的函数
 */
typedef void (*LPFREENODEFUNC) (void*);

/**
 * 创建单向列表
 */
LPFSLIST create_fslist();

/**
 * 释放单向列表所占内存空间
 * @param pstList 被释放的单向列表指针
 * @param bFreeData 若bFreeData=1，将释放列表元素的数据所占用的内存
 */
void free_fslist(LPFSLIST pstList, U8 bFreeData, LPFREENODEFUNC pstFreeFunc);

/**
 * 清空单向列表中的列表元素，并释放列表元素的内存，但不释放单向列表内存
 * @param pstList 被清空的单向列表指针
 * @param bFreeData 若bFreeData=M_TRUE，将释放列表元素的数据所占用的内存
 */
void clear_fslist(LPFSLIST pstList, U8 bFreeData, LPFREENODEFUNC pstFreeFunc);

/**
 * 向单向列表中追加新的节点
 * @param pList 单向列表指针
 * @param pData 要添加的单向列表节点数据
 * @return 返回添加的单向列表节点对象
 */
LPFSLISTNODE fslist_append(LPFSLIST pstList, void* pstData);

/**
 * 向单向列表中追加新的节点
 * @param pList 单向列表指针
 * @param pNode 要添加的单向列表节点
 */
void fslist_append_node(LPFSLIST pstList, LPFSLISTNODE pstNode);

/**
 * 将列表第一个元素从列表中删除
 * @param pList 单向列表指针
 * @return 返回删除的单向列表节点对象
 */
LPFSLISTNODE fslist_remove_head(LPFSLIST pstList);

#ifdef __cplusplus
}
#endif

#endif
