/// -------------------------------------------------
/// ObjNotify_pool.cpp : ֪ͨ�¼������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjNotify_pool.h"
#include "string/tablestring.h"
#include "BaseMessage.h"


/*******************************************************
  �� �� ��: CNotifyPool::CNotifyPool
  ��    ��: CNotifyPool���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CNotifyPool::CNotifyPool()
{
    m_pOwner = 0;
    m_pEventsBuf = 0;
    m_dwEventCount = 0;
    m_fnGetMsgEvent = 0;
}

/*******************************************************
  �� �� ��: CNotifyPool::~CNotifyPool
  ��    ��: CNotifyPool��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CNotifyPool::~CNotifyPool()
{
    m_pOwner = 0;
    
    if (m_pEventsBuf)
    {
        delete [] m_pEventsBuf;
        m_pEventsBuf = 0;
    }

    m_dwEventCount = 0;
}

/*******************************************************
  �� �� ��: CNotifyPool::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CNotifyPool::Init(IObject *owner, DWORD *events, DWORD count, INotify::GET_MSG_EVENT fnGetMsgEvent)
{
    if (m_pEventsBuf || !events || !count)
    {
        return FAILURE;
    }

    Node *pNode = new Node[count];
    if (!pNode)
    {
        return FAILURE;
    }

    (void)memset(pNode, 0, count * sizeof(Node));

    for (DWORD i = 0; i < count; ++i)
    {
        pNode[i].m_dwEvent = events[i];
        pNode[i].m_dwSubscribedCount = 0;
        pNode[i].m_dwProducedCount = 0;
        pNode[i].m_dwDispatchedCount = 0;
    }

    m_pOwner        = owner;
    m_pEventsBuf    = pNode;
    m_dwEventCount  = count;
    m_fnGetMsgEvent = fnGetMsgEvent;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CNotifyPool::OnSubscribe
  ��    ��: ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CNotifyPool::OnSubscribe(DWORD event)
{
    AutoObjLock(m_pOwner);

    if (!m_pEventsBuf)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// �ҳ��¼������ñ����ı�־
    /// ���û�б����Ļ���û���ҵ��¼�������ʧ��
    /////////////////////////////////////////////////
    DWORD dwRc = FAILURE;
    for (DWORD i = 0; i < m_dwEventCount; ++i)
    {
        if (m_pEventsBuf[i].m_dwEvent == event)
        {
            if (m_pEventsBuf[i].m_dwSubscribedCount != (DWORD)(-1))
            {
                m_pEventsBuf[i].m_dwSubscribedCount++;
            }

            dwRc = SUCCESS;
            break;
        }
    }

    return dwRc;
}


/*******************************************************
  �� �� ��: CNotifyPool::OnPublish
  ��    ��: ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CNotifyPool::OnPublish(DWORD event)
{
    AutoObjLock(m_pOwner);

    if (!m_pEventsBuf)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// �ҳ��¼�������Ƿ񱻶���
    /// ���û�б����Ļ���û���ҵ��¼�������ʧ��
    /////////////////////////////////////////////////
    DWORD dwRc = FAILURE;
    for (DWORD i = 0; i < m_dwEventCount; ++i)
    {
        if (m_pEventsBuf[i].m_dwEvent == event)
        {
            if (m_pEventsBuf[i].m_dwProducedCount != (DWORD)(-1))
            {
                m_pEventsBuf[i].m_dwProducedCount++;
            }

            if (m_pEventsBuf[i].m_dwSubscribedCount > 0)
            {
                dwRc = SUCCESS;
            }

            break;
        }
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CNotifyPool::IsSubscribedMsgEvent
  ��    ��: �ж���Ϣ�Ƿ��Ǳ�ָ�������߶��ĵ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CNotifyPool::IsSubscribedMsgEvent(objMsg *msg, DWORD event)
{
    if (!msg) return false;

    AutoObjLock(m_pOwner);

    if (msg->GetMsgType() != DCOP_MSG_OBJECT_EVENT)
    {
        DWORD dwMsgEventID = (m_fnGetMsgEvent)? (*m_fnGetMsgEvent)(msg) : msg->GetMsgType();
        return (event == dwMsgEventID)? true : false;
    }

    if (msg->GetDataLen() < sizeof(DCOP_SESSION_HEAD))
    {
        return false;
    }

    DCOP_SESSION_HEAD *pSessHead = (DCOP_SESSION_HEAD *)msg->GetDataBuf();
    if (!pSessHead)
    {
        return false;
    }

    return (Bytes_GetDword((BYTE *)&(pSessHead->m_attribute)) == event)? true : false;
}

/*******************************************************
  �� �� ��: CNotifyPool::HaveDispatchedMsgEvent
  ��    ��: ���Ѿ��ַ����¼����м���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CNotifyPool::HaveDispatchedMsgEvent(DWORD event)
{
    AutoObjLock(m_pOwner);

    if (!m_pEventsBuf)
    {
        return;
    }

    /////////////////////////////////////////////////
    /// �ҳ��¼������Ѿ��ַ����¼����м���
    /////////////////////////////////////////////////
    for (DWORD i = 0; i < m_dwEventCount; ++i)
    {
        if (m_pEventsBuf[i].m_dwEvent == event)
        {
            if (m_pEventsBuf[i].m_dwDispatchedCount != (DWORD)(-1))
            {
                m_pEventsBuf[i].m_dwDispatchedCount++;
            }

            break;
        }
    }
}

/*******************************************************
  �� �� ��: CNotifyPool::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CNotifyPool::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint || !m_pOwner) return;

    AutoObjLock(m_pOwner);

    if (!m_pEventsBuf)
    {
        return;
    }

    logPrint(STR_FORMAT("'%s'(%d) Event Pool Dump: (Count: %d) \r\n", m_pOwner->Name(), m_pOwner->ID(), m_dwEventCount), logPara);
    CTableString tableStr(4, m_dwEventCount + 1, "  ");
    tableStr << "event";
    tableStr << "subscribedCount";
    tableStr << "producedCount";
    tableStr << "dispatchedCount";

    for (DWORD i = 0; i < m_dwEventCount; ++i)
    {
        tableStr << STR_FORMAT("0x%x", m_pEventsBuf[i].m_dwEvent);
        tableStr << STR_FORMAT("%d", m_pEventsBuf[i].m_dwSubscribedCount);
        tableStr << STR_FORMAT("%d", m_pEventsBuf[i].m_dwProducedCount);
        tableStr << STR_FORMAT("%d", m_pEventsBuf[i].m_dwDispatchedCount);
    }

    tableStr.Show(logPrint, logPara, "=", "-");
}


