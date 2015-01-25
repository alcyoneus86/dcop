/// -------------------------------------------------
/// semStub_win32.cpp : windows����ϵͳ�ź���ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "dcop.h"

#if DCOP_OS == DCOP_OS_WINDOWS

#include "semApi.h"
#include <windows.h>


/// =================================================
///     �ȴ��ź�
/// -------------------------------------------------
/// WaitForSingleObject�ɹ�ʱ����������ֵ:
/// WAIT_ABANDONED - �����mutex����ʱ�����һ�ȡ�����
///     �߳�û���ͷžͽ����ˣ����ʱ������ӵ��Ȩ�ͻ�
///     ����ǰ���õ��̣߳������ź�״ֵ̬����
/// WAIT_OBJECT_0  - ��ȡ������ź�״ֵ̬����
/// WAIT_TIMEOUT   - �ȴ�ʱ�䵽�ˣ����Ƕ���״̬������
/// =================================================


/// =================================================
///     mutex��critical section������
/// -------------------------------------------------
/// 1.��סһ��δ��ӵ�е�mutex������סһ��δ��ӵ�е�
///   critical section��Ҫ���Ѽ���100����ʱ�䡣��Ϊ
///   critical section����Ҫ�������ϵͳ���ġ�
/// 2.mutex���Կ����ʹ�ã�critical section��ֻ�ܹ���
///   ͬһ������ʹ�á�
/// 3.�ȴ�һ��mutexʱ�������ָ���������ȴ�����ʱ�䳤
///   �ȣ���critical section ����Ҫָ���ȴ�ʱ�䣬ֱ��
///   ���뻥����Ϊֹ��
/// =================================================


/*******************************************************
  �� �� ��: STUB_CSInitialize
  ��    ��: ��ʼ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_CSInitialize(OSHANDLE *pHandle, 
                        const char *file, int line)
{
    if (!pHandle)
    {
        return;
    }

    *pHandle = NULL;

    #undef new
    CRITICAL_SECTION *pCS = new (file, line) CRITICAL_SECTION;
    #define new new(__FILE__, __LINE__)
    if (!pCS)
    {
        return;
    }

    InitializeCriticalSection(pCS);

    *pHandle = (OSHANDLE)(pCS);
}

/*******************************************************
  �� �� ��: STUB_CSDelete
  ��    ��: ɾ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_CSDelete(OSHANDLE Handle)
{
    if (!Handle)
    {
        return;
    }

    CRITICAL_SECTION *pCS = (CRITICAL_SECTION *)Handle;

    DeleteCriticalSection(pCS);

    delete pCS;
}

/*******************************************************
  �� �� ��: STUB_CSEnter
  ��    ��: ���뻥����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_CSEnter(OSHANDLE Handle)
{
    if (!Handle)
    {
        return;
    }

    EnterCriticalSection((CRITICAL_SECTION *)Handle);
}

/*******************************************************
  �� �� ��: STUB_CSLeave
  ��    ��: �뿪������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_CSLeave(OSHANDLE Handle)
{
    if (!Handle)
    {
        return;
    }

    LeaveCriticalSection((CRITICAL_SECTION *)Handle);
}

/*******************************************************
  �� �� ��: STUB_EventCreate
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_EventCreate(OSHANDLE *pHandle, 
                        BOOL bHaveEvent, 
                        const char *file, int line)
{
    if (!pHandle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    *pHandle = (OSHANDLE)CreateEvent(
                    NULL,
                    FALSE,
                    bHaveEvent,
                    NULL);
    ERROR_CHECK((*pHandle));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: STUB_EventDestroy
  ��    ��: ɾ���¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_EventDestroy(OSHANDLE Handle)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    ERROR_CHECK(CloseHandle((HANDLE)Handle));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: STUB_EventSend
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_EventSend(OSHANDLE Handle)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    ERROR_CHECK(SetEvent((HANDLE)Handle));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: STUB_EventRecv
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_EventRecv(OSHANDLE Handle, DWORD dwMilliseconds)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    ERROR_CHECK(WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)Handle, 
        ((dwMilliseconds == OSWAIT_FOREVER)? INFINITE : dwMilliseconds)));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: STUB_SemCreate
  ��    ��: �����ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_SemCreate(OSHANDLE *pHandle, 
                        DWORD dwInitCount, 
                        DWORD dwMaxCount, 
                        const char *file, int line)
{
    if (!pHandle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    *pHandle = (OSHANDLE)CreateSemaphore(
                    NULL,
                    (LONG)dwInitCount,
                    (LONG)dwMaxCount,
                    NULL);
    ERROR_CHECK((*pHandle));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: STUB_SemDestroy
  ��    ��: ɾ���ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_SemDestroy(OSHANDLE Handle)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    ERROR_CHECK(CloseHandle((HANDLE)Handle));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: STUB_SemTake
  ��    ��: ��ȡ�ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_SemTake(OSHANDLE Handle, DWORD dwMilliseconds)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    ERROR_CHECK(WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)Handle, 
        ((dwMilliseconds == OSWAIT_FOREVER)? INFINITE : dwMilliseconds)));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: STUB_SemGive
  ��    ��: �ͷ��ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_SemGive(OSHANDLE Handle, DWORD dwGiveCount)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    ERROR_CHECK(ReleaseSemaphore((HANDLE)Handle, dwGiveCount, NULL));

    return SUCCESS;

ERROR_LABEL:

    DWORD dwRc = GetLastError();
    if (!dwRc)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: vRegOsSemStubFunc
  ��    ��: ע���ź���׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void vRegOsSemStubFunc()
{
    LockFuncs lockFuncs = 
    {
        STUB_CSInitialize,
        STUB_CSDelete,
        STUB_CSEnter,
        STUB_CSLeave
    };

    vSetLockFuncs(&lockFuncs);

    EventFuncs eventFuncs = 
    {
        STUB_EventCreate,
        STUB_EventDestroy,
        STUB_EventSend,
        STUB_EventRecv
    };

    vSetEventFuncs(&eventFuncs);

    CounterFuncs counterFuncs = 
    {
        STUB_SemCreate,
        STUB_SemDestroy,
        STUB_SemTake,
        STUB_SemGive
    };

    vSetCounterFuncs(&counterFuncs);
}

/*******************************************************
  �� �� ��: CPPBUILDUNIT_AUTO
  ��    ��: �Զ���װ�ź���׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CPPBUILDUNIT_AUTO(vRegOsSemStubFunc, 0);


#endif // #if DCOP_OS == DCOP_OS_WINDOWS

