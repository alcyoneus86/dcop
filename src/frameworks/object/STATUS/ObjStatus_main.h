/// -------------------------------------------------
/// ObjStatus_main.h : ״̬������˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJSTATUS_MAIN_H_
#define _OBJSTATUS_MAIN_H_

#include "ObjStatus_if.h"


class CStatus : public IStatus
{
public:
    CStatus(Instance *piParent, int argc, char **argv);
    ~CStatus();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Reg(REG *pStateNodes, DWORD dwStateCount);

    DWORD Start();

    DWORD Stop();

    DWORD Tick();

    DWORD SetNewState(DWORD dwNewState = IStatus::Invalid);

    DWORD GetCurState();

    STATE *GetStateInfo(DWORD dwState);

    bool bRun()
    {
        AutoObjLock(this);
        return m_bRunning;
    }

private:
    DWORD EventToOwner(DWORD event);

private:
    STATE *m_pStateNodes;                   // ״̬�ڵ��б�
    DWORD m_dwStateCount;                   // ״̬����
    DWORD m_dwCurState;                     // ��ǰ״ֵ̬
    bool m_bRunning;                        // �Ƿ���������
};


#endif // #ifndef _STATUS_MAIN_H_

