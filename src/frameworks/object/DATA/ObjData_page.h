/// -------------------------------------------------
/// ObjData_page.h : �ڴ�ҳ˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDATA_PAGE_H_
#define _OBJDATA_PAGE_H_

#include "dcop.h"
#include "list/dllist.h"


/// Ĭ�϶�̬ҳ��ļ�¼��
#define DCOP_DATA_DEF_REC_COUNT             16


/// �ڴ�ҳ��
class CMemPage
{
public:

    /// �ڴ�ҳ��Ϣ
    struct PageInfo
    {
        BYTE m_byMagicWord0;                // ͷ��ħ����(�̶�Ϊ'D')
        BYTE m_byMagicWord1;                // ͷ��ħ����(�̶�Ϊ'c')
        BYTE m_byMagicWord2;                // ͷ��ħ����(�̶�Ϊ'o')
        BYTE m_byMagicWord3;                // ͷ��ħ����(�̶�Ϊ'p')
        DWORD m_dwMemCount;                 // �ڴ�������Ŀ
        DWORD m_dwRecCount;                 // ��Ч��¼��Ŀ
        DLL_ENTRY(PageInfo) m_field;        // ������
    };

    /// ��¼ͷ
    struct RecordHead
    {
        PageInfo *m_pPage;                  // �ڴ�ҳ
        DWORD m_dwMemPos;                   // ƫ��ֵ
        DLL_ENTRY(RecordHead) m_field;      // ������
    };

public:
    CMemPage();
    ~CMemPage();

    /// ���ö�̬���볤��
    void SetPageDefSize(DWORD dwPageDefSize) {m_dwPageDefSize = dwPageDefSize;}

    /// �½�һ��ҳ��
    PageInfo *NewPage(DWORD dwRecSize, DWORD dwMaxCount);

    /// ��Ӽ�¼
    RecordHead *AppendRec(BYTE *pbyRec);

    /// ɾ����¼
    DWORD DeleteRec(RecordHead *pRecord);

    /// ��ȡ��һ����Ч��¼����һ����Ч��¼
    RecordHead *GetFirstRec() {return DLL_FIRST(&m_records);}
    RecordHead *GetNextRec(RecordHead *pCurRec) {return DLL_NEXT_LOOP(&m_records, pCurRec, m_field);}

    /// ��ȡָ����Ч��¼
    PageInfo *GetRealRec(DWORD dwRecPos, DWORD &rdwMemPos);

    /// �ҵ�һ�����м�¼
    PageInfo *GetIdleRec(DWORD &rdwMemPos);

    /// �������ҳ��
    void ClearPages();

    /// ��ȡ��¼����
    DWORD GetRecCount() {return m_dwRecCount;}

private:
    DWORD m_dwPageDefSize;                  // ҳ��Ĭ�ϴ�С(ָ���ɵļ�¼����)
    DWORD m_dwRecSize;                      // ��¼��С
    DLL_HEAD(PageInfo) m_pages;             // ҳ�漯��
    DLL_HEAD(RecordHead) m_records;         // ��¼����
    DWORD m_dwRecCount;                     // ��¼����
};


#endif // #ifndef _OBJDATA_PAGE_H_

