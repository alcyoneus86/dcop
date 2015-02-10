/// -------------------------------------------------
/// ObjTimer_main.cpp : ����ʱ��ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjTimer_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "BaseID.h"


DCOP_IMPLEMENT_FACTORY(CTimer, "timer")

DCOP_IMPLEMENT_INSTANCE(CTimer)
    DCOP_IMPLEMENT_INTERFACE(ITimer)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

DCOP_IMPLEMENT_IOBJECT(CTimer)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CTimer::CTimer
  ��    ��: CTimer���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTimer::CTimer(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    m_piDispatch = 0;
    m_pTaskS = 0;
    m_pTaskMs = 0;
}

/*******************************************************
  �� �� ��: CTimer::~CTimer
  ��    ��: CTimer��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTimer::~CTimer()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CTimer::Init
  ��    ��: ��ʼ�����
  ��    ��: root - ���������
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTimer::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    DCOP_QUERY_OBJECT(IDispatch, DCOP_OBJECT_DISPATCH, root, m_piDispatch);
    if (!m_piDispatch)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// ��ʼ����ʱ����
    /////////////////////////////////////////////////

    Enter();

    m_wheelS[WHEEL_S_SEC].Init(WHEEL_ID_S_SEC, 60, 
                            &(m_wheelS[WHEEL_S_MIN]), NULL, 
                            1000, TimeoutProc, this);
    m_wheelS[WHEEL_S_MIN].Init(WHEEL_ID_S_MIN, 60, 
                            &(m_wheelS[WHEEL_S_HOUR]), &(m_wheelS[WHEEL_S_SEC]), 
                            60000, TimeoutProc, this);
    m_wheelS[WHEEL_S_HOUR].Init(WHEEL_ID_S_HOUR, 24, 
                            &(m_wheelS[WHEEL_S_DAY]), &(m_wheelS[WHEEL_S_MIN]), 
                            3600000, TimeoutProc, this);
    m_wheelS[WHEEL_S_DAY].Init(WHEEL_ID_S_DAY, 49, 
                            NULL, &(m_wheelS[WHEEL_S_HOUR]), 
                            86400000, TimeoutProc, this);
    
    m_wheelMs[WHEEL_MS_MILL_SEC].Init(WHEEL_ID_MS_MILL_SEC, 10, 
                            &(m_wheelMs[WHEEL_MS_SEC]), NULL, 
                            TIMER_VALUE_MIN, TimeoutProc, this);
    m_wheelMs[WHEEL_MS_SEC].Init(WHEEL_ID_MS_SEC, 30, 
                            NULL, &(m_wheelMs[WHEEL_MS_MILL_SEC]), 
                            1000, TimeoutProc, this);

    Leave();

    objTask::IPara *pTaskParaS      = new TaskPara(this);
    objTask::IPara *pTaskParaMs     = new TaskPara(this);
    
    m_pTaskS = DCOP_CreateTask("STimerTask",
                STaskEntry,
                1024,
                objTask::OSTASK_PRIORITY_HIGHER,
                pTaskParaS);
    if (!m_pTaskS)
    {
        delete pTaskParaS;
        delete pTaskParaMs;
        return FAILURE;
    }

    m_pTaskMs = DCOP_CreateTask("MsTimerTask",
                MsTaskEntry,
                1024,
                objTask::OSTASK_PRIORITY_HIGHER,
                pTaskParaMs);
    if (!m_pTaskMs)
    {
        delete pTaskParaS;
        delete pTaskParaMs;
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTimer::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimer::Fini()
{
    if (m_pTaskMs)
    {
        delete m_pTaskMs;
        m_pTaskMs = 0;
    }

    if (m_pTaskS)
    {
        delete m_pTaskS;
        m_pTaskS = 0;
    }

    DCOP_RELEASE_INSTANCE(m_piDispatch);
}

/*******************************************************
  �� �� ��: CTimer::Init
  ��    ��: ����һ����ʱ��
  ��    ��: timerType   - ��ʱ������
            dwEvent     - ��ʱ���¼�
            dwTimeOut   - ��ʱʱ��
            recver      - ���շ�
  ��    ��: 
  ��    ��: Handle      - ��ʱ�����
  �޸ļ�¼: 
 *******************************************************/
ITimer::Handle CTimer::Start(ITimer::TYPE timerType, DWORD dwEvent, DWORD dwTimeOut, IObject *recver)
{
    if (dwTimeOut < TIMER_VALUE_MIN) dwTimeOut = TIMER_VALUE_MIN;
    TimerValue value = {recver, dwEvent, dwTimeOut, timerType};
    ITimer::Handle hTimer = ITimer::IWheel::Alloc(&value, sizeof(value));
    if (!hTimer)
    {
        return NULL;
    }

    /////////////////////////////////////////////////
    ///     С��30�룬������뼶��ʱ������������뼶
    /////////////////////////////////////////////////

    Enter();
    DWORD dwRc = InsertToWheel(dwTimeOut, hTimer);
    Leave();

    if (dwRc)
    {
        ITimer::IWheel::Free(hTimer);
        return NULL;
    }

    return hTimer;
}

/*******************************************************
  �� �� ��: CTimer::Stop
  ��    ��: ֹͣһ����ʱ��
  ��    ��: hTimer - ��ʱ�����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimer::Stop(ITimer::Handle hTimer)
{
    AutoObjLock(this);

    ITimer::IWheel *pWheel = ITimer::IWheel::GetWheel(hTimer);
    if (!pWheel)
    {
        ITimer::IWheel::Free(hTimer);
        return;
    }

    DWORD dwWheelID = pWheel->GetWheelID();

    if ((dwWheelID >= WHEEL_ID_S_SEC) && (dwWheelID <= WHEEL_ID_S_DAY))
    {
        m_wheelS[dwWheelID - WHEEL_ID_S_SEC].Del(hTimer);
    }

    if ((dwWheelID >= WHEEL_ID_MS_MILL_SEC) && (dwWheelID <= WHEEL_ID_MS_SEC))
    {
        m_wheelMs[dwWheelID - WHEEL_ID_MS_MILL_SEC].Del(hTimer);
    }

    ITimer::IWheel::Free(hTimer);
}

/*******************************************************
  �� �� ��: CTimer::STaskEntry
  ��    ��: �뼶��ʱ������
  ��    ��: para - �������
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimer::STaskEntry(objTask::IPara *para)
{
    TaskPara *pTaskPara = (TaskPara *)para;
    OSASSERT(pTaskPara != 0);

    CTimer *pThis = pTaskPara->m_pTimer;
    OSASSERT(pThis != 0);

    /// ���õ�ǰϵͳID�����������
    pThis->SetSystemID();

    for (;;)
    {
        pThis->Enter();
        pThis->GetSTickBase()->OnTick();
        pThis->Leave();

        objTask::Delay(1000);
    }
}

/*******************************************************
  �� �� ��: CTimer::MsTaskEntry
  ��    ��: ���뼶��ʱ������
  ��    ��: para - �������
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimer::MsTaskEntry(objTask::IPara *para)
{
    TaskPara *pTaskPara = (TaskPara *)para;
    OSASSERT(pTaskPara != 0);

    CTimer *pThis = pTaskPara->m_pTimer;
    OSASSERT(pThis != 0);

    /// ���õ�ǰϵͳID�����������
    pThis->SetSystemID();

    for (;;)
    {
        pThis->Enter();
        pThis->GetMsTickBase()->OnTick();
        pThis->Leave();

        objTask::Delay(100);
    }
}

/*******************************************************
  �� �� ��: CTimer::TimeoutProc
  ��    ��: ��ʱ������
  ��    ��: handle - ��ʱ�����
            para   - �ص���������
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimer::TimeoutProc(ITimer::Handle handle, void *para)
{
    CTimer *pThis = (CTimer *)para;
    OSASSERT(pThis != 0);

    IDispatch *pDispatch = pThis->GetDispatch();
    OSASSERT(pDispatch != 0);

    TimerValue *pValue = (TimerValue *)ITimer::IWheel::GetValue(handle);
    if (!pValue || !pValue->m_dwEvent || !pValue->m_pObject)
    {
        return;
    }

    objMsg *pMsg = DCOP_CreateMsg(0, pValue->m_dwEvent, pThis->ID());
    pMsg->MsgHead().m_dwDstID = pValue->m_pObject->ID();
    DWORD dwRc = pDispatch->Send(pMsg);
    if (dwRc)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("Send Timeout Msg To Obj:%s Fail(0x%x)!", pValue->m_pObject->Name(), dwRc));
        delete pMsg;
    }

    if (ITimer::TYPE_NOLOOP == pValue->m_timerType)
    {
        return;
    }

    dwRc = pThis->InsertToWheel(pValue->m_dwTimeOut, handle);
    if (dwRc)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("InsertToWheel(owner:%s) Fail(%d)!", pValue->m_pObject->Name(), dwRc));
    }
}

/*******************************************************
  �� �� ��: CTimer::InsertToWheel
  ��    ��: ���뵽��ʱ����
  ��    ��: dwTimeOut   - ��ʱʱ��
            hTimer      - ��ʱ�����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTimer::InsertToWheel(DWORD dwTimeOut, ITimer::Handle hTimer)
{
    DWORD dwRc;

    ITimer::IWheel::SetTimeBase(hTimer, dwTimeOut);

    if ((dwTimeOut >= SEC_TIMER_MAX) || 
        !(dwTimeOut % SEC_TIMER_MIN))
    {
        dwRc = m_wheelS[WHEEL_S_DAY].Add(hTimer);
    }
    else
    {
        dwRc = m_wheelMs[WHEEL_MS_SEC].Add(hTimer);
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CTimer::GetTimeNow
  ��    ��: ��ȡ��ʱ����ǰʱ��
  ��    ��: pWheel  - ��ʱ��������
            dwCount - ��ʱ���ָ���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTimer::GetTimeNow(CTimerWheel *pWheel, DWORD dwCount)
{
    DWORD dwTime = 0;
    for (DWORD i = 0; i < dwCount; ++i)
    {
        dwTime += pWheel[i].GetScalePoint() * pWheel[i].GetHashBase();
    }

    return dwTime;
}

/*******************************************************
  �� �� ��: CTimer::SetSystemID
  ��    ��: ����ϵͳID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimer::SetSystemID()
{
    objTask *pTask = objTask::Current();
    if (!pTask)
    {
        return;
    }

    IObject *piParent = Parent();
    DWORD dwSysID = (piParent)? piParent->ID() : 0;
    DWORD dwObjID = ID();
    DWORD dwRc = pTask->SetLocal(TASK_LOCAL_SYSTEM, &dwSysID, sizeof(dwSysID));
    CHECK_ERRCODE(dwRc, "Set Sys ID To Task Local");
    dwRc = pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
    CHECK_ERRCODE(dwRc, "Set Obj ID To Task Local");
}

