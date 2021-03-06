/// -------------------------------------------------
/// appbase_if.h : 应用基础公共头文件
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _APPBASE_IF_H_
#define _APPBASE_IF_H_

#include "Object_if.h"


/// -------------------------------------------------
/// 定义应用基础接口版本号
/// -------------------------------------------------
INTF_VER(IAppBase, 1, 0, 0);


/// -------------------------------------------------
/// 应用基础接口
/// -------------------------------------------------
interface IAppBase : public IObject
{
};


#endif // #ifndef _APPBASE_IF_H_

