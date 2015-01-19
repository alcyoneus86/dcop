/// -------------------------------------------------
/// darray.cpp : ��̬����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "array/darray.h"


/*******************************************************
  �� �� ��: CDArray::CDArray
  ��    ��: CDArray����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDArray::CDArray()
{
    m_pBuffer = 0;
    m_dwBufMemCount = 0;
    m_dwBufMemStepCount = DARRAY_STEP_COUNT;
    m_dwNodeMaxCount = DARRAY_MAX_COUNT;
    m_dwNodeCount = 0;
    m_dwNodeSize = sizeof(DWORD);
    m_fnDelete = 0;
}

/*******************************************************
  �� �� ��: CDArray::CDArray
  ��    ��: CDArray����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDArray::CDArray(DWORD dwNodeSize, DWORD dwMaxCount)
{
    m_pBuffer = 0;
    m_dwBufMemCount = 0;
    m_dwBufMemStepCount = DARRAY_STEP_COUNT;
    m_dwNodeMaxCount = (dwMaxCount)? dwMaxCount : DARRAY_MAX_COUNT;
    m_dwNodeCount = 0;
    m_dwNodeSize = dwNodeSize;
    m_fnDelete = 0;
}

/*******************************************************
  �� �� ��: CDArray::~CDArray
  ��    ��: CDArray����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDArray::~CDArray()
{
    Clear();
}

/*******************************************************
  �� �� ��: CDArray::Append
  ��    ��: ��ӽڵ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDArray::Append(void *pNode, DWORD dwSize)
{
    bool bRc = Dalloc(1);
    if (!bRc)
    {
        return FAILURE;
    }

    void *pBufTmp = (char *)m_pBuffer + m_dwNodeCount * m_dwNodeSize;
    (void)memset(pBufTmp, 0, m_dwNodeSize);
    DWORD dwCopySize = (!dwSize || (dwSize >= m_dwNodeSize))? m_dwNodeSize : dwSize;
    if (pNode)
    {
        (void)memcpy(pBufTmp, pNode, dwCopySize);
    }

    m_dwNodeCount++;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDArray::Pos
  ��    ��: ��ȡָ��λ�õĽڵ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CDArray::Pos(DWORD index) const
{
    if (!m_pBuffer || (index >= m_dwNodeCount))
    {
        return NULL;
    }

    return (char *)m_pBuffer + index * m_dwNodeSize;
}

/*******************************************************
  �� �� ��: CDArray::Set
  ��    ��: ����ָ��λ�õĽڵ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDArray::Set(DWORD index, void *pNode, DWORD dwSize)
{
    if (!m_pBuffer || (index >= m_dwNodeCount))
    {
        bool bRc = Dalloc(index - m_dwNodeCount + 1);
        if (!bRc)
        {
            return FAILURE;
        }

        m_dwNodeCount = index - m_dwNodeCount + 1;
    }

    void *pBufTmp = (char *)m_pBuffer + index * m_dwNodeSize;
    (void)memset(pBufTmp, 0, m_dwNodeSize);
    DWORD dwCopySize = (!dwSize || (dwSize >= m_dwNodeSize))? m_dwNodeSize : dwSize;
    if (pNode)
    {
        (void)memcpy(pBufTmp, pNode, dwCopySize);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDArray::Find
  ��    ��: ���ҽڵ��λ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDArray::Find(void *pKey, COMPARE fnCompare)
{
    DWORD dwRc = (DWORD)(-1);
    if (!m_pBuffer)
    {
        return dwRc;
    }

    for (DWORD i = 0; i < m_dwNodeCount; ++i)
    {
        bool bFound = false;
        if (fnCompare)
        {
            bFound = (*fnCompare)(Pos(i), pKey);
        }
        else
        {
            bFound = (!memcmp(Pos(i), pKey, m_dwNodeSize))? true : false;
        }

        if (bFound)
        {
            dwRc = i;
            break;
        }
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CDArray::Clear
  ��    ��: �س�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDArray::Clear()
{
    if (!m_pBuffer)
        return;

    if (m_fnDelete)
    {
        for (DWORD i = 0; i < m_dwNodeCount; ++i)
        {
            void *pNode = Pos(i);
            if (!pNode) continue;

            (*m_fnDelete)(i, pNode);
        }
    }

    Dfree();
}

/*******************************************************
  �� �� ��: CDArray::Dalloc
  ��    ��: �����µĶ�̬�ڴ棬�ᱣ��֮ǰ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CDArray::Dalloc(DWORD dwNewNodeCount)
{
    /// �����ڴ��ҳ���������Ҫ��ֱ�ӷ��سɹ�
    if ( m_pBuffer && ((m_dwNodeCount + dwNewNodeCount) <= m_dwBufMemCount) )
    {
        return true;
    }

    /// ���볤��ȡ������������Ҫ�����ֵ
    DWORD dwTmpCount = m_dwBufMemCount + m_dwBufMemStepCount;
    if (dwTmpCount < (m_dwNodeCount + dwNewNodeCount))
    {
        dwTmpCount = m_dwNodeCount + dwNewNodeCount;
    }

    /// ���ܳ����趨����󳤶�
    if (dwTmpCount > m_dwNodeMaxCount)
    {
        if (m_dwNodeMaxCount < (m_dwNodeCount + dwNewNodeCount))
        {
            return false;
        }

        dwTmpCount = m_dwNodeMaxCount;
    }

    /// �޷�����ĳ��ȣ�����ʧ��
    if (!dwTmpCount)
    {
        return false;
    }

    /// �����ڴ棬ʧ�ܷ���
    int *pTmp = (int *)DCOP_Malloc(dwTmpCount * m_dwNodeSize);
    if (!pTmp)
    {
        return false;
    }

    /// �ڴ�����
    (void)memset(pTmp, 0, dwTmpCount * m_dwNodeSize);

    /// �����Ѿ����ڵ��ڴ浽ǰ��
    if (m_pBuffer)
    {
        (void)memcpy(pTmp, m_pBuffer, m_dwNodeCount * m_dwNodeSize);
        DCOP_Free(m_pBuffer);
    }

    /// ����µ��ڴ�ͳ���
    m_pBuffer = pTmp;
    m_dwBufMemCount = dwTmpCount;

    return true;
}

/*******************************************************
  �� �� ��: CDArray::Dfree
  ��    ��: �ͷŶ�̬�ڴ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDArray::Dfree()
{
    if (m_pBuffer)
    {
        DCOP_Free(m_pBuffer);
        m_pBuffer = 0;
    }

    m_dwBufMemCount = 0;
    m_dwNodeCount = 0;
}

