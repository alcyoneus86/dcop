/// -------------------------------------------------
/// secure_if.h : 安全管理公共头文件
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SECURE_IF_H_
#define _SECURE_IF_H_

#include "Object_if.h"


/// -------------------------------------------------
/// 定义ISecure版本号
/// -------------------------------------------------
INTF_VER(ISecure, 1, 0, 0);


/// -------------------------------------------------
/// 安全管理接口
/// -------------------------------------------------
interface ISecure : public IObject
{
    /// 节点
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

