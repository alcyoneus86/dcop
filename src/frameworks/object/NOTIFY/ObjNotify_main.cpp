/// -------------------------------------------------
/// ObjNotify_main.cpp : ���ķ���������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjNotify_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "ObjDispatch_if.h"
#include "BaseID.h"
#include "string/tablestring.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CNotify, "notify")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CNotify)
    DCOP_IMPLEMENT_INTERFACE(INotify)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CNotify)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CNotify::CNotify
  ��    ��: CNotify����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CNotify::CNotify(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    m_SubscribeTmp.SetNodeSize(sizeof(SubscribeNode));

    m_piDispatch = 0;
}

/*******************************************************
  �� �� ��: CNotify::~CNotify
  ��    ��: CNotify����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CNotify::~CNotify()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CNotify::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CNotify::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ�ַ���
    DCOP_QUERY_OBJECT(IDispatch, DCOP_OBJECT_DISPATCH, root, m_piDispatch);
    if (!m_piDispatch)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CNotify::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CNotify::Fini()
{
    DCOP_RELEASE_INSTANCE(m_piDispatch);
}

/*******************************************************
  �� �� ��: CNotify::Proc
  ��    ��: ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CNotify::Proc(objMsg *msg)
{
    if (!msg)
    {
        return;
    }

    AutoObjLock(this);

    DWORD producerID = msg->GetSrcID();
    IT_NOTIFY it = m_events.find(producerID);
    if (it == m_events.end())
    {
        return;
    }

    CNotifyPool *pPool = &((*it).second);

    /////////////////////////////////////////////////
    /// �ӻ���Ķ��Ĳ��Ҳ��ַ���������
    /////////////////////////////////////////////////
    for (DWORD i = 0; i < m_SubscribeTmp.Count(); ++i)
    {
        SubscribeNode *pNode = (SubscribeNode *)m_SubscribeTmp.Pos(i);
        if (!pNode)
        {
            continue;
        }

        /// ���Ƕ��ĵĸ������ߵ��¼�������
        if (pNode->producer != producerID)
        {
            continue;
        }

        /// ���Ƕ��ĵĸ���Ϣ�е��¼�������
        if (!pPool->IsSubscribedMsgEvent(msg, pNode->event))
        {
            continue;
        }

        /// ������Ϣ�����ַ������ĵ�������
        objMsg *msgNew = DCOP_CloneMsg(msg);
        if (!msgNew)
        {
            continue;
        }

        /// �ַ��¼�
        msgNew->MsgHead().m_dwDstID = pNode->consumer;
        DWORD dwRc = m_piDispatch->Send(msgNew);
        if (dwRc)
        {
            CHECK_RETCODE(dwRc, STR_FORMAT("Notify Dispatch Event:0x%x To ObjID:%d Fail!", 
                        pNode->event, pNode->consumer));
            delete msgNew;
        }

        /// ���ѷַ����¼����м���
        pPool->HaveDispatchedMsgEvent(pNode->event);
    }
}

/*******************************************************
  �� �� ��: CNotify::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CNotify::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);

    logPrint(STR_FORMAT("Subscribe Dump: (Count: %d) \r\n", m_SubscribeTmp.Count()), logPara);
    CTableString tableStr(3, m_SubscribeTmp.Count() + 1, "  ");
    tableStr << "consumer";
    tableStr << "producer";
    tableStr << "event";

    for (DWORD i = 0; i < m_SubscribeTmp.Count(); ++i)
    {
        SubscribeNode *pNode = (SubscribeNode *)m_SubscribeTmp.Pos(i);
        if (!pNode)
        {
            continue;
        }

        tableStr << STR_FORMAT("%d", pNode->consumer);
        tableStr << STR_FORMAT("%d", pNode->producer);
        tableStr << STR_FORMAT("0x%x", pNode->event);
    }

    tableStr.Show(logPrint, logPara, "=", "-");

    for (IT_NOTIFY it = m_events.begin(); it != m_events.end(); ++it)
    {
        ((*it).second).Dump(logPrint, logPara, argc, argv);
    }
}

/*******************************************************
  �� �� ��: CNotify::CreatePool
  ��    ��: ���������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
INotify::IPool *CNotify::CreatePool(IObject *producer, DWORD *events, DWORD count, GET_MSG_EVENT fnGetMsgEvent)
{
    if (!producer) return NULL;

    AutoObjLock(this);

    IT_NOTIFY it = m_events.find(producer->ID());
    if (it != m_events.end())
    {
        return &((*it).second);
    }

    CNotifyPool pool;
    it = m_events.insert(m_events.end(), MAP_NOTIFY::value_type(producer->ID(), pool));
    if (it == m_events.end())
    {
        return NULL;
    }

    CNotifyPool *pPool = &((*it).second);
    DWORD dwRc = pPool->Init(producer, events, count, fnGetMsgEvent);
    if (dwRc != SUCCESS)
    {
        (void)m_events.erase(it);
        return NULL;
    }

    ProcTmpOfSubscribe(pPool);
    return pPool;
}

/*******************************************************
  �� �� ��: CNotify::DestroyPool
  ��    ��: ɾ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CNotify::DestroyPool(IPool *pool)
{
    if (!pool) return;

    AutoObjLock(this);

    CNotifyPool *pPool = (CNotifyPool *)pool;
    IObject *pProducer = pPool->GetProducer();
    if (!pProducer)
    {
        return;
    }

    (void)m_events.erase(pProducer->ID());
}

/*******************************************************
  �� �� ��: CNotify::Subscribe
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CNotify::Subscribe(DWORD consumer, DWORD producer, DWORD event)
{
    AutoObjLock(this);

    /// �Ƚ��б��ػ���
    DWORD dwRc = SaveTmpOfSubscribe(consumer, producer, event);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ����Ҳ��������ߣ�ֻ�е������ߴ���ʱ�ٴ���
    IT_NOTIFY it = m_events.find(producer);
    if (it == m_events.end())
    {
        return SUCCESS;
    }

    /// �ҵ������ߣ������¼�����
    CNotifyPool *pPool = &((*it).second);
    if (!pPool)
    {
        return FAILURE;
    }

    return pPool->OnSubscribe(event);
}

/*******************************************************
  �� �� ��: CNotify::SaveTmpOfSubscribe
  ��    ��: �����ĵı��ػ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CNotify::SaveTmpOfSubscribe(DWORD consumer, DWORD producer, DWORD event)
{
    SubscribeNode tmpNode = {consumer, producer, event};

    /// ������ػ����Ѿ����ڣ��򷵻ش���
    if (m_SubscribeTmp.Pos(m_SubscribeTmp.Find(&tmpNode)))
    {
        return FAILURE;
    }

    return m_SubscribeTmp.Append(&tmpNode);
}

/*******************************************************
  �� �� ��: CNotify::ProcTmpOfSubscribe
  ��    ��: �����ĵ���ʱ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CNotify::ProcTmpOfSubscribe(CNotifyPool *pPool)
{
    if (!pPool) return;

    IObject *pProducer = pPool->GetProducer();
    if (!pProducer) return;

    DWORD dwID = pProducer->ID();
    SubscribeNode *pNode = (SubscribeNode *)(m_SubscribeTmp.Pos(m_SubscribeTmp.Find(&dwID, CompareSubscribeNode)));
    if (!pNode) return;

    (void)pPool->OnSubscribe(pNode->event);
}


/*******************************************************
  �� �� ��: CNotify::CompareSubscribeNode
  ��    ��: �Ƚ϶��Ľڵ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CNotify::CompareSubscribeNode(void *pNode, void *pKey)
{
    if (!pNode || !pKey) return false;

    SubscribeNode *pSubscribeNode = (SubscribeNode *)pNode;
    DWORD dwProducer = *(DWORD *)pKey;
    return (pSubscribeNode->producer == dwProducer)? true : false;
}

