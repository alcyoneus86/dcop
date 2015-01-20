/// -------------------------------------------------
/// sem.h : �ź������幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SEM_H_
#define _SEM_H_

#include "dcop.h"


/// ������
#define DCOP_CreateLock() \
    objLock::CreateInstance(__FILE__, __LINE__)


/// ������(�����ź���)
class objLock
{
public:
    static objLock *CreateInstance(const char *file,
                        int line);
    virtual ~objLock() = 0;

    virtual void Enter() = 0;

    virtual void Leave() = 0;
};

/// �Զ���
#define AutoLock(x) AutoLockLine(x, __LINE__)
#define AutoLockLine(x, line) AutoLockLineEx(x, line)
#define AutoLockLineEx(x, line) AutoLockEx __tmp_##line(x)
class AutoLockEx
{
public:
    AutoLockEx() {m_pLock = 0;}
    AutoLockEx(objLock *pLock) {m_pLock = pLock; if (m_pLock) m_pLock->Enter();}
    ~AutoLockEx() {if (m_pLock) m_pLock->Leave(); m_pLock = 0;}
private:
    objLock *m_pLock;
};


/// �����¼�
#define DCOP_CreateEvent(bHaveEvent) \
    objEvent::CreateInstance(bHaveEvent, __FILE__, __LINE__)


/// �¼�����(�������ź���)
class objEvent
{
public:
    static objEvent *CreateInstance(bool bHaveEvent,
                        const char *file,
                        int line);
    virtual ~objEvent() = 0;

    virtual DWORD Send() = 0;

    virtual DWORD Recv(
                        DWORD waitMilliseconds = OSWAIT_FOREVER
                        ) = 0;

};


/// ����������
#define DCOP_CreateCounter(initCount, maxCount) \
    objCounter::CreateInstance(initCount, maxCount, __FILE__, __LINE__)


/// ����������(�����ź���)
class objCounter
{
public:
    static objCounter *CreateInstance(DWORD initCount,
                        DWORD maxCount,
                        const char *file,
                        int line);
    virtual ~objCounter() = 0;

    virtual DWORD Take(
                        DWORD waitMilliseconds = OSWAIT_FOREVER
                        ) = 0;

    virtual DWORD Give(
                        DWORD giveCount = 1
                        ) = 0;

};


#endif // #ifndef _SEM_H_

