/// -------------------------------------------------
/// task.h : ����ϵͳtask�ӿڹ���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TASK_H_
#define _TASK_H_

#include "dcop.h"


/// ��������ĺ�
#define DCOP_CreateTask(szName, pEntry, dwStackSize, dwPriority, pPara) \
    objTask::CreateInstance(szName, pEntry, dwStackSize, dwPriority, pPara, __FILE__, __LINE__)


/// �������(���������˳�ʱ���������������)
class objTask
{
public:
    class IPara                                                 // ��������
    {
    public:
        virtual ~IPara();                                       // ���������˳�ʱ�������ò�������
    };

    typedef void (*OSTASK_ENTRY)(IPara *para);                  // ������ں���ԭ��

    enum OSTASK_PRIORITY                                        // �������ȼ�
    {
        OSTASK_PRIORITY_LOWEST,                                 // ���
        OSTASK_PRIORITY_LOWER,                                  // �ϵ�
        OSTASK_PRIORITY_NORMAL,                                 // ��ͨ
        OSTASK_PRIORITY_HIGHER,                                 // �ϸ�
        OSTASK_PRIORITY_HIGHEST                                 // ���
    };

public:
    static objTask *CreateInstance(const char *szName,
                        OSTASK_ENTRY pEntry,
                        DWORD dwStackSize,
                        DWORD dwPriority,
                        IPara *pPara,
                        const char *file,
                        int line);
    virtual ~objTask() = 0;

    static void Delay(DWORD delayMilliseconds);
    static DWORD Current();
};


/// ԭ�Ӳ�������
class objAtomic
{
public:
    typedef intptr_t T;

public:
    static T Inc(T &val);
    static T Dec(T &val);
    static T Add(T &val, T add);
    static T Sub(T &val, T sub);
    static T Set(T &val, T set);
    static bool CAS(T &val, T cmp, T swap);
};


/// ������(����ԭ�Ӳ��������״̬/����Ƕ��ʹ��)
class objSpinLock
{
public:
    objSpinLock() : m_lock(0) {}
    ~objSpinLock() {}

    void Lock() { while (!objAtomic::CAS(m_lock, 0, 1)) {} }
    void Unlock() { (void)objAtomic::Set(m_lock, 0); }

private:
    objAtomic::T m_lock;
};


/// �Զ�������
#define AutoSpinLock(x) AutoSpinLockLine(x, __LINE__)
#define AutoSpinLockLine(x, line) AutoSpinLockLineEx(x, __LINE__)
#define AutoSpinLockLineEx(x, line) AutoSpinLockEx __tmp_##line(x)
class AutoSpinLockEx
{
public:
    AutoSpinLockEx() {m_pLock = 0;}
    AutoSpinLockEx(objSpinLock *pLock) {m_pLock = pLock; if (m_pLock) m_pLock->Lock();}
    ~AutoSpinLockEx() {if (m_pLock) m_pLock->Unlock(); m_pLock = 0;}
private:
    objSpinLock *m_pLock;
};


/// �������������
#define DCOP_CreateRandom() \
    objRandom::CreateInstance(__FILE__, __LINE__)


/// ���������
class objRandom
{
public:
    static objRandom *CreateInstance(const char *file,
                        int line);
    virtual ~objRandom() = 0;

    /// ���������
    virtual void Gen(void *pBuf, DWORD dwLen) = 0;
};


#endif // #ifndef _TASK_H_

