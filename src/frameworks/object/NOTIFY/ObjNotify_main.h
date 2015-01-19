/// -------------------------------------------------
/// ObjNotify_main.h : ���ķ���������˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJNOTIFY_MAIN_H_
#define _OBJNOTIFY_MAIN_H_

#define INC_MAP

#include "ObjNotify_if.h"
#include "ObjDispatch_if.h"
#include "ObjNotify_pool.h"
#include "array/darray.h"


class CNotify : public INotify
{
public:
    /// ֪ͨ�����MAP
    typedef std::map<DWORD, CNotifyPool> MAP_NOTIFY;
    typedef MAP_NOTIFY::iterator IT_NOTIFY;

    /// ���Ļ���ڵ�
    struct SubscribeNode
    {
        DWORD consumer;
        DWORD producer;
        DWORD event;
    };

public:
    CNotify(Instance *piParent, int argc, char **argv);
    ~CNotify();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();
    void Proc(objMsg *msg);
    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

    IPool *CreatePool(IObject *producer, 
                        DWORD *events, 
                        DWORD count, 
                        GET_MSG_EVENT fnGetMsgEvent = 0);

    void DestroyPool(IPool *pool);

    DWORD Subscribe(DWORD consumer, 
                        DWORD producer, 
                        DWORD event);

private:
    DWORD SaveTmpOfSubscribe(DWORD consumer, 
                        DWORD producer, 
                        DWORD event);

    void ProcTmpOfSubscribe(CNotifyPool *pPool);

    static bool CompareSubscribeNode(void *pNode, 
                        void *pKey);

private:
    MAP_NOTIFY m_events;                            // �¼�����
    CDArray m_SubscribeTmp;                         // ���Ļ���
    IDispatch *m_piDispatch;                        // ��Ϣ������
};


#endif // #ifndef _OBJNOTIFY_MAIN_H_

