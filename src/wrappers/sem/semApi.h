/// -------------------------------------------------
/// semApi.h : ����ϵͳsem�ӿڹ���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SEMAPI_H_
#define _SEMAPI_H_

#include "../osBase.h"


#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////
/// OS��(�����ź���)API�ص��������Ͷ��� - begin
/////////////////////////////////////////////////////

typedef void (*INITIALIZE_LOCK_FUNC)(OSHANDLE *pHandle, const char *file, int line);
typedef void (*DELETE_LOCK_FUNC)(OSHANDLE Handle);
typedef void (*ENTER_LOCK_FUNC)(OSHANDLE Handle);
typedef void (*LEAVE_LOCK_FUNC)(OSHANDLE Handle);

typedef struct tagLockFuncs
{
    INITIALIZE_LOCK_FUNC    m_pInitialize;
    DELETE_LOCK_FUNC        m_pDelete;
    ENTER_LOCK_FUNC         m_pEnter;
    LEAVE_LOCK_FUNC         m_pLeave;
}LockFuncs;
extern LockFuncs g_LockFuncs;

extern void vSetLockFuncs(const LockFuncs* pFuncs);

/////////////////////////////////////////////////////
/// OS��(�����ź���)API�ص��������Ͷ��� - end
/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
/// OS�¼�(�������ź���)API�ص��������Ͷ��� - begin
/////////////////////////////////////////////////////

typedef DWORD (*CREATE_EVENT_FUNC)(OSHANDLE *pHandle, BOOL bHaveEvent, const char *file, int line);
typedef DWORD (*DESTROY_EVENT_FUNC)(OSHANDLE Handle);
typedef DWORD (*SEND_EVENT_FUNC)(OSHANDLE Handle);
typedef DWORD (*RECV_EVENT_FUNC)(OSHANDLE Handle, DWORD dwMilliseconds);

typedef struct tagEventFuncs
{
    CREATE_EVENT_FUNC   m_pCreate;
    DESTROY_EVENT_FUNC  m_pDestroy;
    SEND_EVENT_FUNC     m_pSend;
    RECV_EVENT_FUNC     m_pRecv;
}EventFuncs;
extern EventFuncs g_EventFuncs;

extern void vSetEventFuncs(const EventFuncs* pFuncs);

/////////////////////////////////////////////////////
/// OS�¼�(�������ź���)API�ص��������Ͷ��� - end
/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
/// OS������(�����ź���)API�ص��������Ͷ��� - begin
/////////////////////////////////////////////////////

typedef DWORD (*CREATE_COUNTER_FUNC)(OSHANDLE *pHandle, DWORD dwInitCount, DWORD dwMaxCount, const char *file, int line);
typedef DWORD (*DESTROY_COUNTER_FUNC)(OSHANDLE Handle);
typedef DWORD (*TAKE_COUNTER_FUNC)(OSHANDLE Handle, DWORD dwMilliseconds);
typedef DWORD (*GIVE_COUNTER_FUNC)(OSHANDLE Handle, DWORD dwGiveCount);

typedef struct tagCounterFuncs
{
    CREATE_COUNTER_FUNC     m_pCreate;
    DESTROY_COUNTER_FUNC    m_pDestroy;
    TAKE_COUNTER_FUNC       m_pTake;
    GIVE_COUNTER_FUNC       m_pGive;
}CounterFuncs;
extern CounterFuncs g_CounterFuncs;

extern void vSetCounterFuncs(const CounterFuncs* pFuncs);

/////////////////////////////////////////////////////
/// OS������(�����ź���)API�ص��������Ͷ��� - end
/////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif


#endif // #ifndef _SEMAPI_H_

