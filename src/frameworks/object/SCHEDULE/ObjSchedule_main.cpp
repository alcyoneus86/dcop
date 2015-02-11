/// -------------------------------------------------
/// ObjSchedule_main.cpp : ��Ϣ����������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjSchedule_main.h"
#include "Factory_if.h"
#include "BaseID.h"
#include <time.h>
#include "string/tablestring.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CSchedule, "schedule")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CSchedule)
    DCOP_IMPLEMENT_INTERFACE(ISchedule)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CSchedule)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE_ENABLE()
    DCOP_IMPLEMENT_CONFIG_STRING("taskname", m_szNameConfig)
    DCOP_IMPLEMENT_CONFIG_INTEGER("taskcount", m_dwCountConfig)
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CSchedule::CSchedule
  ��    ��: CSchedule���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSchedule::CSchedule(Instance *piParent, int argc, char **argv)
{
    (void)snprintf(m_szNameConfig, sizeof(m_szNameConfig), "Sched");
    m_szNameConfig[sizeof(m_szNameConfig) - 1] = '\0';
    m_dwCountConfig = SCHEDULE_COUNT_DEFAULT;

    m_piManager = 0;
    m_pNodes = 0;
    m_dwNodeCount = 0;
    m_dwIdleIndex = 0;

    m_fnMsgProc = 0;
    m_pUserArg = 0;
    m_bOrderReach = true;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CSchedule::~CSchedule
  ��    ��: CSchedule��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSchedule::~CSchedule()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CSchedule::Init
  ��    ��: ��ʼ�����
  ��    ��: manager - ���������
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSchedule::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT(IManager, DCOP_OBJECT_MANAGER, root, m_piManager);

    /// ��ȥ��������
    DWORD dwCountConfig = m_dwCountConfig;
    if (!dwCountConfig)
    {
        dwCountConfig = SCHEDULE_COUNT_DEFAULT;
    }

    /// ������Ƚڵ�ռ�
    TaskNode *pTmp = (TaskNode *)DCOP_Malloc(dwCountConfig * sizeof(TaskNode));
    if (!pTmp) return FAILURE;
    (void)memset(pTmp, 0, dwCountConfig * sizeof(TaskNode));

    /// �������е�����
    m_pNodes = pTmp;
    m_dwNodeCount = dwCountConfig;
    DWORD dwRc = CreateAllTask();

    return dwRc;
}

/*******************************************************
  �� �� ��: CSchedule::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::Fini()
{
    Enter();
    DestroyAllTask();
    Leave();

    if (m_pNodes)
    {
        DCOP_Free(m_pNodes);
        m_pNodes = 0;
    }

    m_dwNodeCount = 0;
    m_dwIdleIndex = 0;

    DCOP_RELEASE_INSTANCE(m_piManager);
}

/*******************************************************
  �� �� ��: CSchedule::Dump
  ��    ��: DUMP���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);

    if (!m_pNodes)
    {
        return;
    }

    logPrint(STR_FORMAT("Schedule Dump: (Task Count: %d) (Idle Index: %d) \r\n", m_dwNodeCount, m_dwIdleIndex), logPara);
    CTableString tableStr(5, m_dwNodeCount + 1, "  ");
    tableStr << "InActive";
    tableStr << "LastProcObjID";
    tableStr << "LastCostTime";
    tableStr << "MaxCostTime";
    tableStr << "MaxCostObjID";

    for (DWORD i = 0; i < m_dwNodeCount; ++i)
    {
        tableStr << STR_FORMAT("%d", m_pNodes[i].m_dwInActive);
        tableStr << STR_FORMAT("%d", m_pNodes[i].m_dwLastProcObjID);
        tableStr << STR_FORMAT("%d", m_pNodes[i].m_dwLastCostTimeEnd - m_pNodes[i].m_dwLastCostTimeStart);
        tableStr << STR_FORMAT("%d", m_pNodes[i].m_dwMaxCostTime);
        tableStr << STR_FORMAT("%d", m_pNodes[i].m_dwMaxCostObjID);
    }

    tableStr.Show(logPrint, logPara, "=", "-");
}

/*******************************************************
  �� �� ��: CSchedule::SetProc
  ��    ��: ������Ϣ����ص�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::SetProc(PROC proc, void *arg)
{
    m_fnMsgProc = proc;
    m_pUserArg = arg;
}

/*******************************************************
  �� �� ��: CSchedule::SetSameDst
  ��    ��: �����Ƿ�֧��ͬһĿ�����ε���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::SetSameDst(bool bOrderReach)
{
    m_bOrderReach = bOrderReach;
}

/*******************************************************
  �� �� ��: CSchedule::Join
  ��    ��: ���������
  ��    ��: 
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CSchedule::Join(objMsg *message)
{
    if (!message)
    {
        return FAILURE;
    }

    /// Ϊ�˱�֤����ͬһ���������Ϣ���ε���
    /// ���ڽ��ջ�����в��Ҷ���
    /// Ȼ���ٲ��ҿ��еķ��Ͷ���
    /// �������ε�������:
    /// 1. �û�����Ϊ��ֹ
    /// 2. ֧����Ϣ�����Ķ���
    DWORD dwRecvID = message->GetDstID();
    bool bOrderReach = m_bOrderReach;
    if (bOrderReach && m_piManager)
    {
        IObject *piObj = m_piManager->Child(dwRecvID);
        if (piObj && piObj->bConcurrent())
        {
            bOrderReach = false;
        }
    }

    objMsgQue *pMsgQue = GetRecvQue(dwRecvID, bOrderReach);
    if (!pMsgQue)
    {
        pMsgQue = GetSendQue(dwRecvID, bOrderReach);
    }

    if (!pMsgQue)
    {
        return FAILURE;
    }

    /// ���ж��з��ͣ�ʧ��Ҫ�ѽ��ջ�����е�Ŀ��ID���
    DWORD dwRc = pMsgQue->Send(message);
    if (dwRc != SUCCESS)
    {
        DelRecvObj(dwRecvID, bOrderReach);
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSchedule::CreateTask
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CSchedule::CreateAllTask()
{
    /// ���ⲿ���л��⴦�����ﲻ����

    if (!m_pNodes)
    {
        return FAILURE;
    }

    for (DWORD i = 0; i < m_dwNodeCount; ++i)
    {
        /// ������Ϣ����
        objMsgQue *pMsgQue = DCOP_CreateMsgQue(SCHEDULE_MSGQUE_LEN);
        if (!pMsgQue)
        {
            return FAILURE;
        }

        /// ��������
        (void)snprintf(m_pNodes[i].m_szTaskName, sizeof(m_pNodes[i].m_szTaskName), "t%s%u", m_szNameConfig, (i+1));
        m_pNodes[i].m_szTaskName[sizeof(m_pNodes[i].m_szTaskName) - 1] = '\0';
        objTask::IPara *pTaskPara = new TaskPara(this, pMsgQue, i);
        objTask *pTask = DCOP_CreateTask(m_pNodes[i].m_szTaskName,
                        TaskEntry,
                        1024,
                        objTask::OSTASK_PRIORITY_NORMAL,
                        pTaskPara);
        if (!pTask)
        {
            delete pTaskPara;
            delete pMsgQue;
            return FAILURE;
        }

        m_pNodes[i].m_pTask = pTask;
        m_pNodes[i].m_pMsgQue = pMsgQue;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSchedule::DestroyAllTask
  ��    ��: ������������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::DestroyAllTask()
{
    /// ���ⲿ���л��⴦�����ﲻ����

    if (!m_pNodes)
    {
        return;
    }

    for (DWORD i = 0; i < m_dwNodeCount; ++i)
    {
        if (m_pNodes[i].m_pTask)
        {
            delete m_pNodes[i].m_pTask;
            m_pNodes[i].m_pTask = 0;
        }

        if (m_pNodes[i].m_pMsgQue)
        {
            delete m_pNodes[i].m_pMsgQue;
            m_pNodes[i].m_pMsgQue = 0;
        }
    }
}

/*******************************************************
  �� �� ��: CSchedule::TaskEntry
  ��    ��: �������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::TaskEntry(objTask::IPara *para)
{
    TaskPara *pPara = (TaskPara *)para;
    OSASSERT(pPara != NULL);

    CSchedule *pThis = pPara->m_pSchedule;
    if (!pThis)
    {
        return;
    }

    /// ���õ�ǰϵͳID�����������
    pThis->SetSystemID();

    objMsgQue *pMsgQue = pPara->m_pMsgQue;
    if (!pMsgQue)
    {
        return;
    }

    for (;;)
    {
        DWORD dwRc = pMsgQue->Wait(OSWAIT_FOREVER);
        if (dwRc)
        {
            continue;
        }

        for (;;)
        {
            objMsg *message = NULL;
            dwRc = pMsgQue->Recv(message);
            if (dwRc)
            {
                break;
            }

            if (!message)
            {
                continue;
            }

            DWORD dwProcObjID = message->GetDstID();

            pThis->OnRecvStart(pPara->m_dwIndex, dwProcObjID);
            pThis->OnRecv(message, dwProcObjID);
            pThis->OnRecvEnd(pPara->m_dwIndex, dwProcObjID);

            delete message;
            message = NULL;
        }
    }
}

/*******************************************************
  �� �� ��: CSchedule::OnRecvStart
  ��    ��: ��ʼ���մ���
  ��    ��: dwIndex     - ����
            dwProcObjID - ����Ķ���ID
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::OnRecvStart(DWORD dwIndex, DWORD dwProcObjID)
{
    AutoObjLock(this);

    if (!m_pNodes || (dwIndex >= m_dwNodeCount))
    {
        return;
    }

    m_pNodes[dwIndex].m_dwInActive = DCOP_YES;
    m_pNodes[dwIndex].m_dwLastProcObjID = dwProcObjID;
    m_pNodes[dwIndex].m_dwLastCostTimeStart = (DWORD)(clock()/(CLOCKS_PER_SEC/1000));

    objTask *pTask = m_pNodes[dwIndex].m_pTask;
    if (pTask)
    {
        (void)pTask->SetLocal(TASK_LOCAL_HANDLER, &dwProcObjID, sizeof(dwProcObjID));
    }
}

/*******************************************************
  �� �� ��: CSchedule::OnRecv
  ��    ��: ���մ���
  ��    ��: message     - ��Ϣ
            dwProcObjID - ����Ķ���ID
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::OnRecv(objMsg *message, DWORD dwProcObjID)
{
    /// ֧�ֻص��Ļ���ֱ�ӵ��ûص����д���
    if (m_fnMsgProc)
    {
        (*m_fnMsgProc)(message, m_pUserArg);
        return;
    }

    /// ��Աm_piManager�ڴ������������������ڶ�����ı䣬���Բ��ñ���
    /// ֻ��Ҫ����Ŀ�Ķ������Ϣ������ʱ���б�������(ʹ��Ŀ�Ķ���Ļ�����)
    if (!m_piManager || !message)
    {
        return;
    }

    /// ��ȡ����������ĳ�Ա����
    IObject *pRecvObj = m_piManager->Child(dwProcObjID);
    if (!pRecvObj)
    {
        return;
    }

    /// ���������Ϣ��ڲ����б������ɶ����Լ�����Ϣ������б���(��'DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE')
    pRecvObj->Proc(message);

    /// ����ʱ�������ε���
    bool bOrderReach = m_bOrderReach;
    if (bOrderReach && pRecvObj->bConcurrent())
    {
        bOrderReach = false;
    }

    /// ɾ�����ε���Ľ��ջ���
    DelRecvObj(dwProcObjID, bOrderReach);
}

/*******************************************************
  �� �� ��: CSchedule::OnRecvEnd
  ��    ��: �������մ���
  ��    ��: dwIndex     - ����
            dwProcObjID - ����Ķ���ID
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::OnRecvEnd(DWORD dwIndex, DWORD dwProcObjID)
{
    AutoObjLock(this);

    if (!m_pNodes || (dwIndex >= m_dwNodeCount))
    {
        return;
    }

    m_pNodes[dwIndex].m_dwInActive = DCOP_NO;
    m_pNodes[dwIndex].m_dwLastCostTimeEnd = (DWORD)(clock()/(CLOCKS_PER_SEC/1000));

    DWORD dwCostTime = m_pNodes[dwIndex].m_dwLastCostTimeEnd - m_pNodes[dwIndex].m_dwLastCostTimeStart;
    if (dwCostTime > m_pNodes[dwIndex].m_dwMaxCostTime)
    {
        m_pNodes[dwIndex].m_dwMaxCostTime = dwCostTime;
        m_pNodes[dwIndex].m_dwMaxCostObjID = dwProcObjID;
    }

    objTask *pTask = m_pNodes[dwIndex].m_pTask;
    if (pTask)
    {
        DWORD dwObjID = ID();
        IObject *piParent = Parent();
        if (!dwObjID && piParent)
        {
            dwObjID = piParent->ID();
        }

        (void)pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
    }
}

/*******************************************************
  �� �� ��: CSchedule::OnSend
  ��    ��: ��ȡһ��Idle�������ڷ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSchedule::OnSend()
{
    /// ���ⲿ���л��⴦�����ﲻ����

    if (!m_pNodes)
    {
        return m_dwIdleIndex;
    }

    /// Ĭ����һ������
    DWORD dwIdx = m_dwIdleIndex + 1;
    if (dwIdx >= m_dwNodeCount) dwIdx = 0;

    /// ѭ���ҵ����ж���
    DWORD dwIdxTmp = dwIdx;
    DWORD dwCountTmp = (DWORD)(-1);
    while (dwIdx != m_dwIdleIndex)
    {
        /// ��ȡ��Ϣ����
        objMsgQue *pMsgQue = m_pNodes[dwIdx].m_pMsgQue;
        if (!pMsgQue)
        {
            dwIdx++;
            if (dwIdx >= m_dwNodeCount) dwIdx = 0;
            continue;
        }

        /// ���û����Ϣ����ʹ������̷���
        DWORD dwMsgCount = pMsgQue->Size();
        if (!dwMsgCount && !(m_pNodes[dwIdx].m_dwInActive))
        {
            dwIdxTmp = dwIdx;
            break;
        }

        /// ��¼һ����С����Ϣ����
        if (dwMsgCount < dwCountTmp)
        {
            dwIdxTmp = dwIdx;
            dwCountTmp = dwMsgCount;
        }

        /// ����������Χ�����з�ת
        dwIdx++;
        if (dwIdx >= m_dwNodeCount) dwIdx = 0;
    }

    DWORD dwIdxRc = m_dwIdleIndex;
    m_dwIdleIndex = dwIdxTmp;

    return dwIdxRc;
}

/*******************************************************
  �� �� ��: CSchedule::GetSendQue
  ��    ��: ��ȡ���еķ�����Ϣ����
  ��    ��: dwObjID     - ����ID
            bOrderReach - �Ƿ����ε���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsgQue *CSchedule::GetSendQue(DWORD dwObjID, bool bOrderReach)
{
    AutoObjLock(this);

    if (!m_pNodes)
    {
        return NULL;
    }

    DWORD dwIndex = OnSend();

    if (dwIndex >= m_dwNodeCount)
    {
        return NULL;
    }

    if (bOrderReach)
    {
        /// ֧����ͬĿ�����ε���ʱ��������ջ���
        RecvObjNode recvNode = {1, dwIndex};
        (void)m_recvobjs.insert(MAP_RECVOBJ::value_type(dwObjID, recvNode));
    }

    return m_pNodes[dwIndex].m_pMsgQue;
}

/*******************************************************
  �� �� ��: CSchedule::GetRecvQue
  ��    ��: ���ݶ���ID��ȡ����Ľ�����Ϣ����
  ��    ��: dwObjID     - ����ID
            bOrderReach - �Ƿ����ε���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsgQue *CSchedule::GetRecvQue(DWORD dwObjID, bool bOrderReach)
{
    AutoObjLock(this);

    if (!bOrderReach)
    {
        /// ��֧����ͬĿ�����ε���
        return NULL;
    }

    if (!m_pNodes)
    {
        return NULL;
    }

    IT_RECVOBJ it_recv = m_recvobjs.find(dwObjID);
    if (it_recv == m_recvobjs.end())
    {
        return NULL;
    }

    DWORD dwIndex = ((*it_recv).second).m_dwQueIndex;
    if (dwIndex >= m_dwNodeCount)
    {
        return NULL;
    }

    ((*it_recv).second).m_dwRefCount++;
    if (!((*it_recv).second).m_dwRefCount)
    {
        ((*it_recv).second).m_dwRefCount++;
    }

    return m_pNodes[dwIndex].m_pMsgQue;
}

/*******************************************************
  �� �� ��: CSchedule::DelRecvObj
  ��    ��: �ѽ��ն���ID�ӽ��ջ������ɾ��
  ��    ��: dwObjID     - ����ID
            bOrderReach - �Ƿ����ε���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::DelRecvObj(DWORD dwObjID, bool bOrderReach)
{
    AutoObjLock(this);

    if (!bOrderReach)
    {
        /// ��֧����ͬĿ�����ε���
        return;
    }

    IT_RECVOBJ it_recv = m_recvobjs.find(dwObjID);
    if (it_recv == m_recvobjs.end())
    {
        return;
    }

    if (((*it_recv).second).m_dwRefCount)
    {
        ((*it_recv).second).m_dwRefCount--;
    }

    if (!((*it_recv).second).m_dwRefCount)
    {
        (void)m_recvobjs.erase(it_recv);
    }
}

/*******************************************************
  �� �� ��: CSchedule::SetSystemID
  ��    ��: ����ϵͳID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSchedule::SetSystemID()
{
    objTask *pTask = objTask::Current();
    if (!pTask)
    {
        return;
    }

    DWORD dwSysID = 0;
    if (m_piManager)
    {
        dwSysID = m_piManager->GetSystemID();
    }

    DWORD dwRc = pTask->SetLocal(TASK_LOCAL_SYSTEM, &dwSysID, sizeof(dwSysID));
    CHECK_ERRCODE(dwRc, "Set Sys ID To Task Local");

    DWORD dwObjID = ID();
    IObject *piParent = Parent();
    if (!dwObjID && piParent)
    {
        dwObjID = piParent->ID();
    }

    dwRc = pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
    CHECK_ERRCODE(dwRc, "Set Obj ID To Task Local");
}

