/// -------------------------------------------------
/// command_if.h : �����н��빫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _COMMAND_IF_H_
#define _COMMAND_IF_H_

#include "Object_if.h"


/// -------------------------------------------------
/// ����ICommand�汾��
/// -------------------------------------------------
INTF_VER(ICommand, 1, 0, 0);


/// -------------------------------------------------
/// �����н���ӿڶ���
/// -------------------------------------------------
interface ICommand : public IObject
{
    /// ��ӭ��Ϣ
    virtual void Welcome(
                        const char *username, 
                        DWORD session = 0
                        ) = 0;

    /// ����������
    virtual void Line(
                        const char *command, 
                        DWORD session = 0
                        ) = 0;

    /// �������
    virtual void Out(
                        LOG_PRINT fnOut, 
                        LOG_PARA  pPara = 0
                        ) = 0;
};


#endif // #ifndef _COMMAND_IF_H_

