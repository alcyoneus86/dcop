/// -------------------------------------------------
/// argcfg.cpp : ��������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "cfg/argcfg.h"
#include "err.h"


/*******************************************************
  �� �� ��: CArgCfgTable::CArgCfgTable
  ��    ��: CArgCfgTable���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CArgCfgTable::CArgCfgTable()
{
    m_logPrint = 0;
    m_logPara = 0;
    m_ppCfgItems = 0;
    m_dwCfgCount = 0;
}

/*******************************************************
  �� �� ��: CArgCfgTable::CArgCfgTable
  ��    ��: CArgCfgTable��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CArgCfgTable::~CArgCfgTable()
{
    m_logPrint = 0;
    m_logPara = 0;
    if (m_ppCfgItems)
    {
        DCOP_Free(m_ppCfgItems);
        m_ppCfgItems = 0;
    }
    m_dwCfgCount = 0;
}

/*******************************************************
  �� �� ��: CArgCfgTable::SetPrintFunc
  ��    ��: ���ô�ӡ����
  ��    ��: fnPrint
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CArgCfgTable::SetPrintFunc(LOG_PRINT logPrint, LOG_PARA logPara)
{
    m_logPrint = logPrint;
}

/*******************************************************
  �� �� ��: CArgCfgTable::Reg
  ��    ��: ע��������
  ��    ��: ppCfgItems
            dwCfgCount
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CArgCfgTable::Reg(IArgCfgItemBase **ppCfgItems, DWORD dwCfgCount)
{
    DWORD dwCfgCountTmp = m_dwCfgCount + dwCfgCount;
    IArgCfgItemBase **ppCfgItemsTmp = (IArgCfgItemBase **)DCOP_Malloc(dwCfgCountTmp * sizeof(IArgCfgItemBase *));
    if (!ppCfgItemsTmp)
    {
        return;
    }

    if (m_ppCfgItems)
    {
        (void)memcpy(ppCfgItemsTmp, m_ppCfgItems, m_dwCfgCount * sizeof(IArgCfgItemBase *));
        DCOP_Free(m_ppCfgItems);
        m_ppCfgItems = 0;
    }

    (void)memcpy((char *)ppCfgItemsTmp + m_dwCfgCount * sizeof(IArgCfgItemBase *), 
            ppCfgItems, dwCfgCount * sizeof(IArgCfgItemBase *));
    m_ppCfgItems = ppCfgItemsTmp;
    m_dwCfgCount = dwCfgCountTmp;
}

/*******************************************************
  �� �� ��: CArgCfgTable::Cfg
  ��    ��: �������
  ��    ��: argc
            argv
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CArgCfgTable::Cfg(int argc, char **argv, bool bIgnoreInvalidArg)
{
    if (!m_ppCfgItems || !m_dwCfgCount)
    {
        return FAILURE;
    }

    for (int iPos = 0; iPos < argc; ++iPos)
    {
        bool bCfg = false;
    
        /// �����������������
        for (DWORD i = 0; i < m_dwCfgCount; ++i)
        {
            if (!(m_ppCfgItems[i]))
            {
                continue;
            }

            int iRc = m_ppCfgItems[i]->Cfg(iPos, argc, argv);
            if (iRc < 0)
            {
                continue;
            }

            iPos += iRc;
            bCfg = true;
            break;
        }

        /// �������Ч�����������û��趨���ܺ�����Ч��������ôֻ�ܷ��ش�����
        if (!bIgnoreInvalidArg && !bCfg)
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CArgCfgTable::Help
  ��    ��: ��ʾ������Ϣ
  ��    ��: argc
            argv
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CArgCfgTable::Help(const char *szHelpTitle)
{
    if (!m_logPrint || !m_ppCfgItems || !m_dwCfgCount)
    {
        return;
    }

    m_logPrint(szHelpTitle, m_logPara);

    char szStr[84]; // һ������ӡ���ַ�(2���ո�+16��Name+1���ո�+64��Help+1��������)

    for (DWORD i = 0; i < m_dwCfgCount; ++i)
    {
        if (!(m_ppCfgItems[i]))
        {
            continue;
        }

        CArgCfgType *pCfgType = m_ppCfgItems[i]->GetCfgType();
        if (!pCfgType)
        {
            continue;
        }

        /// ���������ܶ�Ϊ��
        if (!(pCfgType->GetSimpleName()) && !(pCfgType->GetFullName()))
        {
            continue;
        }

        /// ���ü�����ȫ������Ϊ�յĻ����м�Ҫ��һ��"����"����
        DWORD dwSimpleNameLen = (pCfgType->GetSimpleName())? (DWORD)strlen(pCfgType->GetSimpleName()) : 0;
        DWORD dwFullNameLen = (pCfgType->GetFullName())? (DWORD)strlen(pCfgType->GetFullName()) : 0;

        if (!dwSimpleNameLen && !dwFullNameLen)
        {
            continue;
        }

        DWORD dwNameLen = 0;
        char *pszStrName = (char *)DCOP_Malloc(dwSimpleNameLen + 1 + dwFullNameLen + 1);
        if (dwSimpleNameLen)
        {
            dwNameLen += (DWORD)snprintf(pszStrName, dwSimpleNameLen + 1 + dwFullNameLen + 1 - dwNameLen, 
                                        "%s", pCfgType->GetSimpleName());
        }
        if (dwSimpleNameLen && dwFullNameLen)
        {
            dwNameLen += (DWORD)snprintf(pszStrName, dwSimpleNameLen + 1 + dwFullNameLen + 1 - dwNameLen, 
                                        ",");
        }
        if (dwFullNameLen)
        {
            dwNameLen += (DWORD)snprintf(pszStrName, dwSimpleNameLen + 1 + dwFullNameLen + 1 - dwNameLen, 
                                        "%s", pCfgType->GetFullName());
        }

        DWORD dwHelpLen = (pCfgType->GetHelpInfo())? (DWORD)strlen(pCfgType->GetHelpInfo()) : 0;

        /// ��ʽ�������ʾ������Ϣ
        DWORD dwPosName = 0;
        DWORD dwPosHelp = 0;
        while ((dwPosName < dwNameLen) || (dwPosHelp < dwHelpLen))
        {
            (void)memset(szStr, ' ', sizeof(szStr));
            szStr[sizeof(szStr) - 1] = '\0';

            /// ��ʽ����������ÿ�й̶�16��
            DWORD dwCpyLen = 0;
            if (dwPosName < dwNameLen)
            {
                dwCpyLen = ((dwNameLen - dwPosName) < 16)? (dwNameLen - dwPosName) : 16;
                (void)memcpy(szStr + 2, pszStrName, dwCpyLen);
                dwPosName += dwCpyLen;
            }

            /// ��ʽ��������Ϣ��ÿ�й̶�64��
            dwCpyLen = 0;
            if (dwPosHelp < dwHelpLen)
            {
                dwCpyLen = ((dwHelpLen - dwPosHelp) < 64)? (dwHelpLen - dwPosHelp) : 64;
                (void)memcpy(szStr + 19, pCfgType->GetHelpInfo(), dwCpyLen);
                dwPosHelp += dwCpyLen;
            }

            szStr[19 + dwCpyLen] = '\0';
            m_logPrint(szStr, m_logPara);
        }

        DCOP_Free(pszStrName);
    }
}

/*******************************************************
  �� �� ��: CArgCfgTable::Check
  ��    ��: ���������Ƿ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CArgCfgTable::Check()
{
    if (!m_ppCfgItems || !m_dwCfgCount)
    {
        return false;
    }

    for (DWORD i = 0; i < m_dwCfgCount; ++i)
    {
        if (!(m_ppCfgItems[i]))
        {
            continue;
        }

        CArgCfgType *pCfgType = m_ppCfgItems[i]->GetCfgType();
        if (!pCfgType)
        {
            continue;
        }

        if (pCfgType->bMust() && !(m_ppCfgItems[i]->bConfiged()))
        {
            return false;
        }
    }
    
    return true;
}

