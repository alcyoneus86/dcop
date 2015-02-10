/// -------------------------------------------------
/// taskType.cpp : �����װ��ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "taskType.h"
#include "taskApi.h"
#include <string.h>


/*******************************************************
  �� �� ��: objTask::CreateInstance
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objTask *objTask::CreateInstance(const char *cszName,
                        OSTASK_ENTRY pEntry,
                        DWORD dwStackSize,
                        DWORD dwPriority,
                        IPara *pPara,
                        const char *file,
                        int line)
{
    #undef new
    CTaskBase *pTaskBase = new (file, line) CTaskBase();
    #define new new(__FILE__, __LINE__)
    if (!pTaskBase)
    {
        return NULL;
    }

    pTaskBase->SetName(cszName);

    if (pEntry)
    {
        if (pTaskBase->Create(pEntry, dwStackSize, dwPriority, pPara))
        {
            delete pTaskBase;
            pTaskBase = NULL;
        }
    }
    else
    {
        /// û����ھ���ӵ�ǰ����
        pTaskBase->SetID(CTaskBase::Current());
    }

    return pTaskBase;
}

/*******************************************************
  �� �� ��: objTask::~objTask
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objTask::~objTask()
{
}

/*******************************************************
  �� �� ��: objTask::Delay
  ��    ��: ������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void objTask::Delay(DWORD delayMilliseconds)
{
    OS_VDFUNC_CALL(Task, Delay)(delayMilliseconds);
}

/*******************************************************
  �� �� ��: objTask::Current
  ��    ��: ��ȡ��ǰ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objTask *objTask::Current()
{
    osBase *pBase = osBase::Find(OSTYPE_TASK, CTaskBase::Current());
    if (!pBase)
    {
        return NULL;
    }

    return (objTask *)pBase->objGetPtr();
}

/*******************************************************
  �� �� ��: objTask::IPara::~IPara
  ��    ��: �����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objTask::IPara::~IPara()
{
}

/*******************************************************
  �� �� ��: CTaskBase::CTaskBase
  ��    ��: CTaskBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTaskBase::CTaskBase()
{
    m_pEntry = 0;
    m_dwStackSize = 0;
    m_dwPriority = 0;
    m_pPara = 0;

    osBase::vAddToList(OSTYPE_TASK, this);
}

/*******************************************************
  �� �� ��: CTaskBase::~CTaskBase
  ��    ��: CTaskBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTaskBase::~CTaskBase()
{
    (void)Destroy();

    if (m_pPara)
    {
        delete m_pPara;
        m_pPara = 0;
    }
}

/*******************************************************
  �� �� ��: CTaskBase::vAllTaskEntry
  ��    ��: �����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTaskBase::vAllTaskEntry(void *pPara)
{
    CTaskBase *pThis = (CTaskBase *)pPara;
    if (!pThis)
    {
        return;
    }

    /// ����ڴ���õ�ǰ����ID
    pThis->SetID(CTaskBase::Current());

    /// �����������������
    (pThis->pGetEntry())(pThis->pGetPara());

    /// �����Լ�����
    delete pThis;
}

/*******************************************************
  �� �� ��: CTaskBase::Create
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTaskBase::Create(OSTASK_ENTRY pEntry,
                        DWORD dwStackSize,
                        DWORD dwPriority,
                        IPara *pPara)
{
    if (!pEntry)
    {
        return FAILURE;
    }

    m_pEntry = pEntry;
    m_dwStackSize = dwStackSize;
    m_dwPriority = dwPriority;
    m_pPara = pPara;
    OSHANDLE Handle = 0;
    DWORD dwID = 0;

    DWORD dwRc = OS_FUNC_CALL(Task, Create)(&Handle,
                        osBase::cszGetName(),
                        &dwID,
                        vAllTaskEntry,
                        m_dwStackSize,
                        m_dwPriority,
                        this);
    if (SUCCESS == dwRc)
    {
        osBase::vSetHandle(Handle);
    }
    else
    {
        /// ʧ�ܺ󣬲����������ͷ�
        m_pPara = 0;
    }

    TRACE_LOG(STR_FORMAT("Task '%s'(%d) Create rc:0x%x", osBase::cszGetName(), dwID, dwRc));

    return dwRc;
}

/*******************************************************
  �� �� ��: CTaskBase::Destroy
  ��    ��: ɾ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTaskBase::Destroy()
{
    if (!osBase::hGetHandle())
    {
        return SUCCESS;
    }

    DWORD dwRc = OS_FUNC_CALL(Task, Destroy)(osBase::hGetHandle(),
                    osBase::cszGetName(),
                    osBase::dwGetID());

    osBase::vSetHandle(0);

    TRACE_LOG(STR_FORMAT("Task '%s' Destroy rc:0x%x", osBase::cszGetName(), dwRc));

    return dwRc;
}

/*******************************************************
  �� �� ��: CTaskBase::SetLocal
  ��    ��: ���ñ��ر���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTaskBase::SetLocal(DWORD dwPos, void *pVal, DWORD dwLen)
{
    if (!dwLen)
    {
        return FAILURE;
    }

    DWORD dwCurPos = m_sLocal.GetOffSet();
    if (dwCurPos < dwPos)
    {
        DWORD dwRc = m_sLocal.SetNeedMemLen((dwPos - dwCurPos) + dwLen);
        if (dwRc != SUCCESS) return dwRc;
    }

    if (pVal)
    {
        m_sLocal.SetOffset(dwPos) << CBufferPara(pVal, dwLen);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTaskBase::GetLocal
  ��    ��: ��ȡ���ر���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CTaskBase::GetLocal(DWORD dwPos)
{
    return m_sLocal.Buffer(dwPos);
}

/*******************************************************
  �� �� ��: CTaskBase::Current
  ��    ��: ��ȡ��ǰ����ID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CTaskBase::Current()
{
    DWORD dwCurID = OS_FUNC_CALL(Task, Current)();
    if (dwCurID == FAILURE) dwCurID = 0;
    return dwCurID;
}

