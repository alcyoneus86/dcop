/// -------------------------------------------------
/// semStub_linux.cpp : linux����ϵͳ�ź���ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "task.h"

#if DCOP_OS == DCOP_OS_LINUX

#include "semApi.h"
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>


#define  LINUX_SEM_DELAY_TICK 50

/*******************************************************
  �� �� ��: STUB_MutexInitialize
  ��    ��: ��ʼ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_MutexInitialize(OSHANDLE *pHandle, 
                        const char *file, int line)
{
    if (!pHandle)
    {
        return;
    }

    *pHandle = NULL;

    /// �������ռ�
    #undef new
    pthread_mutex_t *pMutex = new (file, line) pthread_mutex_t;
    #define new new(__FILE__, __LINE__)
    if (!pMutex)
    {
        return;
    }

    /// ����ͬһ�߳�Ƕ��������
    pthread_mutexattr_t attrMutex;
    pthread_mutexattr_init(&attrMutex);
    pthread_mutexattr_settype(&attrMutex, PTHREAD_MUTEX_RECURSIVE_NP);

    /// ��ʼ��������
    (void)pthread_mutex_init(pMutex, &attrMutex);

    *pHandle = (OSHANDLE)(pMutex);
}

/*******************************************************
  �� �� ��: STUB_MutexDestroy
  ��    ��: ɾ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_MutexDestroy(OSHANDLE Handle)
{
    if (!Handle)
    {
        return;
    }

    pthread_mutex_t *pMutex = (pthread_mutex_t *)Handle;
    (void)pthread_mutex_destroy(pMutex);

    delete pMutex;
}

/*******************************************************
  �� �� ��: STUB_MutexEnter
  ��    ��: ���뻥����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_MutexEnter(OSHANDLE Handle)
{
    if (!Handle)
    {
        return;
    }

    (void)pthread_mutex_lock((pthread_mutex_t *)Handle);
}

/*******************************************************
  �� �� ��: STUB_MutexLeave
  ��    ��: �뿪������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_MutexLeave(OSHANDLE Handle)
{
    if (!Handle)
    {
        return;
    }

    (void)pthread_mutex_unlock((pthread_mutex_t *)Handle);
}

/*******************************************************
  �� �� ��: STUB_CondInitialize
  ��    ��: ��ʼ�������ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_CondInitialize(OSHANDLE *pHandle, 
                        BOOL bHaveEvent, 
                        const char *file, int line)
{
    if (!pHandle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    *pHandle = NULL;

    /// �������ռ�(������ + ��������)
    #undef new
    char *pTmp = new (file, line) char [sizeof(pthread_mutex_t) + sizeof(pthread_cond_t)];
    #define new new(__FILE__, __LINE__)
    if (!pTmp)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    (void)memset(pTmp, 0, sizeof(pthread_mutex_t) + sizeof(pthread_cond_t));

    pthread_mutex_t *pMutex = (pthread_mutex_t *)pTmp;
    pthread_cond_t *pCond = (pthread_cond_t *)(pTmp + sizeof(pthread_mutex_t));

    /// ��ʼ������������������
    (void)pthread_mutex_init(pMutex, NULL);
    (void)pthread_cond_init(pCond, NULL);

    if (bHaveEvent)
    {
        pthread_cond_signal(pCond);
    }

    *pHandle = (OSHANDLE)(pTmp);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: STUB_CondDestroy
  ��    ��: ɾ�������ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_CondDestroy(OSHANDLE Handle)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    char *pTmp = (char *)Handle;
    pthread_mutex_t *pMutex = (pthread_mutex_t *)pTmp;
    pthread_cond_t *pCond = (pthread_cond_t *)(pTmp + sizeof(pthread_mutex_t));

    (void)pthread_cond_destroy(pCond);
    (void)pthread_mutex_destroy(pMutex);

    delete [] pTmp;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: STUB_CondSend
  ��    ��: ���������ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_CondSend(OSHANDLE Handle)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    pthread_mutex_t *pMutex = (pthread_mutex_t *)Handle;
    pthread_cond_t *pCond = (pthread_cond_t *)(pMutex + 1);

    (void)pthread_cond_signal(pCond);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: STUB_CondRecv
  ��    ��: ���������ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_CondRecv(OSHANDLE Handle, DWORD dwMilliseconds)
{
    if (!Handle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    struct timespec abstime;
    struct timeval now;

    /// ��ȡ��ǰʱ��
    gettimeofday(&now, NULL);
    abstime.tv_sec = now.tv_sec + dwMilliseconds / THOUSAND;
    abstime.tv_nsec = now.tv_usec + (dwMilliseconds % THOUSAND) * THOUSAND;

    /// ��ֹ΢�����
    if (abstime.tv_nsec >= BILLION)
    {
        abstime.tv_sec += abstime.tv_nsec / BILLION;
        abstime.tv_nsec %= BILLION;
    }

    pthread_mutex_t *pMutex = (pthread_mutex_t *)Handle;
    pthread_cond_t *pCond = (pthread_cond_t *)(pMutex + 1);

    int iRc;
    pthread_mutex_lock(pMutex);
    if (OSWAIT_FOREVER == dwMilliseconds)
    {
        iRc = pthread_cond_wait(pCond, pMutex);
    }
    else
    {
        iRc = pthread_cond_timedwait(pCond, pMutex, &abstime);
    }
    pthread_mutex_unlock(pMutex);

    if ((iRc < 0) && (errno == ETIMEDOUT))
    {
        return ERRCODE_SEM_WAIT_TIMEOUT;
    }

    if (!iRc)
    {
        return SUCCESS;
    }

    return FAILURE;
}

/*******************************************************
  �� �� ��: STUB_SemInitialize
  ��    ��: ��ʼ���ź���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_SemInitialize(OSHANDLE *pHandle, 
                        DWORD dwInitCount, 
                        DWORD dwMaxCount, 
                        const char *file, int line)
{
    if (!pHandle)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    *pHandle = NULL;

    /// �������ռ�(�źŵ� + ������)
    #undef new
    char *pTmp = new (file, line) char [sizeof(sem_t) + sizeof(DWORD)];
    #define new new(__FILE__, __LINE__)
    if (!pTmp)
    {
        return ERRCODE_SEM_WRONG_HANDLE;
    }

    (void)memset(pTmp, 0, sizeof(sem_t) + sizeof(DWORD));

    sem_t *pSem = (sem_t *)pTmp;
    DWORD *pdwCount = (DWORD *)(pTmp + sizeof(sem_t));

    /// ��ʼ���źŵ�
    (void)sem_init(pSem, 0, (unsigned int)dwInitCount);
    *pdwCount = dwMaxCount;

    *pHandle = (OSHANDLE)(pTmp);

    return SUCCESS;
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

    char *pTmp = (char *)Handle;
    sem_t *pSem = (sem_t *)pTmp;

    (void)sem_destroy(pSem);

    delete [] pTmp;

    return SUCCESS;
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

    char *pTmp = (char *)Handle;
    sem_t *pSem = (sem_t *)pTmp;

    /// ��Զ�ȴ�: ֱ�ӻ�ȡ
    if (OSWAIT_FOREVER == dwMilliseconds)
    {
        if ((sem_wait(pSem) < 0) && (errno == ETIMEDOUT))
        {
            return ERRCODE_SEM_WAIT_TIMEOUT;
        }

        return SUCCESS;
    }

    /// ���޵ȴ�: linux�ź���û�а���ʱ��ȴ���ֻ����ʱ���Ի�ȡ
    DWORD timeout = 0;
    DWORD delayCount = dwMilliseconds / LINUX_SEM_DELAY_TICK;
    do
    {
        int iRc = sem_trywait(pSem);
        if (!iRc)
        {
            return SUCCESS;
        }

        if (errno != EPERM)
        {
            return (DWORD)errno;
        }

        objTask::Delay(LINUX_SEM_DELAY_TICK);
        timeout++;
    }while (timeout < delayCount);

    return ERRCODE_SEM_WAIT_TIMEOUT;
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

    char *pTmp = (char *)Handle;
    sem_t *pSem = (sem_t *)pTmp;
    DWORD *pdwCount = (DWORD *)(pTmp + sizeof(sem_t));

    int sval = 0;
    (void)sem_getvalue(pSem, &sval);
    if ((DWORD)sval >= (*pdwCount))
    {
        return ERRCODE_SEM_OVER_MAXCOUNT;
    }

    (void)sem_post(pSem);

    return SUCCESS;
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
        STUB_MutexInitialize,
        STUB_MutexDestroy,
        STUB_MutexEnter,
        STUB_MutexLeave
    };

    vSetLockFuncs(&lockFuncs);

    EventFuncs eventFuncs = 
    {
        STUB_CondInitialize,
        STUB_CondDestroy,
        STUB_CondSend,
        STUB_CondRecv
    };

    vSetEventFuncs(&eventFuncs);

    CounterFuncs counterFuncs = 
    {
        STUB_SemInitialize,
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


#endif // #if DCOP_OS == DCOP_OS_LINUX

