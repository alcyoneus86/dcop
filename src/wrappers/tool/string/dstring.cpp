/// -------------------------------------------------
/// dstring.cpp : ��̬�ַ���ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "string/dstring.h"
#include "fs/file.h"


/*******************************************************
  �� �� ��: CDString::CDString
  ��    ��: CDString����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDString::CDString()
{
    m_pBuffer = 0;
}

/*******************************************************
  �� �� ��: CDString::CDString
  ��    ��: CDString����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDString::CDString(const char *cpszStr, DWORD dwLen)
{
    m_pBuffer = Copy(cpszStr, dwLen);
}

/*******************************************************
  �� �� ��: CDString::CDString
  ��    ��: CDString����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDString::CDString(const CDString &rThis)
{
    m_pBuffer = Copy((const char *)rThis);
}

/*******************************************************
  �� �� ��: CDString::~CDString
  ��    ��: CDString����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDString::~CDString()
{
    Clear();
}

/*******************************************************
  �� �� ��: CDString::Length
  ��    ��: ��ȡ�ַ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDString::Length()
{
    if (!m_pBuffer)
    {
        return 0;
    }

    return (DWORD)strlen(m_pBuffer);
}

/*******************************************************
  �� �� ��: CDString::Clear
  ��    ��: ���ַ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::Clear()
{
    if (m_pBuffer)
    {
        DCOP_Free(m_pBuffer);
        m_pBuffer = 0;
    }
}

/*******************************************************
  �� �� ��: CDString::Split
  ��    ��: �ַ����ָ���ַ�������
  ��    ��: chrList - ƥ����ַ��б�
  ��    ��: strList - �ָ���ַ����б�
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::Split(const char *chrList, CDArray &strList, bool bNeedSplitChar)
{
    char *pszStr = Copy(m_pBuffer);
    if (!pszStr) return;

    strList.Clear();
    strList.SetNodeSize(sizeof(CDString) + ((bNeedSplitChar)? sizeof(char) : 0));
    strList.SetNodeDelete(StrListFreeHead);

    while (*pszStr)
    {
        if (strList.Append(&pszStr) != SUCCESS)
        {
            break;
        }

        CDString *pNode = (CDString *)strList.Pos(strList.Count() - 1);
        if (!pNode)
        {
            break;
        }

        char *pSplitPos = Located(pszStr, chrList);
        if (!pSplitPos)
        {
            break;
        }

        /// �ѷָ���ַ�д�뵽������
        if (bNeedSplitChar) *(char *)(pNode + 1) = *pSplitPos;
        *pSplitPos = '\0';
        pszStr = pSplitPos + 1;
    }

    if (!strList.Count())
    {
        DCOP_Free(pszStr);
    }
}

/*******************************************************
  �� �� ��: CDString::Trim
  ��    ��: �ü������ַ�
  ��    ��: chrList - ƥ����ַ��б�
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::Trim(const char *chrList)
{
    TrimLeft(chrList);
    TrimRight(chrList);
}

/*******************************************************
  �� �� ��: CDString::TrimLeft
  ��    ��: �ü�����ַ�
  ��    ��: chrList - ƥ����ַ��б�
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::TrimLeft(const char *chrList, DWORD dwStartPos)
{
    if (!chrList || !m_pBuffer) return;

    DWORD dwListLen = (DWORD)strlen(chrList);
    if (!dwListLen) return;

    DWORD dwStrLen = (DWORD)strlen(m_pBuffer);
    if (!dwStrLen) return;

    /// ����������ƥ��
    DWORD dwStartIdx = dwStartPos;
    if (dwStartIdx >= dwStrLen)
    {
        return;
    }

    while (dwStartIdx < dwStrLen)
    {
        /// ����ƥ���б��е��ַ�
        bool bMatch = false;
        for (DWORD i = 0; i < dwListLen; ++i)
        {
            if (chrList[i] == m_pBuffer[dwStartIdx])
            {
                bMatch = true;
                break;
            }
        }

        /// ����б�ƥ�䵱ǰ�ַ������˳�
        if (!bMatch)
        {
            break;
        }

        /// ���ƥ��Ļ�����������б���
        ++dwStartIdx;
    }

    /// �������ƥ��Ļ�����ֱ���˳�
    if (dwStartIdx == dwStartPos)
    {
        return;
    }

    /// ���ȫ��ƥ��Ļ���������ַ���
    if (dwStartIdx >= dwStrLen)
    {
        m_pBuffer[0] = 0;
        return;
    }

    /// �ƶ�����಻ƥ����ַ������ʼ
    (void)memmove(m_pBuffer + dwStartPos, m_pBuffer + dwStartIdx, dwStrLen - dwStartIdx + 1);
}

/*******************************************************
  �� �� ��: CDString::TrimRight
  ��    ��: �ü��Ҳ��ַ�
  ��    ��: chrList - ƥ����ַ��б�
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::TrimRight(const char *chrList)
{
    if (!chrList || !m_pBuffer) return;

    DWORD dwListLen = (DWORD)strlen(chrList);
    if (!dwListLen) return;

    DWORD dwStrLen = (DWORD)strlen(m_pBuffer);
    if (!dwStrLen) return;

    /// �����Ҳ����ƥ��
    DWORD dwStartIdx = dwStrLen;
    while (dwStartIdx)
    {
        /// ����ƥ���б��е��ַ�
        bool bMatch = false;
        for (DWORD i = 0; i < dwListLen; ++i)
        {
            if (chrList[i] == m_pBuffer[dwStartIdx - 1])
            {
                bMatch = true;
                break;
            }
        }

        /// ����б�ƥ�䵱ǰ�ַ������˳�
        if (!bMatch)
        {
            break;
        }

        /// ���ƥ��Ļ�����������б���
        --dwStartIdx;
    }

    /// �������ƥ��Ļ�����ֱ���˳�
    if (dwStartIdx >= dwStrLen)
    {
        return;
    }

    /// �����Ҳ�ƥ����ַ�����β�����
    m_pBuffer[dwStartIdx] = '\0';
}

/*******************************************************
  �� �� ��: CDString::Insert
  ��    ��: �����ַ���
  ��    ��: dwStartPos  - ��ʼλ��
            cpszStr     - Դ��
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::Insert(DWORD dwStartPos, const char *cpszStr)
{
    if (!cpszStr) return;
    DWORD dwStrOldLen = (m_pBuffer)? (DWORD)strlen(m_pBuffer) : 0;
    DWORD dwStrNewLen = (DWORD)strlen(cpszStr);
    char *pszTmp = (char *)DCOP_Malloc(dwStrOldLen + dwStrNewLen + 1);
    if (!pszTmp)
    {
        return;
    }

    DWORD dwInsertPos = (dwStartPos >= dwStrOldLen)? dwStrOldLen : dwStartPos;
    DWORD dwLeftCount = dwStrOldLen - dwInsertPos;

    if (m_pBuffer)
    {
        if (dwInsertPos)
        {
            (void)memcpy(pszTmp, m_pBuffer, dwInsertPos);
        }
        if (dwLeftCount)
        {
            (void)memcpy(pszTmp + dwInsertPos + dwStrNewLen, m_pBuffer + dwInsertPos, dwLeftCount);
        }
        DCOP_Free(m_pBuffer);
    }

    (void)memcpy(pszTmp + dwInsertPos, cpszStr, dwStrNewLen);
    pszTmp[dwStrOldLen + dwStrNewLen] = '\0';
    m_pBuffer = pszTmp;
}

/*******************************************************
  �� �� ��: CDString::Remove
  ��    ��: �Ƴ��ַ���
  ��    ��: dwStartPos  - ��ʼλ��
            dwLen       - ָ�����ȣ�Ϊ0ʱ�򳬹�Դ������ʱָ��������
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::Remove(DWORD dwStartPos, DWORD dwLen)
{
    if (!m_pBuffer) return;

    DWORD dwStrLen = (DWORD)strlen(m_pBuffer);
    if (!dwStrLen) return;

    if (dwStartPos >= dwStrLen) return;

    /// ��ȡʣ��ĳ���
    DWORD dwLeftLen = dwStrLen - dwStartPos;
    if (!dwLen || (dwLen > dwLeftLen))
    {
        dwLen = dwLeftLen;
    }
    dwLeftLen -= dwLen;

    /// �����ʣ�೤�ȵĻ����򿽱���ǰ��
    if (dwLeftLen)
    {
        (void)memmove(m_pBuffer + dwStartPos, m_pBuffer + dwStartPos + dwLen, dwLeftLen);
    }

    m_pBuffer[dwStartPos + dwLeftLen] = '\0';
}

/*******************************************************
  �� �� ��: CDString::Reverse
  ��    ��: �����ַ�˳��
  ��    ��: strSub      - �Ӵ�
  ��    ��: 
  ��    ��: �ɹ�Ϊ0��ʼ������λ��; ����ΪTAIL
  �޸ļ�¼: 
 *******************************************************/
void CDString::Reverse()
{
    if (!m_pBuffer)
    {
        return;
    }

    /// ��ȡ�ַ�������
    DWORD dwStrLen = (DWORD)strlen(m_pBuffer);
    if (!dwStrLen)
    {
        return;
    }

    /// �����µĿռ�
    char *pszTmp = (char *)DCOP_Malloc(dwStrLen + 1);
    if (!pszTmp)
    {
        return;
    }

    for (DWORD i = 0; i < dwStrLen; ++i)
    {
        pszTmp[i] = m_pBuffer[dwStrLen - i - 1];
    }

    pszTmp[dwStrLen] = '\0';
    DCOP_Free(m_pBuffer);
    m_pBuffer = pszTmp;
}

/*******************************************************
  �� �� ��: CDString::Find
  ��    ��: �����Ӵ�
  ��    ��: strSub      - �Ӵ�
  ��    ��: 
  ��    ��: �ɹ�Ϊ0��ʼ������λ��; ����ΪTAIL
  �޸ļ�¼: 
 *******************************************************/
DWORD CDString::Find(const char *strSub)
{
    if (!m_pBuffer)
    {
        return TAIL;
    }

    const char *pcszSubStr = strstr(m_pBuffer, strSub);
    if (!pcszSubStr || (pcszSubStr < m_pBuffer))
    {
        return TAIL;
    }

    return (DWORD)(pcszSubStr - m_pBuffer);
}

/*******************************************************
  �� �� ��: CDString::LoadFile
  ��    ��: ���ļ��ж�ȡ
            (���Զ���ӽ�����'\0')
  ��    ��: szFile      - �ļ���
            dwOffset    - ƫ��
            dwCount     - ָ�����ȣ�Ϊ0ʱָƫ�ƺ�������ļ�����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDString::LoadFile(const char *szFile, DWORD dwOffset, DWORD dwCount)
{
    if (!szFile) return FAILURE;

    DWORD dwFileLen = DCOP_GetFileLen(szFile);
    if (!dwFileLen)
    {
        return FAILURE;
    }

    if (dwOffset >= dwFileLen)
    {
        return FAILURE;
    }

    DWORD dwReadLen = dwFileLen - dwOffset;
    if (dwCount && (dwReadLen > dwCount))
    {
        dwReadLen = dwCount;
    }

    char *pszBuf = (char *)DCOP_Malloc(dwReadLen + 1);
    if (!pszBuf)
    {
        return FAILURE;
    }

    DWORD dwRc = DCOP_RestoreFromFile(szFile, pszBuf, dwReadLen, dwOffset);
    if (dwRc != SUCCESS)
    {
        DCOP_Free(pszBuf);
        return dwRc;
    }

    pszBuf[dwReadLen] = '\0';
    if (m_pBuffer) DCOP_Free(m_pBuffer);
    m_pBuffer = pszBuf;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDString::Copy
  ��    ��: �����ַ���
  ��    ��: cpszStr     - Դ��
            dwLen       - ָ�����ȣ�Ϊ0ʱʹ��strlen��ȡ����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
char *CDString::Copy(const char *cpszStr, DWORD dwLen)
{
    if (!cpszStr)
    {
        return NULL;
    }

    DWORD dwStrLen = dwLen;
    if (!dwStrLen)
    {
        dwStrLen = (DWORD)strlen(cpszStr);
        if (!dwStrLen) return NULL;
    }

    char *pszTmp = (char *)DCOP_Malloc(dwStrLen + 1);
    if (!pszTmp)
    {
        return NULL;
    }

    (void)memcpy(pszTmp, cpszStr, dwStrLen);
    pszTmp[dwStrLen] = '\0';
    return pszTmp;
}

/*******************************************************
  �� �� ��: CDString::Located
  ��    ��: ��Ŀ���ַ�����ƥ��ָ�����ַ��б��������ַ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
char *CDString::Located(char *pszStr, const char *cpszChr)
{
    if (!pszStr || !cpszChr)
    {
        return NULL;
    }

    /// ѭ��ƥ�䣬���ж��ƥ��λ�ã�����������ƥ��λ��

    char *pszPos = NULL;
    DWORD dwStrLen = (DWORD)strlen(cpszChr);
    for (DWORD i = 0; i < dwStrLen; ++i)
    {
        char *pszPosTmp = strchr(pszStr, cpszChr[i]);
        if (!pszPosTmp)
        {
            continue;
        }

        if (!pszPos || (pszPosTmp < pszPos))
        {
            pszPos = pszPosTmp;
        }
    }

    return pszPos;
}

/*******************************************************
  �� �� ��: CDString::StrListFree
  ��    ��: �ַ��������ͷź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::StrListFree(DWORD index, void *pNode)
{
    if (!pNode) return;

    CDString *pStrObj = (CDString *)pNode;
    pStrObj->Clear();
}

/*******************************************************
  �� �� ��: CDString::StrListFreeHead
  ��    ��: �ַ��������ͷź���
            (�����������ַ�������һƬ�ռ䣬ֻ�ͷ�ͷ�ڵ���ַ����ռ�)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDString::StrListFreeHead(DWORD index, void *pNode)
{
    if (!index) StrListFree(index, pNode);
}

