/// -------------------------------------------------
/// ObjStatus_main.cpp : ״̬������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjStatus_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "BaseMessage.h"


DCOP_IMPLEMENT_FACTORY(CStatus, "status")

DCOP_IMPLEMENT_INSTANCE(CStatus)
    DCOP_IMPLEMENT_INTERFACE(IStatus)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

DCOP_IMPLEMENT_IOBJECT(CStatus)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CStatus::CStatus
  ��    ��: CStatus���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CStatus::CStatus(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    m_pStateNodes = 0;
    m_dwStateCount = 0;
    m_dwCurState = Invalid;
    m_bRunning = false;
}

/*******************************************************
  �� �� ��: CStatus::~CStatus
  ��    ��: CTimer��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CStatus::~CStatus()
{
    if (m_pStateNodes)
    {
        DCOP_Free(m_pStateNodes);
        m_pStateNodes = 0;
    }

    m_dwStateCount = 0;
    m_dwCurState = Invalid;
    m_bRunning = false;

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CStatus::Reg
  ��    ��: ע��״̬��
  ��    ��: pObjOwner       - ������
            pStateNodes     - ״̬�б�
            dwStateCount    - ״̬����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CStatus::Reg(REG *pStateNodes, DWORD dwStateCount)
{
    if (!pStateNodes || !dwStateCount)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    if (m_bRunning)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// 1.����һƬ�µĻ�����
    /////////////////////////////////////////////////
    DWORD dwStateCountTmp = m_dwStateCount + dwStateCount;
    STATE *pStateNodesTmp = (STATE *)DCOP_Malloc(dwStateCountTmp * sizeof(STATE));
    if (!pStateNodesTmp)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// 2.������ǰ��ע����(֧�ַֿ����ע��)
    /////////////////////////////////////////////////
    if (m_pStateNodes)
    {
        (void)memcpy(pStateNodesTmp, m_pStateNodes, m_dwStateCount * sizeof(STATE));
        DCOP_Free(m_pStateNodes);
        m_pStateNodes = 0;
    }

    /////////////////////////////////////////////////
    /// 3.�����µ�ע����
    /////////////////////////////////////////////////
    for (DWORD i = 0; i < dwStateCount; ++i)
    {
        (void)memcpy(pStateNodesTmp + m_dwStateCount + i, pStateNodes + i, sizeof(REG));
        pStateNodesTmp[dwStateCount + i].m_dwRunStateValue = Invalid;
        pStateNodesTmp[dwStateCount + i].m_dwRunPrevState = Invalid;
        pStateNodesTmp[dwStateCount + i].m_dwRunTimeValue = 0;
    }

    m_pStateNodes = pStateNodesTmp;
    m_dwStateCount = dwStateCountTmp;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CStatus::Start
  ��    ��: ��ʼ״̬��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CStatus::Start()
{
    AutoObjLock(this);

    if (m_bRunning)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// ����DCOP_MSG_STATUS_START�¼�
    /////////////////////////////////////////////////
    DWORD dwRc = EventToOwner(DCOP_MSG_STATUS_START);
    if (dwRc)
    {
        return FAILURE;
    }

    m_bRunning = true;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CStatus::Stop
  ��    ��: ֹͣ״̬��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CStatus::Stop()
{
    AutoObjLock(this);

    if (!m_bRunning)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// ����DCOP_MSG_STATUS_STOP�¼�
    /////////////////////////////////////////////////
    DWORD dwRc = EventToOwner(DCOP_MSG_STATUS_STOP);
    if (dwRc)
    {
        return FAILURE;
    }

    m_bRunning = false;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CStatus::Tick
  ��    ��: ��ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CStatus::Tick()
{
    AutoObjLock(this);

    if (!m_bRunning || !m_pStateNodes || !m_dwStateCount || (m_dwCurState >= m_dwStateCount))
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// 1.���м�ʱ����ʱ
    /////////////////////////////////////////////////
    m_pStateNodes[m_dwCurState].m_dwRunTimeValue++;
    if (OSWAIT_FOREVER == m_pStateNodes[m_dwCurState].m_dwTimeout)
    {
        return SUCCESS;
    }

    /////////////////////////////////////////////////
    /// 2.û�г�ʱֱ�ӷ��سɹ�
    /////////////////////////////////////////////////
    DWORD dwTimeValue = m_pStateNodes[m_dwCurState].m_dwRunTimeValue;
    if (dwTimeValue < m_pStateNodes[m_dwCurState].m_dwTimeout)
    {
        return SUCCESS;
    }

    /////////////////////////////////////////////////
    /// 3.��ʱ�Ĵ���DCOP_MSG_STATUS_TIMEOUT�¼�
    /////////////////////////////////////////////////
    DWORD dwRc = EventToOwner(DCOP_MSG_STATUS_TIMEOUT);
    if (dwRc)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// 4.��ʱ�����㣬�����ʱ״̬�����û��߲��䣬ֱ�ӷ��سɹ�
    /////////////////////////////////////////////////
    m_pStateNodes[m_dwCurState].m_dwRunTimeValue = 0;
    DWORD dwBackState = m_pStateNodes[m_dwCurState].m_dwBackStateWhenTimeout;
    if ((dwBackState >= m_dwStateCount) || (dwBackState == m_dwCurState))
    {
        return SUCCESS;
    }

    /////////////////////////////////////////////////
    /// 5.�����趨�ĳ�ʱ״̬
    /////////////////////////////////////////////////
    dwRc = SetNewState(dwBackState);
    return dwRc;
}

/*******************************************************
  �� �� ��: CStatus::SetNewState
  ��    ��: �����µ�״̬
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CStatus::SetNewState(DWORD dwNewState)
{
    AutoObjLock(this);

    if (!m_bRunning || !m_pStateNodes || !m_dwStateCount || (m_dwCurState >= m_dwStateCount))
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// ����DCOP_MSG_STATUS_NEW�¼�
    /////////////////////////////////////////////////
    DWORD dwRc = EventToOwner(DCOP_MSG_STATUS_NEW);
    if (dwRc)
    {
        return FAILURE;
    }

    if (dwNewState >= m_dwStateCount)
    {
        dwNewState = m_pStateNodes[m_dwCurState].m_dwNextStateDefault;
    }

    m_dwCurState = dwNewState;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CStatus::GetCurState
  ��    ��: ��ȡ��ǰ״̬
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CStatus::GetCurState()
{
    AutoObjLock(this);

    return m_dwCurState;
}

/*******************************************************
  �� �� ��: CStatus::GetStateInfo
  ��    ��: ��ȡ״̬��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IStatus::STATE *CStatus::GetStateInfo(DWORD dwState)
{
    AutoObjLock(this);

    if (!m_pStateNodes || !m_dwStateCount || (dwState >= m_dwStateCount))
    {
        return NULL;
    }

    return &(m_pStateNodes[dwState]);
}

/*******************************************************
  �� �� ��: CStatus::EventToOwner
  ��    ��: �����¼���������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CStatus::EventToOwner(DWORD event)
{
    if (!m_piParent)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// 1.ת��������Ϊ������
    /////////////////////////////////////////////////
    IObject *pOwner = 0; 
    if (m_piParent->QueryInterface(ID_INTF(IManager), REF_PTR(pOwner), this) != SUCCESS)
    {
        return FAILURE;
    }

    if (!pOwner)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// 2.ֱ�ӵ��������ߴ���ӿ�
    /////////////////////////////////////////////////
    objMsg *pMsg = DCOP_CreateMsg(0, event, ID());
    if (!pMsg)
    {
        (void)pOwner->Release(this);
        return FAILURE;
    }

    pOwner->Proc(pMsg);
    (void)pOwner->Release(this);
    delete pMsg;

    return SUCCESS;
}

