/// -------------------------------------------------
/// ObjResponse_main.cpp : ��Ӧ������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjResponse_main.h"
#include "Factory_if.h"
#include "BaseID.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CResponse, "response")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CResponse)
    DCOP_IMPLEMENT_INTERFACE(IResponse)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CResponse)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CResponse)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_RESPONSE_TIMER_1S, OnTimer1s)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END


/*******************************************************
  �� �� ��: CResponse::CResponse
  ��    ��: CResponse���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CResponse::CResponse(Instance *piParent, int argc, char **argv)
{
    m_piManager = 0;
    m_piTimer = 0;
    m_hTimer1s = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CResponse::~CResponse
  ��    ��: CResponse��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CResponse::~CResponse()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CResponse::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CResponse::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IManager,     DCOP_OBJECT_MANAGER,    m_piManager)
        DCOP_QUERY_OBJECT_ITEM(ITimer,       DCOP_OBJECT_TIMER,      m_piTimer)
    DCOP_QUERY_OBJECT_END

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CResponse::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponse::Fini()
{
    DCOP_RELEASE_INSTANCE(m_piTimer);
    DCOP_RELEASE_INSTANCE(m_piManager);
}

/*******************************************************
  �� �� ��: CResponse::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponse::OnStart(objMsg *msg)
{
    if (m_piTimer)
    {
        m_hTimer1s = m_piTimer->Start(ITimer::TYPE_LOOP, DCOP_MSG_RESPONSE_TIMER_1S, 1000, this);
    }
}

/*******************************************************
  �� �� ��: CResponse::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponse::OnFinish(objMsg *msg)
{
    if (m_piTimer && m_hTimer1s)
    {
        m_piTimer->Stop(m_hTimer1s);
        m_hTimer1s = 0;
    }
}

/*******************************************************
  �� �� ��: CResponse::OnTimer1s
  ��    ��: 1S��ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponse::OnTimer1s(objMsg *msg)
{
    for (IT_RESPONSE it_rsp = m_requests.begin();
        it_rsp != m_requests.end(); ++it_rsp)
    {
        CResponsePool *pPool = (*it_rsp);
        if (!pPool) continue;

        pPool->OnTick();
    }
}

/*******************************************************
  �� �� ��: CResponse::CreatePool
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IResponse::IPool *CResponse::CreatePool(IObject *owner, DWORD count)
{
    if (!count)
    {
        return NULL;
    }

    AutoObjLock(this);

    CResponsePool *pPool = new CResponsePool();
    if (!pPool)
    {
        return NULL;
    }

    DWORD dwRc = pPool->Init(m_piManager, owner, count);
    if (dwRc != SUCCESS)
    {
        delete pPool;
        return NULL;
    }

    (void)m_requests.insert(pPool);
    return pPool;
}

/*******************************************************
  �� �� ��: CResponse::DestroyPool
  ��    ��: ɾ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CResponse::DestroyPool(IResponse::IPool *pool)
{
    if (!pool)
    {
        return;
    }

    AutoObjLock(this);

    CResponsePool *pPool = (CResponsePool *)pool;
    (void)m_requests.erase(pPool);
    delete pPool;
}

