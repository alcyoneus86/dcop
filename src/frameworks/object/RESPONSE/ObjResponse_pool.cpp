/// -------------------------------------------------
/// ObjResponse_pool.cpp : ��Ӧ�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjResponse_pool.h"
#include "Manager_if.h"
#include "Factory_if.h"
#include "ObjAttribute_if.h"


/*******************************************************
  �� �� ��: CResponsePool::CResponsePool
  ��    ��: CResponsePool���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CResponsePool::CResponsePool()
{
    m_pOwner = 0;
    m_pNodeBuf = 0;
    m_dwNodeCount = 0;
    m_dwLastIdx = 0;
    m_piDispatch = 0;
    m_pTimerWheel = 0;
}

/*******************************************************
  �� �� ��: CResponsePool::~CResponsePool
  ��    ��: CResponsePool��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CResponsePool::~CResponsePool()
{
    m_pOwner = 0;

    if (m_pNodeBuf)
    {
        /// �ͷŶ����еĻỰ��Ϣ�Ͷ�ʱ�����
        for (DWORD i = 0; i < m_dwNodeCount; ++i)
        {
            if (m_pNodeBuf[i].m_pSession)
            {
                DCOP_Free(m_pNodeBuf[i].m_pSession);
                m_pNodeBuf[i].m_pSession = 0;
            }
            if (m_pNodeBuf[i].m_hTimer)
            {
                DelFromWheel(m_pNodeBuf[i].m_hTimer);
                ITimer::IWheel::Free(m_pNodeBuf[i].m_hTimer);
                m_pNodeBuf[i].m_hTimer = 0;
            }
        }
    
        delete [] m_pNodeBuf;
        m_pNodeBuf = 0;
    }

    if (m_pTimerWheel)
    {
        delete m_pTimerWheel;
        m_pTimerWheel = 0;
    }

    DCOP_RELEASE_INSTANCE_REFER(m_pOwner, m_piDispatch);
}

/*******************************************************
  �� �� ��: CResponsePool::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CResponsePool::Init(IObject *root, IObject *owner, DWORD count)
{
    if (m_pNodeBuf || !root || !count)
    {
        return FAILURE;
    }

    DCOP_QUERY_OBJECT_REFER(IDispatch, DCOP_OBJECT_DISPATCH, root, owner, m_piDispatch);
    if (!m_piDispatch)
    {
        return FAILURE;
    }

    m_pTimerWheel = ITimer::IWheel::CreateInstance(WHEEL_S_SEC_ID, 
                        WHEEL_S_SEC_SLOT_COUNT, 
                        NULL, NULL, 
                        WHEEL_S_HASH_BASE, 
                        OnTimeout, this);
    if (!m_pTimerWheel)
    {
        return FAILURE;
    }

    Node *pNode = new Node[count];
    if (!pNode)
    {
        return FAILURE;
    }

    (void)memset(pNode, 0, count * sizeof(Node));

    m_pOwner      = owner;
    m_pNodeBuf    = pNode;
    m_dwNodeCount = count;

    m_dwLastIdx = 1;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CResponsePool::GetNode
  ��    ��: ��ȡ�ڵ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CResponsePool::Node *CResponsePool::GetNode(DWORD dwIdx)
{
    if (!m_pNodeBuf || !m_dwNodeCount)
    {
        return NULL;
    }

    if (!dwIdx || (dwIdx > m_dwNodeCount))
    {
        return NULL;
    }

    return &(m_pNodeBuf[dwIdx - 1]);
}

/*******************************************************
  �� �� ��: CResponsePool::GenIdleIdx
  ��    ��: ���ɿ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CResponsePool::GenIdleIdx()
{
    if (!m_pNodeBuf || !m_dwNodeCount || !m_dwLastIdx)
    {
        return 0;
    }

    DWORD dwStartIdx = m_dwLastIdx;
    if ((++m_dwLastIdx) > m_dwNodeCount) m_dwLastIdx = 1;

    while (m_dwLastIdx != dwStartIdx)
    {
        if (!(m_pNodeBuf[m_dwLastIdx - 1].m_wNewIdx))
        {
            return m_dwLastIdx;
        }

        if ((++m_dwLastIdx) > m_dwNodeCount) m_dwLastIdx = 1;
    }

    return 0;
}

/*******************************************************
  �� �� ��: CResponsePool::OnReq
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CResponsePool::OnReq(DCOP_SESSION_HEAD *pSession,
                        DWORD dwMsgType,
                        DWORD dwSrcID,
                        DWORD dwDstID,
                        DWORD dwRspMsgType,
                        DWORD dwTimeout,
                        DWORD dwSendTryTimes)
{
    if (!pSession || (pSession->m_ack != DCOP_REQ) || !dwTimeout || !dwSendTryTimes)
    {
        return FAILURE;
    }

    /// ���ƻỰ��Ϣ���Ա��ش�
    DWORD dwSessCopyLen = pSession->m_type.m_headSize + Bytes_GetWord((BYTE *)&(pSession->m_type.m_valueLen));
    DCOP_SESSION_HEAD *pSessCopy = (DCOP_SESSION_HEAD *)DCOP_Malloc(dwSessCopyLen);
    if (!dwSessCopyLen)
    {
        return FAILURE;
    }

    /// ��ȡһ�����е�����
    DWORD dwReqIdx = GenIdleIdx();
    Node *pNode = GetNode(dwReqIdx);
    if (!pNode)
    {
        DCOP_Free(pSessCopy);
        return FAILURE;
    }

    /// �ж������Ƿ���Ŀ���
    if (pNode->m_wNewIdx)
    {
        DCOP_Free(pSessCopy);
        return FAILURE;
    }

    /// ������ʱ��
    TimerValue timerValue = {(WORD)dwReqIdx, Bytes_GetWord((BYTE *)&(pSession->m_index)), dwTimeout, dwSendTryTimes};
    ITimer::Handle hTimer = ITimer::IWheel::Alloc(&timerValue, sizeof(timerValue));
    if (!hTimer)
    {
        DCOP_Free(pSessCopy);
        return FAILURE;
    }

    DWORD dwRc = InsertToWheel(hTimer, dwTimeout);
    if (dwRc != SUCCESS)
    {
        DCOP_Free(pSessCopy);
        ITimer::IWheel::Free(hTimer);
        return dwRc;
    }

    /// �ѻỰ��Ϣ�е����������µ�����
    /// ���ű���Ự����������
    pNode->m_wNewIdx = (WORD)dwReqIdx;
    pNode->m_wOldIdx = Bytes_GetWord((BYTE *)&(pSession->m_index));
    Bytes_SetWord((BYTE *)&(pSession->m_index), (WORD)dwReqIdx);
    (void)memcpy(pSessCopy, pSession, dwSessCopyLen);

    pNode->m_pSession = pSessCopy;
    pNode->m_dwMsgType = dwMsgType;
    pNode->m_dwSrcID = dwSrcID;
    pNode->m_dwDstID = dwDstID;
    pNode->m_dwRspMsgType = dwRspMsgType;
    pNode->m_hTimer = hTimer;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CResponsePool::OnRsp
  ��    ��: ������Ӧʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CResponsePool::OnRsp(DCOP_SESSION_HEAD *pSession)
{
    if (!pSession || !DCOP_RSP(pSession->m_ack))
    {
        return FAILURE;
    }

    /// �����Ӧ��Ϣ�е�����
    DWORD dwReqIdx = Bytes_GetWord((BYTE *)&(pSession->m_index));
    Node *pNode = GetNode(dwReqIdx);
    if (!pNode)
    {
        return FAILURE;
    }

    /// �ж������Ƿ���ȷ(����ת�����ֵ��ֱ�ӱȽ�)
    if ((pNode->m_wNewIdx != dwReqIdx) ||
        (!(pNode->m_pSession)) ||
        (!(pNode->m_hTimer)) ||
        (pNode->m_pSession->m_session != pSession->m_session) ||
        (pNode->m_pSession->m_user != pSession->m_user) ||
        (pNode->m_pSession->m_tty != pSession->m_tty) ||
        (pNode->m_pSession->m_attribute != pSession->m_attribute))
    {
        return FAILURE;
    }

    /// ȡ��ԭ��������
    Bytes_SetWord((BYTE *)&(pSession->m_index), pNode->m_wOldIdx);

    /// ��Ӧ�����Ļ��ȷ��سɹ���ֻ�н�����Ӧ�Ŵ�������ɾ������
    if (pSession->m_ack == DCOP_RSP_CON)
    {
        return SUCCESS;
    }

    /// ͣ����ɾ����ʱ��
    DelFromWheel(pNode->m_hTimer);
    ITimer::IWheel::Free(pNode->m_hTimer);

    /// ɾ������ĻỰ��Ϣ
    DCOP_Free(pNode->m_pSession);
    (void)memset(pNode, 0, sizeof(Node));

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CResponsePool::OnTick
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponsePool::OnTick()
{
    AutoObjLock(m_pOwner);

    if (!m_pTimerWheel)
    {
        return;
    }

    m_pTimerWheel->OnTick();
}

/*******************************************************
  �� �� ��: CResponsePool::OnTimeout
  ��    ��: ��Ӧ��ʱʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponsePool::OnTimeout(ITimer::Handle handle, void *para)
{
    if (!handle || !para)
    {
        return;
    }

    CResponsePool *pThis = (CResponsePool *)para;
    TimerValue *pValue = (TimerValue *)ITimer::IWheel::GetValue(handle);
    DWORD dwTrySendTimes = pThis->TrySendTimes(pValue, handle);
    if (dwTrySendTimes)
    {
        /// �����Դ���������������Ϣ�Ѿ����ͳ�ȥ�ˣ������ͷž��
        return;
    }

    /// ���Դ����Ѿ�Ϊ0�����߷�������ʧ�ܣ�ֻ���ͷž��
    ITimer::IWheel::Free(handle);
}

/*******************************************************
  �� �� ��: CResponsePool::InsertToWheel
  ��    ��: ���뵽��ʱ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CResponsePool::InsertToWheel(ITimer::Handle hTimer, DWORD dwTimeout)
{
    if (dwTimeout)
    {
        ITimer::IWheel::SetTimeBase(hTimer, dwTimeout);
    }

    AutoObjLock(m_pOwner);

    if (!m_pTimerWheel)
    {
        return FAILURE;
    }

    return m_pTimerWheel->Add(hTimer);
}

/*******************************************************
  �� �� ��: CResponsePool::DelFromWheel
  ��    ��: �Ӷ�ʱ������ɾ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponsePool::DelFromWheel(ITimer::Handle hTimer)
{
    AutoObjLock(m_pOwner);

    if (!m_pTimerWheel)
    {
        return;
    }

    m_pTimerWheel->Del(hTimer);
}

/*******************************************************
  �� �� ��: CResponsePool::TrySendTimes
  ��    ��: �������Է���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CResponsePool::TrySendTimes(TimerValue *pValue, ITimer::Handle hTimer)
{
    if (!m_piDispatch || !pValue || !hTimer)
    {
        return 0;
    }

    /// ��ȡ��ʱ���е�����
    DWORD dwReqIdx = pValue->m_wNewIdx;
    Node *pNode = GetNode(dwReqIdx);
    if (!pNode)
    {
        return 0;
    }

    /// ���ڵ��Ƿ���ȷ(�����Ǳ��˵�Node)
    if ((pNode->m_wNewIdx != dwReqIdx) ||
        (pNode->m_wOldIdx != pValue->m_wOldIdx))
    {
        return 0;
    }

    DCOP_SESSION_HEAD *pSession = 0;
    DWORD dwSessCopyLen = 0;
    objMsg *pMsg = 0;
    DWORD dwRc = SUCCESS;

    /// ��黺�桢�����ط�����
    ERROR_CHECK(pNode->m_pSession != NULL);
    ERROR_CHECK(pValue->m_dwSendTryTimes > 0);

    /// ������Ϣ
    pSession = pNode->m_pSession;
    dwSessCopyLen = pSession->m_type.m_headSize + Bytes_GetWord((BYTE *)&(pSession->m_type.m_valueLen));
    pMsg = DCOP_LoadMsg(dwSessCopyLen, pNode->m_dwMsgType, pNode->m_dwSrcID, pSession, dwSessCopyLen);
    ERROR_CHECK(pMsg != NULL);

    /// ������ʱ��
    dwRc = InsertToWheel(hTimer);
    ERROR_CHECK(dwRc == SUCCESS);

    /// ������Ϣ
    pMsg->MsgHead().m_dwDstID = pNode->m_dwDstID;
    dwRc = m_piDispatch->Send(pMsg);
    if (dwRc != SUCCESS) delete pMsg;
    ERROR_CHECK(dwRc == SUCCESS);

    /// �������Դ���(��������)
    return (pValue->m_dwSendTryTimes--);


ERROR_LABEL :

    /// �������
    if (pNode->m_pSession)
    {
        OnRspTimeout(pNode);
        DCOP_Free(pNode->m_pSession);
        pNode->m_pSession = 0;
    }

    /// ɾ���ڵ�
    (void)memset(pNode, 0, sizeof(Node));

    return 0;
}

/*******************************************************
  �� �� ��: CResponsePool::OnRspTimeout
  ��    ��: ��Ӧ��ʱ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponsePool::OnRspTimeout(Node *pNode)
{
    if (!pNode || !pNode->m_pSession)
    {
        return;
    }

    CAttribute::PACK_SESS_HEAD sessHead(pNode->m_pSession);
    sessHead.m_ack = DCOP_RSP_END;

    CAttribute::PACK_RSP_HEAD rspHead;
    rspHead.m_retCode = TIMEOUT;

    CDStream sRspParaData;
    CAttribute::PACK_MSG_NODE packNodes[] = 
    {
        {(DCOP_MSG_HEAD *)&rspHead, NULL, 0, sRspParaData},
    };

    (void)IObjectMember::PackMsg(m_piDispatch, NULL, DCOP_OBJECT_RESPONSE, 
                        pNode->m_dwSrcID, pNode->m_dwRspMsgType, 
                        &sessHead, packNodes, ARRAY_SIZE(packNodes));
}

