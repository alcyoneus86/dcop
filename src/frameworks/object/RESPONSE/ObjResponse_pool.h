/// -------------------------------------------------
/// ObjResponse_pool.h : ��Ӧ�����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJRESPONSE_POOL_H_
#define _OBJRESPONSE_POOL_H_

#include "ObjResponse_if.h"
#include "ObjDispatch_if.h"
#include "ObjTimer_if.h"


/// -------------------------------------------------
/// ������Ӧ�����
/// -------------------------------------------------
class CResponsePool : public IResponse::IPool
{
public:
    /// ����ؽڵ�
    struct Node
    {
        WORD m_wNewIdx;                     // �µ�����
        WORD m_wOldIdx;                     // �ϵ�����
        DCOP_SESSION_HEAD *m_pSession;      // �Ự��Ϣ
        DWORD m_dwMsgType;                  // ��Ϣ����
        DWORD m_dwSrcID;                    // ԴID
        DWORD m_dwDstID;                    // Ŀ��ID
        DWORD m_dwRspMsgType;               // ��Ӧ��Ϣ����
        ITimer::Handle m_hTimer;            // ��ʱ�����
    };

    /// ��ʱ����������ID����ֵ�Ͳ�λ����
    static const DWORD WHEEL_S_SEC = 0;
    static const DWORD WHEEL_S_SEC_ID = 1;
    static const DWORD WHEEL_S_HASH_BASE = 1000;
    static const DWORD WHEEL_S_SEC_SLOT_COUNT = 32;

    /// TimerValue��ע��Ķ�ʱ���ֽڵ�ֵ
    struct TimerValue
    {
        WORD m_wNewIdx;                     // �µ�����
        WORD m_wOldIdx;                     // �ϵ�����
        DWORD m_dwTimeout;                  // ��ʱʱ��
        DWORD m_dwSendTryTimes;             // �ش�����
    };

public:
    CResponsePool();
    ~CResponsePool();

    DWORD Init(IObject *root, IObject *owner, DWORD count);

    DWORD OnReq(DCOP_SESSION_HEAD *pSession,
                        DWORD dwMsgType,
                        DWORD dwSrcID,
                        DWORD dwDstID,
                        DWORD dwRspMsgType,
                        DWORD dwTimeout,
                        DWORD dwSendTryTimes);

    DWORD OnRsp(DCOP_SESSION_HEAD *pSession);

    void OnTick();

private:
    Node *GetNode(DWORD dwIdx);
    DWORD GenIdleIdx();
    static void OnTimeout(ITimer::Handle handle, void *para);
    DWORD InsertToWheel(ITimer::Handle hTimer, DWORD dwTimeOut = 0);
    void  DelFromWheel(ITimer::Handle hTimer);
    DWORD TrySendTimes(TimerValue *pValue, ITimer::Handle hTimer);
    void OnRspTimeout(Node *pNode);

private:
    IObject *m_pOwner;
    Node *m_pNodeBuf;
    DWORD m_dwNodeCount;
    DWORD m_dwLastIdx;
    IDispatch *m_piDispatch;
    ITimer::IWheel *m_pTimerWheel;
};



#endif // #ifndef _OBJRESPONSE_POOL_H_

