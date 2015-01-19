/// -------------------------------------------------
/// ObjNotify_pool.h : ֪ͨ�¼�����ع���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJNOTIFY_POOL_H_
#define _OBJNOTIFY_POOL_H_

#include "ObjNotify_if.h"


/// -------------------------------------------------
/// �¼������
/// -------------------------------------------------
class CNotifyPool : public INotify::IPool
{
public:
    struct Node
    {        
        DWORD m_dwEvent;                        // �¼�����ֵ
        DWORD m_dwSubscribedCount;              // �����Ĵ���
        DWORD m_dwProducedCount;                // ����������
        DWORD m_dwDispatchedCount;              // ���ַ�����
    };

public:
    CNotifyPool();
    ~CNotifyPool();

    DWORD Init(IObject *owner, DWORD *events, DWORD count, INotify::GET_MSG_EVENT fnGetMsgEvent);

    IObject *GetProducer() {return m_pOwner;}

    DWORD OnSubscribe(DWORD event);

    DWORD OnPublish(DWORD event);

    bool IsSubscribedMsgEvent(objMsg *msg, DWORD event);

    void HaveDispatchedMsgEvent(DWORD event);

    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

private:
    IObject *m_pOwner;                          // �¼�������
    Node *m_pEventsBuf;                         // �¼�������
    DWORD m_dwEventCount;                       // �¼�����ֵ
    INotify::GET_MSG_EVENT m_fnGetMsgEvent;     // �¼�ֵ��ȡ
};



#endif // #define _OBJNOTIFY_POOL_H_

