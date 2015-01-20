/// -------------------------------------------------
/// semType.h : �ź�������˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "semType.h"
#include "semApi.h"


/*******************************************************
  �� �� ��: objLock::CreateInstance
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objLock *objLock::CreateInstance(const char *file, int line)
{
    #undef new
    return new (file, line) CLockBase(file, line);
    #define new new(__FILE__, __LINE__)
}

/*******************************************************
  �� �� ��: objLock::~objLock
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objLock::~objLock()
{
}

/*******************************************************
  �� �� ��: CLockBase::CLockBase
  ��    ��: CLockBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLockBase::CLockBase(const char *file, int line)
{
    Initialize(file, line);
}

/*******************************************************
  �� �� ��: CLockBase::~CLockBase
  ��    ��: CLockBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLockBase::~CLockBase()
{
    Delete();
}

/*******************************************************
  �� �� ��: CLockBase::Enter
  ��    ��: ���뻥����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLockBase::Enter()
{
    OS_VDFUNC_CALL(Lock, Enter)(osBase::hGetHandle());
}

/*******************************************************
  �� �� ��: CLockBase::Leave
  ��    ��: �뿪������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLockBase::Leave()
{
    OS_VDFUNC_CALL(Lock, Leave)(osBase::hGetHandle());
}

/*******************************************************
  �� �� ��: CLockBase::Initialize
  ��    ��: ��ʼ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLockBase::Initialize(const char *file, int line)
{
    OSHANDLE hLock = 0;

    OS_VDFUNC_CALL(Lock, Initialize)(&hLock, file, line);

    osBase::vSetHandle(hLock);
}

/*******************************************************
  �� �� ��: CLockBase::Delete
  ��    ��: ɾ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLockBase::Delete()
{
    OS_VDFUNC_CALL(Lock, Delete)(osBase::hGetHandle());

    osBase::vSetHandle(0);
}

/*******************************************************
  �� �� ��: objEvent::CreateInstance
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objEvent *objEvent::CreateInstance(bool bHaveEvent, const char *file, int line)
{
    #undef new
    return new (file, line) CEventBase(bHaveEvent, file, line);
    #define new new(__FILE__, __LINE__)
}

/*******************************************************
  �� �� ��: objEvent::~objEvent
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objEvent::~objEvent()
{
}

/*******************************************************
  �� �� ��: CEventBase::CEventBase
  ��    ��: CEventBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CEventBase::CEventBase(bool bHaveEvent, const char *file, int line)
{
    Create(bHaveEvent, file, line);
}

/*******************************************************
  �� �� ��: CEventBase::~CEventBase
  ��    ��: CEventBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CEventBase::~CEventBase()
{
    (void)Destroy();
}

/*******************************************************
  �� �� ��: CEventBase::Create
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CEventBase::Create(bool bHaveEvent, const char *file, int line)
{
    OSHANDLE hEvent = 0;
    
    DWORD dwRc = OS_FUNC_CALL(Event, Create)(&hEvent, (bHaveEvent?TRUE:FALSE), file, line);
    if (dwRc)
    {
        return dwRc;
    }

    osBase::vSetHandle(hEvent);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CEventBase::Send
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CEventBase::Send()
{
    return OS_FUNC_CALL(Event, Send)(osBase::hGetHandle());
}

/*******************************************************
  �� �� ��: CEventBase::Recv
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CEventBase::Recv(DWORD waitMilliseconds)
{
    return OS_FUNC_CALL(Event, Recv)(osBase::hGetHandle(), waitMilliseconds);
}

/*******************************************************
  �� �� ��: CEventBase::Destroy
  ��    ��: ɾ���¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CEventBase::Destroy()
{
    DWORD dwRc = OS_FUNC_CALL(Event, Destroy)(osBase::hGetHandle());
    if (dwRc)
    {
        return dwRc;
    }

    osBase::vSetHandle(0);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: objCounter::CreateInstance
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objCounter *objCounter::CreateInstance(DWORD initCount, DWORD maxCount, const char *file, int line)
{
    #undef new
    return new (file, line) CCounterBase(initCount, maxCount, file, line);
    #define new new(__FILE__, __LINE__)
}

/*******************************************************
  �� �� ��: objCounter::~objCounter
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objCounter::~objCounter()
{
}

/*******************************************************
  �� �� ��: CCounterBase::CCounterBase
  ��    ��: CCounterBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CCounterBase::CCounterBase(DWORD initCount, DWORD maxCount, const char *file, int line)
{
    Create(initCount, maxCount, file, line);
}

/*******************************************************
  �� �� ��: CCounterBase::~CCounterBase
  ��    ��: CCounterBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CCounterBase::~CCounterBase()
{
    (void)Destroy();
}

/*******************************************************
  �� �� ��: CCounterBase::Create
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCounterBase::Create(DWORD initCount, DWORD maxCount, const char *file, int line)
{
    OSHANDLE hCounter = 0;
    
    DWORD dwRc = OS_FUNC_CALL(Counter, Create)(&hCounter, initCount, maxCount, file, line);
    if (dwRc)
    {
        return dwRc;
    }

    osBase::vSetHandle(hCounter);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CCounterBase::Take
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCounterBase::Take(DWORD waitMilliseconds)
{
    return OS_FUNC_CALL(Counter, Take)(osBase::hGetHandle(), waitMilliseconds);
}

/*******************************************************
  �� �� ��: CCounterBase::Give
  ��    ��: �ͷż�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCounterBase::Give(DWORD giveCount)
{
    return OS_FUNC_CALL(Counter, Give)(osBase::hGetHandle(), giveCount);
}

/*******************************************************
  �� �� ��: CCounterBase::Destroy
  ��    ��: ɾ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCounterBase::Destroy()
{
    DWORD dwRc = OS_FUNC_CALL(Counter, Destroy)(osBase::hGetHandle());
    if (dwRc)
    {
        return dwRc;
    }

    osBase::vSetHandle(0);

    return SUCCESS;
}

