/// -------------------------------------------------
/// ObjData_handle.cpp : ���ݾ��ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_main.h"
#include "ObjData_handle.h"
#include "cpu/bytes.h"
#include "string/dstring.h"


/*******************************************************
  �� �� ��: IDataHandle
  ��    ��: IDataHandle���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IDataHandle::IDataHandle()
{
    m_piData = NULL;
    m_piModel = NULL;
    m_pOwner = NULL;
    m_dwAttrID = 0;
    m_dwType = IData::TYPE_MEM;
    *m_szTableName = '\0';
    m_pFields = NULL;
    m_dwFieldCount = 0;
    m_pShips = NULL;
    m_dwShipCount = 0;
    m_dwRecSize = 0;
    m_curIdentify = 0;
}

/*******************************************************
  �� �� ��: IDataHandle::~IDataHandle
  ��    ��: IDataHandle��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IDataHandle::~IDataHandle()
{
    m_piData = NULL;
    m_piModel = NULL;
    m_pOwner = NULL;
    m_dwAttrID = 0;
    m_dwType = 0;
    *m_szTableName = '\0';
    if (m_pFields)
    {
        DCOP_Free(m_pFields);
        m_pFields = NULL;
    }
    m_dwFieldCount = 0;
    if (m_pShips)
    {
        DCOP_Free(m_pShips);
        m_pShips = NULL;
    }
    m_dwShipCount = 0;
    m_dwRecSize = 0;
    m_curIdentify = 0;
}

/*******************************************************
  �� �� ��: IDataHandle::Init
  ��    ��: ����
  ��    ��: dwType
            dwAttrID
            pOwner
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::Init(DWORD dwType, 
                DWORD dwAttrID, 
                IObject *pOwner, 
                IModel *piModel, 
                IData *piData)
{
    if (m_pFields)
    {
        /// �����ֶ���Ϣ��˵���Ѿ���ʼ������
        return FAILURE;
    }

    if (!pOwner || !piModel)
    {
        /// �������Ϊ�գ������ʼ��ʱ�����ظ������
        return FAILURE;
    }

    const char *pszTableName = piModel->GetTableName(dwAttrID);
    if (!pszTableName)
    {
        return FAILURE;
    }

    DWORD dwFieldCount = 0;
    DWORD dwRecSize = 0;
    Field *pFields = CopyFields(piModel, dwAttrID, dwFieldCount, dwRecSize);
    if (!pFields)
    {
        /// �����ֶ�ʧ�ܣ�ֻ�з��ش�����
        return FAILURE;
    }

    m_piData = piData;
    m_piModel = piModel;
    m_pOwner = pOwner;
    m_dwAttrID = dwAttrID;
    m_dwType = dwType;
    (void)memset(m_szTableName, 0, sizeof(m_szTableName));
    (void)snprintf(m_szTableName, sizeof(m_szTableName), "%s", pszTableName);
    m_pFields = pFields;
    m_dwFieldCount = dwFieldCount;
    m_dwRecSize = dwRecSize;

    m_pShips = CopyShips(piModel, dwAttrID, m_dwShipCount);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: IDataHandle::FillRecord
  ��    ��: ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::FillRecord(Field *pFields, DWORD dwFieldCount,
                        BYTE *pbyRec, DWORD dwRecSize,
                        DCOP_PARA_NODE *pPara, DWORD dwParaCount,
                        void *pData, DWORD dwDataLen)
{
    /// ������
    if (!pFields || !dwFieldCount ||
        !pbyRec || !dwRecSize ||
        !pPara || !dwParaCount ||
        !pData || !dwDataLen)
    {
        return FAILURE;
    }

    /// ʹ��Ĭ��ֵ���(��¼��һ���ֽ��Ǳ�ʶλ)
    BYTE *pbyRecTmp = pbyRec;
    DWORD dwKeyCount = 0;
    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        /// ��ȡȱʡֵ
        DWORD defaultValue = pFields[i].m_defaultValue;

        /// �Զ�����IDentify
        if (IsIdentify(pFields[i].m_fieldType))
        {
            defaultValue = IdentifyGenerator(i + 1, 
                        pFields[i].m_fieldSize, 
                        pFields[i].m_minValue, 
                        pFields[i].m_maxValue);
        }
        else
        {
            /// ���Զ����ɵģ�ͳ�ƹؼ�������
            if (pFields[i].m_isKey)
            {
                dwKeyCount++;
            }
        }

        /// ����ȱʡֵ
        BYTE byDefValue[4];
        Bytes_SetDword(byDefValue, defaultValue);
        if (SetRecValue(pFields[i].m_fieldType, 
                        pbyRecTmp, pFields[i].m_fieldSize, 
                        byDefValue, sizeof(byDefValue)
                        ) != SUCCESS)
        {
            (void)memset(pbyRecTmp, 0, pFields[i].m_fieldSize);
        }

        pbyRecTmp += pFields[i].m_fieldSize;
    }

    /// �ٰ�������ֵ���
    DWORD dwRc = SUCCESS;
    DWORD dwDataPos = 0;
    DWORD dwKeyCountTmp = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwParaCount; ++dwParaIdx)
    {
        /// ��Ҫ�жϲ���ID����ȷ��
        if (!pPara[dwParaIdx].m_paraID || (pPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            dwRc = FAILURE;
            break;
        }

        /// ��ȡ�ֶ�����
        DWORD dwFieldIdx = pPara[dwParaIdx].m_paraID - 1;

        /// �Զ����ɵ�IDentify���ܱ�����
        if (!IsIdentify(pFields[dwFieldIdx].m_fieldType))
        {
            /// �ж����ֵĴ�С��Χ
            if (IsDigital(pFields[dwFieldIdx].m_fieldType))
            {
                DWORD dwValue = Bytes_GetDwordValue((BYTE *)pData + dwDataPos, pPara[dwParaIdx].m_paraSize);
                DWORD dwMinValue = pFields[dwFieldIdx].m_minValue;
                DWORD dwMaxValue = pFields[dwFieldIdx].m_maxValue;
                if ((dwMinValue != dwMaxValue) && ((dwValue < dwMinValue) || (dwValue > dwMaxValue)))
                {
                    dwRc = FAILURE;
                    break;
                }
            }

            /// ��������ֵ
            pbyRecTmp = pbyRec + pFields[dwFieldIdx].m_fieldOffset;
            dwRc = SetRecValue(pFields[dwFieldIdx].m_fieldType,
                            pbyRecTmp, pFields[dwFieldIdx].m_fieldSize,
                            (BYTE *)pData + dwDataPos, 
                            pPara[dwParaIdx].m_paraSize);
            if (dwRc != SUCCESS)
            {
                break;
            }

            if (pFields[dwFieldIdx].m_isKey)
            {
                dwKeyCountTmp++;
            }
        }

        /// ����ƫ��
        dwDataPos += pPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwDataLen)
        {
            dwRc = FAILURE;
            break;
        }

        /// ���ҹؼ��������ظ�
        if (pFields[dwFieldIdx].m_isKey)
        {
            void *pKeyData = DCOP_Malloc(pFields[dwFieldIdx].m_fieldSize);
            if (!pKeyData)
            {
                dwRc = FAILURE;
                break;
            }

            (void)memcpy(pKeyData, pbyRecTmp, pFields[dwFieldIdx].m_fieldSize);
            DWORD dwCount = 0;
            dwRc = CountRecord(DCOP_CONDITION_ONE, pPara + dwParaIdx, 1, 
                        pKeyData, pFields[dwFieldIdx].m_fieldSize, 
                        dwCount);
            if (!dwRc && dwCount)
            {
                /// �ҵ��ظ���¼
                DCOP_Free(pKeyData);
                dwRc = FAILURE;
                break;
            }

            DCOP_Free(pKeyData);
        }
    }

    /// �Ƚ�����ؼ����Ƿ������趨�ؼ���
    if (dwKeyCountTmp < dwKeyCount)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: IDataHandle::CopyFields
  ��    ��: �����ֶ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IDataHandle::Field *IDataHandle::CopyFields(IModel *piModel, DWORD dwAttrID, 
                        DWORD &rdwFieldCount, DWORD &rdwRecSize)
{
    if (!piModel)
    {
        return NULL;
    }

    /// ��ȡ��ģ�е��ֶ��б�
    DWORD dwFieldCount = 0;
    IModel::Field *pFields = piModel->GetFields(dwAttrID, dwFieldCount);
    if (!pFields || !dwFieldCount)
    {
        return NULL;
    }

    /// �����ֶ��б�Ŀ���
    Field *pFieldsCopy = (Field *)DCOP_Malloc(dwFieldCount * sizeof(Field));
    if (!pFieldsCopy)
    {
        return NULL;
    }

    /// ���㵥����¼��С
    DWORD dwRecSize = 0;
    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        (void)memcpy(pFieldsCopy + i, pFields + i, sizeof(IModel::Field));
        pFieldsCopy[i].m_fieldOffset = dwRecSize;
        dwRecSize += pFields[i].m_fieldSize;
    }
    if (!dwRecSize)
    {
        DCOP_Free(pFieldsCopy);
        return NULL;
    }

    rdwFieldCount = dwFieldCount;
    rdwRecSize = dwRecSize;

    return pFieldsCopy;
}

/*******************************************************
  �� �� ��: IDataHandle::CopyShips
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IModel::Relation *IDataHandle::CopyShips(IModel *piModel, 
                DWORD dwAttrID, 
                DWORD &rdwShipCount)
{
    if (!piModel)
    {
        return NULL;
    }

    /// ��ȡ��ģ�еĹ����б�
    DWORD dwShipCount = 0;
    IModel::Relation *pShips = piModel->GetShips(dwAttrID, dwShipCount);
    if (!pShips || !dwShipCount)
    {
        return NULL;
    }

    /// ���������б�Ŀ���
    IModel::Relation *pShipsCopy = (IModel::Relation *)DCOP_Malloc(dwShipCount * sizeof(IModel::Relation));
    if (!pShipsCopy)
    {
        return NULL;
    }

    (void)memcpy(pShipsCopy, pShips, dwShipCount * sizeof(IModel::Relation));

    rdwShipCount = dwShipCount;
    return pShipsCopy;
}

/*******************************************************
  �� �� ��: IDataHandle::GetOutPara
  ��    ��: ��ȡ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DCOP_PARA_NODE *IDataHandle::GetOutPara(Field *pFields, 
                        DWORD dwFieldCount, 
                        DWORD &rdwOutParaCount, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        DWORD *pdwOutDataLen, 
                        bool bSpecPara)
{
    DCOP_PARA_NODE *pOutPara = 0;
    DWORD dwOutParaCount = 0;
    DWORD dwOutDataLen = 0;
    DWORD dwSpecParaCount = 0;

    /// ��ȡȫ���ֶ�
    if (!pReqPara || !dwReqParaCount)
    {
        pOutPara = (DCOP_PARA_NODE *)DCOP_Malloc(dwFieldCount * sizeof(DCOP_PARA_NODE));
        if (!pOutPara) return NULL;
    }

    /// ���������ֶ�
    if (!pReqPara || !dwReqParaCount || bSpecPara)
    {
        for (DWORD dwFieldIdx = 0; dwFieldIdx < dwFieldCount; ++dwFieldIdx)
        {
            if (!pReqPara || !dwReqParaCount)
            {
                dwOutDataLen += pFields[dwFieldIdx].m_fieldSize;
                pOutPara[dwOutParaCount].m_paraID = dwFieldIdx + 1;
                pOutPara[dwOutParaCount].m_opCode = DCOP_OPCODE_NONE;
                pOutPara[dwOutParaCount].m_paraType = pFields[dwFieldIdx].m_fieldType;
                pOutPara[dwOutParaCount].m_paraSize = pFields[dwFieldIdx].m_fieldSize;
                dwOutParaCount++;
            }
            else
            {
                if ((pFields[dwFieldIdx].m_fieldType == IModel::FIELD_IDENTIFY) || 
                     (pFields[dwFieldIdx].m_isKey))
                {
                    dwSpecParaCount++;
                }
            }
        }
    }

    /// ��ȡȫ���ֶ�
    if (!pReqPara || !dwReqParaCount)
    {
        rdwOutParaCount = dwOutParaCount;
        if (pdwOutDataLen) *pdwOutDataLen = dwOutDataLen;
        return pOutPara;
    }

    pOutPara = (DCOP_PARA_NODE *)DCOP_Malloc((dwSpecParaCount + dwReqParaCount) * sizeof(DCOP_PARA_NODE));
    if (!pOutPara)
    {
        return NULL;
    }

    dwOutParaCount = 0;
    dwOutDataLen = 0;

    /// ��ȡ�����ֶ�
    if (dwSpecParaCount)
    {
        for (DWORD dwFieldIdx = 0; dwFieldIdx < dwFieldCount; ++dwFieldIdx)
        {
            if ((pFields[dwFieldIdx].m_fieldType == IModel::FIELD_IDENTIFY) || 
                    (pFields[dwFieldIdx].m_isKey))
            {
                dwOutDataLen += pFields[dwFieldIdx].m_fieldSize;
                pOutPara[dwOutParaCount].m_paraID = dwFieldIdx + 1;
                pOutPara[dwOutParaCount].m_opCode = DCOP_OPCODE_NONE;
                pOutPara[dwOutParaCount].m_paraType = pFields[dwFieldIdx].m_fieldType;
                pOutPara[dwOutParaCount].m_paraSize = pFields[dwFieldIdx].m_fieldSize;
                dwOutParaCount++;
                if (dwOutParaCount >= dwSpecParaCount)
                {
                    break;
                }
            }
        }
    }

    /// ָ���ֶ����
    for (DWORD dwParaIdx = 0; dwParaIdx < dwReqParaCount; ++dwParaIdx)
    {
        if (DCOP_SPECPARA(pReqPara[dwParaIdx].m_paraID, DCOP_FIELD_RELATION))
        {
            if (GetRelPara(pReqPara[dwParaIdx].m_paraID, pOutPara[dwOutParaCount]) == SUCCESS)
            {
                dwOutDataLen += pOutPara[dwOutParaCount].m_paraSize;
                dwOutParaCount++;
            }

            continue;
        }

        if (!pReqPara[dwParaIdx].m_paraID || (pReqPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            continue;
        }

        DWORD dwFieldIdx = pReqPara[dwParaIdx].m_paraID - 1;
        dwOutDataLen += pFields[dwFieldIdx].m_fieldSize;
        pOutPara[dwOutParaCount].m_paraID = dwFieldIdx + 1;
        pOutPara[dwOutParaCount].m_opCode = DCOP_OPCODE_NONE;
        pOutPara[dwOutParaCount].m_paraType = pFields[dwFieldIdx].m_fieldType;
        pOutPara[dwOutParaCount].m_paraSize = pFields[dwFieldIdx].m_fieldSize;
        dwOutParaCount++;
    }

    rdwOutParaCount = dwOutParaCount;
    if (pdwOutDataLen) *pdwOutDataLen = dwOutDataLen;
    return pOutPara;
}

/*******************************************************
  �� �� ��: IDataHandle::CopyRecord
  ��    ��: ������¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::CopyRecord(Field *pFields, DWORD dwFieldCount,
                        BYTE *pbyRec, DWORD dwRecSize,
                        DCOP_PARA_NODE *pPara, DWORD dwParaCount,
                        void *pData, DWORD dwDataLen)
{
    /// ������
    if (!pFields || !dwFieldCount ||
        !pbyRec || !dwRecSize ||
        !pPara || !dwParaCount ||
        !pData || !dwDataLen)
    {
        return FAILURE;
    }

    /// �Ӽ�¼�а�ָ���ֶν��п���
    DWORD dwRc = SUCCESS;
    DWORD dwDataPos = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwParaCount; ++dwParaIdx)
    {
        if (DCOP_SPECPARA(pPara[dwParaIdx].m_paraID, DCOP_FIELD_RELATION))
        {
            dwRc = GetRelRecord(pPara[dwParaIdx], pbyRec, (BYTE *)pData + dwDataPos);
            if (dwRc != SUCCESS)
            {
                break;
            }

            dwDataPos += pPara[dwParaIdx].m_paraSize;
            if (dwDataPos > dwDataLen)
            {
                dwRc = FAILURE;
                break;
            }

            continue;
        }

        if (!pPara[dwParaIdx].m_paraID || (pPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            continue;
        }

        /// ��ȡ�ֶ�����
        DWORD dwFieldIdx = pPara[dwParaIdx].m_paraID - 1;

        /// ��������ֵ
        BYTE *pbyRecTmp = pbyRec + pFields[dwFieldIdx].m_fieldOffset;
        dwRc = SetRecValue(pFields[dwFieldIdx].m_fieldType, 
                        (BYTE *)pData + dwDataPos, 
                        pPara[dwParaIdx].m_paraSize, 
                        pbyRecTmp, pFields[dwFieldIdx].m_fieldSize);
        if (dwRc != SUCCESS)
        {
            break;
        }

        dwDataPos += pPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwDataLen)
        {
            dwRc = FAILURE;
            break;
        }
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: IDataHandle::UpdateRecord
  ��    ��: ���¼�¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::UpdateRecord(Field *pFields, DWORD dwFieldCount,
                        BYTE *pbyRec, DWORD dwRecSize,
                        DCOP_PARA_NODE *pPara, DWORD dwParaCount,
                        void *pData, DWORD dwDataLen)
{
    DWORD dwRc = SUCCESS;
    DWORD dwReqDataPos = 0;

    /// ������
    if (!pFields || !dwFieldCount ||
        !pbyRec || !dwRecSize ||
        !pPara || !dwParaCount ||
        !pData || !dwDataLen)
    {
        return FAILURE;
    }

    /// ѭ���Ѳ������µ���¼��
    for (DWORD dwParaIdx = 0; dwParaIdx < dwParaCount; ++dwParaIdx)
    {
        if (!pPara[dwParaIdx].m_paraID || (pPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            continue;
        }

        DWORD dwFieldIdx = pPara[dwParaIdx].m_paraID - 1;
        BYTE *pbyRecTmp = pbyRec + pFields[dwFieldIdx].m_fieldOffset;
        dwRc = SetRecValue(pFields[dwFieldIdx].m_fieldType, 
                        pbyRecTmp, pFields[dwFieldIdx].m_fieldSize,
                        (BYTE *)pData + dwReqDataPos, 
                        pPara[dwParaIdx].m_paraSize);
        if (dwRc != SUCCESS)
        {
            break;
        }

        dwReqDataPos += pPara[dwParaIdx].m_paraSize;
        if (dwReqDataPos > dwDataLen)
        {
            dwRc = FAILURE;
            break;
        }
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: IDataHandle::IsDigital
  ��    ��: �Ƿ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IDataHandle::IsDigital(DWORD dwFieldType)
{
    if ((dwFieldType == IModel::FIELD_IDENTIFY) ||
        (dwFieldType == IModel::FIELD_BYTE) ||
        (dwFieldType == IModel::FIELD_WORD) ||
        (dwFieldType == IModel::FIELD_DWORD) ||
        (dwFieldType == IModel::FIELD_SHORT) ||
        (dwFieldType == IModel::FIELD_INTEGER) ||
        (dwFieldType == IModel::FIELD_NUMBER))
    {
        return true;
    }

    return false;
}

/*******************************************************
  �� �� ��: IDataHandle::IsIdentify
  ��    ��: �Ƿ��Ǳ�ʶ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IDataHandle::IsIdentify(DWORD dwFieldType)
{
    if (dwFieldType == IModel::FIELD_IDENTIFY)
    {
        return true;
    }

    return false;
}

/*******************************************************
  �� �� ��: IDataHandle::SetRecValue
  ��    ��: ����ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::SetRecValue(DWORD dwFieldType, 
                        void *pDst, DWORD dwDstSize, 
                        void *pSrc, DWORD dwSrcSize)
{
    OSASSERT(pDst != NULL);

    if (!pSrc || !dwSrcSize)
    {
        return FAILURE;
    }

    /// ��������ֵĻ����Ȼ�ȡ��DWORD�ı���ֵ��
    DWORD dwValue = 0;
    if (IsDigital(dwFieldType))
    {
        dwValue = Bytes_GetDwordValue((BYTE *)pSrc, dwSrcSize);
    }
    else if (IModel::FIELD_STRING == dwFieldType)
    {
        CDString strTmp((char *)pSrc, dwSrcSize);
        if (strTmp.Length() > dwDstSize)
        {
            return FAILURE;
        }
    }
    else
    {
        if (dwSrcSize > dwDstSize)
        {
            return FAILURE;
        }
    }

    /// �������ͽ��и�ֵ
    if (IsDigital(dwFieldType))
    {
        Bytes_SetDwordValue(dwValue, (BYTE *)pDst, dwDstSize);
    }
    else
    {
        (void)memcpy(pDst, pSrc, dwSrcSize);
        if (dwDstSize > dwSrcSize)
        {
            (void)memset((BYTE *)pDst + dwSrcSize, 0, dwDstSize - dwSrcSize);
        }
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: IDataHandle::IdentifyGenerator
  ��    ��: ID������(ѭ��+1��ֱ�����ɲ����ڵ�ID)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::IdentifyGenerator(DWORD fieldID, DWORD fieldSize, DWORD minValue, DWORD maxValue)
{
    /// ��Сֵ�����ֵ��Ȼ��߷��˾���û�����÷�Χ
    if (minValue >= maxValue)
    {
        minValue = 1;
        if (fieldSize == sizeof(BYTE))
        {
            maxValue = 0xff;
        }
        else if (fieldSize == sizeof(WORD))
        {
            maxValue = 0xffff;
        }
        else
        {
            maxValue = 0xffffffff;
        }
    }

    void *pKeyData = DCOP_Malloc(fieldSize);
    if (!pKeyData)
    {
        return 0;
    }

    DCOP_PARA_NODE findPara = {fieldID, DCOP_OPCODE_NONE, IModel::FIELD_IDENTIFY, (WORD)fieldSize};

    for (DWORD i = minValue; i < maxValue; ++i)
    {
        m_curIdentify++;
        if ((m_curIdentify > maxValue) || (m_curIdentify < minValue))
        {
            /// ������Χ������Сֵ��ʼ
            m_curIdentify = minValue;
        }

        Bytes_SetDwordValue(m_curIdentify, (BYTE *)pKeyData, fieldSize);
        DWORD dwCount = 0;
        DWORD dwRc = CountRecord(DCOP_CONDITION_ONE, &findPara, 1, 
                    pKeyData, fieldSize, 
                    dwCount);
        if (!dwRc && dwCount)
        {
            /// �ҵ��ظ���¼
            continue;
        }

        break;
    }

    DCOP_Free(pKeyData);
    
    /// ����һ�µ�ǰֵ
    SaveCurIdentify(m_curIdentify);

    return m_curIdentify;
}

/*******************************************************
  �� �� ��: IDataHandle::GetOffsetAndLimit
  ��    ��: ��ȡ��¼ƫ�ƺ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void IDataHandle::GetOffsetAndLimit(DWORD &rdwOffset, 
                DWORD &rdwLimit, 
                DCOP_PARA_NODE *pPara, 
                DWORD dwParaCount, 
                void *pData, 
                DWORD dwDataLen)
{
    rdwOffset = 0;
    rdwLimit = 0;

    DWORD dwDataPos = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwParaCount; ++dwParaIdx)
    {
        /// ��ȡ��¼ƫ�ƺ�����
        if (pPara[dwParaIdx].m_paraID == DCOP_SPECPARA_OFFSET)
        {
            rdwOffset = Bytes_GetDwordValue((BYTE *)pData + dwDataPos, 
                        pPara[dwParaIdx].m_paraSize);
        }
        else if (pPara[dwParaIdx].m_paraID == DCOP_SPECPARA_LIMIT)
        {
            rdwLimit = Bytes_GetDwordValue((BYTE *)pData + dwDataPos, 
                        pPara[dwParaIdx].m_paraSize);
        }
        else
        {
        }

        /// ƫ����һ������λ��
        dwDataPos += pPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwDataLen)
        {
            break;
        }
    }
}

/*******************************************************
  �� �� ��: IDataHandle::bMatchRecord
  ��    ��: �Ƿ�ƥ���¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IDataHandle::bMatchRecord(Field *pFields, DWORD dwFieldCount, 
                        BYTE *pbyRec, BYTE byCond, 
                        DCOP_PARA_NODE *pPara, DWORD dwParaCount, 
                        void *pData, DWORD dwDataLen)
{
    if (!pPara && !dwParaCount && !pData && !dwDataLen)
    {
        return true;
    }

    bool bRc = true;
    DWORD dwDataPos = 0;
    for (DWORD dwParaIdx = 0; dwParaIdx < dwParaCount; ++dwParaIdx)
    {
        /// �ֶ�ID����Ϊ0
        if (!pPara[dwParaIdx].m_paraID || (pPara[dwParaIdx].m_paraID > dwFieldCount))
        {
            if (byCond)
            {
                bRc = false;
                break;
            }
            continue;
        }

        /// ��ȡ�ֶ�����
        DWORD dwFieldIdx = pPara[dwParaIdx].m_paraID - 1;
        BYTE *pbyRecTmp = pbyRec + pFields[dwFieldIdx].m_fieldOffset;

        /// ��ֵƥ��
        bool bMatch = false;
        if (IsDigital(pFields[dwFieldIdx].m_fieldType))
        {
            DWORD dwParaValue = Bytes_GetDwordValue((BYTE *)pData + dwDataPos, 
                        pPara[dwParaIdx].m_paraSize);
            DWORD dwRecValue = Bytes_GetDwordValue(pbyRecTmp, 
                        pFields[dwFieldIdx].m_fieldSize);
            bMatch = bDigitalMatch(pPara[dwParaIdx].m_opCode, 
                        dwParaValue, 
                        dwRecValue);
        }
        /// �ַ���ƥ��
        else if (IModel::FIELD_STRING == pFields[dwFieldIdx].m_fieldType)
        {
            CDString strPara((char *)pData + dwDataPos, pPara[dwParaIdx].m_paraSize);
            CDString strRec((char *)pbyRecTmp, pFields[dwFieldIdx].m_fieldSize);
            bMatch = bStringMatch(pPara[dwParaIdx].m_opCode, 
                        (const char *)strPara, 
                        (const char *)strRec);
        }
        /// ����ƥ��
        else
        {
            if (pPara[dwParaIdx].m_paraSize == pFields[dwFieldIdx].m_fieldSize)
            {
                bMatch = bBufferMatch(pPara[dwParaIdx].m_opCode, 
                        (char *)pData + dwDataPos, 
                        (char *)pbyRecTmp, 
                        pFields[dwFieldIdx].m_fieldSize);
            }
        }

        /// ��ȫƥ���У������ǰ��ƥ�䣬����false
        if (byCond && !bMatch)
        {
            bRc = false;
            break;
        }

        /// ����ƥ���У������ǰƥ�䣬�򷵻�true
        if (!byCond && bMatch)
        {
            bRc = true;
            break;
        }

        /// ƫ����һ������λ��
        dwDataPos += pPara[dwParaIdx].m_paraSize;
        if (dwDataPos > dwDataLen)
        {
            bRc = false;
            break;
        }

        bRc = bMatch;
    }

    return bRc;
}

/*******************************************************
  �� �� ��: IDataHandle::bDigitalMatch
  ��    ��: �Ƿ������ַ�ʽƥ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IDataHandle::bDigitalMatch(DWORD dwOpCode, DWORD dwParaValue, DWORD dwRecValue)
{
    bool bRc = false;
    switch (dwOpCode)
    {
        case DCOP_OPCODE_EQUAL:
            bRc = (dwRecValue == dwParaValue)? true : false;
            break;
        case DCOP_OPCODE_MORE_EQUAL:
            bRc = (dwRecValue >= dwParaValue)? true : false;
            break;
        case DCOP_OPCODE_MORE:
            bRc = (dwRecValue > dwParaValue)? true : false;
            break;
        case DCOP_OPCODE_LESS:
            bRc = (dwRecValue < dwParaValue)? true : false;
            break;
        case DCOP_OPCODE_LESS_EQUAL:
            bRc = (dwRecValue <= dwParaValue)? true : false;
            break;
        case DCOP_OPCODE_NOT_EQUAL:
            bRc = (dwRecValue != dwParaValue)? true : false;
            break;
        default:
            bRc = false;
            break;
    }

    return bRc;
}

/*******************************************************
  �� �� ��: IDataHandle::bStringMatch
  ��    ��: �Ƿ����ַ�����ʽƥ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IDataHandle::bStringMatch(DWORD dwOpCode, const char *pParaStr, const char *pRecStr)
{
    bool bRc = false;
    switch (dwOpCode)
    {
        case DCOP_OPCODE_EQUAL:
            bRc = (!strcmp(pRecStr, pParaStr))? true : false;
            break;
        case DCOP_OPCODE_MORE_EQUAL:
            bRc = (strcmp(pRecStr, pParaStr) >= 0)? true : false;
            break;
        case DCOP_OPCODE_MORE:
            bRc = (strcmp(pRecStr, pParaStr) > 0)? true : false;
            break;
        case DCOP_OPCODE_LESS:
            bRc = (strcmp(pRecStr, pParaStr) < 0)? true : false;
            break;
        case DCOP_OPCODE_LESS_EQUAL:
            bRc = (strcmp(pRecStr, pParaStr) <= 0)? true : false;
            break;
        case DCOP_OPCODE_NOT_EQUAL:
            bRc = (strcmp(pRecStr, pParaStr))? true : false;
            break;
        case DCOP_OPCODE_INCLUDE:
            bRc = (strstr(pRecStr, pParaStr))? true : false;
            break;
        case DCOP_OPCODE_NOT_INCLUDE:
            bRc = (!strstr(pRecStr, pParaStr))? true : false;
            break;
        default:
            bRc = false;
            break;
    }

    return bRc;
}

/*******************************************************
  �� �� ��: IDataHandle::bBufferMatch
  ��    ��: �Ƿ���Buf��ʽƥ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IDataHandle::bBufferMatch(DWORD dwOpCode, void *pParaBuf, void *pRecBuf, DWORD dwBufLen)
{
    bool bRc = false;
    switch (dwOpCode)
    {
        case DCOP_OPCODE_EQUAL:
            bRc = (!memcmp(pRecBuf, pParaBuf, dwBufLen))? true : false;
            break;
        case DCOP_OPCODE_MORE_EQUAL:
            bRc = (memcmp(pRecBuf, pParaBuf, dwBufLen) >= 0)? true : false;
            break;
        case DCOP_OPCODE_MORE:
            bRc = (memcmp(pRecBuf, pParaBuf, dwBufLen) > 0)? true : false;
            break;
        case DCOP_OPCODE_LESS:
            bRc = (memcmp(pRecBuf, pParaBuf, dwBufLen) < 0)? true : false;
            break;
        case DCOP_OPCODE_LESS_EQUAL:
            bRc = (memcmp(pRecBuf, pParaBuf, dwBufLen) <= 0)? true : false;
            break;
        case DCOP_OPCODE_NOT_EQUAL:
            bRc = (memcmp(pRecBuf, pParaBuf, dwBufLen))? true : false;
            break;
        default:
            bRc = false;
            break;
    }

    return bRc;
}

/*******************************************************
  �� �� ��: IDataHandle::GetRelation
  ��    ��: ��ȡ���������Ժ��ֶ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IModel::Relation *IDataHandle::GetRelation(DWORD dwRelID)
{
    if (!DCOP_SPECPARA(dwRelID, DCOP_FIELD_RELATION))
    {
        return NULL;
    }

    if (!m_piModel || !m_dwFieldCount || !m_pShips || !m_dwShipCount)
    {
        return NULL;
    }

    DWORD dwFieldID = (DWORD)((dwRelID & DCOP_FIELD_LOW1) >> 8);
    if (!dwFieldID || (dwFieldID > m_dwFieldCount))
    {
        return NULL;
    }

    IModel::Relation *pShip = NULL;
    for (DWORD i = 0; i < m_dwShipCount; ++i)
    {
        if (m_pShips[i].m_relID == dwFieldID)
        {
            pShip = &(m_pShips[i]);
            break;
        }
    }

    return pShip;
}

/*******************************************************
  �� �� ��: IDataHandle::GetRelPara
  ��    ��: ��ȡ�����Ĳ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::GetRelPara(DWORD dwRelID, DCOP_PARA_NODE &rPara)
{
    IModel::Relation *pShip = GetRelation(dwRelID);
    if (!pShip)
    {
        return FAILURE;
    }

    DWORD dwFieldCount = 0;
    IModel::Field *pFields = m_piModel->GetFields(pShip->m_attrID, dwFieldCount);
    if (!pFields || !dwFieldCount)
    {
        return FAILURE;
    }

    DWORD dwRelFieldID = (DWORD)(dwRelID & DCOP_FIELD_LOW0);
    if (!dwRelFieldID || (dwRelFieldID > dwFieldCount))
    {
        return FAILURE;
    }

    rPara.m_paraID = dwRelID;
    rPara.m_opCode = DCOP_OPCODE_NONE;
    rPara.m_paraType = pFields[dwRelFieldID - 1].m_fieldType;
    rPara.m_paraSize = pFields[dwRelFieldID - 1].m_fieldSize;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: IDataHandle::GetRelRecord
  ��    ��: ��ȡ�����ļ�¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IDataHandle::GetRelRecord(DCOP_PARA_NODE &rPara, BYTE *pbyRec, void *pData)
{
    if (!pbyRec || !pData || !m_piData || !m_pFields)
    {
        return FAILURE;
    }

    IModel::Relation *pShip = GetRelation(rPara.m_paraID);
    if (!pShip)
    {
        return FAILURE;
    }

    if (!pShip->m_relID || (pShip->m_relID > m_dwFieldCount))
    {
        return FAILURE;
    }

    BYTE *pbyRecTmp = pbyRec + m_pFields[pShip->m_relID - 1].m_fieldOffset;
    if (!pbyRecTmp)
    {
        return FAILURE;
    }

    CData *piData = (CData *)m_piData;
    if (!piData)
    {
        return FAILURE;
    }

    /// ��ȡ���ݾ��
    IData::Handle hData = piData->GetHandle(pShip->m_attrID);
    if (!hData)
    {
        return FAILURE;
    }

    /// ���������ֶ�(�������ֶ�����������������һ�µ�)
    DCOP_PARA_NODE condPara;
    condPara.m_paraID = pShip->m_fieldID;
    condPara.m_opCode = DCOP_OPCODE_NONE;
    condPara.m_paraType = m_pFields[pShip->m_relID - 1].m_fieldType;
    condPara.m_paraSize = m_pFields[pShip->m_relID - 1].m_fieldSize;

    /// ���������ֶ�(��ѯ���ֶ������ѻ�ȡ��ֱ�������)
    DCOP_PARA_NODE reqPara;
    reqPara.m_paraID = (DWORD)(rPara.m_paraID & DCOP_FIELD_LOW0);
    reqPara.m_opCode = DCOP_OPCODE_NONE;
    reqPara.m_paraType = rPara.m_paraType;
    reqPara.m_paraSize = rPara.m_paraSize;

    /// ���в�ѯ(������Ψһֵ���м���)
    DCOP_PARA_NODE *pRspPara = NULL;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;
    DWORD dwRc = piData->QueryRecord(hData, DCOP_CONDITION_ONE, 
                        &condPara, 1, 
                        pbyRecTmp, condPara.m_paraSize, 
                        &reqPara, 1, 
                        pRspPara, dwRspParaCount, 
                        aRspData);
    if (dwRc == SUCCESS)
    {
        if (pRspPara) DCOP_Free(pRspPara);
        dwRc = SetRecValue(rPara.m_paraType, 
                        pData, rPara.m_paraSize, 
                        aRspData.Pos(0), aRspData.Size());
    }
    if (dwRc != SUCCESS)
    {
        (void)memset(pData, 0, rPara.m_paraSize);
    }

    return SUCCESS;
}

