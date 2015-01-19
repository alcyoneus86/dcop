/// -------------------------------------------------
/// tablestring.cpp : ����ַ���ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "string/tablestring.h"


/*******************************************************
  �� �� ��: CTableString::CTableString
  ��    ��: CTableString����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTableString::CTableString()
{
    m_colNode = 0;
    m_colCount = 0;

    m_rowNode = 0;
    m_rowCount = 0;

    m_dwColPos = 0;
    m_dwRowPos = 0;

    m_cpszAlign = 0;
}

/*******************************************************
  �� �� ��: CTableString::CTableString
  ��    ��: CTableString����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTableString::CTableString(DWORD dwColCount, DWORD dwRowCount, const char *cpszAlign)
{
    m_colNode = 0;
    m_colCount = 0;

    m_rowNode = 0;
    m_rowCount = 0;

    m_dwColPos = 0;
    m_dwRowPos = 0;

    m_cpszAlign = 0;

    (void)Init(dwColCount, dwRowCount);

    Indent(cpszAlign);
}

/*******************************************************
  �� �� ��: CTableString::~CTableString
  ��    ��: CTableString����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTableString::~CTableString()
{
    Clear();
}

/*******************************************************
  �� �� ��: CTableString::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTableString::Init(DWORD dwColCount, DWORD dwRowCount)
{
    if (!dwColCount || !dwRowCount)
    {
        return FAILURE;
    }

    DWORD *pColNode = (DWORD *)DCOP_Malloc(dwColCount * sizeof(DWORD));
    if (!pColNode)
    {
        return FAILURE;
    }

    (void)memset(pColNode, 0, dwColCount * sizeof(DWORD));

    CDString *pRowNode = new CDString[dwRowCount];
    if (!pRowNode)
    {
        DCOP_Free(pColNode);
        return FAILURE;
    }

    Clear();

    m_colNode = pColNode;
    m_rowNode = pRowNode;
    m_colCount = dwColCount;
    m_rowCount = dwRowCount;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTableString::operator <<
  ��    ��: �����ַ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTableString &CTableString::operator <<(const char *cpszStr)
{
    if ((m_dwColPos >= m_colCount) || 
        (m_dwRowPos >= m_rowCount))
    {
        return *this;
    }

    /// ��ȡ�п��
    DWORD dwStrLen = (DWORD)strlen(cpszStr) + 2;
    DWORD dwBlankLen = 2;
    if (!m_colNode[m_dwColPos])
    {
        m_colNode[m_dwColPos] = dwStrLen;
    }
    else if (dwStrLen > m_colNode[m_dwColPos])
    {
        ExpandCol(dwStrLen);
    }
    else
    {
        dwBlankLen = m_colNode[m_dwColPos] - dwStrLen + 2;
    }

    /// ��ʼ����
    if (!m_dwColPos && m_cpszAlign)
    {
        m_rowNode[m_dwRowPos] << m_cpszAlign;
    }

    /// ������Ϣ
    m_rowNode[m_dwRowPos] << cpszStr;

    /// �Ҳ�ո�
    if (dwBlankLen)
    {
        char *pszBlank = (char *)DCOP_Malloc(dwBlankLen + 1);
        if (!pszBlank)
        {
            return *this;
        }

        (void)memset(pszBlank, ' ', dwBlankLen);
        pszBlank[dwBlankLen] = '\0';

        m_rowNode[m_dwRowPos] << pszBlank;
        DCOP_Free(pszBlank);
    }

    /// β������
    m_dwColPos++;
    if (m_dwColPos >= m_colCount)
    {
        m_rowNode[m_dwRowPos] << "\r\n";
        m_dwRowPos++;
        m_dwColPos = 0;
    }

    return *this;
}

/*******************************************************
  �� �� ��: CTableString::Line
  ��    ��: ��ȡָ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDString *CTableString::Line(DWORD dwIdx) const
{
    if (!m_rowNode || !m_rowCount)
    {
        return NULL;
    }

    if (dwIdx >= m_rowCount)
    {
        return NULL;
    }

    return &(m_rowNode[dwIdx]);
}

/*******************************************************
  �� �� ��: CTableString::Clear
  ��    ��: ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTableString::Clear()
{
    if (m_colNode)
    {
        DCOP_Free(m_colNode);
        m_colNode = 0;
    }

    if (m_rowNode)
    {
        delete [] m_rowNode;
        m_rowNode = 0;
    }

    m_colCount = 0;
    m_rowCount = 0;
}

/*******************************************************
  �� �� ��: CTableString::ExpandCol
  ��    ��: ��չ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTableString::ExpandCol(DWORD dwStrLen)
{
    if (!m_colNode || !m_colCount || 
        !m_rowNode || !m_rowCount || 
        (m_dwColPos >= m_colCount) ||
        (m_dwRowPos >= m_rowCount))
    {
        return;
    }

    if (dwStrLen <= m_colNode[m_dwColPos])
    {
        return;
    }

    DWORD dwBlankLen = dwStrLen - m_colNode[m_dwColPos];
    char *pszBlank = (char *)DCOP_Malloc(dwBlankLen + 1);
    if (!pszBlank)
    {
        return;
    }

    (void)memset(pszBlank, ' ', dwBlankLen);
    pszBlank[dwBlankLen] = '\0';

    DWORD dwOffset = GetColOffset();
    for (DWORD i = 0; i < m_dwRowPos; ++i)
    {
        m_rowNode[i].Insert(dwOffset, pszBlank);
    }

    m_colNode[m_dwColPos] = dwStrLen;
    DCOP_Free(pszBlank);
}

/*******************************************************
  �� �� ��: CTableString::GetColOffset
  ��    ��: ��ȡ��ƫ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTableString::GetColOffset()
{
    if (!m_colNode || !m_colCount) return 0;

    DWORD dwOffset = (m_cpszAlign)? (DWORD)strlen(m_cpszAlign) : 0;
    DWORD dwCurCol = (m_dwColPos >= m_colCount)? (m_colCount - 1) : m_dwColPos;
    for (DWORD i = 0; i <= dwCurCol; ++i)
    {
        dwOffset += m_colNode[i];
    }

    return dwOffset;
}

/*******************************************************
  �� �� ��: CTableString::Show
  ��    ��: ��ʾ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTableString::Show(LOG_PRINT logPrint, LOG_PARA logPara, const char *cpszMargin, const char *cpszTitle) const
{
    if (!logPrint || !m_rowNode) return;

    char *pszMargin = 0;
    char *pszTitle = 0;
    DWORD dwFirstLineLen = m_rowNode[0].Length();
    if (dwFirstLineLen && (cpszMargin || cpszTitle))
    {
        if (cpszMargin)
        {
            pszMargin = (char *)DCOP_Malloc(dwFirstLineLen + 3);
            if (pszMargin)
            {
                (void)memset(pszMargin, *cpszMargin, dwFirstLineLen);
                pszMargin[dwFirstLineLen] = '\r';
                pszMargin[dwFirstLineLen + 1] = '\n';
                pszMargin[dwFirstLineLen + 2] = '\0';
            }
        }

        if (cpszTitle)
        {
            pszTitle = (char *)DCOP_Malloc(dwFirstLineLen + 3);
            if (pszTitle)
            {
                (void)memset(pszTitle, *cpszTitle, dwFirstLineLen);
                pszTitle[dwFirstLineLen] = '\r';
                pszTitle[dwFirstLineLen + 1] = '\n';
                pszTitle[dwFirstLineLen + 2] = '\0';
            }
        }
    }

    /// ��ʾ�ϱ߱���
    if (pszMargin) logPrint(pszMargin, logPara);

    /// ��ʾ��������
    for (DWORD i = 0; i < Count(); ++i)
    {
        /// ��ʾ����
        const char *cpszTmp = m_rowNode[i];
        if (!cpszTmp) continue;

        /// ��ʾ����
        logPrint(cpszTmp, logPara);
        if (!i && pszTitle) logPrint(pszTitle, logPara);
    }

    /// ��ʾ�±߱���
    if (pszMargin) logPrint(pszMargin, logPara);

    if (pszMargin) DCOP_Free(pszMargin);
    if (pszTitle) DCOP_Free(pszTitle);
}

