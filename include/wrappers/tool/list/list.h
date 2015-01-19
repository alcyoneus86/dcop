/// -------------------------------------------------
/// list.h : �������ݽṹ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_LIST_LIST_H_
#define _TOOL_LIST_LIST_H_


/////////////////////////////////////////////////////
///                List����˵��
/// -------------------------------------------------
/// ����DHCP�Ĵ��룬ʵ����һ����������
/// �Ƚ������Ŀ�����LIST_ENTRY�еĳ�Աle_prev����
/// ����ƽʱ˫�������е�ǰ��ָ��(ָ��ǰһ��Ԫ��)����
/// ��ǰ��ǰһ��Ԫ�ص�le_next��Ա��ָ��(ָ���ָ��)��
/// �ô���Ȼ�Ǵ����١�Ч�ʸߡ�
/// -------------------------------------------------
/// �÷�:
/// �ڵ����͵�����:
///     struct LstNode
///     {
///         LIST_ENTRY(LstNode) m_field;
///         DATETYPE datavalue;
///     };
/// ����ͷ����:
///     LIST_HEAD(LstNode) m_head;
/// ��ʼ������ͷ:
///     LIST_INIT(&m_head);
/// ��ʼ������ڵ�:
///     LIST_NODE_INIT(pNode, m_field);
/// ��ȡ��һ���ڵ�:
///     LIST_NEXT(pNode, m_field);
/////////////////////////////////////////////////////

/* 
 * List declarations. 
 */

#define LIST_HEAD(type)                                                         \
    struct {                                                                    \
        type *lh_first;                                                         \
        int count;                                                              \
    }

#define LIST_ENTRY(type)                                                        \
    struct {                                                                    \
        type *le_next;                                                          \
        type **le_prev;                                                         \
    }

/* 
 * List functions. 
 */

#define LIST_EMPTY(head)        (!((head)->lh_first) || !((head)->count))

#define LIST_FIRST(head)        ((head)->lh_first)

#define LIST_COUNT(head)        ((head)->count)

#define LIST_NEXT(elm, field)   ((elm)->field.le_next)

#define LIST_INIT(head)                                                         \
    do {                                                                        \
        LIST_FIRST((head)) = 0;                                                 \
        LIST_COUNT(head) = 0;                                                   \
    } while (0)

#define LIST_NODE_INIT(elm, field)                                              \
    do {                                                                        \
        LIST_NEXT(elm, field) = 0;                                              \
        ((elm)->field.le_prev) = 0;                                             \
    } while (0)

#define LIST_INSERT_HEAD(head, elm, field)                                      \
    do {                                                                        \
        if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != 0)                \
            LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field);       \
        LIST_FIRST((head)) = (elm);                                             \
        (elm)->field.le_prev = &LIST_FIRST((head));                             \
        LIST_COUNT(head)++;                                                     \
    } while (0)

#define LIST_REMOVE(head, elm, field)                                           \
    do {                                                                        \
        if (LIST_NEXT((elm), field) != 0)                                       \
            LIST_NEXT((elm), field)->field.le_prev = (elm)->field.le_prev;      \
        *((elm)->field.le_prev) = LIST_NEXT((elm), field);                      \
        if ((elm) == LIST_FIRST(head))                                          \
            LIST_FIRST(head) = LIST_NEXT(elm, field);                           \
        LIST_COUNT(head)--;                                                     \
    } while (0)

#define LIST_CLEAR(head, type, field, free)                                     \
    do {                                                                        \
        type *___pNodeLoop = LIST_FIRST(head);                                  \
        while (___pNodeLoop)                                                    \
        {                                                                       \
            type *___pNodeFree = ___pNodeLoop;                                  \
            ___pNodeLoop = LIST_NEXT(___pNodeLoop, field);                      \
            LIST_REMOVE(head, ___pNodeFree, field);                             \
            free(___pNodeFree);                                                 \
        }                                                                       \
        LIST_INIT(head);                                                        \
    } while (0)


#endif // #ifndef _TOOL_LIST_LIST_H_

