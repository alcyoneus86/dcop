/// -------------------------------------------------
/// ObjNotify_if.h : ���ķ��������󹫹�ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJNOTIFY_IF_H_
#define _OBJNOTIFY_IF_H_

#include "Object_if.h"


/////////////////////////////////////////////////////
///                  '�¼�ID'˵��
/// -------------------------------------------------
/// ����¼��Ƕ��������¼�(DCOP_MSG_OBJECT_EVENT):
///     '�¼�ID'������Ϣ�Ự�е�'����ID'
/// ����¼����������͵��¼�(�Զ���ʵ���¼�Ϊ׼):
///     '�¼�ID'������Ϣ����ֵ(���綨ʱ���¼�)
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// ����INotify�汾��
/// -------------------------------------------------
INTF_VER(INotify, 1, 0, 0);


/// -------------------------------------------------
/// ����֪ͨ�ӿ�(�����߷����¼��������߶����¼�)
/// -------------------------------------------------
interface INotify : public IObject
{
    /// ����Ϣ�л�ȡ�¼�ֵ�Ļص�����
    typedef DWORD (*GET_MSG_EVENT)(objMsg *msg);
    
    /// �¼������(��������ʹ�ã��ڱ��ػ�����Щ�¼�������)
    interface IPool
    {
        /// ����ʱ
        virtual DWORD OnSubscribe(
                        DWORD event
                        ) = 0;

        /// ����ʱ
        virtual DWORD OnPublish(
                        DWORD event
                        ) = 0;
    };

    /// ���������(�������ߵ���)(����Init�ӿڵ���)
    virtual IPool *CreatePool(
                        IObject *producer,          // ������
                        DWORD *events,              // �������¼��б�
                        DWORD count,                // �¼�����
                        GET_MSG_EVENT fnGetMsgEvent = 0 // ��Ϣ�¼�ֵ��ȡ�ص�
                        ) = 0;

    /// ɾ�������(�������ߵ���)(����Fini�ӿڵ���)
    virtual void DestroyPool(
                        IPool *pool                 // �����Ļ����
                        ) = 0;

    /// �¼�����(�������ߵ���)(����Init�ӿڵ���)
    virtual DWORD Subscribe(
                        DWORD consumer,             // ������
                        DWORD producer,             // ������
                        DWORD event                 // �¼�
                        ) = 0;

};


/// -------------------------------------------------
/// �����¼� - ����
/// -------------------------------------------------
#define SUBSCRIBE_EVENT_START(Notify)               \
    DWORD __rcSubscribe = SUCCESS;                  \
    INotify *__objNotify = Notify;                  \
    do {


/// -------------------------------------------------
/// �����¼� - ����
/// -------------------------------------------------
#define SUBSCRIBE_EVENT_ITEM(Object, Event)         \
    __rcSubscribe = __objNotify->Subscribe(ID(), Object, Event); \
    if (__rcSubscribe != SUCCESS) break;


/// -------------------------------------------------
/// �����¼� - ��β
/// -------------------------------------------------
#define SUBSCRIBE_EVENT_END                         \
    } while (0);                                    \
    if (__rcSubscribe != SUCCESS) return __rcSubscribe;


#endif // #ifndef _OBJNOTIFY_IF_H_

