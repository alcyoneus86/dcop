/// -------------------------------------------------
/// secure_if.h : ��ȫ������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SECURE_IF_H_
#define _SECURE_IF_H_

#include "Object_if.h"


/////////////////////////////////////////////////////
///               '��ȫ�ӿ�'˵��
/// -------------------------------------------------
/// '��ȫ�ӿ�'(secure interface)���ⲿ����ʱ���а�ȫ
/// ��֤����֤���ƽӿڡ�
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// ����ISecure�汾��
/// -------------------------------------------------
INTF_VER(ISecure, 1, 0, 0);


/// -------------------------------------------------
/// ��ȫ����ӿ�
/// -------------------------------------------------
interface ISecure : public IObject
{
    /// �ڵ�
    struct Node
    {
        DWORD m_ownerAttr;
        DWORD m_ownerField;
        DWORD m_ownerRight;
        DWORD m_visitorRight;
        DWORD m_userAttr;
        DWORD m_userField;
        DWORD m_userRight;
        DWORD m_managerAttr;
        DWORD m_managerField;
        DWORD m_managerRight;
        DWORD m_systemOperator;
    };

    /// ע�ᰲȫ������
    virtual DWORD RegRule(
                        Node *rules,                // ������
                        DWORD count                 // ��������
                        ) = 0;
};


#endif // #ifndef _SECURE_IF_H_

