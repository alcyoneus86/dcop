/// -------------------------------------------------
/// ObjData_tosql.h : ��TLVת��ΪSQL���˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_tosql.h"
#include "Factory_if.h"
#include "ObjTimer_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CTlvToSQL, "TLV2SQL")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CTlvToSQL)
    DCOP_IMPLEMENT_INTERFACE(ITlvToSQL)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// �������������Ͷ���(��DCOP_OPCODE_TYPEһһ��Ӧ)
/// -------------------------------------------------
const char *CTlvToSQL::ArgOpCode[] = 
{
    "",
    "+",
    "-",
    "*",
    "/",
    "=",
    ">=",
    ">",
    "<",
    "<=",
    "!=",
    "LIKE",
    "NOT LIKE",
};


/*******************************************************
  �� �� ��: CTlvToSQL::CTlvToSQL
  ��    ��: CTlvToSQL���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTlvToSQL::CTlvToSQL(Instance *piParent, int argc, char **argv)
{
    /// �����Ӷ��󣬲������ø����󣬶����Ӷ��������������ͷ����ã�����������Ϊ���ü�������1���޷��ߵ�����
    DCOP_CONSTRUCT_INSTANCE(NULL);

    m_piDataHandle = (piParent)? (IDataHandle *)piParent : NULL;
}

/*******************************************************
  �� �� ��: CTlvToSQL::~CTlvToSQL
  ��    ��: CTlvToSQL��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTlvToSQL::~CTlvToSQL()
{
    m_piDataHandle = NULL;

    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetCreateFieldList
  ��    ��: ��ȡ���������б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetCreateFieldList(CDString &strArgList)
{
    if (!m_piDataHandle) return FAILURE;

    IDataHandle::Field *pFields = m_piDataHandle->GetFields();
    DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    bool bFirstKey = true;
    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        switch (pFields[i].m_fieldType)
        {
            case IModel::FIELD_IDENTIFY:
                strArgList << STR_FORMAT("%s SERIAL", pFields[i].m_fieldName);
                break;
            case IModel::FIELD_BYTE:
                strArgList << STR_FORMAT("%s TINYINT UNSIGNED DEFAULT %d", pFields[i].m_fieldName, pFields[i].m_defaultValue);
                break;
            case IModel::FIELD_WORD:
                strArgList << STR_FORMAT("%s SMALLINT UNSIGNED DEFAULT %d", pFields[i].m_fieldName, pFields[i].m_defaultValue);
                break;
            case IModel::FIELD_DWORD:
                strArgList << STR_FORMAT("%s INT UNSIGNED DEFAULT %d", pFields[i].m_fieldName, pFields[i].m_defaultValue);
                break;
            case IModel::FIELD_SHORT:
                strArgList << STR_FORMAT("%s SMALLINT DEFAULT %d", pFields[i].m_fieldName, pFields[i].m_defaultValue);
                break;
            case IModel::FIELD_INTEGER:
                strArgList << STR_FORMAT("%s INT DEFAULT %d", pFields[i].m_fieldName, pFields[i].m_defaultValue);
                break;
            case IModel::FIELD_NUMBER:
                strArgList << STR_FORMAT("%s BIGINT UNSIGNED DEFAULT %d", pFields[i].m_fieldName, pFields[i].m_defaultValue);
                break;
            case IModel::FIELD_BOOL:
                strArgList << STR_FORMAT("%s TINYINT", pFields[i].m_fieldName);
                break;
            case IModel::FIELD_CHAR:
                strArgList << STR_FORMAT("%s CHAR(%d)", pFields[i].m_fieldName, pFields[i].m_fieldSize);
                break;
            case IModel::FIELD_STRING:
                strArgList << STR_FORMAT("%s VARCHAR(%d)", pFields[i].m_fieldName, pFields[i].m_fieldSize);
                break;
            case IModel::FIELD_BUFFER:
                strArgList << STR_FORMAT("%s VARBINARY(%d)", pFields[i].m_fieldName, pFields[i].m_fieldSize);
                break;
            case IModel::FIELD_DATE:
                strArgList << STR_FORMAT("%s DATE", pFields[i].m_fieldName);
                break;
            case IModel::FIELD_TIME:
                strArgList << STR_FORMAT("%s TIME", pFields[i].m_fieldName);
                break;
            case IModel::FIELD_IP:
                strArgList << STR_FORMAT("%s INT UNSIGNED DEFAULT 0", pFields[i].m_fieldName);
                break;
            case IModel::FIELD_PTR:
                strArgList << STR_FORMAT("%s VARBINARY(%d) DEFAULT 0x0", pFields[i].m_fieldName, sizeof(void *));
                break;
            case IModel::FIELD_TIMER:
                strArgList << STR_FORMAT("%s VARBINARY(%d) DEFAULT 0x0", pFields[i].m_fieldName, sizeof(ITimer::Node));
                break;
            case IModel::FIELD_PASS:
                strArgList << STR_FORMAT("%s VARBINARY(%d)", pFields[i].m_fieldName, pFields[i].m_fieldSize);
            default:
                break;
        }

        /// ��ӹؼ��ֱ�ʶ
        if (pFields[i].m_isKey)
        {
            if (bFirstKey) strArgList << " PRIMARY";
            strArgList << " KEY";
        }

        /// �ֶμ���
        if (i != (dwFieldCount - 1))
        {
            strArgList << ", ";
        }
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetInsertFieldList
  ��    ��: ��ȡ��������б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetInsertFieldList(DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        CDString &strArgList)
{
    if (!pReqPara || !dwReqParaCount || 
        !pReqData || !dwReqDataLen)
    {
        return FAILURE;
    }

    if (!m_piDataHandle) return FAILURE;

    IDataHandle::Field *pFields = m_piDataHandle->GetFields();
    DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    CDString strNameList;
    CDString strValueList;
    DWORD dwRc = SUCCESS;
    DWORD dwDataPos = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwReqParaCount; ++dwParaIdx)
    {
        /// ��Ҫ�жϲ���ID����ȷ��
        if (!pReqPara[dwParaIdx].m_paraID || (pReqPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            dwRc = FAILURE;
            break;
        }

        /// ��ȡ�ֶ�����
        DWORD dwFieldIdx = pReqPara[dwParaIdx].m_paraID - 1;

        /// �Զ����ɵ�IDentify���ܱ�����
        if (m_piDataHandle->IsIdentify(pFields[dwFieldIdx].m_fieldType))
        {
            /// ����ƫ��
            dwDataPos += pReqPara[dwParaIdx].m_paraSize;
            if (dwDataPos > dwReqDataLen)
            {
                dwRc = FAILURE;
                break;
            }

            continue;
        }

        strNameList << STR_FORMAT("%s, ", pFields[dwFieldIdx].m_fieldName);

        /// �ж����ֵĴ�С��Χ
        if ((m_piDataHandle->IsDigital(pFields[dwFieldIdx].m_fieldType)) || 
            (IModel::FIELD_IP == pFields[dwFieldIdx].m_fieldType))
        {
            DWORD dwValue = Bytes_GetDwordValue((BYTE *)pReqData + dwDataPos, pReqPara[dwParaIdx].m_paraSize);
            DWORD dwMinValue = pFields[dwFieldIdx].m_minValue;
            DWORD dwMaxValue = pFields[dwFieldIdx].m_maxValue;
            if ((dwMinValue != dwMaxValue) && ((dwValue < dwMinValue) || (dwValue > dwMaxValue)))
            {
                dwRc = FAILURE;
                break;
            }

            strValueList << STR_FORMAT("%d, ", dwValue);
        }
        else if (IModel::FIELD_CHAR == pFields[dwFieldIdx].m_fieldType)
        {
            strValueList << STR_FORMAT("'%c', ", *((char *)pReqData + dwDataPos));
        }
        else if (IModel::FIELD_STRING == pFields[dwFieldIdx].m_fieldType)
        {
            CDString strTmp((char *)pReqData + dwDataPos, pReqPara[dwParaIdx].m_paraSize);
            strValueList << STR_FORMAT("'%s', ", (const char *)strTmp);
        }
        else if ((IModel::FIELD_BUFFER == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_PTR == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_TIMER == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_PASS == pFields[dwFieldIdx].m_fieldType))
        {
            strValueList << "0x" << CBufferString((BYTE *)pReqData + dwDataPos, 
                                    pReqPara[dwParaIdx].m_paraSize) << ", ";
        }
        else
        {
        }

        /// ����ƫ��
        dwDataPos += pReqPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwReqDataLen)
        {
            dwRc = FAILURE;
            break;
        }
    }

    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    strNameList.TrimRight(", ");
    strValueList.TrimRight(", ");

    strArgList << STR_FORMAT("(%s) ", (const char *)strNameList) 
               << STR_FORMAT("VALUES (%s)", (const char *)strValueList);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetCondFieldList
  ��    ��: ��ȡ���������б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetCondFieldList(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        CDString &strArgList)
{
    if (!pCondPara && !dwCondParaCount && !pCondData && !dwCondDataLen)
    {
        return SUCCESS;
    }

    if (!pCondPara || !dwCondParaCount || !pCondData || !dwCondDataLen)
    {
        return FAILURE;
    }

    if (!m_piDataHandle) return FAILURE;

    IDataHandle::Field *pFields = m_piDataHandle->GetFields();
    DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    DWORD dwRc = SUCCESS;
    DWORD dwDataPos = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwCondParaCount; ++dwParaIdx)
    {
        /// ��Ҫ�жϲ���ID����ȷ��
        if (!pCondPara[dwParaIdx].m_paraID || (pCondPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            continue;
        }

        /// ��ȡ�ֶ�����
        DWORD dwFieldIdx = pCondPara[dwParaIdx].m_paraID - 1;

        /// ��ȡ����������
        dwRc  = GetCondArgOpCode(byCond, 
                        pCondPara[dwParaIdx].m_opCode, 
                        pCondPara[dwParaIdx].m_paraType, 
                        pFields[dwFieldIdx].m_fieldName, 
                        strArgList);
        if (dwRc != SUCCESS)
        {
            break;
        }

        /// ��ȡ��������ֵ
        if ((m_piDataHandle->IsDigital(pFields[dwFieldIdx].m_fieldType)) || 
            (IModel::FIELD_IP == pFields[dwFieldIdx].m_fieldType))
        {
            DWORD dwValue = Bytes_GetDwordValue((BYTE *)pCondData + dwDataPos, pCondPara[dwParaIdx].m_paraSize);
            strArgList << STR_FORMAT("%d ", dwValue);
        }
        else if (IModel::FIELD_BOOL == pFields[dwFieldIdx].m_fieldType)
        {
            strArgList << ((*((BYTE *)pCondData + dwDataPos))? "TRUE" : "FALSE");
        }
        else if (IModel::FIELD_CHAR == pFields[dwFieldIdx].m_fieldType)
        {
            strArgList << STR_FORMAT("'%c' ", *((char *)pCondData + dwDataPos));
        }
        else if (IModel::FIELD_STRING == pFields[dwFieldIdx].m_fieldType)
        {
            CDString strTmp((char *)pCondData + dwDataPos, pCondPara[dwParaIdx].m_paraSize);
            strArgList << STR_FORMAT("'%s' ", (const char *)strTmp);
        }
        else if ((IModel::FIELD_BUFFER == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_PTR == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_TIMER == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_PASS == pFields[dwFieldIdx].m_fieldType))
        {
            strArgList << "0x" << CBufferString((BYTE *)pCondData + dwDataPos, 
                                  pCondPara[dwParaIdx].m_paraSize) << " ";
        }
        else
        {
        }

        /// ����ƫ��
        dwDataPos += pCondPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwCondDataLen)
        {
            dwRc = FAILURE;
            break;
        }

        /// �ֶμ���
        if (dwParaIdx != (dwCondParaCount - 1))
        {
            strArgList << ((byCond == DCOP_CONDITION_ANY)? "OR " : "AND ");
        }
    }

    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    strArgList.TrimRight(" ");

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetReqFieldList
  ��    ��: ��ȡ��������б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetReqFieldList(DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        CDString &strArgList)
{
    if (!pReqPara || !dwReqParaCount || 
        !pReqData || !dwReqDataLen)
    {
        return FAILURE;
    }

    if (!m_piDataHandle) return FAILURE;

    IDataHandle::Field *pFields = m_piDataHandle->GetFields();
    DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    DWORD dwRc = SUCCESS;
    DWORD dwDataPos = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwReqParaCount; ++dwParaIdx)
    {
        /// ��Ҫ�жϲ���ID����ȷ��
        if (!pReqPara[dwParaIdx].m_paraID || (pReqPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            dwRc = FAILURE;
            break;
        }

        /// ��ȡ�ֶ�����
        DWORD dwFieldIdx = pReqPara[dwParaIdx].m_paraID - 1;

        /// ��ȡ���������
        dwRc  = GetReqArgOpCode(pReqPara[dwParaIdx].m_opCode, 
                        pReqPara[dwParaIdx].m_paraType, 
                        pFields[dwFieldIdx].m_fieldName, 
                        strArgList);
        if (dwRc != SUCCESS)
        {
            break;
        }

        /// ��ȡ��������ֵ
        if ((m_piDataHandle->IsDigital(pFields[dwFieldIdx].m_fieldType)) || 
            (IModel::FIELD_IP == pFields[dwFieldIdx].m_fieldType))
        {
            DWORD dwValue = Bytes_GetDwordValue((BYTE *)pReqData + dwDataPos, pReqPara[dwParaIdx].m_paraSize);
            strArgList << STR_FORMAT("%d, ", dwValue);
        }
        else if (IModel::FIELD_BOOL == pFields[dwFieldIdx].m_fieldType)
        {
            strArgList << ((*((BYTE *)pReqData + dwDataPos))? "1" : "0");
        }
        else if (IModel::FIELD_CHAR == pFields[dwFieldIdx].m_fieldType)
        {
            strArgList << STR_FORMAT("'%c', ", *((char *)pReqData + dwDataPos));
        }
        else if (IModel::FIELD_STRING == pFields[dwFieldIdx].m_fieldType)
        {
            CDString strTmp((char *)pReqData + dwDataPos, pReqPara[dwParaIdx].m_paraSize);
            strArgList << STR_FORMAT("'%s', ", (const char *)strTmp);
        }
        else if ((IModel::FIELD_BUFFER == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_PTR == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_TIMER == pFields[dwFieldIdx].m_fieldType) || 
            (IModel::FIELD_PASS == pFields[dwFieldIdx].m_fieldType))
        {
            strArgList << "0x" << CBufferString((BYTE *)pReqData + dwDataPos, 
                                  pReqPara[dwParaIdx].m_paraSize) << ", ";
        }
        else
        {
        }

        /// ����ƫ��
        dwDataPos += pReqPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwReqDataLen)
        {
            dwRc = FAILURE;
            break;
        }
    }

    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    strArgList.TrimRight(", ");

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetFieldNameList
  ��    ��: ��ȡ���������б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetFieldNameList(DCOP_PARA_NODE *pPara, 
                        DWORD dwParaCount, 
                        CDString &strArgList, 
                        const char *cszSplit, 
                        CDString *pStrJoinTable)
{
    if (!pPara || !dwParaCount)
    {
        strArgList << "*";
        return SUCCESS;
    }

    if (!m_piDataHandle) return FAILURE;

    IDataHandle::Field *pFields = m_piDataHandle->GetFields();
    DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    DWORD dwRc = SUCCESS;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwParaCount; ++dwParaIdx)
    {
        /// ��ȡ�������͵��������
        if (DCOP_SPECPARA(pPara[dwParaIdx].m_paraID, DCOP_FIELD_RELATION) && pStrJoinTable)
        {
            dwRc = GetJoinTable(pPara[dwParaIdx].m_paraID, strArgList, *pStrJoinTable);
            if (dwRc != SUCCESS)
            {
                break;
            }

            strArgList << cszSplit;
            continue;
        }

        /// ��Ҫ�жϲ���ID����ȷ��
        if (!pPara[dwParaIdx].m_paraID || (pPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            dwRc = FAILURE;
            break;
        }

        /// ��ȡ�ֶ�����
        DWORD dwFieldIdx = pPara[dwParaIdx].m_paraID - 1;

        /// ��ȡ����
        strArgList << pFields[dwFieldIdx].m_fieldName << cszSplit;
    }

    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    strArgList.TrimRight(cszSplit);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetRspFieldList
  ��    ��: ��ȡ��Ӧ�����б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetRspFieldList(DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        MYSQL_RES *res, 
                        DCOP_PARA_NODE *&rpRspPara, 
                        DWORD &rdwRspParaCount, 
                        CDArray &aRspData)
{
    if (dwReqParaCount)
    {
        if (!pReqPara) return FAILURE;
    }

    if (!m_piDataHandle) return FAILURE;

    /// ��ȡ�ֶ���Ϣ
    IDataHandle::Field *pFields = m_piDataHandle->GetFields();
    DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    /// ��ȡ��Ӧ����
    DWORD dwRspParaCount = 0;
    DWORD dwRspDataLen = 0;
    DCOP_PARA_NODE *pRspPara = m_piDataHandle->GetOutPara(pFields, dwFieldCount, dwRspParaCount, 
                        pReqPara, dwReqParaCount, &dwRspDataLen);
    if (!pRspPara)
    {
        return FAILURE;
    }

    /// ��Ӧ���ݳ��ȴ���
    if ((dwRspParaCount != (DWORD)mysql_num_fields(res)) || !dwRspDataLen)
    {
        DCOP_Free(pRspPara);
        return FAILURE;
    }

    void *pRspData = DCOP_Malloc(dwRspDataLen);
    if (!pRspData)
    {
        DCOP_Free(pRspPara);
        return FAILURE;
    }

    (void)memset(pRspData, 0, dwRspDataLen);
    aRspData.SetNodeSize(dwRspDataLen);
    aRspData.SetMaxCount((DWORD)mysql_num_rows(res));

    DWORD dwRc = SUCCESS;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        unsigned long *lens = mysql_fetch_lengths(res);
        dwRc = GetRspArgValue(pRspPara, dwRspParaCount, row, lens, pRspData, dwRspDataLen);
        if (dwRc != SUCCESS)
        {
            break;
        }

        (void)aRspData.Append(pRspData);
    }

    DCOP_Free(pRspData);
    if (!aRspData.Count())
    {
        DCOP_Free(pRspPara);
        return (dwRc) ? dwRc : FAILURE;
    }

    rpRspPara = pRspPara;
    rdwRspParaCount = dwRspParaCount;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetIdentifyName
  ��    ��: ��ȡIdentify�ֶ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetIdentifyName(CDString &strIdName)
{
    if (!m_piDataHandle) return FAILURE;

    IDataHandle::Field *pFields = m_piDataHandle->GetFields();
    DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        /// ��ӹؼ��ֱ�ʶ
        if (pFields[i].m_fieldType == IModel::FIELD_IDENTIFY)
        {
            strIdName << pFields[i].m_fieldName;
            return SUCCESS;
        }
    }

    return FAILURE;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetCondArgOpCode
  ��    ��: ��ȡ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetCondArgOpCode(BYTE cond, BYTE opCode, BYTE paraType, const char *fieldName, CDString &strArgList)
{
    if (cond == DCOP_CONDITION_ONE)
    {
        strArgList << fieldName << "=";
        return SUCCESS;
    }

    if (opCode >= ARRAY_SIZE(ArgOpCode))
    {
        return FAILURE;
    }

    /// �Ⱥ�ǰ��Ķ���������������Ⱥź����(���Ⱥ�)�����߼�������
    if (opCode < DCOP_OPCODE_EQUAL)
    {
        return FAILURE;
    }

    const char *pcszArgOpCode = ArgOpCode[opCode];

    /// ���⴦���������
    if (paraType == IModel::FIELD_BOOL)
    {
        pcszArgOpCode = "IS";
    }

    strArgList << fieldName << pcszArgOpCode;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetReqArgOpCode
  ��    ��: ��ȡ���������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetReqArgOpCode(BYTE opCode, BYTE paraType, const char *fieldName, CDString &strArgList)
{
    if (opCode >= ARRAY_SIZE(ArgOpCode))
    {
        return FAILURE;
    }

    /// �Ⱥź���Ķ����߼����������Ⱥ�(���Ⱥ�)ǰ��Ĳ������������
    if (opCode > DCOP_OPCODE_EQUAL)
    {
        return FAILURE;
    }

    const char *pcszArgOpCode = ArgOpCode[opCode];

    /// ���⴦���������
    if (opCode == DCOP_OPCODE_EQUAL)
    {
        strArgList << fieldName << pcszArgOpCode;
        return SUCCESS;
    }

    strArgList << fieldName << "=" << fieldName << pcszArgOpCode;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetRspArgValue
  ��    ��: ��ȡ��Ӧ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetRspArgValue(DCOP_PARA_NODE *pRspPara, DWORD dwRspParaCount, 
                        MYSQL_ROW row, unsigned long *lens, 
                        void *pRspData, DWORD dwRspDataLen)
{
    DWORD dwRc = SUCCESS;
    DWORD dwDataPos = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwRspParaCount; ++dwParaIdx)
    {
        if (!(row[dwParaIdx]))
        {
            (void)memset((char *)pRspData + dwDataPos, 0, pRspPara[dwParaIdx].m_paraSize);
            dwDataPos += pRspPara[dwParaIdx].m_paraSize;
            if (dwDataPos > dwRspDataLen)
            {
                dwRc = FAILURE;
                break;
            }

            continue;
        }

        /// ��ȡ��������ֵ
        if ((m_piDataHandle->IsDigital(pRspPara[dwParaIdx].m_paraType)) || 
            (IModel::FIELD_IP == pRspPara[dwParaIdx].m_paraType))
        {
            Bytes_SetDwordValue((DWORD)atoi(row[dwParaIdx]), (BYTE *)pRspData + dwDataPos, pRspPara[dwParaIdx].m_paraSize);
        }
        else if (IModel::FIELD_BOOL == pRspPara[dwParaIdx].m_paraType)
        {
            *((BYTE *)pRspData + dwDataPos) = (atoi(row[dwParaIdx])? 1 : 0);
        }
        else if (IModel::FIELD_CHAR == pRspPara[dwParaIdx].m_paraType)
        {
            *((char *)pRspData + dwDataPos) = *(row[dwParaIdx]);
        }
        else if (IModel::FIELD_STRING == pRspPara[dwParaIdx].m_paraType)
        {
            (void)strncpy((char *)pRspData + dwDataPos, row[dwParaIdx], pRspPara[dwParaIdx].m_paraSize);
        }
        else if ((IModel::FIELD_BUFFER == pRspPara[dwParaIdx].m_paraType) || 
            (IModel::FIELD_PTR == pRspPara[dwParaIdx].m_paraType) || 
            (IModel::FIELD_TIMER == pRspPara[dwParaIdx].m_paraType) || 
            (IModel::FIELD_PASS == pRspPara[dwParaIdx].m_paraType))
        {
            /// ���ڶ��������ͣ������ѯ����ֱ���Ƕ��������ݣ��Ͳ��벻ͬ:
            /// ����ʱ��������������Ҫת�����ַ���ʽ��: 0x01020abcc
            /// ���Բ�ѯ֮��Ķ���������ֱ�ӿ���������Ҫת���ɶ���������.
            DWORD dwCopyDataLen = MIN((DWORD)(pRspPara[dwParaIdx].m_paraSize), (DWORD)(lens[dwParaIdx]));
            if (dwCopyDataLen) (void)memcpy((BYTE *)pRspData + dwDataPos, row[dwParaIdx], dwCopyDataLen);
            DWORD dwLeftBufLen = (DWORD)(pRspPara[dwParaIdx].m_paraSize) - dwCopyDataLen;
            if (dwLeftBufLen) (void)memset((BYTE *)pRspData + dwDataPos + dwCopyDataLen, 0, dwLeftBufLen);
        }
        else
        {
        }

        /// ����ƫ��
        dwDataPos += pRspPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwRspDataLen)
        {
            dwRc = FAILURE;
            break;
        }
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CTlvToSQL::GetJoinTable
  ��    ��: ��ȡ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTlvToSQL::GetJoinTable(DWORD dwRelID, 
                        CDString &strFieldName,
                        CDString &strJoinTable)
{
    if (!m_piDataHandle) return FAILURE;

    IModel::Relation *pShip = m_piDataHandle->GetRelation(dwRelID);
    if (!pShip)
    {
        return FAILURE;
    }

    IModel *piModel = m_piDataHandle->GetModel();
    if (!piModel)
    {
        return FAILURE;
    }

    const char *pcszRelTable = piModel->GetTableName(pShip->m_attrID);
    if (!pcszRelTable)
    {
        return FAILURE;
    }

    DWORD dwRelFieldCount = 0;
    IModel::Field *pRelFields = piModel->GetFields(pShip->m_attrID, dwRelFieldCount);
    if (!pRelFields || !dwRelFieldCount)
    {
        return FAILURE;
    }

    if (!pShip->m_fieldID || (pShip->m_fieldID > dwRelFieldCount))
    {
        return FAILURE;
    }

    DWORD dwRelFieldID = (DWORD)(dwRelID & DCOP_FIELD_LOW0);
    if (!dwRelFieldID || (dwRelFieldID > dwRelFieldCount))
    {
        return FAILURE;
    }

    /// ����ֶ�(��ʽ:'����.�ֶ���')
    strFieldName << pcszRelTable << "." << pRelFields[dwRelFieldID - 1].m_fieldName;

    /// ���������ӣ�����Ѿ����ӾͲ������(����:���һ�����ж�������ֶΣ���Ҫ�ѹؼ����ӵ��ֶ�д��ǰ��)
    CDString strJoinTableTmp = " left Join ";
    strJoinTableTmp << pcszRelTable;
    DWORD dwIdx = strJoinTable.Find((const char *)strJoinTableTmp);
    if (dwIdx == CDString::TAIL)
    {
        IDataHandle::Field *pFields = m_piDataHandle->GetFields();
        DWORD dwFieldCount = m_piDataHandle->GetFieldCount();
        if (!pFields || !dwFieldCount)
        {
            return FAILURE;
        }

        DWORD dwFieldID = pShip->m_relID;
        if (!dwFieldID || (dwFieldID > dwFieldCount))
        {
            return FAILURE;
        }

        strJoinTable << strJoinTableTmp << STR_FORMAT(" on %s.%s=%s.%s", 
                        m_piDataHandle->GetTableName(), pFields[dwFieldID - 1].m_fieldName, 
                        pcszRelTable, pRelFields[pShip->m_fieldID - 1].m_fieldName);
    }

    return SUCCESS;
}

