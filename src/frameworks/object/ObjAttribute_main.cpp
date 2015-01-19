/// -------------------------------------------------
/// ObjAttribute_main.cpp : ��������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjAttribute_main.h"
#include "cpu/bytes.h"
#include "sock.h"


/*******************************************************
  �� �� ��: IObjectMember::Action
  ��    ��: Action
  ��    ��: pMsg            - ԭʼ��Ϣ
            sessionHead     - ��ԭʼ��Ϣ�н����ĻỰ��Ϣͷ
            aCondHeads      - ��ԭʼ��Ϣ�н�����������Ϣͷ
            pReqPara        - ��ԭʼ��Ϣ�н�������������б�
            dwReqParaCount  - ��ԭʼ��Ϣ�н����������������
            pReqData        - ��ԭʼ��Ϣ�н�������������
            dwReqDataLen    - ��ԭʼ��Ϣ�н������������ݳ���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void IObjectMember::Action(objMsg *pMsg, 
                        const DCOP_SESSION_HEAD &sessionHead, 
                        const CDArray &aCondHeads, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen)
{
    if (!pMsg)
    {
        CHECK_RETCODE(FAILURE, "Session Msg Error!");
        return;
    }

    CAttribute *pAttr = GetAttribute();
    if (!pAttr)
    {
        CHECK_RETCODE(FAILURE, "Attr Type Error!");
        return;
    }

    /// ׼����Ӧ����
    DWORD dwRc = SUCCESS;
    CAttribute::PACK_RSP_HEAD rspHead;
    CDArray aRspData;

    /// ���ݻỰ�������ͽ��в���
    switch (sessionHead.m_ctrl)
    {
        case DCOP_CTRL_SET:
            dwRc = Set(pReqData, dwReqDataLen);
            break;
        case DCOP_CTRL_GET:
        {
            DWORD dwRspDataLen = 0;
            void *pRspData = Get(dwRspDataLen);
            if (!pRspData)
            {
                dwRc = FAILURE;
                break;
            }
            aRspData.SetNodeSize(dwRspDataLen);
            dwRc = aRspData.Append(pRspData);
            if (dwRc != SUCCESS)
            {
                break;
            }
            rspHead.m_paraLen = (WORD)aRspData.Size();
        }
            break;
        default:
            dwRc = FAILURE;
            break;
    }

    /// ��Ӧ������
    rspHead.m_retCode = dwRc;
    rspHead.m_recordCount = 1;
    Notify((DCOP_MSG_HEAD *)&rspHead, aRspData, (DCOP_SESSION_HEAD *)&sessionHead, pMsg->GetSrcID());
}

/*******************************************************
  �� �� ��: IObjectMember::Notify
  ��    ��: Notify
  ��    ��: pHead           - ���͵���Ϣͷ
            aData           - ���͵������б�
            pSessionHead    - ֮ǰ�ĻỰ��Ϣ(�¼��޴�ͷ)
            dwDstID         - ���͵�Ŀ����Ϣ(�¼��޴�ͷ)
            pPara           - ���͵Ĳ����б�(����)
            dwParaCount     - ���͵Ĳ�������(����)
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void IObjectMember::Notify(DCOP_MSG_HEAD *pHead, 
                        const CDArray &aData, 
                        DCOP_SESSION_HEAD *pSessionHead, 
                        DWORD dwDstID, 
                        DCOP_PARA_NODE *pPara, 
                        DWORD dwParaCount)
{
    if (!pHead)
    {
        CHECK_RETCODE(FAILURE, "Notify Head Error!");
        return;
    }

    IObject *pOwner = GetOwner();
    CAttribute *pAttr = GetAttribute();
    IDispatch *pDispatch = GetDispatch();
    if (!pOwner || !pAttr || !pDispatch)
    {
        CHECK_RETCODE(FAILURE, "Obj Attr Error!");
        return;
    }

    CAttribute::PACK_SESS_HEAD sessHead(pSessionHead);
    sessHead.m_attribute = pAttr->GetID();
    DWORD dwMsgType;

    if (pHead->m_headType == DCOP_MSG_HEAD_EVENT)
    {
        /// ���¼��Ļ�����Ҫ���ж��Ƿ����˶���
        INotify::IPool *pPool = GetNotifyPool();

        if (!pPool || (pPool->OnPublish(sessHead.m_attribute) != SUCCESS))
            return;

        /// �¼��Ĺ̶�Ŀ�ĵ���notify���
        dwDstID = DCOP_OBJECT_NOTIFY;

        /// �����¼���ʶ
        if (!pSessionHead) sessHead.m_ctrl = DCOP_CTRL_EVENT;
        sessHead.m_ack = DCOP_EVT;
        dwMsgType = DCOP_MSG_OBJECT_EVENT;
    }
    else
    {
        /// ������Ӧ��ʶ
        sessHead.m_ack = DCOP_RSP_END;
        dwMsgType = DCOP_MSG_OBJECT_RESPONSE;
    }

    /// ��װ��Ϣ������Ŀ�ĵ�
    DWORD dwRc = PackMsg(pDispatch, pOwner->ID(), dwDstID, dwMsgType, 
                        &sessHead, pHead, 
                        pPara, dwParaCount, 
                        aData);

    CHECK_RETCODE(dwRc, "Send Notify Msg Error!");
}

/*******************************************************
  �� �� ��: CObjectMember<IData*>::Action
  ��    ��: Action
  ��    ��: pMsg            - ԭʼ��Ϣ
            sessionHead     - ��ԭʼ��Ϣ�н����ĻỰ��Ϣͷ
            aCondHeads      - ��ԭʼ��Ϣ�н�����������Ϣͷ
            pReqPara        - ��ԭʼ��Ϣ�н�������������б�
            dwReqParaCount  - ��ԭʼ��Ϣ�н����������������
            pReqData        - ��ԭʼ��Ϣ�н�������������
            dwReqDataLen    - ��ԭʼ��Ϣ�н������������ݳ���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CObjectMember<IData*>::Action(objMsg *pMsg, 
                        const DCOP_SESSION_HEAD &sessionHead, 
                        const CDArray &aCondHeads, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen)
{
    if (!pMsg)
    {
        CHECK_RETCODE(FAILURE, "Session Msg Error!");
        return;
    }

    CAttribute *pAttr = GetAttribute();
    if (!pAttr || (pAttr->GetType() != IModel::TYPE_DATA))
    {
        CHECK_RETCODE(FAILURE, "Attr Type Error!");
        return;
    }

    IData *pData = GetData();
    if (!pData)
    {
        CHECK_RETCODE(FAILURE, "Get Data Error!");
        return;
    }

    /// ׼����������
    DWORD dwRc = SUCCESS;
    DCOP_CONDITION_HEAD *pCondHead = (DCOP_CONDITION_HEAD *)aCondHeads.Pos(0);
    void *pCondParaData = (pCondHead)? *(void **)(pCondHead + 1) : NULL;
    DWORD dwCondParaCount = (pCondHead)? pCondHead->m_paraCount : 0;
    DWORD dwCondDataLen = (pCondHead)? pCondHead->m_paraLen : 0;
    CDStream sCondPara(dwCondParaCount * sizeof(DCOP_PARA_NODE));
    DCOP_PARA_NODE *pCondPara = (DCOP_PARA_NODE *)sCondPara.Buffer();
    void *pCondData = NULL;

    /// ׼�������Ӧ
    CAttribute::PACK_RSP_HEAD rspHead;
    DCOP_PARA_NODE *pRspPara = NULL;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;

    /// �������ݶ�����д���
    switch (sessionHead.m_ctrl)
    {
        case DCOP_CTRL_CREATE:
            dwRc = Create(&sessionHead);
            break;
        case DCOP_CTRL_DESTROY:
            dwRc = Destroy(&sessionHead);
            break;
        case DCOP_CTRL_ADD:
            dwRc = AddRecord(pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, 
                        &sessionHead);
            break;
        case DCOP_CTRL_DEL:
            pCondData = GetMsgParaData(pCondParaData, 
                        dwCondParaCount, dwCondDataLen, 
                        pCondPara);
            dwRc = DelRecord((pCondHead)? pCondHead->m_condition : 0, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        &sessionHead);
            break;
        case DCOP_CTRL_SET:
            pCondData = GetMsgParaData(pCondParaData, 
                        dwCondParaCount, dwCondDataLen, 
                        pCondPara);
            dwRc = EditRecord((pCondHead)? pCondHead->m_condition : 0, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, 
                        &sessionHead);
            break;
        case DCOP_CTRL_GET:
            pCondData = GetMsgParaData(pCondParaData, 
                        dwCondParaCount, dwCondDataLen, 
                        pCondPara);
            if (IsCountReq(pReqPara, dwReqParaCount))
            {
                dwRc = pData->CountRecord(GetDataHandle(), 
                        (pCondHead)? pCondHead->m_condition : 0, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        rspHead.m_recordCount);
            }
            else
            {
                dwRc = pData->QueryRecord(GetDataHandle(), 
                        (pCondHead)? pCondHead->m_condition : 0, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        pReqPara, dwReqParaCount, 
                        pRspPara, dwRspParaCount, 
                        aRspData);
            }
            break;
        default:
            dwRc = FAILURE;
            break;
    }

    /// ��Ӧ������
    rspHead.m_retCode = dwRc;
    Notify((DCOP_MSG_HEAD *)&rspHead, aRspData, (DCOP_SESSION_HEAD *)&sessionHead, pMsg->GetSrcID(), pRspPara, dwRspParaCount);
    if (pRspPara) DCOP_Free(pRspPara);
}

/*******************************************************
  �� �� ��: CObjectMember<IData*>::Create
  ��    ��: Create
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectMember<IData*>::Create(const DCOP_SESSION_HEAD *pSessionHead)
{
    CAttribute *pAttr = GetAttribute();
    if (!pAttr || (pAttr->GetType() != IModel::TYPE_DATA))
    {
        return FAILURE;
    }

    IData *pData = GetData();
    if (!pData)
    {
        return FAILURE;
    }

    DWORD dwRc = pData->Create(GetDataType(), pAttr->GetID(), GetOwner(), m_hData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// �ϱ��¼�
    CAttribute::PACK_SESS_HEAD sessHead(pSessionHead);
    sessHead.m_ctrl = DCOP_CTRL_CREATE;
    CAttribute::PACK_EVT_HEAD evtHead;
    CDArray aEvtData;

    Notify((DCOP_MSG_HEAD *)&evtHead, aEvtData, &sessHead);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectMember<IData*>::Destroy
  ��    ��: Destroy
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectMember<IData*>::Destroy(const DCOP_SESSION_HEAD *pSessionHead)
{
    CAttribute *pAttr = GetAttribute();
    if (!pAttr || (pAttr->GetType() != IModel::TYPE_DATA))
    {
        return FAILURE;
    }

    IData *pData = GetData();
    if (!pData)
    {
        return FAILURE;
    }

    DWORD dwRc = pData->Destroy(pAttr->GetID());
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// �ϱ��¼�
    CAttribute::PACK_SESS_HEAD sessHead(pSessionHead);
    sessHead.m_ctrl = DCOP_CTRL_DESTROY;
    CAttribute::PACK_EVT_HEAD evtHead;
    CDArray aEvtData;

    Notify((DCOP_MSG_HEAD *)&evtHead, aEvtData, &sessHead);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectMember<IData*>::AddRecord
  ��    ��: AddRecord
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectMember<IData*>::AddRecord(DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen, 
                        const DCOP_SESSION_HEAD *pSessionHead)
{
    IData *pData = GetData();
    if (!pData)
    {
        return FAILURE;
    }

    /// ׼������¼�
    CAttribute::PACK_SESS_HEAD sessHead(pSessionHead);
    sessHead.m_ctrl = DCOP_CTRL_ADD;
    CAttribute::PACK_EVT_HEAD evtHead;
    DCOP_PARA_NODE *pEvtPara = NULL;
    DWORD dwEvtParaCount = 0;
    CDArray aEvtData;

    DWORD dwRc = pData->AddRecord(GetDataHandle(), 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, 
                        &pEvtPara, &dwEvtParaCount, 
                        &aEvtData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// �ϱ��¼�
    Notify((DCOP_MSG_HEAD *)&evtHead, aEvtData, &sessHead, 0, pEvtPara, dwEvtParaCount);
    if (pEvtPara) DCOP_Free(pEvtPara);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectMember<IData*>::DelRecord
  ��    ��: DelRecord
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectMember<IData*>::DelRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, 
                        DWORD dwCondParaCount, 
                        void *pCondData, 
                        DWORD dwCondDataLen, 
                        const DCOP_SESSION_HEAD *pSessionHead)
{
    IData *pData = GetData();
    if (!pData)
    {
        return FAILURE;
    }

    /// ׼������¼�
    CAttribute::PACK_SESS_HEAD sessHead(pSessionHead);
    sessHead.m_ctrl = DCOP_CTRL_DEL;
    CAttribute::PACK_EVT_HEAD evtHead;
    DCOP_PARA_NODE *pEvtPara = NULL;
    DWORD dwEvtParaCount = 0;
    CDArray aEvtData;

    DWORD dwRc = pData->DelRecord(GetDataHandle(), 
                        byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        &pEvtPara, &dwEvtParaCount, 
                        &aEvtData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// �ϱ��¼�
    Notify((DCOP_MSG_HEAD *)&evtHead, aEvtData, &sessHead, 0, pEvtPara, dwEvtParaCount);
    if (pEvtPara) DCOP_Free(pEvtPara);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectMember<IData*>::EditRecord
  ��    ��: EditRecord
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectMember<IData*>::EditRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, 
                        DWORD dwCondParaCount, 
                        void *pCondData, 
                        DWORD dwCondDataLen, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen, 
                        const DCOP_SESSION_HEAD *pSessionHead)
{
    IData *pData = GetData();
    if (!pData)
    {
        return FAILURE;
    }

    /// ׼������¼�
    CAttribute::PACK_SESS_HEAD sessHead(pSessionHead);
    sessHead.m_ctrl = DCOP_CTRL_SET;
    CAttribute::PACK_EVT_HEAD evtHead;
    DCOP_PARA_NODE *pEvtPara = NULL;
    DWORD dwEvtParaCount = 0;
    CDArray aEvtData;

    DWORD dwRc = pData->EditRecord(GetDataHandle(), 
                        byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, 
                        &pEvtPara, &dwEvtParaCount, 
                        &aEvtData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// �ϱ��¼�
    Notify((DCOP_MSG_HEAD *)&evtHead, aEvtData, &sessHead, 0, pEvtPara, dwEvtParaCount);
    if (pEvtPara) DCOP_Free(pEvtPara);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectMember<CMethod>::Action
  ��    ��: Action
  ��    ��: pMsg            - ԭʼ��Ϣ
            sessionHead     - ��ԭʼ��Ϣ�н����ĻỰ��Ϣͷ
            aCondHeads      - ��ԭʼ��Ϣ�н�����������Ϣͷ
            pReqPara        - ��ԭʼ��Ϣ�н�������������б�
            dwReqParaCount  - ��ԭʼ��Ϣ�н����������������
            pReqData        - ��ԭʼ��Ϣ�н�������������
            dwReqDataLen    - ��ԭʼ��Ϣ�н������������ݳ���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CObjectMember<CMethod>::Action(objMsg *pMsg, 
                        const DCOP_SESSION_HEAD &sessionHead, 
                        const CDArray &aCondHeads, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen)
{
    CMethod::ACTION action = m_method.GetAction();
    if (!action)
    {
        IObjectMember::Action(pMsg, sessionHead, aCondHeads, pReqPara, dwReqParaCount, pReqData, dwReqDataLen);
        return;
    }

    (*action)(GetOwner(), pMsg, sessionHead, aCondHeads, pReqPara, dwReqParaCount, pReqData, dwReqDataLen);
}

/*******************************************************
  �� �� ��: CObjectMemberIndex::CObjectMemberIndex
  ��    ��: CObjectMemberIndex����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CObjectMemberIndex::CObjectMemberIndex()
{
    m_ppIndexTable = 0;
    m_dwIndexCount = 0;
}

/*******************************************************
  �� �� ��: CObjectMemberIndex::CObjectMemberIndex
  ��    ��: CObjectMemberIndex����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CObjectMemberIndex::~CObjectMemberIndex()
{
    if (m_ppIndexTable)
    {
        DCOP_Free(m_ppIndexTable);
        m_ppIndexTable = 0;
    }
    m_dwIndexCount = 0;
}

/*******************************************************
  �� �� ��: CObjectMemberIndex::Init
  ��    ��: CObjectMemberIndex��ʼ��
  ��    ��: ppMembers
            dwCount
            pDispatch
            pNotifyPool
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectMemberIndex::Init(IObjectMember **ppMembers, DWORD dwCount, IDispatch *pDispatch, INotify::IPool *pNotifyPool)
{
    IObjectMember **ppTmp = (IObjectMember **)DCOP_Malloc(sizeof(IObjectMember *) * dwCount);
    if (!ppTmp)
    {
        return FAILURE;
    }

    (void)memcpy(ppTmp, ppMembers, sizeof(IObjectMember *) * dwCount);

    for (DWORD i = 0; i < dwCount; ++i)
    {
        if (ppTmp[i] == NULL)
        {
            DCOP_Free(ppTmp);
            return FAILURE;
        }

        ppTmp[i]->SetDispatch(pDispatch);
        ppTmp[i]->SetNotifyPool(pNotifyPool);
    }

    m_ppIndexTable = ppTmp;
    m_dwIndexCount = dwCount;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectMemberIndex::Dispatch
  ��    ��: �ַ���Ϣ
  ��    ��: msg
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CObjectMemberIndex::Dispatch(objMsg *pMsg)
{
    if (!m_ppIndexTable || !m_dwIndexCount)
    {
        return;
    }

    if (!pMsg)
    {
        return;
    }

    /// ��ȡ��Ϣͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(pMsg->GetDataBuf(), pMsg->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    for (DWORD i = 0; i < aSessHeads.Count(); ++i)
    {
        /// ��ȡ�Ựͷ
        DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(i);
        if (!pSessionHead)
        {
            continue;
        }

        /// ��ȡ����ֵ
        DWORD dwAttrID = pSessionHead->m_attribute;
        if (!dwAttrID)
        {
            continue;
        }

        /// ֻ��������
        if (pSessionHead->m_ack != DCOP_REQ)
        {
            continue;
        }

        /// ��ȡ�Ự����
        void *pSessionData = *(void **)(pSessionHead + 1);

        /// ��ȡ�������
        CDArray aCondHeads;
        CDArray aReqHeads;
        IObjectMember::GetMsgHead(pSessionData, pSessionHead->m_type.m_valueLen, 0, &aCondHeads, &aReqHeads, 0, 0);
        DCOP_REQUEST_HEAD *pReqHead = (DCOP_REQUEST_HEAD *)aReqHeads.Pos(0);
        void *pReqParaData = (pReqHead)? *(void **)(pReqHead + 1) : NULL;
        DWORD dwReqParaCount = (pReqHead)? pReqHead->m_paraCount : 0;
        DWORD dwReqDataLen = (pReqHead)? pReqHead->m_paraLen : 0;
        CDStream sReqPara(dwReqParaCount * sizeof(DCOP_PARA_NODE));
        DCOP_PARA_NODE *pReqPara = (DCOP_PARA_NODE *)sReqPara.Buffer();
        void *pReqData = IObjectMember::GetMsgParaData(pReqParaData, dwReqParaCount, dwReqDataLen, pReqPara);

        /// �ַ���ָ������
        for (DWORD i = 0; i < m_dwIndexCount; ++i)
        {
            /// ��Ӧ�����������������ֵ
            if (!(m_ppIndexTable[i]))
            {
                continue;
            }

            /// ��Ӧ������ֵ����ƥ��
            CAttribute *pAttr = m_ppIndexTable[i]->GetAttribute();
            if (!pAttr || (pAttr->GetID() != dwAttrID))
            {
                continue;
            }

            /// ���ô���ӿڽ��д���
            m_ppIndexTable[i]->Action(pMsg, *pSessionHead, aCondHeads, pReqPara, dwReqParaCount, pReqData, dwReqDataLen);
            break;
        }
    }
}

/*******************************************************
  �� �� ��: ParseMsg
  ��    ��: ������Ϣ
  ��    ��: pBuf
            dwLen
            rdwOffset
  ��    ��: rdwOffset
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DCOP_MSG_HEAD *ParseMsg(void *pBuf, DWORD dwLen, DWORD &rdwOffset)
{
    if (!pBuf)
    {
        return NULL;
    }

    if (!dwLen || (dwLen <= rdwOffset))
    {
        return NULL;
    }

    if (!CheckMsgHead((BYTE *)pBuf + rdwOffset, dwLen - rdwOffset))
    {
        return NULL;
    }

    DCOP_MSG_HEAD *pHead = (DCOP_MSG_HEAD *)((BYTE *)pBuf + rdwOffset);
    rdwOffset += pHead->m_headSize + Bytes_GetWord((BYTE *)&(pHead->m_valueLen));

    return pHead;
}

/*******************************************************
  �� �� ��: CheckMsgHead
  ��    ��: �����Ϣͷ
  ��    ��: pBuf
            dwLen
  ��    ��: 
  ��    ��: �Ƿ���Ϣͷ
  �޸ļ�¼: 
 *******************************************************/
bool CheckMsgHead(void *pBuf, DWORD dwLen)
{
    if (dwLen < sizeof(DCOP_MSG_HEAD))
    {
        return false;
    }

    DCOP_MSG_HEAD *pHead = (DCOP_MSG_HEAD *)pBuf;
    if (!pHead)
    {
        return false;
    }

    if (pHead->m_headType >= DCOP_MSG_HEAD_COUNT)
    {
        return false;
    }

    if (pHead->m_headSize != DCOP_MSG_HEAD_SIZE[pHead->m_headType])
    {
        return false;
    }

    if (dwLen < (DWORD)(pHead->m_headSize + Bytes_GetWord((BYTE *)&(pHead->m_valueLen))))
    {
        return false;
    }

    return true;
}

/*******************************************************
  �� �� ��: IObjectMember::GetMsgHead
  ��    ��: ��ȡָ�����͵���Ϣͷ
  ��    ��: pBuf        - ������ָ��
            dwLen       - ����������
  ��    ��: aHeads      - ��Ϣͷ����
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void IObjectMember::GetMsgHead(void *pBuf, DWORD dwLen, 
                        CDArray *paSessHeads, 
                        CDArray *paCondHeads, 
                        CDArray *paReqHeads, 
                        CDArray *paRspHeads, 
                        CDArray *paEvtHeads)
{
    DWORD dwOffset = 0;
    DCOP_MSG_HEAD *pHead = NULL;
    while ((pHead = ParseMsg(pBuf, dwLen, dwOffset)) != NULL)
    {
        /// ������������Ϣͷ
        CDArray *paHeads = NULL;
        switch (pHead->m_headType)
        {
            case DCOP_MSG_HEAD_SESSION:
                if (paSessHeads) paHeads = paSessHeads;
                break;
            case DCOP_MSG_HEAD_CONDITION:
                if (paCondHeads) paHeads = paCondHeads;
                break;
            case DCOP_MSG_HEAD_REQUEST:
                if (paReqHeads) paHeads = paReqHeads;
                break;
            case DCOP_MSG_HEAD_RESPONSE:
                if (paRspHeads) paHeads = paRspHeads;
                break;
            case DCOP_MSG_HEAD_EVENT:
                if (paEvtHeads) paHeads = paEvtHeads;
                break;
            default:
                break;
        }

        /// ��̬�����еĽڵ�����'��Ϣͷ'+'����ָ��'���
        /// '��Ϣͷ'��ֱ�ӿ���������Ϣͷ���Ѿ������ֽ�ת��
        /// '����ָ��'��ֱ��ָ��ԭʼ��Ϣ������Ϣͷ����Ĳ�����������
        if (paHeads)
        {
            /// д��'��Ϣͷ'����̬�������
            paHeads->SetNodeSize(pHead->m_headSize + sizeof(void *));
            if (paHeads->Append(pHead, pHead->m_headSize) != SUCCESS) continue;

            /// ��ȡ��̬�������ڵ�
            void *pNode = paHeads->Pos(paHeads->Count() - 1);
            if (!pNode) continue;

            /// ת���ֽ���
            ChangeMsgHeadOrder((DCOP_MSG_HEAD *)pNode);

            /// ����Ϣͷ����'����ָ��'���뵽��̬����ڵ����
            if (pHead->m_valueLen)
            {
                *(void **)((BYTE *)pNode + pHead->m_headSize) = (BYTE *)pHead + pHead->m_headSize;
            }
        }
    }
}

/*******************************************************
  �� �� ��: IObjectMember::GetMsgParaData
  ��    ��: ��ȡ��Ϣ����������
  ��    ��: pMsgParaData    - ��Ϣ����
            dwParaCount     - ��������
            dwDataLen       - ��������
            pPara           - �����ڴ�
  ��    ��: pPara           - �������
  ��    ��: �������
  �޸ļ�¼: 
 *******************************************************/
void *IObjectMember::GetMsgParaData(void *pMsgParaData, 
                        DWORD dwParaCount, 
                        DWORD dwDataLen, 
                        DCOP_PARA_NODE *pPara)
{
    if (!pMsgParaData)
    {
        return NULL;
    }

    if (pPara && dwParaCount)
    {
        (void)memcpy(pPara, pMsgParaData, dwParaCount * sizeof(DCOP_PARA_NODE));
        BYTES_CHANGE_PARA_NODE_ORDER(pPara, dwParaCount);
    }

    return (dwDataLen)? ((BYTE *)pMsgParaData + dwParaCount * sizeof(DCOP_PARA_NODE)) : NULL;
}

/*******************************************************
  �� �� ��: ChangeMsgHeadOrder
  ��    ��: �ı���Ϣͷ�ֽ���
  ��    ��: pHead       - ��Ϣͷ
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void ChangeMsgHeadOrder(DCOP_MSG_HEAD *pHead)
{
    if (!pHead) return;

    switch (pHead->m_headType)
    {
        case DCOP_MSG_HEAD_SESSION:
        {
            BYTES_CHANGE_SESSION_HEAD_ORDER(pHead);
        }
            break;
        case DCOP_MSG_HEAD_CONDITION:
        {
            BYTES_CHANGE_CONDITION_HEAD_ORDER(pHead);
        }
            break;
        case DCOP_MSG_HEAD_REQUEST:
        {
            BYTES_CHANGE_REQUEST_HEAD_ORDER(pHead);
        }
            break;
        case DCOP_MSG_HEAD_RESPONSE:
        {
            BYTES_CHANGE_RESPONSE_HEAD_ORDER(pHead);
        }
            break;
        case DCOP_MSG_HEAD_EVENT:
        {
            BYTES_CHANGE_EVENT_HEAD_ORDER(pHead);
        }
            break;
        default:
            break;
    }
}

/*******************************************************
  �� �� ��: AddMsgParaData
  ��    ��: ������Ϣ����������
  ��    ��: pHead       - ��Ϣͷ
            dwParaCount - ��������
            dwDataLen   - ��������
            pPara       - ����
            pData       - ����
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void AddMsgParaData(DCOP_MSG_HEAD *pHead, 
                DWORD dwParaCount, DWORD dwDataLen, 
                DCOP_PARA_NODE *pPara, void *pData)
{
    if (!pHead) return;

    if ((pHead->m_headType == DCOP_MSG_HEAD_SESSION) || 
        (pHead->m_headType >= DCOP_MSG_HEAD_COUNT))
    {
        return;
    }

    if (!pPara) dwParaCount = 0;
    if (!pData) dwDataLen = 0;

    if (pHead->m_valueLen < (dwParaCount * sizeof(DCOP_PARA_NODE) + dwDataLen))
    {
        return;
    }

    BYTE *pbyBuf = (BYTE *)pHead + pHead->m_headSize;
    DWORD dwOffset = 0;

    /// ���ݲ���������Ӳ���
    DCOP_PARA_NODE *pParaWritten = (dwParaCount)? (DCOP_PARA_NODE *)pbyBuf : NULL;
    if (pParaWritten)
    {
        (void)memcpy(pParaWritten, pPara, dwParaCount * sizeof(DCOP_PARA_NODE));
        BYTES_CHANGE_PARA_NODE_ORDER(pParaWritten, dwParaCount);
        dwOffset += dwParaCount * sizeof(DCOP_PARA_NODE);
    }

    /// �������ݳ����������
    void *pDataWritten = (dwDataLen)? (pbyBuf + dwOffset) : NULL;
    if (pDataWritten)
    {
        (void)memcpy(pDataWritten, pData, dwDataLen);
        dwOffset += dwDataLen;
    }

    switch (pHead->m_headType)
    {
        case DCOP_MSG_HEAD_CONDITION:
        {
            DCOP_CONDITION_HEAD *pCondHead = (DCOP_CONDITION_HEAD *)pHead;
            pCondHead->m_paraCount = (BYTE)dwParaCount;
            pCondHead->m_paraLen = (WORD)dwDataLen;
        }
            break;
        case DCOP_MSG_HEAD_REQUEST:
        {
            DCOP_REQUEST_HEAD *pReqHead = (DCOP_REQUEST_HEAD *)pHead;
            pReqHead->m_paraCount = (WORD)dwParaCount;
            pReqHead->m_paraLen = (WORD)dwDataLen;
        }
            break;
        case DCOP_MSG_HEAD_RESPONSE:
        {
            DCOP_RESPONSE_HEAD *pRspHead = (DCOP_RESPONSE_HEAD *)pHead;
            pRspHead->m_paraCount = (WORD)dwParaCount;
            pRspHead->m_paraLen = (WORD)dwDataLen;
        }
            break;
        case DCOP_MSG_HEAD_EVENT:
        {
            DCOP_EVENT_HEAD *pEvtHead = (DCOP_EVENT_HEAD *)pHead;
            pEvtHead->m_paraCount = (WORD)dwParaCount;
            pEvtHead->m_paraLen = (WORD)dwDataLen;
        }
            break;
        default:
            break;
    }

    pHead->m_valueLen = (WORD)dwOffset;
}

/*******************************************************
  �� �� ��: IsCountReq
  ��    ��: �Ƿ���������
            (�������true,��������ڽ��м�¼����ͳ��)
  ��    ��: pReqPara        - �����ֶ�
            dwReqParaCount  - �ֶ�����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IsCountReq(DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount)
{
    if (!pReqPara)
    {
        return false;
    }

    for (DWORD i = 0; i < dwReqParaCount; ++i)
    {
        if (pReqPara[i].m_paraID == DCOP_SPECPARA_COUNT)
        {
            return true;
        }
    }

    return false;
}

/*******************************************************
  �� �� ��: InitMsgHead
  ��    ��: ��ʼ��ָ�����͵���Ϣͷ
  ��    ��: pBuf        - δ��ʼ����Buf
            dwLen       - Buf����
            byHeadType  - ��Ҫ��ʼ����ͷ������
            pHeadCopy   - ��Ҫ��������Ϣͷ
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void InitMsgHead(void *pBuf, DWORD dwLen, BYTE byHeadType, const DCOP_MSG_HEAD *pHeadCopy)
{
    if (!pBuf || (byHeadType >= DCOP_MSG_HEAD_COUNT) || 
        (dwLen < sizeof(DCOP_MSG_HEAD)) || 
        (dwLen < DCOP_MSG_HEAD_SIZE[byHeadType]))
    {
        return;
    }

    (void)memset(pBuf, 0, dwLen);

    DCOP_MSG_HEAD *pHead = (DCOP_MSG_HEAD *)pBuf;
    pHead->m_headType = byHeadType;
    pHead->m_headSize = DCOP_MSG_HEAD_SIZE[byHeadType];
    pHead->m_valueLen = (WORD)(dwLen - pHead->m_headSize);
    if (pHeadCopy)
    {
        (void)memcpy(pHead + 1, pHeadCopy + 1, 
                        pHead->m_headSize - sizeof(DCOP_MSG_HEAD));
    }
}

/*******************************************************
  �� �� ��: IObjectMember::PackMsg
  ��    ��: �����Ϣ
  ��    ��: pDispatch       - �ַ���
            dwSrcID         - Դ��ַ
            dwDstID         - Ŀ�ĵ�ַ
            dwMsgType       - ��Ϣ����
            pSessionHead    - �Ự��Ϣͷ
            pMsgHead        - ������Ϣͷ
            pMsgPara        - ��Ϣ��������
            dwMsgParaCount  - ��Ϣ��������
            aMsgParaData    - ��Ϣ�����б�
            dwSendTryTimes  - �������Դ���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IObjectMember::PackMsg(IDispatch *pDispatch, 
                        DWORD dwSrcID, 
                        DWORD dwDstID, 
                        DWORD dwMsgType, 
                        DCOP_SESSION_HEAD *pSessionHead, 
                        DCOP_MSG_HEAD *pMsgHead, 
                        DCOP_PARA_NODE *pMsgPara, 
                        DWORD dwMsgParaCount, 
                        const CDArray &aMsgParaData, 
                        DWORD dwSendTryTimes)
{
    if (!pDispatch || !dwDstID || !pSessionHead || !pMsgHead)
    {
        /// �����������
        return FAILURE;
    }

    if (!pMsgPara) dwMsgParaCount = 0;

    /////////////////////////////////////////////////
    /// ������һ����Ϣ�����ռ���󳤶Ȳ��ܳ���MTU
    /////////////////////////////////////////////////
    DWORD dwMTU = pDispatch->GetMTU();
    DWORD dwMsgParaLen = (aMsgParaData.Count())? aMsgParaData.Size() : 0;
    DWORD dwLen = pSessionHead->m_type.m_headSize + 
                        pMsgHead->m_headSize * ((aMsgParaData.Count())? aMsgParaData.Count() : 1) + 
                        dwMsgParaCount * sizeof(DCOP_PARA_NODE) + 
                        dwMsgParaLen * aMsgParaData.Count();
    if (dwLen > dwMTU) dwLen = dwMTU;

    if ((pSessionHead->m_type.m_headSize + 
            pMsgHead->m_headSize + 
            dwMsgParaCount * sizeof(DCOP_PARA_NODE) + 
            dwMsgParaLen) > dwLen)
    {
        /// �Ų���һ����¼
        return FAILURE;
    }

    objMsg *pMsg = DCOP_CreateMsg(dwLen, dwMsgType, dwSrcID);
    if (!pMsg) return FAILURE;

    DCOP_SESSION_HEAD *pBuf = (DCOP_SESSION_HEAD *)pMsg->GetDataBuf();
    if (!pBuf)
    {
        delete pMsg;
        return FAILURE;
    }

    InitMsgHead(pBuf, dwLen, DCOP_MSG_HEAD_SESSION, (DCOP_MSG_HEAD *)pSessionHead);
    DWORD dwRc = SUCCESS;

    /////////////////////////////////////////////////
    /// �հ��������̷���
    /////////////////////////////////////////////////
    if (!aMsgParaData.Count())
    {
        DCOP_MSG_HEAD *pHead = (DCOP_MSG_HEAD *)(pBuf + 1);
        InitMsgHead(pHead, dwLen - pSessionHead->m_type.m_headSize, pMsgHead->m_headType, pMsgHead);
        AddMsgParaData(pHead, dwMsgParaCount, 0, pMsgPara, 0);
        ChangeMsgHeadOrder(pHead);

        if (DCOP_RSP(pBuf->m_ack)) pBuf->m_ack = DCOP_RSP_END;
        pBuf->m_count = 1;
        pBuf->m_type.m_valueLen = (WORD)(pHead->m_headSize);
        BYTES_CHANGE_SESSION_HEAD_ORDER(pBuf);
        pMsg->MsgHead().m_dwDataLen = sizeof(DCOP_SESSION_HEAD) + pHead->m_headSize;
        dwRc = DispatchMsg(pDispatch, dwDstID, pMsg, dwSendTryTimes);
        if (dwRc) delete pMsg;
        return dwRc;
    }

    /////////////////////////////////////////////////
    /// �������ѭ���������Ϣ����ʱ����
    /////////////////////////////////////////////////
    objMsg *pMsgSend = NULL;
    DCOP_SESSION_HEAD *pBufSend = NULL;
    DCOP_MSG_HEAD *pHeadSend = NULL;
    DWORD dwOffset = 0;
    DWORD dwSendCount = 0;
    for (DWORD i = 0; i < aMsgParaData.Count(); ++i)
    {
        /// ʣ�µ������޷�����һ����¼��ֻ�н��з���
        if ((dwOffset + pMsgHead->m_headSize + dwMsgParaLen) > dwLen)
        {
            if (DCOP_RSP(pBufSend->m_ack)) pBufSend->m_ack = DCOP_RSP_CON;
            pBufSend->m_count = (WORD)dwSendCount;
            pBufSend->m_type.m_valueLen = (WORD)(dwOffset - sizeof(DCOP_SESSION_HEAD));
            BYTES_CHANGE_SESSION_HEAD_ORDER(pBufSend);
            pMsgSend->MsgHead().m_dwDataLen = dwOffset;
            dwRc = DispatchMsg(pDispatch, dwDstID, pMsgSend, dwSendTryTimes);
            if (dwRc) delete pMsgSend;
            pMsgSend = 0;
        }

        /// û����Ϣ�ʹ�ԭʼ��Ϣ����
        if (!pMsgSend)
        {
            pMsgSend = DCOP_CloneMsg(pMsg);
            if (!pMsgSend)
            {
                dwRc = FAILURE;
                break;
            }

            /// ���еļ�¼������ͬ���͵ģ���˻Ựͷ��һ����
            pBufSend = (DCOP_SESSION_HEAD *)pMsgSend->GetDataBuf();
            if (!pBufSend)
            {
                delete pMsgSend;
                dwRc = FAILURE;
                break;
            }

            dwOffset = sizeof(DCOP_SESSION_HEAD);
            dwSendCount = 0;
        }

        /// ��ʼ����Ϣͷ
        pHeadSend = (DCOP_MSG_HEAD *)((BYTE *)pBufSend + dwOffset);
        InitMsgHead(pHeadSend, pMsgHead->m_headSize + 
                        ((!dwSendCount)? dwMsgParaCount : 0) * sizeof(DCOP_PARA_NODE) + 
                        dwMsgParaLen, pMsgHead->m_headType, pMsgHead);

        /// ��Ӳ���
        AddMsgParaData((DCOP_MSG_HEAD *)pHeadSend, 
                        (!dwSendCount)? dwMsgParaCount : 0, 
                        dwMsgParaLen, 
                        (!dwSendCount)? pMsgPara : NULL, 
                        aMsgParaData.Pos(i));

        /// �������Ӧ�����¼�����Ҫ��¼���������
        if (pMsgHead->m_headType == DCOP_MSG_HEAD_RESPONSE)
        {
            DCOP_RESPONSE_HEAD *pRspHead = (DCOP_RESPONSE_HEAD *)pHeadSend;
            pRspHead->m_recordCount = aMsgParaData.Count();
            pRspHead->m_recordIndex = i;
        }
        if (pMsgHead->m_headType == DCOP_MSG_HEAD_EVENT)
        {
            DCOP_EVENT_HEAD *pEvtHead = (DCOP_EVENT_HEAD *)pHeadSend;
            pEvtHead->m_recordCount = aMsgParaData.Count();
            pEvtHead->m_recordIndex = i;
        }

        dwOffset += pHeadSend->m_headSize + pHeadSend->m_valueLen;
        ChangeMsgHeadOrder(pHeadSend);
        ++dwSendCount;
    }

    /////////////////////////////////////////////////
    /// ������������
    /////////////////////////////////////////////////
    if (pMsgSend)
    {
        if (DCOP_RSP(pBufSend->m_ack)) pBufSend->m_ack = DCOP_RSP_END;
        pBufSend->m_count = (WORD)dwSendCount;
        pBufSend->m_type.m_valueLen = (WORD)(dwOffset - sizeof(DCOP_SESSION_HEAD));
        BYTES_CHANGE_SESSION_HEAD_ORDER(pBufSend);
        pMsgSend->MsgHead().m_dwDataLen = dwOffset;
        dwRc = DispatchMsg(pDispatch, dwDstID, pMsgSend, dwSendTryTimes);
        if (dwRc) delete pMsgSend;
    }

    delete pMsg;
    return dwRc;
}

/*******************************************************
  �� �� ��: IObjectMember::PackMsg
  ��    ��: �����Ϣ
  ��    ��: pDispatch       - �ַ���
            dwSrcID         - Դ��ַ
            dwDstID         - Ŀ�ĵ�ַ
            dwMsgType       - ��Ϣ����
            pSessionHead    - �Ự��Ϣͷ
            pPackNode       - ����ڵ�ָ��
            dwPackNodeCount - ����ڵ����
            pReqPool        - ���󻺳��
            dwRspMsgType    - ��Ӧ��Ϣ����
            dwTimeout       - ��Ӧ��ʱʱ��
            dwSendTryTimes  - �������Դ���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IObjectMember::PackMsg(IDispatch *pDispatch, 
                        objMsg **ppOutMsg, 
                        DWORD dwSrcID, 
                        DWORD dwDstID, 
                        DWORD dwMsgType, 
                        DCOP_SESSION_HEAD *pSessionHead, 
                        CAttribute::PACK_MSG_NODE *pPackNode, 
                        DWORD dwPackNodeCount, 
                        IResponse::IPool *pReqPool, 
                        DWORD dwRspMsgType, 
                        DWORD dwTimeout, 
                        DWORD dwSendTryTimes)
{
    if (!pDispatch && !ppOutMsg) return FAILURE;
    if (!pSessionHead) return FAILURE;
    if (dwPackNodeCount && !pPackNode) return FAILURE;

    /////////////////////////////////////////////////
    /// ��ȡ����ڵ���ܳ���
    /////////////////////////////////////////////////
    DWORD dwPackNodeTotalLen = 0;
    for (DWORD i = 0; i < dwPackNodeCount; ++i)
    {
        /// �ڵ�ͷ����Ϊ��
        if (!(pPackNode[i].m_pMsgHead)) continue;
        if (!(pPackNode[i].m_dwMsgParaCount) && !(pPackNode[i].m_sMsgParaData.Length()))
        {
            continue;
        }

        /// ������ܳ���
        dwPackNodeTotalLen += pPackNode[i].m_pMsgHead->m_headSize + 
                        pPackNode[i].m_dwMsgParaCount * sizeof(DCOP_PARA_NODE) + 
                        pPackNode[i].m_sMsgParaData.Length();
    }

    /////////////////////////////////////////////////
    /// ������Ϣ�����ռ���󳤶Ȳ��ܳ���MTU
    /////////////////////////////////////////////////
    DWORD dwLen = pSessionHead->m_type.m_headSize + dwPackNodeTotalLen;
    if (pDispatch && (dwLen > pDispatch->GetMTU()))
    {
        return FAILURE;
    }

    objMsg *pMsg = DCOP_CreateMsg(dwLen, dwMsgType, dwSrcID);
    if (!pMsg) return FAILURE;

    DCOP_SESSION_HEAD *pBuf = (DCOP_SESSION_HEAD *)pMsg->GetDataBuf();
    if (!pBuf)
    {
        delete pMsg;
        return FAILURE;
    }

    InitMsgHead(pBuf, dwLen, DCOP_MSG_HEAD_SESSION, (DCOP_MSG_HEAD *)pSessionHead);
    DWORD dwRc = SUCCESS;

    /// �հ����̷���
    if (!dwPackNodeTotalLen)
    {
        pBuf->m_count = 0;
        pBuf->m_type.m_valueLen = 0;
        BYTES_CHANGE_SESSION_HEAD_ORDER(pBuf);
        pMsg->MsgHead().m_dwDataLen = sizeof(DCOP_SESSION_HEAD);
        pMsg->MsgHead().m_dwDstID = dwDstID;

        if (pDispatch)
        {
            dwRc = DispatchMsg(pDispatch, dwDstID, pMsg, dwSendTryTimes, 
                        pReqPool, dwRspMsgType, dwTimeout);
            if (dwRc) delete pMsg;
            return dwRc;
        }

        *ppOutMsg = pMsg;
        return SUCCESS;
    }

    /////////////////////////////////////////////////
    /// ���������뵽��Ϣ����
    /////////////////////////////////////////////////
    DCOP_MSG_HEAD *pHead = (DCOP_MSG_HEAD *)(pBuf + 1);
    DWORD dwOffset = sizeof(DCOP_SESSION_HEAD);
    DWORD dwSendCount = 0;
    for (DWORD i = 0; i < dwPackNodeCount; ++i)
    {
        /// �ڵ�ͷ����Ϊ��
        if (!(pPackNode[i].m_pMsgHead)) continue;
        if (!(pPackNode[i].m_dwMsgParaCount) && !(pPackNode[i].m_sMsgParaData.Length()))
        {
            continue;
        }

        DWORD dwPackNodeLen = pPackNode[i].m_pMsgHead->m_headSize + 
                        pPackNode[i].m_dwMsgParaCount * sizeof(DCOP_PARA_NODE) + 
                        pPackNode[i].m_sMsgParaData.Length();

        InitMsgHead(pHead, dwPackNodeLen, pPackNode[i].m_pMsgHead->m_headType, pPackNode[i].m_pMsgHead);
        AddMsgParaData(pHead, pPackNode[i].m_dwMsgParaCount, pPackNode[i].m_sMsgParaData.Length(), 
                        pPackNode[i].m_pMsgPara, pPackNode[i].m_sMsgParaData.Buffer());

        ChangeMsgHeadOrder(pHead);
        pHead = (DCOP_MSG_HEAD *)((BYTE *)pHead + dwPackNodeLen);
        dwOffset += dwPackNodeLen;
        ++dwSendCount;
    }

    #if 0
    PrintBuffer(STR_FORMAT("<Send Req Msg> len:%d type:0x%x src:%d, dst:%d", 
                        pMsg->GetDataLen(), pMsg->GetMsgType(), pMsg->GetSrcID(), pMsg->GetDstID()), 
                        pMsg->GetDataBuf(), pMsg->GetDataLen(), printf, 0);
    #endif

    pBuf->m_count = (WORD)dwSendCount;
    pBuf->m_type.m_valueLen = (WORD)(dwOffset - sizeof(DCOP_SESSION_HEAD));
    BYTES_CHANGE_SESSION_HEAD_ORDER(pBuf);
    pMsg->MsgHead().m_dwDataLen = dwOffset;
    pMsg->MsgHead().m_dwDstID = dwDstID;
    
    if (pDispatch)
    {
        
        dwRc = DispatchMsg(pDispatch, dwDstID, pMsg, dwSendTryTimes, 
                        pReqPool, dwRspMsgType, dwTimeout);
        if (dwRc) delete pMsg;
        return dwRc;
    }

    *ppOutMsg = pMsg;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CAttribute::PACK_SESS_HEAD
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CAttribute::PACK_SESS_HEAD::PACK_SESS_HEAD(const DCOP_SESSION_HEAD *pSessHead)
{
    InitMsgHead(this, sizeof(DCOP_SESSION_HEAD), DCOP_MSG_HEAD_SESSION, (DCOP_MSG_HEAD *)pSessHead);
    m_ver = DCOP_SESSION_VER;
}

/*******************************************************
  �� �� ��: CAttribute::PACK_COND_HEAD
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CAttribute::PACK_COND_HEAD::PACK_COND_HEAD()
{
    InitMsgHead(this, sizeof(DCOP_CONDITION_HEAD), DCOP_MSG_HEAD_CONDITION);
}

/*******************************************************
  �� �� ��: CAttribute::PACK_REQ_HEAD
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CAttribute::PACK_REQ_HEAD::PACK_REQ_HEAD()
{
    InitMsgHead(this, sizeof(DCOP_REQUEST_HEAD), DCOP_MSG_HEAD_REQUEST);
}

/*******************************************************
  �� �� ��: CAttribute::PACK_RSP_HEAD
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CAttribute::PACK_RSP_HEAD::PACK_RSP_HEAD()
{
    InitMsgHead(this, sizeof(DCOP_RESPONSE_HEAD), DCOP_MSG_HEAD_RESPONSE);
}

/*******************************************************
  �� �� ��: CAttribute::PACK_EVT_HEAD
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CAttribute::PACK_EVT_HEAD::PACK_EVT_HEAD()
{
    InitMsgHead(this, sizeof(DCOP_EVENT_HEAD), DCOP_MSG_HEAD_EVENT);
}

/*******************************************************
  �� �� ��: DispatchMsg
  ��    ��: �ַ���Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD DispatchMsg(IDispatch *pDispatch, DWORD dwDstID, objMsg *pMsg, 
                        DWORD dwSendTryTimes, IResponse::IPool *pReqPool, 
                        DWORD dwRspMsgType, DWORD dwTimeout)
{
    if (!pMsg) return FAILURE;

    if (!dwSendTryTimes) dwSendTryTimes = SEND_TRY_TIMES_DEFAULT;

    DWORD dwRc = FAILURE;
    DWORD dwHaveTryTimes = 0;
    while ((dwRc != SUCCESS) && (dwHaveTryTimes++ < dwSendTryTimes))
    {
        if (pReqPool)
        {
            DCOP_SESSION_HEAD *pBuf = (DCOP_SESSION_HEAD *)pMsg->GetDataBuf();
            if (!pBuf)
            {
                return FAILURE;
            }

            if (pBuf->m_ack == DCOP_REQ)
            {
                if (!dwRspMsgType) dwRspMsgType = DCOP_MSG_OBJECT_RESPONSE;
                if (!dwTimeout) dwTimeout = SEND_TIMEOUT_DEFAULT;
                dwRc = pReqPool->OnReq(pBuf, pMsg->GetMsgType(), pMsg->GetSrcID(), 
                        dwDstID, dwRspMsgType, dwTimeout, dwSendTryTimes);
                if (dwRc)
                {
                    continue;
                }
            }
        }

        pMsg->MsgHead().m_dwDstID = dwDstID;
        dwRc = pDispatch->Send(pMsg);
    }

    return dwRc;
}
