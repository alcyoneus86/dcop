/// -------------------------------------------------
/// ObjTimer_main.h : ����ʱ��˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJTIMER_MAIN_H_
#define _OBJTIMER_MAIN_H_

#include "ObjTimer_if.h"
#include "ObjDispatch_if.h"
#include "ObjTimer_wheel.h"
#include "task.h"


/// ��ʱ��ѡ���뼶�ͺ��뼶�����ֵ/��Сֵ
#define SEC_TIMER_MAX           30000
#define SEC_TIMER_MIN           1000


/// ��ʱ��ʵ����
class CTimer : public ITimer
{
public:
    /// ��ʱ���������
    struct TaskPara : public objTask::IPara
    {
        CTimer *m_pTimer;

        TaskPara()
        {
            m_pTimer = 0;
        }

        TaskPara(CTimer *pTimer)
        {
            m_pTimer = pTimer;
        }

        ~TaskPara()
        {
            m_pTimer = 0;
        }
    };

    /// �뼶��ʱ��
    enum WHEEL_S
    {
        WHEEL_S_SEC = 0,
        WHEEL_S_MIN,
        WHEEL_S_HOUR,
        WHEEL_S_DAY
    };

    /// ���뼶��ʱ��
    enum WHEEL_MS
    {
        WHEEL_MS_MILL_SEC = 0,
        WHEEL_MS_SEC
    };

    /// ��ʱ��ID
    enum WHEEL_ID
    {
        WHEEL_ID_S_SEC = 1,
        WHEEL_ID_S_MIN,
        WHEEL_ID_S_HOUR,
        WHEEL_ID_S_DAY,
        WHEEL_ID_MS_MILL_SEC,
        WHEEL_ID_MS_SEC
    };

    /// TimerValue��ע��Ķ�ʱ����Ϣ
    struct TimerValue
    {
        IObject *m_pObject;                         // ����
        DWORD m_dwEvent;                            // �¼�
        DWORD m_dwTimeOut;                          // ��ʱʱ��
        ITimer::TYPE m_timerType;                   // ��ʱ������
    };
    
public:
    CTimer(Instance *piParent, int argc, char **argv);
    ~CTimer();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    ITimer::Handle Start(ITimer::TYPE timerType, DWORD dwEvent, DWORD dwTimeOut, IObject *recver);

    void Stop(ITimer::Handle hTimer);

    static void STaskEntry(objTask::IPara *para);
    static void MsTaskEntry(objTask::IPara *para);
    static void TimeoutProc(ITimer::Handle handle, void *para);

    CTimerWheel *GetSTickBase() {return &(m_wheelS[WHEEL_S_SEC]);}
    CTimerWheel *GetMsTickBase() {return &(m_wheelMs[WHEEL_MS_MILL_SEC]);}

    IDispatch *GetDispatch() {return m_piDispatch;}

    DWORD InsertToWheel(DWORD dwTimeOut, ITimer::Handle hTimer);
    DWORD GetTimeNow(CTimerWheel *pWheel, DWORD dwCount);

private:
    IDispatch * m_piDispatch;       // ��Ϣ������
    objTask *   m_pTaskS;           // �뼶����
    CTimerWheel m_wheelS[4];        // �뼶��ʱ��
    objTask *   m_pTaskMs;          // ���뼶����
    CTimerWheel m_wheelMs[2];       // ���뼶��ʱ��
};


#endif // #ifndef _OBJTIMER_MAIN_H_

