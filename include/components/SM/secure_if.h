/// -------------------------------------------------
/// secure_if.h : ��ȫ������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SECURE_IF_H_
#define _SECURE_IF_H_

#include "Object_if.h"


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
    struct NODE
    {
        DWORD OwnerAttr;
        DWORD OwnerField;
        DWORD OwnerRight;
        DWORD VisitorAttr;
        DWORD VisitorField;
        DWORD VisitorRight;
        DWORD UserAttr;
        DWORD UserField;
        DWORD UserRight;
        DWORD AdminAttr;
        DWORD AdminField;
        DWORD AdminRight;
    };
};


#endif // #ifndef _SECURE_IF_H_

