/// -------------------------------------------------
/// ObjDispatch_main.h : ��Ϣ�ַ�������˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDISPATCH_MAIN_H_
#define _OBJDISPATCH_MAIN_H_

#include "ObjDispatch_if.h"
#include "ObjAttribute_if.h"
#include "ObjSchedule_if.h"


class CDispatch : public IDispatch
{
public:
    CDispatch(Instance *piParent, int argc, char **argv);
    ~CDispatch();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);

    void Fini();

    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

    DWORD GetMTU();

    DWORD Send(objMsg *message);

    DWORD SendAndWait(objMsg *request, objMsg **response, DWORD waittime);

    DECLARE_ATTRIBUTE_INDEX(StackLayer);
    DECLARE_ATTRIBUTE(IObject*, pInfLayer);                     // �ӿڲ� : ʹ�õ��ȶ���

    /// ����Ϣ��ؿ���
    void OpenMsgHook();

    /// �ر���Ϣ��ؿ���
    void CloseMsgHook();

    /// ��ȡ��Ϣ��ؿ���״̬
    DWORD GetMsgHookFlag();

    /// �ֲ�ʽЭ��ջ����ӿ�
    static DWORD StackRecvMsg(
                    DWORD dwSrcNodeID, 
                    void *pMsgBuf, 
                    DWORD dwBufLength, 
                    void *pUserArg
                    );

private:
    DWORD m_dwMsgHookFlag;              // ��Ϣ��ر�ʶ

};


#endif // _OBJDISPATCH_MAIN_H_

