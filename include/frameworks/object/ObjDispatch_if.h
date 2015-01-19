/// -------------------------------------------------
/// ObjDispatch_if.h : ��Ϣ�ַ������󹫹�ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDISPATCH_IF_H_
#define _OBJDISPATCH_IF_H_

#include "Object_if.h"
#include "msg.h"


/// -------------------------------------------------
/// Ĭ����Ϣ����(ʹ������ʱ����Ϣ����)
/// -------------------------------------------------
#define DISPATCH_LENGTH_DEFAULT             0xFFFFFFFF


/// -------------------------------------------------
/// ����IDispatch�汾��
/// -------------------------------------------------
INTF_VER(IDispatch, 1, 0, 0);


/// -------------------------------------------------
/// ��Ϣ�ַ��ӿ�
/// -------------------------------------------------
interface IDispatch : public IObject
{
    /// ��ȡMTU��С
    virtual DWORD GetMTU(
                        ) = 0;

    /// �첽������������Ϣ(����֤�Զ��Ƿ��յ�)
    virtual DWORD Send(objMsg *message) = 0;

    /// ������Ϣ��ȴ���Ӧ(����ֱ���յ��Զ���Ӧ)
    virtual DWORD SendAndWait(
                        objMsg *request, 
                        objMsg **response, 
                        DWORD waittime
                        ) = 0;
};


#endif // #ifndef _OBJDISPATCH_IF_H_

