/// -------------------------------------------------
/// dstring.h : ��̬�ַ�������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _STRING_DSTRING_H_
#define _STRING_DSTRING_H_

#include "dcop.h"
#include "array/darray.h"


/////////////////////////////////////////////////////
///                ��̬�ַ���˵��
/// -------------------------------------------------
/// ��̬�ַ�����Ҫ���ڶ�̬�洢���ͷ�(char *)�����ַ���
/// �ķ�����װ���࣬Ϊ��ʹ�ü�࣬�����:
///     * ����sizeof(CDString)��sizeof(char *)��ȫһ��
///     * ��CDString�ĳ�Ա��char *�⣬�벻���������Ա
/// -------------------------------------------------
/// �ַ������� : ��CDArray��ϳ�Ϊ�ַ������飬CDArray��
/// ÿ���ڵ�ռ䣬����CDString�Ŀռ䣬����(char *)ָ��
/// �Ŀռ䡣
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// Buffer�ַ���
/// -------------------------------------------------
class CBufferString
{
public:
    CBufferString(void *pBuf, DWORD dwBufLen, const char *cszFormat = "%02x", DWORD dwSize = 2)
    {
        m_szStr = NULL;
        if (!pBuf || !dwBufLen) return;

        DWORD dwMemLen = dwBufLen * dwSize + 1;
        m_szStr = (char *)DCOP_Malloc(dwMemLen);
        if (!m_szStr) return;

        BYTE *pbyBuf = (BYTE *)pBuf;
        DWORD dwOffset = 0;
        for (DWORD i = 0; i < dwBufLen; ++i)
        {
            dwOffset += (DWORD)snprintf(m_szStr + dwOffset, 
                        dwMemLen - dwOffset, 
                        cszFormat, 
                        pbyBuf[i]);
        }
        m_szStr[dwMemLen - 1] = '\0';
    }
    ~CBufferString() {if (m_szStr) DCOP_Free(m_szStr); m_szStr = NULL;}

    operator const char *() { return m_szStr;}

private:
    CBufferString() {m_szStr = NULL;}
    char *m_szStr;
};


/// -------------------------------------------------
/// ��̬�ַ���
/// -------------------------------------------------
class CDString
{
public:
    /// �ַ���β��λ��
    static const DWORD TAIL = (DWORD)(-1);

    /// �����ַ�������
    static const DWORD WHOLE = (DWORD)(-1);

public:
    CDString();
    CDString(const char *cpszStr, DWORD dwLen = 0);
    CDString(const CDString &rThis);
    ~CDString();

    /// ��ȡ����
    DWORD Length();

    /// ���
    void Clear();

    /// �ָ��ַ���
    void Split(const char *chrList, CDArray &strList, bool bNeedSplitChar = true);

    /// �ü�
    void Trim(const char *chrList);
    void TrimLeft(const char *chrList, DWORD dwStartPos = 0);
    void TrimRight(const char *chrList);

    /// ����
    void Insert(DWORD dwStartPos, const char *cpszStr);

    /// �Ƴ�
    void Remove(DWORD dwStartPos, DWORD dwLen = 0);

    /// ����
    void Reverse();

    /// ����
    DWORD Find(const char *strSub);

    /// ���ļ��ж�ȡ
    DWORD LoadFile(const char *szFile, DWORD dwOffset = 0, DWORD dwCount = 0);

    /// ����
    void Set(const char *cpszStr, DWORD dwLen = 0)
    {
        if (m_pBuffer) DCOP_Free(m_pBuffer);
        m_pBuffer = Copy(cpszStr, dwLen);
    }

    /// ��ȡ
    char *Get()
    {
        return m_pBuffer;
    }

    /// ��ȡ
    char Get(DWORD dwIdx)
    {
        if (!m_pBuffer) return '\0';
        DWORD dwLen = (DWORD)strlen(m_pBuffer);
        if (!dwLen) return '\0';
        if (dwIdx >= dwLen) dwIdx = dwLen - 1;
        return m_pBuffer[dwIdx];
    }

    /// ��ֵ������
    CDString& operator = (const char *cpszStr)
    {
        Set(cpszStr);
        return *this;
    }

    CDString& operator = (const CDString &rThis)
    {
        *this = (const char *)rThis;
        return *this;
    }

    /// ת��������
    operator const char *() const
    {
        return m_pBuffer;
    }

    /// �ȺŲ�����
    bool operator == (const char *cpszStr)
    {
        if (!cpszStr || !m_pBuffer)
        {
            return (!cpszStr && !m_pBuffer)? true : false;
        }

        return (!strcmp(cpszStr, m_pBuffer))? true : false;
    }

    /// ���������
    CDString &operator <<(const char *cpszStr)
    {
        /// ������β��
        Insert(TAIL, cpszStr);
        return *this;
    }

    /// ��ӡ���ַ����Ļص�
    static void Print(const char *info, void *para)
    {
        CDString *pString = (CDString *)para;
        if (!pString) return;
        pString->Insert(TAIL, info);
    }

private:
    char *Copy(const char *cpszStr, DWORD dwLen = 0);
    char *Located(char *pszStr, const char *cpszChr);
    static void StrListFree(DWORD index, void *pNode);
    static void StrListFreeHead(DWORD index, void *pNode);

private:
    char *m_pBuffer;
};

#endif // #ifndef _STRING_DSTRING_H_

