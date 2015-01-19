/// -------------------------------------------------
/// ObjData_mem.cpp : �ڴ�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_mem.h"
#include "Factory_if.h"
#include "cpu/bytes.h"
#include "string/tablestring.h"
#include "sock.h"
#include "ObjTimer_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CDataMem, "DataMem")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CDataMem)
    DCOP_IMPLEMENT_INTERFACE(IDataHandle)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END


/*******************************************************
  �� �� ��: CDataMem::CDataMem
  ��    ��: CDataMem���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDataMem::CDataMem(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);

}

/*******************************************************
  �� �� ��: CDataMem::~CDataMem
  ��    ��: CDataMem��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDataMem::~CDataMem()
{

    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CDataMem::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::Init(DWORD dwAttrID, IObject *pOwner, IModel *piModel)
{
    /// �ȵ��ø���ĳ�ʼ��
    DWORD dwRc = IDataHandle::Init(IData::TYPE_MEM, dwAttrID, pOwner, piModel, (IData *)m_piParent);
    if (dwRc)
    {
        return dwRc;
    }

    /// ��ȡ���Ե�Ĭ�ϼ�¼��С
    DWORD dwDefRecCount = piModel->GetDefRecCount(dwAttrID);
    if (!dwDefRecCount)
    {
        dwDefRecCount = DCOP_DATA_DEF_REC_COUNT;
    }

    /// �½��ڴ�ҳ
    if (m_memPage.NewPage(GetRecSize(), dwDefRecCount) == NULL)
    {
        return FAILURE;
    }

    /// �����ؼ�������
    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    DCOP_PARA_NODE keyPara;
    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        if (!pFields[i].m_isKey)
            continue;

        keyPara.m_paraID = i + 1;
        keyPara.m_opCode = 0;
        keyPara.m_paraType = pFields[i].m_fieldType;
        keyPara.m_paraSize = pFields[i].m_fieldSize;
        (void)AddKeyIdx(&keyPara, 1);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMem::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDataMem::Dump(LOG_PRINT logPrint, LOG_PARA logPara)
{
    if (!logPrint)
    {
        return;
    }

    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return;
    }

    AutoObjLock(pObject);

    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return;
    }

    logPrint(STR_FORMAT("Mem Data Table: ['%s'], Records Count: %d \r\n", GetTableName(), GetRecCount()), logPara);
    CTableString tableStr(dwFieldCount, GetRecCount() + 1, "  ");

    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        tableStr << pFields[i].m_fieldName;
    }

    DCOP_START_TIME();

    for (CMemPage::RecordHead *pRecord = m_memPage.GetFirstRec();
        pRecord != NULL;
        pRecord = m_memPage.GetNextRec(pRecord))
    {
        BYTE *pbyRec = (BYTE *)(pRecord + 1);
        DWORD dwOffset = 0;
        for (DWORD i = 0; i < dwFieldCount; ++i)
        {
            switch (pFields[i].m_fieldType)
            {
                case IModel::FIELD_BYTE:
                    tableStr << STR_FORMAT("0x%02x", *(pbyRec + dwOffset));
                    break;
                case IModel::FIELD_WORD:
                    tableStr << STR_FORMAT("0x%04x", Bytes_GetWord(pbyRec + dwOffset));
                    break;
                case IModel::FIELD_DWORD:
                    tableStr << STR_FORMAT("0x%08lx", Bytes_GetDword(pbyRec + dwOffset));
                    break;
                case IModel::FIELD_CHAR:
                    tableStr << STR_FORMAT("%c", *(char *)(pbyRec + dwOffset));
                    break;
                case IModel::FIELD_SHORT:
                case IModel::FIELD_INTEGER:
                    tableStr << STR_FORMAT("%d", (int)Bytes_GetDwordValue(pbyRec + dwOffset, pFields[i].m_fieldSize));
                    break;
                case IModel::FIELD_IDENTIFY:
                case IModel::FIELD_NUMBER:
                    tableStr << STR_FORMAT("%d", Bytes_GetDwordValue(pbyRec + dwOffset, pFields[i].m_fieldSize));
                    break;
                case IModel::FIELD_STRING:
                {
                    CDString strTmp((char *)pbyRec + dwOffset, pFields[i].m_fieldSize);
                    tableStr << (const char *)strTmp;
                }
                    break;
                case IModel::FIELD_BUFFER:
                    tableStr << CBufferString(pbyRec + dwOffset, pFields[i].m_fieldSize);
                    break;
                case IModel::FIELD_DATE:
                    break;
                case IModel::FIELD_TIME:
                    break;
                case IModel::FIELD_IP:
                {
                    char szIP[OSSOCK_IPSIZE];
                    (void)memset(szIP, 0, sizeof(szIP));
                    objSock::GetIPStringByValue(*(DWORD *)(pbyRec + dwOffset), szIP);
                    tableStr << szIP;
                }
                    break;
                case IModel::FIELD_PTR:
                    tableStr << STR_FORMAT("%p", *(void **)(pbyRec + dwOffset));
                    break;
                case IModel::FIELD_TIMER:
                {
                    CDString strTimer;
                    ITimer::IWheel::GetString((ITimer::Handle)(pbyRec + dwOffset), strTimer);
                    tableStr << (const char *)strTimer;
                }
                    break;
                case IModel::FIELD_PASS:
                    tableStr << "********";
                    break;
                default:
                    break;
            }

            dwOffset += pFields[i].m_fieldSize;

        }
    }

    DCOP_END_TIME();

    tableStr.Show(logPrint, logPara, "=", "-");

    logPrint(STR_FORMAT("[cost time: %d ms] \r\n", DCOP_COST_TIME()), logPara);
}

/*******************************************************
  �� �� ��: CDataMem::AddRecord
  ��    ��: ���һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::AddRecord(DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    /// ��ȡ��������
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    /// ��ȡ�ֶ���Ϣ
    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    /// �����¼�ռ�
    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec)
    {
        return FAILURE;
    }

    /// ʹ����������ı�����
    AutoObjLock(pObject);

    /// ����������ֶ�
    DWORD dwRc = FillRecord(pFields, dwFieldCount, 
                        pbyRec, GetRecSize(), 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen);
    if (dwRc)
    {
        DCOP_Free(pbyRec);
        return FAILURE;
    }

    /// ��Ӽ�¼���ڴ�ҳ����
    CMemPage::RecordHead *pRecord = m_memPage.AppendRec(pbyRec);
    if (!pRecord)
    {
        DCOP_Free(pbyRec);
        return FAILURE;
    }

    /// ͬʱ�����������(ʹ���ڴ��¼ָ��)
    CRecIdx::CIdx idx(pRecord);
    dwRc = m_recIdx.OnAddRec(pFields, (BYTE *)(pRecord + 1), idx);
    if (dwRc)
    {
        DCOP_Free(pbyRec);
        (void)m_memPage.DeleteRec(pRecord);
        return FAILURE;
    }

    /// �����¼�
    if (ppEvtPara && pdwEvtParaCount && pEvtData)
    {
        DCOP_PARA_NODE *pEvtPara = 0;
        DWORD dwEvtParaCount = 0;
        pEvtPara = GetOutPara(pFields, dwFieldCount, dwEvtParaCount);
        if (!pEvtPara)
        {
            DCOP_Free(pbyRec);
            return SUCCESS;
        }

        *ppEvtPara = pEvtPara;
        *pdwEvtParaCount = dwEvtParaCount;

        pEvtData->SetNodeSize(GetRecSize());
        (void)pEvtData->Append(pbyRec);
    }

    DCOP_Free(pbyRec);
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMem::DelRecord
  ��    ��: ɾ��һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::DelRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    /// ��ȡ��������
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    /// ��ȡ�ֶ���Ϣ
    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    /// �����¼�ռ�
    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec)
    {
        return FAILURE;
    }

    /// ʹ����������ı�����
    AutoObjLock(pObject);

    /// ��ȷƥ�����(ͨ���ؼ�������)
    if (DCOP_CONDITION_ONE == byCond)
    {
        /// ���Ҽ�¼
        CRecIdx::CIdx idx = m_recIdx.FindRec(pCondPara, dwCondParaCount, pCondData, dwCondDataLen);
        CMemPage::RecordHead *pRecord = (CMemPage::RecordHead *)(void *)idx;
        if (!pRecord)
        {
            DCOP_Free(pbyRec);
            return FAILURE;
        }

        /// ��ȡ��¼
        (void)memcpy(pbyRec, (BYTE *)(pRecord + 1), GetRecSize());

        /// ɾ����¼
        DWORD dwRc = m_memPage.DeleteRec(pRecord);
        if (dwRc)
        {
            DCOP_Free(pbyRec);
            return dwRc;
        }

        /// ɾ������
        m_recIdx.OnDelRec(pFields, pbyRec);

        /// �����¼�
        if (ppEvtPara && pdwEvtParaCount && pEvtData)
        {
            DCOP_PARA_NODE *pEvtPara = 0;
            DWORD dwEvtParaCount = 0;
            pEvtPara = GetOutPara(pFields, dwFieldCount, dwEvtParaCount);
            if (!pEvtPara)
            {
                DCOP_Free(pbyRec);
                return SUCCESS;
            }

            *ppEvtPara = pEvtPara;
            *pdwEvtParaCount = dwEvtParaCount;

            pEvtData->SetNodeSize(GetRecSize());
            (void)pEvtData->Append(pbyRec);
        }

        DCOP_Free(pbyRec);
        return SUCCESS;
    }

    /// ģ��ƥ�����(��������ƥ��)
    DWORD dwDelCount = 0;
    if (pEvtData) pEvtData->SetNodeSize(GetRecSize());
    CMemPage::RecordHead *pRecord = m_memPage.GetFirstRec();
    while (pRecord != NULL)
    {
        CMemPage::RecordHead *pRecordTmp = pRecord;
        pRecord = m_memPage.GetNextRec(pRecord);

        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecordTmp + 1), byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
        if (!bRc) continue;

        /// ��ȡ��¼
        (void)memcpy(pbyRec, (BYTE *)(pRecordTmp + 1), GetRecSize());

        /// ɾ��ҳ���еļ�¼
        DWORD dwRc = m_memPage.DeleteRec(pRecordTmp);
        if (dwRc) continue;

        /// ͬʱ����ɾ������
        m_recIdx.OnDelRec(pFields, pbyRec);
        dwDelCount++;

        /// �����¼�
        if (pEvtData) (void)pEvtData->Append(pbyRec);
    }

    DCOP_Free(pbyRec);

    if (!dwDelCount)
    {
        return FAILURE;
    }

    /// �����¼�
    if (ppEvtPara && pdwEvtParaCount && pEvtData)
    {
        DCOP_PARA_NODE *pEvtPara = 0;
        DWORD dwEvtParaCount = 0;
        pEvtPara = GetOutPara(pFields, dwFieldCount, dwEvtParaCount);
        if (!pEvtPara)
        {
            pEvtData->Clear();
            return SUCCESS;
        }

        *ppEvtPara = pEvtPara;
        *pdwEvtParaCount = dwEvtParaCount;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMem::EditRecord
  ��    ��: �༭һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::EditRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    /// ��ȡ��������
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    /// ��ȡ�ֶ���Ϣ
    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    /// ��ȡ�¼�����
    DCOP_PARA_NODE *pEvtPara = 0;
    DWORD dwEvtParaCount = 0;
    void *pEvtRec = 0;
    DWORD dwEvtDataLen = 0;
    if (ppEvtPara && pdwEvtParaCount)
    {
        pEvtPara = GetOutPara(pFields, dwFieldCount, dwEvtParaCount, 
                        pReqPara, dwReqParaCount, &dwEvtDataLen, true);
        if (dwEvtDataLen)
        {
            pEvtRec = DCOP_Malloc(dwEvtDataLen);
        }
    }

    /// ʹ����������ı�����
    AutoObjLock(pObject);

    /// ��ȷƥ�����(ͨ���ؼ�������)
    if (DCOP_CONDITION_ONE == byCond)
    {
        CRecIdx::CIdx idx = m_recIdx.FindRec(pCondPara, dwCondParaCount, pCondData, dwCondDataLen);
        CMemPage::RecordHead *pRecord = (CMemPage::RecordHead *)(void *)idx;
        if (!pRecord) return FAILURE;

        /// ���¼�¼
        DWORD dwRc = UpdateRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), GetRecSize(), 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen);
        if (dwRc != SUCCESS)
        {
            if (pEvtRec) DCOP_Free(pEvtRec);
            if (pEvtPara) DCOP_Free(pEvtPara);
            return dwRc;
        }

        /// ������¼
        if (pEvtRec && pEvtPara && 
            (CopyRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), GetRecSize(), 
                        pEvtPara, dwEvtParaCount, 
                        pEvtRec, dwEvtDataLen) != SUCCESS))
        {
            DCOP_Free(pEvtRec);
            DCOP_Free(pEvtPara);
            return SUCCESS;
        }

        /// �����¼�
        if (ppEvtPara && pdwEvtParaCount)
        {
            *ppEvtPara = pEvtPara;
            *pdwEvtParaCount = dwEvtParaCount;
        }
        if (pEvtData)
        {
            pEvtData->SetNodeSize(GetRecSize());
            (void)pEvtData->Append(pEvtRec);
        }

        if (pEvtRec) DCOP_Free(pEvtRec);
        return SUCCESS;
    }

    /// ģ��ƥ�����(��������ƥ��)
    DWORD dwEditCount = 0;
    if (pEvtData) pEvtData->SetNodeSize(GetRecSize());
    for (CMemPage::RecordHead *pRecord = m_memPage.GetFirstRec();
        pRecord != NULL;
        pRecord = m_memPage.GetNextRec(pRecord))
    {
        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
        if (!bRc) continue;

        /// ���¼�¼
        DWORD dwRc = UpdateRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), GetRecSize(), 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen);
        if (dwRc != SUCCESS)
        {
            continue;
        }

        /// ������¼
        if (pEvtRec && pEvtPara && 
            (CopyRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), GetRecSize(), 
                        pEvtPara, dwEvtParaCount, 
                        pEvtRec, dwEvtDataLen) == SUCCESS))
        {
            (void)pEvtData->Append(pEvtRec);
        }

        dwEditCount++;
    }

    if (pEvtRec) DCOP_Free(pEvtRec);

    if (!dwEditCount)
    {
        if (pEvtPara) DCOP_Free(pEvtPara);
        return FAILURE;
    }

    if (ppEvtPara && pdwEvtParaCount)
    {
        *ppEvtPara = pEvtPara;
        *pdwEvtParaCount = dwEvtParaCount;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMem::QueryRecord
  ��    ��: ��ѯ������¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::QueryRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        DCOP_PARA_NODE *&rpRspPara, DWORD &rdwRspParaCount, 
                        CDArray &aRspData)
{
    /// ��ȡ��������
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    /// ��ȡ�ֶ���Ϣ
    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    /// ��ȡ��Ӧ����
    DWORD dwRspParaCount = 0;
    DWORD dwRspDataLen = 0;
    DCOP_PARA_NODE *pRspPara = GetOutPara(pFields, dwFieldCount, dwRspParaCount, 
                        pReqPara, dwReqParaCount, &dwRspDataLen);
    if (!pRspPara)
    {
        return FAILURE;
    }

    /// ��Ӧ���ݳ��ȴ���
    if (!dwRspDataLen)
    {
        DCOP_Free(pRspPara);
        return FAILURE;
    }

    /// ������Ӧ���ݿռ�
    void *pRspData = DCOP_Malloc(dwRspDataLen);
    if (!pRspData)
    {
        DCOP_Free(pRspPara);
        return FAILURE;
    }

    aRspData.SetNodeSize(dwRspDataLen);
    aRspData.SetMaxCount(m_memPage.GetRecCount());

    /// ʹ����������ı�����
    AutoObjLock(pObject);

    /// ��ȷƥ�����(ͨ���ؼ�������)
    if (DCOP_CONDITION_ONE == byCond)
    {
        CRecIdx::CIdx idx = m_recIdx.FindRec(pCondPara, dwCondParaCount, pCondData, dwCondDataLen);
        CMemPage::RecordHead *pRecord = (CMemPage::RecordHead *)(void *)idx;
        if (!pRecord)
        {
            DCOP_Free(pRspPara);
            DCOP_Free(pRspData);
            return FAILURE;
        }

        DWORD dwRc = CopyRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), GetRecSize(), 
                        pRspPara, dwRspParaCount, 
                        pRspData, dwRspDataLen);
        if (dwRc)
        {
            DCOP_Free(pRspPara);
            DCOP_Free(pRspData);
            return FAILURE;
        }

        dwRc = aRspData.Append(pRspData);
        DCOP_Free(pRspData);
        if (dwRc)
        {
            DCOP_Free(pRspPara);
            return FAILURE;
        }

        rpRspPara = pRspPara;
        rdwRspParaCount = dwRspParaCount;

        return SUCCESS;
    }

    /// ��ȡ��¼��
    DWORD dwReqOffset = 0;
    DWORD dwReqLimit = 0;
    GetOffsetAndLimit(dwReqOffset, dwReqLimit, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
    if (!dwReqLimit) dwReqLimit = 0xffffffff;

    /// ģ��ƥ�����(��������ƥ��)
    DWORD dwRspOffset = 0;
    DWORD dwRspLimit = 0;
    for (CMemPage::RecordHead *pRecord = m_memPage.GetFirstRec();
        pRecord != NULL;
        pRecord = m_memPage.GetNextRec(pRecord))
    {
        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
        if (!bRc) continue;

        /// ָ����¼��
        if ((dwRspOffset < dwReqOffset) || (dwRspLimit >= dwReqLimit))
        {
            dwRspOffset++;
            continue;
        }
        dwRspOffset++;
        dwRspLimit++;

        /// ��������ļ�¼
        DWORD dwRc = CopyRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), GetRecSize(), 
                        pRspPara, dwRspParaCount, 
                        pRspData, dwRspDataLen);
        if (dwRc) continue;

        (void)aRspData.Append(pRspData);
    }

    DCOP_Free(pRspData);
    if (!aRspData.Count())
    {
        DCOP_Free(pRspPara);
        return FAILURE;
    }

    rpRspPara = pRspPara;
    rdwRspParaCount = dwRspParaCount;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMem::CountRecord
  ��    ��: ͳ�Ƽ�¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::CountRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DWORD &rdwCount)
{
    /// ��ȡ��������
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    /// ��ȡ�ֶ���Ϣ
    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    /// ʹ����������ı�����
    AutoObjLock(pObject);

    /// ��ȷƥ�����(ͨ���ؼ�������)
    if (DCOP_CONDITION_ONE == byCond)
    {
        CRecIdx::CIdx idx = m_recIdx.FindRec(pCondPara, dwCondParaCount, pCondData, dwCondDataLen);
        CMemPage::RecordHead *pRecord = (CMemPage::RecordHead *)(void *)idx;
        rdwCount = (!pRecord)? 0 : 1;
        return SUCCESS;
    }

    /// ģ��ƥ�����(��������ƥ��)
    rdwCount = 0;
    for (CMemPage::RecordHead *pRecord = m_memPage.GetFirstRec();
        pRecord != NULL;
        pRecord = m_memPage.GetNextRec(pRecord))
    {
        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        (BYTE *)(pRecord + 1), byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
        if (!bRc) continue;

        rdwCount++;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMem::AddKeyIdx
  ��    ��: ��ӹؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::AddKeyIdx(DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    AutoObjLock(pObject);

    /// ����ӹؼ���
    DWORD dwRc = m_recIdx.AddKey(pIdxPara, dwIdxParaCount);
    if (dwRc)
    {
        return dwRc;
    }

    /// �ٹ�������
    for (CMemPage::RecordHead *pRecord = m_memPage.GetFirstRec();
        pRecord != NULL;
        pRecord = m_memPage.GetNextRec(pRecord))
    {
        /// ������ӵ�����
        CRecIdx::CIdx idx(pRecord);
        dwRc = m_recIdx.BldKeyIdx(pIdxPara, dwIdxParaCount, GetFields(), (BYTE *)(pRecord + 1), idx);
        if (dwRc)
        {
            (void)m_recIdx.DelKey(pIdxPara, dwIdxParaCount);
            return FAILURE;
        }
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMem::DelKeyIdx
  ��    ��: ɾ���ؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMem::DelKeyIdx(DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    AutoObjLock(pObject);

    return m_recIdx.DelKey(pIdxPara, dwIdxParaCount);
}

