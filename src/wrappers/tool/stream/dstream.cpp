/// -------------------------------------------------
/// dstream.h : ��̬��������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "stream/dstream.h"
#include "os.h"
#include <memory.h>
#include <ctype.h>


/*******************************************************
  �� �� ��: CDStream::CDStream
  ��    ��: CDStream����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream::CDStream()
{
    m_pBuffer = 0;
    m_dwMemLen = 0;
    m_dwDataLen = 0;
    m_dwOffset = 0;

    m_dwMemMaxLen = DSTREAM_DMEM_MAX_LEN;
    m_dwMemStepLen = DSTREAM_DMEM_STEP_LEN;

    m_dwRecordLen = 0;
    m_dwRecordCount = 0;
    m_dwRecordIndex = 0;

    m_bInputStrTerminal = false;
}

/*******************************************************
  �� �� ��: CDStream::CDStream
  ��    ��: CDStream����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream::CDStream(DWORD dwNeedMemLen)
{
    m_pBuffer = 0;
    m_dwMemLen = 0;
    m_dwDataLen = 0;
    m_dwOffset = 0;

    m_dwMemMaxLen = DSTREAM_DMEM_MAX_LEN;
    m_dwMemStepLen = DSTREAM_DMEM_STEP_LEN;

    m_dwRecordLen = 0;
    m_dwRecordCount = 0;
    m_dwRecordIndex = 0;

    m_bInputStrTerminal = false;

    if (dwNeedMemLen) (void)SetNeedMemLen(dwNeedMemLen);
}

/*******************************************************
  �� �� ��: CDStream::~CDStream
  ��    ��: CDStream����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream::~CDStream()
{
    Dfree();
}

/*******************************************************
  �� �� ��: CDStream::Clear
  ��    ��: ���������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDStream::Clear()
{
    Dfree();

    m_dwMemMaxLen = DSTREAM_DMEM_MAX_LEN;

    m_dwRecordLen = 0;
    m_dwRecordCount = 0;
    m_dwRecordIndex = 0;
}

/*******************************************************
  �� �� ��: CDStream::Remove
  ��    ��: ��ָ��λ��ɾ��ָ�����ȵ��ַ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDStream::Remove(DWORD dwOffset, DWORD dwLength)
{
    if (!m_pBuffer || !m_dwDataLen)
    {
        return FAILURE;
    }

    /// ���ƫ�Ƴ������ݳ��ȣ������Ϊ���һ���ֽ�
    if (dwOffset >= m_dwDataLen)
    {
        dwOffset = m_dwDataLen - 1;
    }

    /// ���ƫ��+���ȳ������ݳ��ȣ������Ϊ�������ݳ���
    if ((dwOffset + dwLength) > m_dwDataLen)
    {
        dwLength = m_dwDataLen - dwOffset;
    }

    /// ��ȡβ�����������ݳ��ȣ���ǰ�����ƶ�
    DWORD dwLeftLen = m_dwDataLen - (dwOffset + dwLength);
    if (dwLeftLen)
    {
        (void)memmove((BYTE *)m_pBuffer + dwOffset, 
                        (BYTE *)m_pBuffer + dwOffset + dwLength, 
                        dwLeftLen);
        (void)memset((BYTE *)m_pBuffer + dwOffset + dwLeftLen, 
                        0, 
                        dwLength);
    }

    m_dwDataLen = dwOffset + dwLeftLen;
    if (m_dwOffset > m_dwDataLen)
    {
        m_dwOffset = m_dwDataLen;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDStream::AppendRecord
  ��    ��: ��Ӽ�¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDStream::AppendRecord()
{
    if (!m_dwRecordCount)
    {
        return FAILURE;
    }

    /// ��һ����Ӽ�¼ʱ���ѵ�ǰƫ������Ϊ��¼����
    if (m_dwRecordCount == 1)
    {
        m_dwRecordLen = m_dwOffset;
    }

    if (!m_dwRecordLen)
    {
        return FAILURE;
    }

    /// �����µļ�¼���ڴ泤�ȣ�����������ܳ����µļ�¼����
    DWORD dwRc = SetNeedMemLen(m_dwRecordLen);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    (void)SetMemMaxLen((m_dwRecordCount + 1) * m_dwRecordLen);

    m_dwRecordIndex = m_dwRecordCount;
    m_dwOffset = m_dwRecordIndex * m_dwRecordLen;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDStream::SetCurRecord
  ��    ��: ���ü�¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::SetCurRecord(DWORD dwCurIdx)
{
    if (dwCurIdx >= m_dwRecordCount)
    {
        dwCurIdx = m_dwRecordCount - 1;
    }

    m_dwRecordIndex = dwCurIdx;
    m_dwOffset = m_dwRecordIndex * m_dwRecordLen;

    return *this;
}

/*******************************************************
  �� �� ��: CDStream::Dalloc
  ��    ��: ���붯̬�ڴ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CDStream::Dalloc(DWORD dwNeedMemLen)
{
    /// �����ڴ��ҳ���������Ҫ��ֱ�ӷ��سɹ�
    if ( m_pBuffer && ((m_dwOffset + dwNeedMemLen) <= m_dwMemLen) )
    {
        return true;
    }

    /// ���볤��ȡ������������Ҫ�����ֵ
    DWORD dwTmpCount = m_dwMemLen + m_dwMemStepLen;
    if (dwTmpCount < (m_dwOffset + dwNeedMemLen))
    {
        dwTmpCount = m_dwOffset + dwNeedMemLen;
    }

    /// ���ܳ����趨����󳤶�
    if (dwTmpCount > m_dwMemMaxLen)
    {
        if (m_dwMemMaxLen < (m_dwOffset + dwNeedMemLen))
        {
            return false;
        }

        dwTmpCount = m_dwMemMaxLen;
    }

    /// �޷�����ĳ��ȣ�����ʧ��
    if (!dwTmpCount)
    {
        return false;
    }

    /// �����ڴ棬ʧ�ܷ���
    int *pTmp = (int *)DCOP_Malloc(dwTmpCount);
    if (!pTmp)
    {
        return false;
    }

    /// �ڴ�����
    (void)memset(pTmp, 0, dwTmpCount);

    /// �����Ѿ����ڵ��ڴ浽ǰ��
    if (m_pBuffer)
    {
        (void)memcpy(pTmp, m_pBuffer, m_dwOffset);
        DCOP_Free(m_pBuffer);
    }

    /// ����µ��ڴ�ͳ���
    m_pBuffer = pTmp;
    m_dwMemLen = dwTmpCount;

    return true;
}

/*******************************************************
  �� �� ��: CDStream::Dfree
  ��    ��: �ͷŶ�̬�ڴ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDStream::Dfree()
{
    if (m_pBuffer)
    {
        free(m_pBuffer);
        m_pBuffer = 0;
    }

    m_dwMemLen = 0;
    m_dwDataLen = 0;
    m_dwOffset = 0;
}

/*******************************************************
  �� �� ��: CDStream::CheckWriteMem
  ��    ��: ���д�ڴ泤��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CDStream::CheckWriteMem(DWORD dwWriteMemLen)
{
    /// ֱ��ʹ�ö�̬�ڴ棬������Զ�����
    return Dalloc(dwWriteMemLen);
}

/*******************************************************
  �� �� ��: CDStream::CheckReadMem
  ��    ��: �����ڴ泤��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CDStream::CheckReadMem(DWORD dwReadMemLen)
{
    if (!m_pBuffer)
    {
        return false;
    }

    /// �����Ȳ��ܳ������ݳ���
    if ((dwReadMemLen + m_dwOffset) > m_dwDataLen)
    {
        return false;
    }

    return true;
}

/*******************************************************
  �� �� ��: CDStream::UpdateWritePos
  ��    ��: ����д��λ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDStream::UpdateWritePos(DWORD dwWriteMemLen)
{
    /// ����д��ƫ��
    m_dwOffset += dwWriteMemLen;

    /// ���д�󳬹����ݳ��ȣ��������ݳ���
    if (m_dwOffset > m_dwDataLen)
    {
        m_dwDataLen = m_dwOffset;
    }

    /// ������µļ�¼��ʼ����¼������һ
    if (m_dwRecordIndex == m_dwRecordCount)
    {
        m_dwRecordCount++;
    }
}

/*******************************************************
  �� �� ��: CDStream::UpdateReadPos
  ��    ��: ���¶���λ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDStream::UpdateReadPos(DWORD dwReadMemLen)
{
    /// ���¶���ƫ��
    m_dwOffset += dwReadMemLen;
}


/*******************************************************
  �� �� ��: CDStream::LoadFile
  ��    ��: ���ļ��ж�ȡ
  ��    ��: szFile      - �ļ���
            dwOffset    - ƫ��
            dwLen       - ָ�����ȣ�Ϊ0ʱָƫ�ƺ�������ļ�����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDStream::LoadFile(const char *szFile, DWORD dwOffset, DWORD dwCount)
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

    BYTE *pbyBuf = (BYTE *)DCOP_Malloc(dwReadLen);
    if (!pbyBuf)
    {
        return FAILURE;
    }

    DWORD dwRc = DCOP_RestoreFromFile(szFile, pbyBuf, dwReadLen, dwOffset);
    if (dwRc != SUCCESS)
    {
        DCOP_Free(pbyBuf);
        return dwRc;
    }

    if (m_pBuffer) DCOP_Free(m_pBuffer);
    m_pBuffer = pbyBuf;
    m_dwMemLen = dwReadLen;
    m_dwDataLen = dwReadLen;
    m_dwOffset = dwReadLen;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDStream::operator <<
  ��    ��: ����BYTE
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator <<(BYTE byPara)
{
    if (!CheckWriteMem(sizeof(BYTE)))
    {
        return *this;
    }

    *((BYTE *)m_pBuffer + m_dwOffset) = byPara;

    UpdateWritePos(sizeof(BYTE));
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator <<
  ��    ��: ����WORD
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator <<(WORD wPara)
{
    if (!CheckWriteMem(sizeof(WORD)))
    {
        return *this;
    }

    Bytes_SetWord((BYTE *)m_pBuffer + m_dwOffset, wPara);

    UpdateWritePos(sizeof(WORD));
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator <<
  ��    ��: ����DWORD
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator <<(DWORD dwPara)
{
    if (!CheckWriteMem(sizeof(DWORD)))
    {
        return *this;
    }

    Bytes_SetDword((BYTE *)m_pBuffer + m_dwOffset, dwPara);

    UpdateWritePos(sizeof(DWORD));
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator <<
  ��    ��: �����ַ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator <<(const char *cpszPara)
{
    if (!cpszPara) return *this;

    DWORD dwStrLen = (DWORD)strlen(cpszPara);
    if (!dwStrLen)
    {
        return *this;
    }

    /// ���ָ���˰���������������Ͻ���������
    if (m_bInputStrTerminal)
    {
        dwStrLen++;
    }

    if (!CheckWriteMem(dwStrLen))
    {
        return *this;
    }

    (void)memcpy((char *)m_pBuffer + m_dwOffset, cpszPara, dwStrLen);

    UpdateWritePos(dwStrLen);
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator <<
  ��    ��: ����Buffer
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator <<(const CBufferPara &crbufPara)
{
    void *pBuf = crbufPara.GetBuf();
    DWORD dwBufLen = crbufPara.GetBufLen();
    if (!pBuf || !dwBufLen)
    {
        return *this;
    }

    if (!CheckWriteMem(dwBufLen))
    {
        return *this;
    }

    (void)memcpy((BYTE *)m_pBuffer + m_dwOffset, pBuf, dwBufLen);
    Bytes_ChangeOrderByRule(crbufPara.GetRule(), 
                        crbufPara.GetRuleCount(), 
                        (BYTE *)m_pBuffer + m_dwOffset, 
                        dwBufLen);

    UpdateWritePos(dwBufLen);
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator >>
  ��    ��: ���BYTE
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator >>(BYTE &rbyPara)
{
    if (!CheckReadMem(sizeof(BYTE)))
    {
        return *this;
    }

    rbyPara = *((BYTE *)m_pBuffer + m_dwOffset);

    UpdateReadPos(sizeof(BYTE));
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator >>
  ��    ��: ���WORD
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator >>(WORD &rwPara)
{
    if (!CheckReadMem(sizeof(WORD)))
    {
        return *this;
    }

    rwPara = Bytes_GetWord((BYTE *)m_pBuffer + m_dwOffset);

    UpdateReadPos(sizeof(WORD));
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator >>
  ��    ��: ���DWORD
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator >>(DWORD &rdwPara)
{
    if (!CheckReadMem(sizeof(DWORD)))
    {
        return *this;
    }

    rdwPara = Bytes_GetDword((BYTE *)m_pBuffer + m_dwOffset);

    UpdateReadPos(sizeof(DWORD));
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator >>
  ��    ��: ����ַ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator >>(CDString &rstrPara)
{
    rstrPara.Clear();
    if (!CheckReadMem(sizeof(char)))
    {
        return *this;
    }

    rstrPara.Set((char *)m_pBuffer + m_dwOffset, m_dwDataLen - m_dwOffset);

    UpdateReadPos(rstrPara.Length() + 1);
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::operator >>
  ��    ��: ���Buffer
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDStream &CDStream::operator >>(CBufferPara &rbufPara)
{
    void *pBuf = rbufPara.GetBuf();
    DWORD dwBufLen = rbufPara.GetBufLen();
    if (!pBuf || !dwBufLen)
    {
        return *this;
    }

    if (!CheckReadMem(dwBufLen))
    {
        return *this;
    }

    (void)memcpy(pBuf, (BYTE *)m_pBuffer + m_dwOffset, dwBufLen);
    Bytes_ChangeOrderByRule(rbufPara.GetRule(), 
                        rbufPara.GetRuleCount(), 
                        pBuf, 
                        dwBufLen);

    UpdateReadPos(dwBufLen);
    return *this;
}

/*******************************************************
  �� �� ��: CDStream::Output
  ��    ��: �������̬����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDStream::Output(CDArray &rArray)
{
    rArray.Clear();
    rArray.SetNodeSize(m_dwRecordLen);

    for (DWORD i = 0; i < m_dwRecordCount; ++i)
    {
        (void)rArray.Append((BYTE *)m_pBuffer + i * m_dwRecordLen);
    }
}

/*******************************************************
  �� �� ��: CDStream::ToBuf
  ��    ��: ת��Ϊ�ڴ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CDStream::ToBuf()
{
    void *pBuf = Buffer();
    DWORD dwLen = Length();

    if (!pBuf || !dwLen)
    {
        return NULL;
    }

    void *pTmp = DCOP_Malloc(dwLen);
    if (!pTmp)
    {
        return NULL;
    }

    (void)memcpy(pTmp, pBuf, dwLen);
    return pTmp;
}

/*******************************************************
  �� �� ��: CDStream::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDStream::Dump(LOG_PRINT logPrint, LOG_PARA logPara, DWORD recNo)
{
    if (!logPrint) return;

    logPrint("-----------------------------------------------\r\n", logPara);
    logPrint("Stream Dump: \r\n", logPara);

    for (DWORD i = 0; i < m_dwRecordCount; ++i)
    {
        if (recNo && (recNo != (i + 1))) continue;

        PrintBuffer(STR_FORMAT("<Record No:%d> \r\n", i + 1), 
                        (BYTE *)m_pBuffer + i * m_dwRecordLen, m_dwRecordLen, 
                        logPrint, logPara);
    }

    logPrint(STR_FORMAT("[Record Count: %d] \r\n", m_dwRecordCount), logPara);
    logPrint("-----------------------------------------------\r\n", logPara);
}

