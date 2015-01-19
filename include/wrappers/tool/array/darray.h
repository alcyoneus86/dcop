/// -------------------------------------------------
/// darray.h : ��̬���鹫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _ARRAY_DARRAY_H_
#define _ARRAY_DARRAY_H_

#include "dcop.h"


/// ��̬����Ĭ�ϲ���
#define DARRAY_STEP_COUNT           5

/// ��̬�ڴ��������
#define DARRAY_MAX_COUNT            65535


/// ��̬����
class CDArray
{
public:
    /// �Ƚϻص�����
    typedef bool (*COMPARE)(void *pNode, void *pKey);

    /// ɾ���ص�����
    typedef void (*DELETE)(DWORD index, void *pNode);

public:
    CDArray();
    CDArray(DWORD dwNodeSize, DWORD dwMaxCount = 0);
    ~CDArray();

    /// �����������
    void SetStepCount(DWORD dwStepCount) {m_dwBufMemStepCount = dwStepCount;}
    void SetMaxCount(DWORD dwMaxCount) {m_dwNodeMaxCount = dwMaxCount;}
    void SetNodeSize(DWORD dwNodeSize) {m_dwNodeSize = dwNodeSize;}
    void SetNodeDelete(DELETE fnDelete) {m_fnDelete = fnDelete;}

    /// ��ȡ�ڵ��С�ͽڵ����
    DWORD Size() const {return m_dwNodeSize;}
    DWORD Count() const {return m_dwNodeCount;}

    /// ���ò����ӿ�
    DWORD Append(void *pNode, DWORD dwSize = 0);
    void *Pos(DWORD index) const;
    DWORD Set(DWORD index, void *pNode, DWORD dwSize = 0);
    void *Get() const {return m_pBuffer;}
    DWORD Find(void *pNode, COMPARE fnCompare = 0);
    void  Clear();

private:
    /// ��̬����һ���ڴ�
    bool Dalloc(DWORD dwNewNodeCount);

    /// �ͷŶ�̬�ڴ�
    void Dfree();

private:
    void *m_pBuffer;                            // �ڴ��ַ
    DWORD m_dwBufMemCount;                      // �ڴ�����������
    DWORD m_dwBufMemStepCount;                  // �ڴ涯̬��������
    DWORD m_dwNodeMaxCount;                     // ���ڵ�����
    DWORD m_dwNodeCount;                        // �ڵ����
    DWORD m_dwNodeSize;                         // �ڵ��С
    DELETE m_fnDelete;                          // �ڵ�ɾ���ص�����
};


#endif // #ifndef _ARRAY_DARRAY_H_

