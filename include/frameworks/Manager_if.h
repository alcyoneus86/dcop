/// -------------------------------------------------
/// Manager_if.h : ���������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _MANAGER_IF_H_
#define _MANAGER_IF_H_

#include "Object_if.h"


/// -------------------------------------------------
/// ����IManager�汾��
/// -------------------------------------------------
INTF_VER(IManager, 1, 0, 0);


/// -------------------------------------------------
/// ����������ӿ�
/// -------------------------------------------------
interface IManager : public IObject
{
    /// ����һ������
    virtual DWORD InsertObject(
                        IObject *pObject
                        ) = 0;

    /// ɾ��һ������
    virtual DWORD DeleteObject(
                        IObject *pObject
                        ) = 0;

    /// ��ȡ������ϵͳID
    virtual DWORD GetSystemID(
                        ) = 0;

    /// ��ȡ������ϵͳID
    virtual const char *GetSystemInfo(
                        ) = 0;

    /// �㲥��Ϣ�����ж���
    virtual void BroadcastMsg(
                        objMsg *msg
                        ) = 0;
};


/// -------------------------------------------------
/// 'DCOP_EVENT_MANAGER_INITALLOBJECTS' ����: 
///     Count(DWORD) + ����ID�����б�(DWORD ... )
/// -------------------------------------------------


#endif // #ifndef _OBJMANAGER_IF_H_

