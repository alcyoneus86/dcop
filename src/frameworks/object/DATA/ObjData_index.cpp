/// -------------------------------------------------
/// ObjData_index.cpp : ��¼����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_index.h"
#include "ObjData_main.h"


/*******************************************************
  �� �� ��: CRecIdx::CKey::CKey
  ��    ��: CKey���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CKey::CKey() : m_pKey(NULL), m_dwLen(0)
{
}

/*******************************************************
  �� �� ��: CRecIdx::CKey::CKey
  ��    ��: CKey���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CKey::CKey(DWORD dwLen) : m_pKey(NULL), m_dwLen(0)
{
    if (!dwLen) return;

    void *pKey = DCOP_Malloc(dwLen);
    if (!pKey) return;

    (void)memset(pKey, 0, dwLen);

    m_pKey = pKey;
    m_dwLen = dwLen;
}

/*******************************************************
  �� �� ��: CRecIdx::CKey::CKey
  ��    ��: CKey���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CKey::CKey(const void *cpKey, DWORD dwLen) : m_pKey(NULL), m_dwLen(0)
{
    if (!dwLen) return;

    void *pKey = DCOP_Malloc(dwLen);
    if (!pKey) return;

    if (!cpKey)
        (void)memset(pKey, 0, dwLen);
    else
        (void)memcpy(pKey, cpKey, dwLen);

    m_pKey = pKey;
    m_dwLen = dwLen;
}

/*******************************************************
  �� �� ��: CRecIdx::CKey::CKey
  ��    ��: CKey���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CKey::CKey(const CKey &rKey) : m_pKey(NULL), m_dwLen(0)
{
    *this = rKey;
}

/*******************************************************
  �� �� ��: CRecIdx::CKey::~CKey
  ��    ��: CKey��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CKey::~CKey()
{
    if (m_pKey)
    {
        DCOP_Free(m_pKey);
        m_pKey = NULL;
    }

    m_dwLen = 0;
}

/*******************************************************
  �� �� ��: operator=
  ��    ��: ��ֵ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CKey& CRecIdx::CKey::operator=(const void *cpKey)
{
    if (!m_dwLen)
    {
        return *this;
    }

    if (!m_pKey)
    {
        m_pKey = DCOP_Malloc(m_dwLen);
        if (!m_pKey)
        {
            return *this;
        }
    }

    (void)memcpy(m_pKey, cpKey, m_dwLen);

    return *this;
}

/*******************************************************
  �� �� ��: operator=
  ��    ��: ��ֵ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CKey& CRecIdx::CKey::operator=(const CRecIdx::CKey &rKey)
{
    DWORD dwLen = rKey.GetLen();
    if (!dwLen)
    {
        return *this;
    }

    void *pKey = DCOP_Malloc(dwLen);
    if (!pKey)
    {
        return *this;
    }

    void *pBuf = rKey.GetKey();
    if (!pBuf)
        (void)memset(pKey, 0, dwLen);
    else
        (void)memcpy(pKey, pBuf, dwLen);

    if (m_pKey) DCOP_Free(m_pKey);

    m_pKey = pKey;
    m_dwLen = dwLen;

    return *this;
}

/*******************************************************
  �� �� ��: operator<
  ��    ��: �Ƚϲ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CRecIdx::CKey::operator<(const CRecIdx::CKey &rKey) const
{
    if (!GetKey() || !rKey.GetKey())
    {
        return (GetKey() < rKey.GetKey())? true : false;
    }

    if (!GetLen() || !rKey.GetLen() || (GetLen() != rKey.GetLen()))
    {
        return (GetLen() < rKey.GetLen())? true : false;
    }

    if (memcmp(GetKey(), rKey.GetKey(), GetLen()) < 0)
    {
        return true;
    }

    return false;
}

/*******************************************************
  �� �� ��: operator==
  ��    ��: �Ƚϲ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CRecIdx::CKey::operator==(const CKey &rKey) const
{
    if (!GetKey() || !rKey.GetKey())
    {
        return (GetKey() == rKey.GetKey())? true : false;
    }

    if (!GetLen() || !rKey.GetLen())
    {
        return (GetLen() == rKey.GetLen())? true : false;
    }

    if (GetLen() != rKey.GetLen())
    {
        return false;
    }

    if (!memcmp(GetKey(), rKey.GetKey(), GetLen()))
    {
        return true;
    }

    return false;
}

/*******************************************************
  �� �� ��: CRecIdx::CIdx::CIdx
  ��    ��: CIdx���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CIdx::CIdx()
{
    m_idx.m_ptr = NULL;
}

/*******************************************************
  �� �� ��: CRecIdx::CIdx::CIdx
  ��    ��: CIdx���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CIdx::CIdx(void *ptr)
{
    m_idx.m_ptr = ptr;
}

/*******************************************************
  �� �� ��: CRecIdx::CIdx::CIdx
  ��    ��: CIdx���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CIdx::CIdx(DWORD pos)
{
    m_idx.m_pos = pos;
}

/*******************************************************
  �� �� ��: CRecIdx::CIdx::~CIdx
  ��    ��: CIdx��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CIdx::~CIdx()
{
    m_idx.m_ptr = NULL;
}

/*******************************************************
  �� �� ��: CRecIdx::CRecIdx
  ��    ��: CRecIdx���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CRecIdx()
{
}

/*******************************************************
  �� �� ��: CRecIdx::~CRecIdx
  ��    ��: CRecIdx��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::~CRecIdx()
{
}

/*******************************************************
  �� �� ��: CRecIdx::AddKey
  ��    ��: ��ӹؼ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CRecIdx::AddKey(DCOP_PARA_NODE *pPara, DWORD dwParaCount)
{
    if (!pPara || !dwParaCount) return FAILURE;

    CKey fieldKey(pPara, sizeof(DCOP_PARA_NODE) * dwParaCount);
    ClearType(fieldKey);

    IT_FLDIDX it_field = m_idx.find(fieldKey);
    if (it_field != m_idx.end())
    {
        return FAILURE;
    }

    MAP_RECIDX recordMap;
    (void)m_idx.insert(m_idx.end(), MAP_FLDIDX::value_type(fieldKey, recordMap));

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CRecIdx::DelKey
  ��    ��: ɾ���ؼ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CRecIdx::DelKey(DCOP_PARA_NODE *pPara, DWORD dwParaCount)
{
    if (!pPara || !dwParaCount) return FAILURE;

    CKey fieldKey(pPara, sizeof(DCOP_PARA_NODE) * dwParaCount);
    ClearType(fieldKey);

    IT_FLDIDX it_field = m_idx.find(fieldKey);
    if (it_field == m_idx.end())
    {
        return FAILURE;
    }

    (void)m_idx.erase(it_field);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CRecIdx::OnAddRec
  ��    ��: ���������¼�����Ĳ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CRecIdx::OnAddRec(IDataHandle::Field *pFields, BYTE *pbyRec, CIdx recordIdx)
{
    /// û��������ֱ�ӷ��سɹ�
    DWORD dwIdxCount = (DWORD)m_idx.size();
    if (!dwIdxCount)
    {
        return SUCCESS;
    }

    /// ׼�����Ƽ�¼�����Ŀռ�
    CKey *pRecordKey = new CKey[dwIdxCount];
    if (!pRecordKey)
    {
        return FAILURE;
    }

    /// �������������������Ƿ��ظ�
    DWORD dwIdxPos = 0;
    for (IT_FLDIDX it_field = m_idx.begin();
        it_field != m_idx.end(); ++it_field)
    {
        /// ��ȡ��������
        CKey fieldKey((*it_field).first);
        DCOP_PARA_NODE *pPara = (DCOP_PARA_NODE *)fieldKey.GetKey();
        DWORD dwParaCount = fieldKey.GetLen() / sizeof(DCOP_PARA_NODE);
        if (!pPara || !dwParaCount)
        {
            delete [] pRecordKey;
            return FAILURE;
        }

        /// ��ȡ���ݳ���
        DWORD dwDataLen = 0;
        for (DWORD i = 0; i < dwParaCount; ++i)
        {
            dwDataLen += pFields[pPara[i].m_paraID - 1].m_fieldSize;
        }
        if (!dwDataLen)
        {
            delete [] pRecordKey;
            return FAILURE;
        }

        /// ������������
        BYTE *pData = (BYTE *)DCOP_Malloc(dwDataLen);
        if (!pData)
        {
            delete [] pRecordKey;
            return FAILURE;
        }
        DWORD dwOffset = 0;
        for (DWORD i = 0; i < dwParaCount; ++i)
        {
            (void)memcpy(pData + dwOffset, pbyRec + pFields[pPara[i].m_paraID - 1].m_fieldOffset, 
                    pFields[pPara[i].m_paraID - 1].m_fieldSize);
            dwOffset += pFields[pPara[i].m_paraID - 1].m_fieldSize;
        }

        /// ���Ҽ�¼����
        CKey recordKey(pData, dwDataLen);
        DCOP_Free(pData);
        IT_RECIDX it_record = ((*it_field).second).find(recordKey);
        if (it_record != ((*it_field).second).end())
        {
            delete [] pRecordKey;
            return FAILURE;
        }

        pRecordKey[dwIdxPos++] = recordKey;
    }

    /// �����Ӽ�¼��������Ӧ���ֶ�����
    dwIdxPos = 0;
    for (IT_FLDIDX it_field = m_idx.begin();
        it_field != m_idx.end(); ++it_field)
    {
        (void)((*it_field).second).insert(MAP_RECIDX::value_type(pRecordKey[dwIdxPos++], recordIdx));
    }

    delete [] pRecordKey;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CRecIdx::OnDelRec
  ��    ��: ɾ��������¼�����Ĳ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CRecIdx::OnDelRec(IDataHandle::Field *pFields, BYTE *pbyRec)
{
    /// ��������
    for (IT_FLDIDX it_field = m_idx.begin();
        it_field != m_idx.end();
        ++it_field)
    {
        /// ��ȡ��������
        CKey fieldKey((*it_field).first);
        DCOP_PARA_NODE *pPara = (DCOP_PARA_NODE *)fieldKey.GetKey();
        DWORD dwParaCount = fieldKey.GetLen() / sizeof(DCOP_PARA_NODE);
        if (!pPara || !dwParaCount)
        {
            continue;
        }

        /// ��ȡ���ݳ���
        DWORD dwDataLen = 0;
        for (DWORD i = 0; i < dwParaCount; ++i)
        {
            dwDataLen += pFields[pPara[i].m_paraID - 1].m_fieldSize;
        }
        if (!dwDataLen)
        {
            continue;
        }

        /// ������������
        BYTE *pData = (BYTE *)DCOP_Malloc(dwDataLen);
        if (!pData) continue;
        DWORD dwOffset = 0;
        for (DWORD i = 0; i < dwParaCount; ++i)
        {
            (void)memcpy(pData + dwOffset, pbyRec + pFields[pPara[i].m_paraID - 1].m_fieldOffset, 
                    pFields[pPara[i].m_paraID - 1].m_fieldSize);
            dwOffset += pFields[pPara[i].m_paraID - 1].m_fieldSize;
        }

        /// ���Ҽ�¼����
        CKey recordKey(pData, dwDataLen);
        DCOP_Free(pData);
        IT_RECIDX it_record = ((*it_field).second).find(recordKey);
        if (it_record == ((*it_field).second).end())
        {
            continue;
        }

        (void)((*it_field).second).erase(it_record);
    }
}

/*******************************************************
  �� �� ��: CRecIdx::BldKeyIdx
  ��    ��: �����ؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CRecIdx::BldKeyIdx(DCOP_PARA_NODE *pPara, DWORD dwParaCount, 
                        IDataHandle::Field *pFields, BYTE *pbyRec, 
                        CIdx recordIdx)
{
    if (!pPara || !dwParaCount) return FAILURE;

    CKey fieldKey(pPara, sizeof(DCOP_PARA_NODE) * dwParaCount);
    ClearType(fieldKey);

    IT_FLDIDX it_field = m_idx.find(fieldKey);
    if (it_field == m_idx.end())
    {
        return FAILURE;
    }

    /// ��ȡ���ݳ���
    DWORD dwDataLen = 0;
    for (DWORD i = 0; i < dwParaCount; ++i)
    {
        dwDataLen += pFields[pPara[i].m_paraID - 1].m_fieldSize;
    }
    if (!dwDataLen)
    {
        return FAILURE;
    }

    /// ������������
    BYTE *pData = (BYTE *)DCOP_Malloc(dwDataLen);
    if (!pData) return FAILURE;
    DWORD dwOffset = 0;
    for (DWORD i = 0; i < dwParaCount; ++i)
    {
        (void)memcpy(pData + dwOffset, pbyRec + pFields[pPara[i].m_paraID - 1].m_fieldOffset, 
                pFields[pPara[i].m_paraID - 1].m_fieldSize);
        dwOffset += pFields[pPara[i].m_paraID - 1].m_fieldSize;
    }

    /// ���Ҽ�¼����
    CKey recordKey(pData, dwDataLen);
    DCOP_Free(pData);
    IT_RECIDX it_record = ((*it_field).second).find(recordKey);
    if (it_record != ((*it_field).second).end())
    {
        return FAILURE;
    }

    (void)((*it_field).second).insert(MAP_RECIDX::value_type(recordKey, recordIdx));
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CRecIdx::FindRec
  ��    ��: ���Ҽ�¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CRecIdx::CIdx CRecIdx::FindRec(DCOP_PARA_NODE *pPara, DWORD dwParaCount, 
                void *pData, DWORD dwDataLen)
{
    CKey fieldKey(pPara, sizeof(DCOP_PARA_NODE) * dwParaCount);
    ClearType(fieldKey);

    CKey recordKey(pData, dwDataLen);

    /// �����ֶ�����
    IT_FLDIDX it_field = m_idx.find(fieldKey);
    if (it_field == m_idx.end())
    {
        return CIdx();
    }

    /// ���Ҽ�¼����
    IT_RECIDX it_record = ((*it_field).second).find(recordKey);
    if (it_record == ((*it_field).second).end())
    {
        return CIdx();
    }

    return ((*it_record).second);
}

/*******************************************************
  �� �� ��: CRecIdx::ClearType
  ��    ��: ����ؼ������Ĳ���/����(ֻ���ֶ�ID�ʹ�С��������)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CRecIdx::ClearType(CKey &fieldKey)
{
    DCOP_PARA_NODE *pPara = (DCOP_PARA_NODE *)fieldKey.GetKey();
    if (!pPara)
    {
        return;
    }

    DWORD dwCount = fieldKey.GetLen() / sizeof(DCOP_PARA_NODE);
    if (!dwCount)
    {
        return;
    }

    for (DWORD i = 0; i < dwCount; ++i)
    {
        pPara[i].m_opCode = 0;
        pPara[i].m_paraType = 0;
    }
}

