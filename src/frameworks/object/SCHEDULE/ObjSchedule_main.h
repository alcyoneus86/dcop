/// -------------------------------------------------
/// ObjSchedule_main.h : ��Ϣ����������˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJSCHEDULE_MAIN_H_
#define _OBJSCHEDULE_MAIN_H_

#define INC_MAP

#include "ObjSchedule_if.h"
#include "Manager_if.h"
#include "task.h"


/// Ĭ�ϲ�����������
#define SCHEDULE_COUNT_DEFAULT                  5

/// ����������
#define SCHEDULE_TASKNAME_LEN                   16

/// ��Ϣ���г���
#define SCHEDULE_MSGQUE_LEN                     1024


/// -------------------------------------------------
/// ���û��ӿڲ�ʵ�ֵ�����
/// -------------------------------------------------
class CSchedule : public ISchedule
{
public:
    /// �������ڵ�
    struct TaskNode
    {
        char m_szTaskName[SCHEDULE_TASKNAME_LEN];
        objTask *m_pTask;
        objMsgQue *m_pMsgQue;
        DWORD m_dwInActive;
        DWORD m_dwLastProcObjID;
        DWORD m_dwLastCostTimeStart;
        DWORD m_dwLastCostTimeEnd;
        DWORD m_dwMaxCostTime;
        DWORD m_dwMaxCostObjID;
    };

    /// �������ڱ�������ߵȴ�������Ķ���ڵ��MAP
    struct RecvObjNode
    {
        DWORD m_dwRefCount;
        DWORD m_dwQueIndex;
    };
    typedef std::map<DWORD, RecvObjNode> MAP_RECVOBJ;
    typedef MAP_RECVOBJ::iterator IT_RECVOBJ;

    /// �������
    struct TaskPara : public objTask::IPara
    {
        CSchedule *m_pSchedule;
        objMsgQue *m_pMsgQue;
        DWORD m_dwIndex;

        TaskPara()
        {
            m_pSchedule = 0;
            m_pMsgQue = 0;
            m_dwIndex = 0;
        }

        TaskPara(CSchedule *pSchedule, objMsgQue *pMsgQue, DWORD dwIndex)
        {
            m_pSchedule = pSchedule;
            m_pMsgQue = pMsgQue;
            m_dwIndex = dwIndex;
        }

        ~TaskPara()
        {
            m_pSchedule = 0;
            m_pMsgQue = 0;
            m_dwIndex = 0;
        }
    };

public:
    CSchedule(Instance *piParent, int argc, char **argv);
    ~CSchedule();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();
    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

    void SetProc(PROC proc, void *arg);

    void SetSameDst(bool bOrderReach);

    DWORD Join(objMsg *message);

private:
    DWORD CreateAllTask();
    void  DestroyAllTask();

    static void TaskEntry(objTask::IPara *para);

    void OnRecvStart(DWORD dwIndex, DWORD dwProcObjID);
    void OnRecv(objMsg *message, DWORD dwProcObjID);
    void OnRecvEnd(DWORD dwIndex, DWORD dwProcObjID);

    DWORD OnSend();
    objMsgQue *GetSendQue(DWORD dwObjID, bool bOrderReach);
    objMsgQue *GetRecvQue(DWORD dwObjID, bool bOrderReach);
    void DelRecvObj(DWORD dwObjID, bool bOrderReach);

    void SetSystemID();

private:
    char m_szNameConfig[OSNAME_LENGTH];
    DWORD m_dwCountConfig;
    MAP_RECVOBJ m_recvobjs;
    IManager *m_piManager;
    TaskNode *m_pNodes;
    DWORD m_dwNodeCount;
    DWORD m_dwIdleIndex;
    PROC m_fnMsgProc;
    void *m_pUserArg;
    bool m_bOrderReach;
};


#endif // #ifndef _OBJSCHEDULE_MAIN_H_

