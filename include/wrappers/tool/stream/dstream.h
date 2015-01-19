/// -------------------------------------------------
/// dstream.h : ��̬��������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_STREAM_DSTREAM_H_
#define _TOOL_STREAM_DSTREAM_H_

#include "dcop.h"
#include "cpu/bytes.h"
#include "string/dstring.h"
#include "fs/file.h"


/////////////////////////////////////////////////////
///                ��̬����3���÷�
/// -------------------------------------------------
/// 1. ��̬�ڴ�: ������ֵ�������볤�Ⱥ��ٰ�����(SetMemStepLen)�����µ��ڴ�
/// 2. ��̬�ڴ�: ��ָ������������Ҫ���ڴ�(SetMemNeedLen)���ɽ����ڴ��Ԥ����
/// 3. �ֶμ�¼: ����Ӽ�¼(AppendRecord)����һ����Ӽ�¼ʱ��ѵ�ǰ������(��
///    m_dwRecordCountΪ0ʱ��m_dwOffset��ֵ)����Ϊ������¼�ĳ��ȣ����������
///    ���л���ݼ�¼�ĳ����������ڴ档
/////////////////////////////////////////////////////


#define DSTREAM_FIELD_NAME_LEN      16          // �ֶ����ֳ���
#define DSTREAM_DMEM_MAX_LEN        65535       // �ڴ���󳤶�
#define DSTREAM_DMEM_STEP_LEN       32          // �ڴ��������


/// -------------------------------------------------
/// Buffer����
/// -------------------------------------------------
class CBufferPara
{
public:
    CBufferPara()
    {
        m_pBuf = 0;
        m_dwBufLen = 0;
        m_adwRul = 0;
        m_dwRuleCount = 0;
        m_bFree = false;
    }
    CBufferPara(void *pBuf, DWORD dwBufLen, 
                    const BYTES_CHANGE_RULE *adwRule = 0, 
                    DWORD dwRuleCount = 0)
    {
        m_pBuf = pBuf;
        m_dwBufLen = dwBufLen;
        m_adwRul = adwRule;
        m_dwRuleCount = dwRuleCount;
        m_bFree = false;
    }
    CBufferPara(const char *szStr, const char *cszFormat, DWORD dwSize)
    {
        m_pBuf = 0;
        m_dwBufLen = 0;
        m_adwRul = 0;
        m_dwRuleCount = 0;
        m_bFree = false;

        if (!szStr || !(*szStr)) return;

        DWORD dwStrLen = (DWORD)strlen(szStr);
        DWORD dwBufLen = (dwStrLen < dwSize)? 1 : (dwStrLen / dwSize);
        BYTE *pbyBuf = (BYTE *)DCOP_Malloc(dwBufLen);
        if (!pbyBuf) return;

        do
        {
            /// ���Ȳ���һ����λ��ֻ���һ���ֽڵ�0
            if (dwStrLen < dwSize)
            {
                *pbyBuf = 0;
                break;
            }

            /// �����㹻����ѭ������ת��
            DWORD dwOffset = 0;
            DWORD i = 0;
            while (dwOffset < dwStrLen)
            {
                int iRc = sscanf(szStr + dwOffset, cszFormat, &(pbyBuf[i++]));
                if (iRc <= 0) break;

                dwOffset += dwSize;
                if (i >= dwBufLen) break;
            }
        } while (0);

        m_bFree = true;
        m_pBuf = pbyBuf;
        m_dwBufLen = dwBufLen;
        
    }
    CBufferPara(const char *szFile, DWORD dwOffset = 0, DWORD dwCount = 0)
    {
        m_pBuf = 0;
        m_dwBufLen = 0;
        m_adwRul = 0;
        m_dwRuleCount = 0;
        m_bFree = false;

        if (!szFile) return;

        DWORD dwFileLen = DCOP_GetFileLen(szFile);
        if (!dwFileLen)
        {
            return;
        }

        if (dwOffset >= dwFileLen)
        {
            return;
        }

        DWORD dwReadLen = dwFileLen - dwOffset;
        if (dwCount && (dwReadLen > dwCount))
        {
            dwReadLen = dwCount;
        }

        BYTE *pbyBuf = (BYTE *)DCOP_Malloc(dwReadLen);
        if (!pbyBuf)
        {
            return;
        }

        DWORD dwRc = DCOP_RestoreFromFile(szFile, pbyBuf, dwReadLen, dwOffset);
        if (dwRc != SUCCESS)
        {
            DCOP_Free(pbyBuf);
            return;
        }

        m_bFree = true;
        m_pBuf = pbyBuf;
        m_dwBufLen = dwReadLen;

    }
    ~CBufferPara()
    {
        if (m_bFree) DCOP_Free(m_pBuf);
        m_bFree = false;
        m_pBuf = 0;
        m_dwBufLen = 0;
        m_adwRul = 0;
        m_dwRuleCount = 0;
    }

    void *GetBuf() const {return m_pBuf;}
    DWORD GetBufLen() const {return m_dwBufLen;}
    const BYTES_CHANGE_RULE *GetRule() const {return m_adwRul;}
    DWORD GetRuleCount() const {return m_dwRuleCount;}

private:
    void *m_pBuf;
    DWORD m_dwBufLen;
    const BYTES_CHANGE_RULE *m_adwRul;
    DWORD m_dwRuleCount;
    bool m_bFree;
};


/// -------------------------------------------------
/// ��̬��
/// -------------------------------------------------
class CDStream
{
public:
    CDStream();
    CDStream(DWORD dwNeedMemLen);
    ~CDStream();

    /// �����ڴ���󳤶ȡ��ڴ����������ָ�������������ʵʱ��Ҫһ���ڴ�
    CDStream &SetMemMaxLen(DWORD dwMemMaxLen) {m_dwMemMaxLen = (dwMemMaxLen)? dwMemMaxLen : DSTREAM_DMEM_MAX_LEN; return *this;}
    CDStream &SetMemStepLen(DWORD dwMemStepLen) {m_dwMemStepLen = (dwMemStepLen)? dwMemStepLen : DSTREAM_DMEM_STEP_LEN; return *this;}
    CDStream &SetInputStrTerminal(bool bInputStrTerminal) {m_bInputStrTerminal = bInputStrTerminal; return *this;}
    DWORD SetNeedMemLen(DWORD dwNeedMemLen) {return (Dalloc(dwNeedMemLen))? SUCCESS : FAILURE;}

    /// ����ƫ����Ϣ����ȡƫ����Ϣ������ƫ�ƻ�ȡ�����ַ����ȡ���ȡ����������
    void Clear();
    DWORD Remove(DWORD dwOffset, DWORD dwLength);
    CDStream &SetOffset(DWORD dwOffset) {m_dwOffset = (dwOffset > m_dwDataLen)? m_dwDataLen : dwOffset; return *this;}
    DWORD GetOffSet() const {return m_dwOffset;}
    void *Buffer(DWORD dwOffset = 0) const {return (dwOffset > m_dwDataLen)? ((BYTE *)m_pBuffer + m_dwDataLen) : ((BYTE *)m_pBuffer + dwOffset);}
    DWORD Length() const {return m_dwDataLen;}
    CDStream &Reset() {SetOffset(0); return *this;}

    /// ��Ӽ�¼(Ĭ����1����¼��,���Ե�һ�β��õ���)�����õ�ǰ��¼(��0��ʼ)����ȡ��¼����
    DWORD AppendRecord();
    CDStream &SetCurRecord(DWORD dwCurIdx);
    DWORD GetRecordCount() const {return m_dwRecordCount;}

    /// ���ļ��ж�ȡ
    DWORD LoadFile(const char *szFile, DWORD dwOffset = 0, DWORD dwCount = 0);

    /// ����
    CDStream &operator <<(BYTE byPara);
    CDStream &operator <<(WORD wPara);
    CDStream &operator <<(DWORD dwPara);
    CDStream &operator <<(const char *cpszPara);
    CDStream &operator <<(const CBufferPara &crbufPara);

    /// ���
    CDStream &operator >>(BYTE &rbyPara);
    CDStream &operator >>(WORD &rwPara);
    CDStream &operator >>(DWORD &rdwPara);
    CDStream &operator >>(CDString &rstrPara);
    CDStream &operator >>(CBufferPara &rbufPara);

    /// �������̬����
    void Output(CDArray &rArray);

    /// ת��Ϊ�ڴ�
    void *ToBuf();

    /// Dump
    void Dump(LOG_PRINT logPrint, LOG_PARA logPara = 0, DWORD recNo = 0);

private:
    /// ��̬����һ���ڴ桢�ͷŶ�̬�ڴ�
    bool Dalloc(DWORD dwNeedMemLen);
    void Dfree();

    /// ����д/�ɶ��ڴ�
    bool CheckWriteMem(DWORD dwWriteMemLen);
    bool CheckReadMem(DWORD dwReadMemLen);

    /// ����д��/����λ��
    void UpdateWritePos(DWORD dwWriteMemLen);
    void UpdateReadPos(DWORD dwReadMemLen);

private:
    void *m_pBuffer;                            // �ڴ��ַ
    DWORD m_dwMemLen;                           // �ڴ泤��
    DWORD m_dwDataLen;                          // ���ݳ���
    DWORD m_dwOffset;                           // ����ƫ��
    DWORD m_dwMemMaxLen;                        // �ڴ���󳤶�
    DWORD m_dwMemStepLen;                       // �ڴ��������
    DWORD m_dwRecordLen;                        // ��¼����
    DWORD m_dwRecordCount;                      // ��¼����
    DWORD m_dwRecordIndex;                      // ��¼��������
    bool m_bInputStrTerminal;                   // �Ƿ������ַ���������
};


#endif // #ifndef _TOOL_STREAM_DSTREAM_H_

