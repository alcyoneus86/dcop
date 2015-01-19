/// -------------------------------------------------
/// ObjStatus_if.h : ״̬�����󹫹�ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJSTATUS_IF_H_
#define _OBJSTATUS_IF_H_

#include "Object_if.h"


/////////////////////////////////////////////////////
///                    ʹ��˵��
/// -------------------------------------------------
/// 1.״̬��Ϊ�Ӷ��󣬱��뱻���еĶ��󵥶�����
/// 2.����ʱ�����߶����Լ���Ϊ״̬���ĸ�������
/// 2.״̬������ע��˳��״ֵ̬��0��ʼ����
/// 3.״̬�����԰���Ĭ��˳�������Ҳ�����ֶ�����Ǩ��
/// 4.״̬��Ϊ����״̬���������ͻ������Զ����¼�����:
///   ======== [����] ================ [״̬��] ========== [�����¼�] ============== [����] ======
///   User(�����¼������) ----------> [Start]    ---> DCOP_MSG_STATUS_START   ---> User(�����¼�)
///                                     |
///                                     V
///   User(�����¼������) ----> +---> [NewState] ---> DCOP_MSG_STATUS_NEW     ---> User(�����¼�)
///                              |      |
///                        (ѭ��)|      V (��ʱ)
///   User(����ʱ���¼�) ----> +------+----->   ---> DCOP_MSG_STATUS_TIMEOUT ---> User(�����¼�)
///                                     |
///                                     V (����)
///   User(�����¼������) ----------> [Stop]     ---> DCOP_MSG_STATUS_STOP    ---> User(�����¼�)
/////////////////////////////////////////////////////


/// ����IStatusMachine�汾��
INTF_VER(IStatus, 1, 0, 0);


/// ״̬���ӿ�
interface IStatus : public IObject
{   
    /// ״̬����(ע��)
    struct REG
    {
        const char *m_cszName;              // ״̬��(״̬����ֻ���泣���ַ�����ַ)
        DWORD m_dwNextStateDefault;         // Ĭ����һ״̬(״ֵ̬)
        DWORD m_dwTimeout;                  // ״̬��ʱʱ��(��Tick()�ӿڱ����õļ��Ϊ��λ)
        DWORD m_dwBackStateWhenTimeout;     // ��ʱ����˵�״̬
    };

    /// ״̬����(����ʱ)
    struct STATE : public REG
    {
        DWORD m_dwRunStateValue;            // ����ʱ״ֵ̬(��0��ʼ��Ҳ��ע��˳�������)
        DWORD m_dwRunPrevState;             // ����ʱǰһ״̬
        DWORD m_dwRunTimeValue;             // ����ʱʱ���ʱ�̶�
    };

    /// ��Ч״ֵ̬
    static const DWORD Invalid = (DWORD)(-1);

    /// ע��״̬(����0Ϊ�ɹ�������Ϊʧ��)
    virtual DWORD Reg(
                REG *pStateNodes,           // ״̬�ڵ��б�
                DWORD dwStateCount          // ״̬��
                ) = 0;

    /// ��ʼ
    virtual DWORD Start() = 0;

    /// ֹͣ
    virtual DWORD Stop() = 0;

    /// ��ʱ����
    virtual DWORD Tick() = 0;

    /// Ǩ�Ƶ���״̬(Ĭ��ΪԤ��״̬)(����0Ϊ�ɹ�������Ϊʧ��)
    virtual DWORD SetNewState(DWORD dwNewState = Invalid) = 0;

    /// ��ȡ��ǰ״ֵ̬(����InvalidΪ��Чֵ������Ϊ��Чֵ)
    virtual DWORD GetCurState() = 0;

    /// ��ȡָ��״̬����Ϣ
    virtual STATE *GetStateInfo(DWORD dwState) = 0;

    /// �Ƿ�������״̬
    virtual bool bRun() = 0;
};


#endif // #ifndef _OBJSTATUSMACHINE_IF_H_

