/// -------------------------------------------------
/// taskApi.h : ����ϵͳtask�ӿڹ���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TASKAPI_H_
#define _TASKAPI_H_

#include "../osBase.h"


#ifdef __cplusplus
extern "C" {
#endif


/////////////////////////////////////////////////////
/// OS����API�ص��������Ͷ��� - begin
/////////////////////////////////////////////////////

typedef DWORD (*CREATE_TASK_FUNC)(
                    OSHANDLE *pHandle,
                    const char *cszName,
                    DWORD *pdwID,
                    void (*pEntry)(void *pPara),
                    DWORD dwStackSize,
                    DWORD dwPriority,
                    void *pPara);
typedef DWORD (*DESTROY_TASK_FUNC)(
                    OSHANDLE Handle,
                    const char *cszName,
                    DWORD dwID);
typedef void (*DELAY_TASK_FUNC)(
                    DWORD dwMilliseconds);
typedef DWORD (*CURRENT_TASK_FUNC)();

typedef struct tagTaskFuncs
{
    CREATE_TASK_FUNC  m_pCreate;
    DESTROY_TASK_FUNC m_pDestroy;
    DELAY_TASK_FUNC   m_pDelay;
    CURRENT_TASK_FUNC m_pCurrent;
}TaskFuncs;
extern TaskFuncs g_TaskFuncs;

extern void vSetTaskFuncs(const TaskFuncs* pFuncs);

/////////////////////////////////////////////////////
/// OS�߳�API�ص��������Ͷ��� - end
/////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif


#endif // #ifndef _TASKAPI_H_
