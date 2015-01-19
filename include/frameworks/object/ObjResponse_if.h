/// -------------------------------------------------
/// ObjResponse_if.h : ��Ӧ�����󹫹�ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJRESPONSE_IF_H_
#define _OBJRESPONSE_IF_H_

#include "Object_if.h"
#include "BaseMessage.h"


/// -------------------------------------------------
/// ����IResponse�汾��
/// -------------------------------------------------
INTF_VER(IResponse, 1, 0, 0);


/// -------------------------------------------------
/// ������Ӧ�ӿ�
/// -------------------------------------------------
interface IResponse : public IObject
{
    /// ���󻺳��(��������ʹ�ã��ڱ��ػ�����Щ����δ����Ӧ)
    interface IPool
    {
        /// ��������ʱ
        virtual DWORD OnReq(
                        DCOP_SESSION_HEAD *pSession,
                        DWORD dwMsgType,
                        DWORD dwSrcID,
                        DWORD dwDstID,
                        DWORD dwRspMsgType,
                        DWORD dwTimeout,
                        DWORD dwSendTryTimes
                        ) = 0;

        /// ������Ӧʱ
        virtual DWORD OnRsp(
                        DCOP_SESSION_HEAD *pSession
                        ) = 0;
    };


    /// ����������
    virtual IPool *CreatePool(
                        IObject *owner,
                        DWORD count
                        ) = 0;

    /// ɾ��������
    virtual void DestroyPool(
                        IPool *pool
                        ) = 0;
};


#endif // #ifndef _OBJRESPONSE_IF_H_

