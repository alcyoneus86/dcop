/// -------------------------------------------------
/// ObjData_page.cpp : �ڴ�ҳʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_page.h"


/*******************************************************
  �� �� ��: CMemPage::CMemPage
  ��    ��: CMemPage���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemPage::CMemPage()
{
    m_dwPageDefSize = DCOP_DATA_DEF_REC_COUNT;
    m_dwRecSize = 0;
    DLL_INIT(&m_pages);
    DLL_INIT(&m_records);
    m_dwRecCount = 0;
}

/*******************************************************
  �� �� ��: CMemPage::~CMemPage
  ��    ��: CMemPage��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemPage::~CMemPage()
{
    ClearPages();
}

/*******************************************************
  �� �� ��: CMemPage::NewPage
  ��    ��: �½�ҳ��
  ��    ��: dwRecSize
            dwMaxCount
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemPage::PageInfo *CMemPage::NewPage(DWORD dwRecSize, DWORD dwMaxCount)
{
    /// ��Ϊ��ʱ����¼��С������趨��һ��
    if (!DLL_EMPTY(&m_pages))
    {
        if (dwRecSize != m_dwRecSize)
        {
            return NULL;
        }
    }

    /// ����ҳ��ʵ�ʴ�С
    DWORD dwMemLen = dwMaxCount * (sizeof(RecordHead) + dwRecSize);
    if (!dwMemLen)
    {
        return NULL;
    }

    /// ����ҳ���ڴ�
    PageInfo *pPage = (PageInfo *)DCOP_Malloc(sizeof(PageInfo) + dwMemLen);
    if (!pPage)
    {
        return NULL;
    }

    /// ��ʼ��ҳ��
    pPage->m_byMagicWord0 = OSBUF_MAGIC0;
    pPage->m_byMagicWord1 = OSBUF_MAGIC1;
    pPage->m_byMagicWord2 = OSBUF_MAGIC2;
    pPage->m_byMagicWord3 = OSBUF_MAGIC3;
    pPage->m_dwMemCount = dwMaxCount;
    pPage->m_dwRecCount = 0;

    (void)memset(pPage + 1, 0, dwMemLen);

    /// ��һ�ε�������Ϊ�趨ֵ��������
    if (DLL_EMPTY(&m_pages))
    {
        m_dwRecSize = dwRecSize;
    }

    /// ���ҳ�浽β��
    DLL_INSERT_TAIL(&m_pages, pPage, m_field);

    return pPage;
}

/*******************************************************
  �� �� ��: CMemPage::AppendRec
  ��    ��: ��Ӽ�¼(�Զ��ҵ���ɾ���Ŀ���λ�ã�û�п���λ�õĻ����½�ҳ��)
  ��    ��: pbyRec
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemPage::RecordHead *CMemPage::AppendRec(BYTE *pbyRec)
{
    /// �ҵ�һ�����м�¼��������һ���µ�ҳ��
    DWORD dwMemPos = 0;
    PageInfo *pPage = GetIdleRec(dwMemPos);
    if (!pPage)
    {
        pPage = NewPage(m_dwRecSize, m_dwPageDefSize);
        if (!pPage)
        {
            return NULL;
        }

        dwMemPos = 0;
    }

    /// ������¼
    BYTE *pbyRecStart = (BYTE *)(pPage + 1);
    RecordHead *pRecord = (RecordHead *)(pbyRecStart + dwMemPos * (sizeof(RecordHead) + m_dwRecSize));
    (void)memcpy(pRecord + 1, pbyRec, m_dwRecSize);

    /// �Ѽ�¼�ҽӵ�����ĩ
    DLL_INSERT_TAIL(&m_records, pRecord, m_field);
    pRecord->m_pPage = pPage;
    pRecord->m_dwMemPos = dwMemPos;

    /// ��¼����1
    pPage->m_dwRecCount++;
    m_dwRecCount++;

    return pRecord;
}

/*******************************************************
  �� �� ��: CMemPage::DeleteRec
  ��    ��: ɾ����¼
  ��    ��: dwRecPos
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMemPage::DeleteRec(RecordHead *pRecord)
{
    if (!pRecord)
    {
        return FAILURE;
    }
    
    PageInfo *pPage = pRecord->m_pPage;
    if (!pPage)
    {
        return FAILURE;
    }

    /// �Ѽ�¼������ɾ�����ѱ�ͷ���
    DLL_REMOVE(&m_records, pRecord, m_field);
    (void)memset(pRecord, 0, sizeof(RecordHead));

    /// ��ҳ���¼����1
    if (pPage->m_dwRecCount) pPage->m_dwRecCount--;
    if (m_dwRecCount) m_dwRecCount--;

    /// ���ҳ���¼��Ϊ0����Ҫɾ����ҳ��
    if (!pPage->m_dwRecCount)
    {
        DLL_REMOVE(&m_pages, pPage, m_field);
        DCOP_Free(pPage);
        pPage = NULL;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CMemPage::GetRealRec
  ��    ��: ��ȡָ����Ч��¼
  ��    ��: dwRecPos
            rdwMemPos
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemPage::PageInfo *CMemPage::GetRealRec(DWORD dwRecPos, DWORD &rdwMemPos)
{
    DWORD dwRecLoop = 0;

    PageInfo *pPageLoop = DLL_FIRST(&m_pages);
    while (pPageLoop)
    {
        PageInfo *pPage = pPageLoop;

        /// ��Ч��¼���ڱ�ҳ�ڣ���������
        if ((dwRecPos - dwRecLoop) < pPage->m_dwRecCount)
        {
            dwRecLoop += pPage->m_dwRecCount;
            pPageLoop = DLL_NEXT_LOOP(&m_pages, pPageLoop, m_field);
            continue;
        }

        /// ������ǰҳ������м�¼
        BYTE *pbyRecStart = (BYTE *)(pPage + 1);
        DWORD dwRecIdx = dwRecLoop;
        DWORD dwMemPos = 0;
        while (dwMemPos < pPage->m_dwMemCount)
        {
            /// ��¼ͷ���е�ҳ��ָ�벻Ϊ�ղ�����Ч��¼
            RecordHead *pRecord = (RecordHead *)(pbyRecStart + dwMemPos * (sizeof(RecordHead) + m_dwRecSize));
            if (pRecord->m_pPage)
            {
                /// �ҵ���Ӧ�ļ�¼��
                if (dwRecIdx == dwRecPos)
                {
                    rdwMemPos = dwMemPos;
                    return pPage;
                }

                dwRecIdx++;
            }

            dwMemPos++;
        }

        break;
    }

    return NULL;
}

/*******************************************************
  �� �� ��: CMemPage::GetIdleRec
  ��    ��: �ҵ�һ�����м�¼
  ��    ��: rdwMemPos
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemPage::PageInfo *CMemPage::GetIdleRec(DWORD &rdwMemPos)
{
    PageInfo *pPageLoop = DLL_FIRST(&m_pages);
    while (pPageLoop)
    {
        /// �������е�ҳ��
        PageInfo *pPage = pPageLoop;
        pPageLoop = DLL_NEXT_LOOP(&m_pages, pPageLoop, m_field);

        /// ����ڴ���������Ч��¼��ȣ�˵����ҳ��û�п���λ��
        if (pPage->m_dwMemCount == pPage->m_dwRecCount)
        {
            continue;
        }

        /// ��ǰ�п���ҳ���������ǰҳ������м�¼
        BYTE *pbyRecStart = (BYTE *)(pPage + 1);
        DWORD dwMemPos = 0;
        while (dwMemPos < pPage->m_dwMemCount)
        {
            /// ��¼ͷ���е�ҳ��ָ��Ϊ�ղ�����Ч��¼
            RecordHead *pRecord = (RecordHead *)(pbyRecStart + dwMemPos * (sizeof(RecordHead) + m_dwRecSize));
            if (!(pRecord->m_pPage))
            {
                rdwMemPos = dwMemPos;
                return pPage;
            }

            dwMemPos++;
        }
    }

    return NULL;
}

/*******************************************************
  �� �� ��: CMemPage::ClearPages
  ��    ��: �������ҳ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemPage::ClearPages()
{
    PageInfo *pPageLoop = DLL_FIRST(&m_pages);
    while (pPageLoop)
    {
        PageInfo *pPage = pPageLoop;
        pPageLoop = DLL_NEXT_LOOP(&m_pages, pPageLoop, m_field);

        DCOP_Free(pPage);
    }

    DLL_INIT(&m_pages);
    DLL_INIT(&m_records);
}

