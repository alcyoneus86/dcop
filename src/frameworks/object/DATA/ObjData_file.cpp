/// -------------------------------------------------
/// ObjData_file.cpp : �ļ�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_file.h"
#include "Factory_if.h"
#include "fs/file.h"
#include "string/tablestring.h"
#include "sock.h"
#include "ObjTimer_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CDataFile, "DataFile")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CDataFile)
    DCOP_IMPLEMENT_INTERFACE(IDataHandle)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END


/*******************************************************
  �� �� ��: CDataFile::BytesChangeFileHead
  ��    ��: �ļ�ͷ�ֽ���ת��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDataFile::BytesChangeFileHead(FileHead *pFileHead)
{
    const BYTES_CHANGE_RULE FileHeadBORule[] = 
    {
        {SIZE_OF(CDataFile::FileHead, m_headerSize), OFFSET_OF(CDataFile::FileHead, m_headerSize)},
        {SIZE_OF(CDataFile::FileHead, m_objectID), OFFSET_OF(CDataFile::FileHead, m_objectID)},
        {SIZE_OF(CDataFile::FileHead, m_attrID), OFFSET_OF(CDataFile::FileHead, m_attrID)},
        {SIZE_OF(CDataFile::FileHead, m_fieldCount), OFFSET_OF(CDataFile::FileHead, m_fieldCount)},
        {SIZE_OF(CDataFile::FileHead, m_recordCount), OFFSET_OF(CDataFile::FileHead, m_recordCount)},
        {SIZE_OF(CDataFile::FileHead, m_totalCount), OFFSET_OF(CDataFile::FileHead, m_totalCount)},
        {SIZE_OF(CDataFile::FileHead, m_curIdentify), OFFSET_OF(CDataFile::FileHead, m_curIdentify)}
    };
    Bytes_ChangeOrderByRule(FileHeadBORule, ARRAY_SIZE(FileHeadBORule), pFileHead, sizeof(FileHead));
}

/*******************************************************
  �� �� ��: CDataFile::BytesChangeFieldInfo
  ��    ��: �ֶ���Ϣ�ֽ���ת��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDataFile::BytesChangeFieldInfo(IModel::Field *pFileInfo)
{
    const BYTES_CHANGE_RULE FieldInfoBORule[] = 
    {
        {SIZE_OF(IModel::Field, m_fieldSize), OFFSET_OF(IModel::Field, m_fieldSize)},
        {SIZE_OF(IModel::Field, m_defaultValue), OFFSET_OF(IModel::Field, m_defaultValue)},
        {SIZE_OF(IModel::Field, m_minValue), OFFSET_OF(IModel::Field, m_minValue)},
        {SIZE_OF(IModel::Field, m_maxValue), OFFSET_OF(IModel::Field, m_maxValue)}
    };
    Bytes_ChangeOrderByRule(FieldInfoBORule, ARRAY_SIZE(FieldInfoBORule), pFileInfo, sizeof(IModel::Field));
}

/*******************************************************
  �� �� ��: CDataFile::CDataFile
  ��    ��: CDataFile���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDataFile::CDataFile(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);

    (void)memset(&m_head, 0, sizeof(m_head));

    if (argv && (argc > 0))
    {
        m_fileName = (const char *)argv[0];
        if (m_fileName.Length() && (m_fileName.Get(CDString::TAIL) != '/'))
        {
            m_fileName << "/";
        }
    }
}

/*******************************************************
  �� �� ��: CDataFile::~CDataFile
  ��    ��: CDataFile��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDataFile::~CDataFile()
{
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CDataFile::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::Init(DWORD dwAttrID, IObject *pOwner, IModel *piModel)
{
    /////////////////////////////////////////////////
    /// �ȵ��ø���ĳ�ʼ��
    /////////////////////////////////////////////////
    DWORD dwRc = IDataHandle::Init(IData::TYPE_MEM, dwAttrID, pOwner, piModel, (IData *)m_piParent);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /////////////////////////////////////////////////
    /// ��ȡ�ֶλ�����Ϣ(д���ļ�ͷ���ֶ��ǻ�����Ϣ)
    /////////////////////////////////////////////////
    IModel::Field *pBaseFields = (IModel::Field *)DCOP_Malloc(
                        GetFieldCount() * sizeof(IModel::Field));
    if (!pBaseFields) return FAILURE;
    Field *pFields = GetFields();
    if (!pFields)
    {
        DCOP_Free(pBaseFields);
        return FAILURE;
    }
    for (DWORD i = 0; i < GetFieldCount(); ++i)
    {
        (void)memcpy(&(pBaseFields[i]), &(pFields[i]), sizeof(IModel::Field));
    }

    /////////////////////////////////////////////////
    /// ��ȡ�ļ���
    /////////////////////////////////////////////////
    const char *cszTableName = GetTableName();
    if (!cszTableName || !(*cszTableName))
    {
        DCOP_Free(pBaseFields);
        return FAILURE;
    }

    m_fileName << cszTableName << ".dbf";

    /////////////////////////////////////////////////
    /// ��ȡ�ļ�ͷ
    /////////////////////////////////////////////////
    dwRc = DCOP_RestoreFromFile(m_fileName, &m_head, sizeof(m_head), 0);
    if (!dwRc)
    {
        /// ��ȡ��ת���ļ�ͷ���ֽ���
        BytesChangeFileHead(&m_head);

        /// �ɹ��󣬽���ϵ��У��
        if (strcmp(m_head.m_tableName, cszTableName) ||
            (m_head.m_objectID != pOwner->ID()) ||
            (m_head.m_attrID != dwAttrID) ||
            (m_head.m_headerSize != sizeof(FileHead)) ||
            (m_head.m_fieldCount != GetFieldCount()))
        {
            DCOP_Free(pBaseFields);
            return FAILURE;
        }

        /// �Ƚ��ֶ��Ƿ�һ��
        IModel::Field *pSavedFields = (IModel::Field *)DCOP_Malloc(
                        GetFieldCount() * sizeof(IModel::Field));
        if (!pSavedFields)
        {
            DCOP_Free(pBaseFields);
            return FAILURE;
        }
        dwRc = DCOP_RestoreFromFile(m_fileName, pSavedFields, 
                        GetFieldCount() * sizeof(IModel::Field), sizeof(m_head));
        if (!dwRc)
        {
            for (DWORD i = 0; i < GetFieldCount(); ++i)
            {
                /// ��ȡ��ת���ֶε��ֽ���
                BytesChangeFieldInfo(pSavedFields + i);
            }
        }
        if (dwRc || (memcmp(pSavedFields, pBaseFields, 
                        GetFieldCount() * sizeof(IModel::Field)) != 0))
        {
            DCOP_Free(pBaseFields);
            DCOP_Free(pSavedFields);
            return FAILURE;
        }
        DCOP_Free(pBaseFields);
        DCOP_Free(pSavedFields);

        /// �ָ���ǰID��ʶ
        SetCurIdentify(m_head.m_curIdentify);

        /// ��������
        dwRc = BldAllIdx();
        if (dwRc != SUCCESS) return dwRc;

        return SUCCESS;
    }

    /////////////////////////////////////////////////
    /// ��ȡʧ�ܺ󣬳�ʼ���ļ�
    /////////////////////////////////////////////////
    (void)memset(m_head.m_tableName, 0, sizeof(m_head.m_tableName));
    (void)snprintf(m_head.m_tableName, sizeof(m_head.m_tableName), "%s", cszTableName);
    m_head.m_ver = 1;
    m_head.m_level = 0;
    m_head.m_objectID = pOwner->ID();
    m_head.m_attrID = dwAttrID;
    m_head.m_headerSize = sizeof(FileHead);
    m_head.m_fieldCount = GetFieldCount();
    m_head.m_recordCount = 0;
    m_head.m_totalCount = 0;
    /// д��ǰת���ļ�ͷ���ֽ���(��Ϊ�ļ�ͷ�ڱ��ػ�Ҫʹ�ã����Կ���һ��)
    FileHead headTmp;
    (void)memcpy(&headTmp, &m_head, sizeof(m_head));
    BytesChangeFileHead(&headTmp);
    dwRc = DCOP_SaveToFile(m_fileName, &headTmp, sizeof(headTmp), 0);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /////////////////////////////////////////////////
    /// д���ֶ���Ϣ
    /////////////////////////////////////////////////
    /// д��ǰת���ֶ���Ϣ���ֽ���(��Ϊ�ֶ���Ϣ������ʹ���ˣ����Ծ�ֱ�ӽ���ת��)
    for (DWORD i = 0; i < GetFieldCount(); ++i)
    {
        /// ��ȡ��ת���ֶε��ֽ���
        BytesChangeFieldInfo(pBaseFields + i);
    }
    dwRc = DCOP_SaveToFile(m_fileName, pBaseFields, 
                        GetFieldCount() * sizeof(IModel::Field), sizeof(m_head));
    DCOP_Free(pBaseFields);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ��������
    (void)BldAllIdx();
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataFile::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDataFile::Dump(LOG_PRINT logPrint, LOG_PARA logPara)
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

    logPrint(STR_FORMAT("File Data Table: ['%s'], Records Count: %d \r\n", (const char *)m_fileName, GetRecCount()), logPara);
    CTableString tableStr(dwFieldCount, GetRecCount() + 1, "  ");

    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        tableStr << pFields[i].m_fieldName;
    }

    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec)
    {
        return;
    }

    DCOP_START_TIME();

    for (DWORD dwRecNo = 1; dwRecNo <= m_head.m_totalCount; ++dwRecNo)
    {
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc != SUCCESS) continue;

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

    DCOP_Free(pbyRec);
}

/*******************************************************
  �� �� ��: CDataFile::AddRecord
  ��    ��: ���һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::AddRecord(DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
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
    if (dwRc != SUCCESS)
    {
        DCOP_Free(pbyRec);
        return dwRc;
    }

    /// ��Ӽ�¼���ļ���
    DWORD dwRecNo = AddRecord(pbyRec);
    if (!dwRecNo)
    {
        DCOP_Free(pbyRec);
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
  �� �� ��: CDataFile::DelRecord
  ��    ��: ɾ��һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::DelRecord(BYTE byCond, 
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
        DWORD dwRecNo = (DWORD)idx;
        if (!dwRecNo)
        {
            DCOP_Free(pbyRec);
            return FAILURE;
        }

        /// ��ȡ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc != SUCCESS)
        {
            DCOP_Free(pbyRec);
            return dwRc;
        }

        /// ɾ����¼
        dwRc = DelRecord(dwRecNo);
        if (dwRc != SUCCESS)
        {
            DCOP_Free(pbyRec);
            return dwRc;
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

    /// ģ��ƥ�����(��������ƥ��)
    DWORD dwDelCount = 0;
    if (pEvtData) pEvtData->SetNodeSize(GetRecSize());
    for (DWORD dwRecNo = 1; dwRecNo <= m_head.m_totalCount; ++dwRecNo)
    {
        /// ��ȡ��ǰ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc) continue;

        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        pbyRec, byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
        if (!bRc) continue;

        /// ɾ����¼
        dwRc = DelRecord(dwRecNo);
        if (dwRc) continue;
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
  �� �� ��: CDataFile::EditRecord
  ��    ��: �༭һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::EditRecord(BYTE byCond, 
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

    /// ׼�����Ƽ�¼�Ŀռ�
    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec)
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
        /// ���Ҽ�¼
        CRecIdx::CIdx idx = m_recIdx.FindRec(pCondPara, dwCondParaCount, pCondData, dwCondDataLen);
        DWORD dwRecNo = (DWORD)idx;
        if (!dwRecNo)
        {
            DCOP_Free(pbyRec);
            if (pEvtRec) DCOP_Free(pEvtRec);
            if (pEvtPara) DCOP_Free(pEvtPara);
            return FAILURE;
        }

        /// ��ȡ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc != SUCCESS)
        {
            DCOP_Free(pbyRec);
            if (pEvtRec) DCOP_Free(pEvtRec);
            if (pEvtPara) DCOP_Free(pEvtPara);
            return dwRc;
        }

        /// ���¼�¼
        dwRc = UpdateRecord(pFields, dwFieldCount, 
                        pbyRec, GetRecSize(), 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen);
        if (dwRc != SUCCESS)
        {
            DCOP_Free(pbyRec);
            if (pEvtRec) DCOP_Free(pEvtRec);
            if (pEvtPara) DCOP_Free(pEvtPara);
            return dwRc;
        }

        dwRc = SetRecord(dwRecNo, pbyRec);
        if (dwRc != SUCCESS)
        {
            DCOP_Free(pbyRec);
            if (pEvtRec) DCOP_Free(pEvtRec);
            if (pEvtPara) DCOP_Free(pEvtPara);
            return dwRc;
        }

        /// ������¼
        if (pEvtRec && pEvtPara && 
            (CopyRecord(pFields, dwFieldCount, 
                        pbyRec, GetRecSize(), 
                        pEvtPara, dwEvtParaCount, 
                        pEvtRec, dwEvtDataLen) != SUCCESS))
        {
            DCOP_Free(pbyRec);
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

        DCOP_Free(pbyRec);
        if (pEvtRec) DCOP_Free(pEvtRec);
        return SUCCESS;
    }

    /// ģ��ƥ�����(��������ƥ��)
    DWORD dwEditCount = 0;
    if (pEvtData) pEvtData->SetNodeSize(GetRecSize());
    for (DWORD dwRecNo = 1; dwRecNo <= m_head.m_totalCount; ++dwRecNo)
    {
        /// ��ȡ��ǰ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc) continue;

        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        pbyRec, byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
        if (!bRc) continue;

        /// ���µ�ǰ��¼
        dwRc = UpdateRecord(pFields, dwFieldCount, 
                        pbyRec, GetRecSize(), 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen);
        if (dwRc != SUCCESS) continue;

        dwRc = SetRecord(dwRecNo, pbyRec);
        if (dwRc != SUCCESS) continue;

        /// ������¼
        if (pEvtRec && pEvtPara && 
            (CopyRecord(pFields, dwFieldCount, 
                        pbyRec, GetRecSize(), 
                        pEvtPara, dwEvtParaCount, 
                        pEvtRec, dwEvtDataLen) == SUCCESS))
        {
            (void)pEvtData->Append(pEvtRec);
        }

        dwEditCount++;
    }

    DCOP_Free(pbyRec);
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
  �� �� ��: CDataFile::QueryRecord
  ��    ��: ��ѯ������¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::QueryRecord(BYTE byCond, 
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
    aRspData.SetMaxCount(m_head.m_recordCount);

    /// ׼�����Ƽ�¼�Ŀռ�
    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec)
    {
        DCOP_Free(pRspPara);
        DCOP_Free(pRspData);
        return FAILURE;
    }

    /// ʹ����������ı�����
    AutoObjLock(pObject);

    /// ��ȷƥ�����(ͨ���ؼ�������)
    if (DCOP_CONDITION_ONE == byCond)
    {
        /// ���Ҽ�¼
        CRecIdx::CIdx idx = m_recIdx.FindRec(pCondPara, dwCondParaCount, pCondData, dwCondDataLen);
        DWORD dwRecNo = (DWORD)idx;
        if (!dwRecNo)
        {
            DCOP_Free(pbyRec);
            DCOP_Free(pRspPara);
            DCOP_Free(pRspData);
            return FAILURE;
        }

        /// ��ȡ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc)
        {
            DCOP_Free(pbyRec);
            DCOP_Free(pRspPara);
            DCOP_Free(pRspData);
            return FAILURE;
        }

        /// ������¼
        dwRc = CopyRecord(pFields, dwFieldCount, 
                        pbyRec, GetRecSize(), 
                        pRspPara, dwRspParaCount, 
                        pRspData, dwRspDataLen);
        DCOP_Free(pbyRec);
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
    for (DWORD dwRecNo = 1; dwRecNo <= m_head.m_totalCount; ++dwRecNo)
    {
        /// ��ȡ��ǰ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc) continue;

        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        pbyRec, byCond, 
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
        dwRc = CopyRecord(pFields, dwFieldCount, 
                        pbyRec, GetRecSize(), 
                        pRspPara, dwRspParaCount, 
                        pRspData, dwRspDataLen);
        if (dwRc) continue;

        (void)aRspData.Append(pRspData);
    }

    DCOP_Free(pbyRec);
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
  �� �� ��: CDataFile::CountRecord
  ��    ��: ͳ�Ƽ�¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::CountRecord(BYTE byCond, 
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
        /// ���Ҽ�¼
        CRecIdx::CIdx idx = m_recIdx.FindRec(pCondPara, dwCondParaCount, pCondData, dwCondDataLen);
        DWORD dwRecNo = (DWORD)idx;
        rdwCount = (!dwRecNo)? 0 : 1;
        return SUCCESS;
    }

    /// ׼�����Ƽ�¼�Ŀռ�
    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec)
    {
        return FAILURE;
    }

    /// ģ��ƥ�����(��������ƥ��)
    rdwCount = 0;
    for (DWORD dwRecNo = 1; dwRecNo <= m_head.m_totalCount; ++dwRecNo)
    {
        /// ��ȡ��ǰ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc) continue;

        /// �Ƿ�ƥ�䵱ǰ��¼
        bool bRc = bMatchRecord(pFields, dwFieldCount, 
                        pbyRec, byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
        if (!bRc) continue;

        rdwCount++;
    }

    DCOP_Free(pbyRec);
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataFile::AddKeyIdx
  ��    ��: ��ӹؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::AddKeyIdx(DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    AutoObjLock(pObject);

    /// ����ӹؼ���
    DWORD dwRc = m_recIdx.AddKey(pIdxPara, dwIdxParaCount);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// �ٹ�������
    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec) return FAILURE;
    for (DWORD dwRecNo = 1; dwRecNo <= m_head.m_totalCount; ++dwRecNo)
    {
        /// ��ȡ��ǰ��¼
        dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc != SUCCESS)
        {
            dwRc = SUCCESS;
            continue;
        }

        /// ������ӵ�����
        CRecIdx::CIdx idx(dwRecNo);
        dwRc = m_recIdx.BldKeyIdx(pIdxPara, dwIdxParaCount, GetFields(), pbyRec, idx);
        if (dwRc != SUCCESS)
        {
            (void)m_recIdx.DelKey(pIdxPara, dwIdxParaCount);
            dwRc = FAILURE;
            break;
        }
    }

    DCOP_Free(pbyRec);
    return dwRc;
}

/*******************************************************
  �� �� ��: CDataFile::DelKeyIdx
  ��    ��: ɾ���ؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::DelKeyIdx(DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    IObject *pObject = GetOwner();
    if (!pObject)
    {
        return FAILURE;
    }

    AutoObjLock(pObject);

    return m_recIdx.DelKey(pIdxPara, dwIdxParaCount);
}

/*******************************************************
  �� �� ��: CDataFile::SaveCurIdentify
  ��    ��: ���浱ǰID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDataFile::SaveCurIdentify(DWORD curIdentify)
{
    /// �����ļ�ͷ
    m_head.m_curIdentify = curIdentify;
    /// д��ǰת���ļ�ͷ���ֽ���(��Ϊ�ļ�ͷ�ڱ��ػ�Ҫʹ�ã����Կ���һ��)
    FileHead headTmp;
    (void)memcpy(&headTmp, &m_head, sizeof(m_head));
    BytesChangeFileHead(&headTmp);
    (void)DCOP_SaveToFile(m_fileName, &headTmp, sizeof(headTmp), 0);
}

/*******************************************************
  �� �� ��: CDataFile::AddRecord
  ��    ��: ��Ӽ�¼(д��������¼)
  ��    ��: 
  ��    ��: 
  ��    ��: ��ӳɹ�ʱ���ؼ�¼��(>=1), ���򷵻�0
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::AddRecord(BYTE *pbyRec)
{
    if (!pbyRec)
    {
        return 0;
    }

    /// �ȴ����������
    DWORD dwRecIdx = GetIdleRec();
    CRecIdx::CIdx idx(dwRecIdx + 1);
    DWORD dwRc = m_recIdx.OnAddRec(GetFields(), pbyRec, idx);
    if (dwRc)
    {
        return 0;
    }

    /// ���Ƽ�¼������Ч��ʶ
    BYTE *pRecord = (BYTE *)DCOP_Malloc(1 + GetRecSize());
    if (!pRecord)
    {
        m_recIdx.OnDelRec(GetFields(), pbyRec);
        return 0;
    }
    *pRecord = REC_FLAG_VALID;
    (void)memcpy(pRecord + 1, pbyRec, GetRecSize());

    /// ��������¼���µ��ļ���
    DWORD dwPosIdx = sizeof(FileHead) + sizeof(IModel::Field) * m_head.m_fieldCount;
    dwRc = DCOP_SaveToFile(m_fileName, pRecord, 1 + GetRecSize(), 
                        dwPosIdx + dwRecIdx * (1 + GetRecSize()));
    DCOP_Free(pRecord);
    if (dwRc)
    {
        m_recIdx.OnDelRec(GetFields(), pbyRec);
        return 0;
    }

    /// �����ļ�ͷ
    m_head.m_recordCount++;
    if (dwRecIdx >= m_head.m_totalCount) m_head.m_totalCount++;
    /// д��ǰת���ļ�ͷ���ֽ���(��Ϊ�ļ�ͷ�ڱ��ػ�Ҫʹ�ã����Կ���һ��)
    FileHead headTmp;
    (void)memcpy(&headTmp, &m_head, sizeof(m_head));
    BytesChangeFileHead(&headTmp);
    (void)DCOP_SaveToFile(m_fileName, &headTmp, sizeof(headTmp), 0);

    return dwRecIdx + 1;
}

/*******************************************************
  �� �� ��: CDataFile::DelRecord
  ��    ��: ɾ����¼(ֻ�ǰѱ�ʶ��Ϊ��Ч)
  ��    ��: 
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::DelRecord(DWORD dwRecNo)
{
    if (!dwRecNo || (dwRecNo > m_head.m_totalCount))
    {
        return FAILURE;
    }

    /// �Ȼ�ȡ��¼������ʶ�Ƿ��Ѿ���Ч
    BYTE *pRecord = (BYTE *)DCOP_Malloc(1 + GetRecSize());
    if (!pRecord) return FAILURE;
    DWORD dwPosIdx = sizeof(FileHead) + sizeof(IModel::Field) * m_head.m_fieldCount;
    DWORD dwRecIdx = dwRecNo - 1;
    DWORD dwRc = DCOP_RestoreFromFile(m_fileName, pRecord, 1 + GetRecSize(), 
                        dwPosIdx + dwRecIdx * (1 + GetRecSize()));
    if (dwRc || !(*pRecord))
    {
        DCOP_Free(pRecord);
        return FAILURE;
    }

    /// �Ѽ�¼�ı�ʶ��Ϊ��Ч
    *pRecord = REC_FLAG_NULL;
    dwRc = DCOP_SaveToFile(m_fileName, pRecord, 1, 
                        dwPosIdx + dwRecIdx * (1 + GetRecSize()));
    if (dwRc)
    {
        DCOP_Free(pRecord);
        return dwRc;
    }

    /// �����ļ�ͷ
    if (m_head.m_recordCount) m_head.m_recordCount--;
    /// д��ǰת���ļ�ͷ���ֽ���(��Ϊ�ļ�ͷ�ڱ��ػ�Ҫʹ�ã����Կ���һ��)
    FileHead headTmp;
    (void)memcpy(&headTmp, &m_head, sizeof(m_head));
    BytesChangeFileHead(&headTmp);
    (void)DCOP_SaveToFile(m_fileName, &headTmp, sizeof(headTmp), 0);

    /// ����ɾ������
    m_recIdx.OnDelRec(GetFields(), pRecord + 1);
    DCOP_Free(pRecord);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataFile::SetRecord
  ��    ��: ���ü�¼(�滻������¼)[��¼������Ч]
  ��    ��: 
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::SetRecord(DWORD dwRecNo, BYTE *pbyRec)
{
    if (!dwRecNo || (dwRecNo > m_head.m_totalCount) || (!pbyRec))
    {
        return FAILURE;
    }

    /// ���жϱ�ʶ�Ƿ��Ѿ���Ч
    BYTE byFlag = REC_FLAG_NULL;
    DWORD dwPosIdx = sizeof(FileHead) + sizeof(IModel::Field) * m_head.m_fieldCount;
    DWORD dwRecIdx = dwRecNo - 1;
    DWORD dwRc = DCOP_RestoreFromFile(m_fileName, &byFlag, 1, 
                        dwPosIdx + dwRecIdx * (1 + GetRecSize()));
    if (dwRc || !byFlag)
    {
        return FAILURE;
    }

    /// ���Ƽ�¼������Ч��ʶ
    BYTE *pRecord = (BYTE *)DCOP_Malloc(1 + GetRecSize());
    if (!pRecord) return FAILURE;
    *pRecord = REC_FLAG_VALID;
    (void)memcpy(pRecord + 1, pbyRec, GetRecSize());

    /// ��������¼���µ��ļ���
    dwRc = DCOP_SaveToFile(m_fileName, pRecord, 1 + GetRecSize(), 
                        dwPosIdx + dwRecIdx * (1 + GetRecSize()));
    DCOP_Free(pRecord);

    return dwRc;
}

/*******************************************************
  �� �� ��: CDataFile::GetRecord
  ��    ��: ��ȡ��¼(����������¼)
  ��    ��: 
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::GetRecord(DWORD dwRecNo, BYTE *pbyRec)
{
    if (!dwRecNo || (dwRecNo > m_head.m_totalCount) || (!pbyRec))
    {
        return FAILURE;
    }

    /// ��ȡ������¼
    BYTE *pRecord = (BYTE *)DCOP_Malloc(1 + GetRecSize());
    if (!pRecord) return FAILURE;
    DWORD dwPosIdx = sizeof(FileHead) + sizeof(IModel::Field) * m_head.m_fieldCount;
    DWORD dwRecIdx = dwRecNo - 1;
    DWORD dwRc = DCOP_RestoreFromFile(m_fileName, pRecord, 1 + GetRecSize(), 
                        dwPosIdx + dwRecIdx * (1 + GetRecSize()));
    if (dwRc)
    {
        DCOP_Free(pRecord);
        return FAILURE;
    }

    /// �����¼��ʶ��Ч, ���ش���
    if (!(*pRecord))
    {
        DCOP_Free(pRecord);
        return FAILURE;
    }

    /// ���ƻ�ȡ���ļ�¼
    (void)memcpy(pbyRec, pRecord + 1, GetRecSize());
    DCOP_Free(pRecord);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataFile::GetIdleRec
  ��    ��: �ҵ�һ�����м�¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::GetIdleRec()
{
    DWORD dwRecIdx = m_head.m_totalCount;
    DWORD dwPosIdx = sizeof(FileHead) + sizeof(IModel::Field) * m_head.m_fieldCount;
    for (DWORD i = 0; i < m_head.m_totalCount; ++i)
    {
        BYTE byFlag = REC_FLAG_NULL;
        DWORD dwRc = DCOP_RestoreFromFile(m_fileName, &byFlag, 1, dwPosIdx);
        if (dwRc)
        {
            break;
        }

        if (!byFlag)
        {
            dwRecIdx = i;
            break;
        }

        dwPosIdx += 1 + GetRecSize();
    }

    return dwRecIdx;
}

/*******************************************************
  �� �� ��: CDataFile::BldAllIdx
  ��    ��: ������������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataFile::BldAllIdx()
{
    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    /// ��ӹؼ�������
    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        if (!pFields[i].m_isKey) continue;

        DCOP_PARA_NODE keyPara = 
        {
            i + 1, // �ֶ�IDΪ"����ֵ+1"
            DCOP_OPCODE_NONE,
            pFields[i].m_fieldType,
            pFields[i].m_fieldSize
        };
        (void)m_recIdx.AddKey(&keyPara, 1);
    }

    /// û�м�¼���Ͳ��ù�������
    if (!m_head.m_totalCount)
    {
        return SUCCESS;
    }

    /// Ϊ��ȡ��¼����ռ�
    BYTE *pbyRec = (BYTE *)DCOP_Malloc(GetRecSize());
    if (!pbyRec)
    {
        return FAILURE;
    }

    /// ������¼��Ӽ�¼����
    for (DWORD dwRecNo = 1; dwRecNo <= m_head.m_totalCount; ++dwRecNo)
    {
        /// ��ȡ��ǰ��¼
        DWORD dwRc = GetRecord(dwRecNo, pbyRec);
        if (dwRc) continue;

        CRecIdx::CIdx idx(dwRecNo);
        dwRc = m_recIdx.OnAddRec(pFields, pbyRec, idx);
        if (dwRc)
        {
            DCOP_Free(pbyRec);
            return FAILURE;
        }
    }

    DCOP_Free(pbyRec);
    return SUCCESS;
}

