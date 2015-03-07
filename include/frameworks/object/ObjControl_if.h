/// -------------------------------------------------
/// ObjControl_if.h : ���������󹫹�ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJCONTROL_IF_H_
#define _OBJCONTROL_IF_H_

#include "Object_if.h"


/// -------------------------------------------------
/// ����IControl�汾��
/// -------------------------------------------------
INTF_VER(IControl, 1, 0, 0);


/// -------------------------------------------------
/// �������ӿ�(ͨ������Ԥ��Ľӿڽ��п���)
/// -------------------------------------------------
interface IControl : public IObject
{
    /// ���ƻص�����
    typedef DWORD (*ON_CTRL_FUNC)(
                        objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        IObject *piCtrler,
                        bool bLastNode
                        );

    /// ���Ƶ�����
    struct Node
    {
        DWORD m_attribute;                          // ����ID
        BYTE m_ctrl;                                // ��������
        BYTE m_ack;                                 // Ӧ������
        BYTE m_group;                               // �û�������
        BYTE m_tty;                                 // �ն�����
        ON_CTRL_FUNC m_function;                    // �ص�����
    };

    /// �������ӿ�
    interface IChain
    {
        /// ����ʱ
        virtual DWORD OnCtrl(
                        objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue
                        ) = 0;
    };

    /// ����������(��'��������'�ڳ�ʼ��ʱ����)
    virtual IChain *CreateChain(
                        IObject *ctrlee             // ��������
                        ) = 0;

    /// ɾ��������(��'��������'�ڽ���ʱ����)
    virtual void DestroyChain(
                        IChain *chain               // ������
                        ) = 0;

    /// ע����Ƶ�(��'������'(������������Ķ���)��'����'�¼��е���)
    virtual DWORD RegCtrlNode(
                        IObject *ctrler,            // ������
                        DWORD ctrlee,               // ��������
                        Node *ctrls,                // ���Ƶ��б�
                        DWORD count                 // ���Ƶ�����
                        ) = 0;
};


#endif // #ifndef _OBJCONTROL_IF_H_

