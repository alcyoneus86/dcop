/// -------------------------------------------------
/// hashtableindex.h : HASH�����������๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _HASHTABLEINDEX_H_
#define _HASHTABLEINDEX_H_

#include "type.h"


/// Ĭ��HASH��ĳ���
const DWORD HASHTABLE_LENGTH_DEFAULT = 256;

/// Ĭ�Ͽ�ֵ
const DWORD HASHTABLE_NULLVALUE_DEFAULT = 0xffffffffL;


/// HASH������������
class CHashTableIdxTool
{
public:
    /// �Զ���HASH�㷨�ص�����ԭ��
    typedef DWORD (*FUNC_HASH_INDEX)(
        DWORD dwID,
        void *pPara
        );

private:
    DWORD *m_pHashIndexTable;               // HASH���ַ
    DWORD m_dwTableLength;                  // HASH����
    FUNC_HASH_INDEX m_fnHashFunc;           // HASH�㷨�ص�����
    void *m_pHashFuncPara;                  // HASH�㷨�ص���������
    DWORD m_dwNullValue;                    // ��ֵ

public:
    CHashTableIdxTool();
    CHashTableIdxTool(DWORD dwHashLength, 
        FUNC_HASH_INDEX fnHashFunc,
        void *pHashFuncPara,
        DWORD dwNullValue);
    ~CHashTableIdxTool();

    void vSetTableLength(DWORD dwLength = 0);

    DWORD dwSetIDToIdx(DWORD dwID);
    DWORD dwGetIdxByID(DWORD dwID);
    DWORD dwGetIDByIdx(DWORD dwIdx);
    DWORD dwClearIdxInID(DWORD dwID);
    DWORD dwClearIdxInIdx(DWORD dwIdx);
};


#endif // #ifndef _HASHTABLEINDEX_H_
