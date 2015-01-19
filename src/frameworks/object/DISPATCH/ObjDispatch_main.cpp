/// -------------------------------------------------
/// ObjDispatch_main.cpp : ��Ϣ�ַ�������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjDispatch_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "BaseID.h"
#include "sock.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CDispatch, "dispatch")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CDispatch)
    DCOP_IMPLEMENT_INTERFACE(IDispatch)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CDispatch)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CDispatch::CDispatch
  ��    ��: CDispatch���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDispatch::CDispatch(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    m_dwMsgHookFlag = DCOP_CLOSE;
}

/*******************************************************
  �� �� ��: CDispatch::~CDispatch
  ��    ��: CDispatch��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDispatch::~CDispatch()
{
    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CDispatch::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    ISchedule *piSchedule = 0;
    DCOP_QUERY_OBJECT(ISchedule, DCOP_OBJECT_SCHEDULE, root, piSchedule);
    if (!piSchedule)
    {
        return FAILURE;
    }

    m_pInfLayer = piSchedule;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDispatch::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDispatch::Fini()
{
}

/*******************************************************
  �� �� ��: CDispatch::Dump
  ��    ��: DUMP���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDispatch::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
}

/*******************************************************
  �� �� ��: CDispatch::GetMTU
  ��    ��: ��ȡMTU
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::GetMTU()
{
    /// ͨ����MTU��������������Ϣͷ��������Ӧ��ȥͷ����С
    return OSSOCK_DEFAULT_MTU - OSMsgHeader::GetHeaderSize();
}

/*******************************************************
  �� �� ��: CDispatch::Send
  ��    ��: ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::Send(objMsg *message)
{
    AutoObjLock(this);

    ISchedule *pSchedule = dynamic_cast<ISchedule *>((IObject *)m_pInfLayer);
    if (!pSchedule || !message)
    {
        return FAILURE;
    }

    return pSchedule->Join(message);
}

/*******************************************************
  �� �� ��: CDispatch::SendAndWait
  ��    ��: ���ͺ�ȴ���Ӧ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::SendAndWait(objMsg *request, objMsg **response, DWORD waittime)
{
    AutoObjLock(this);

    return 0;
}

/*******************************************************
  �� �� ��: CDispatch::OpenMsgHook
  ��    ��: ����Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDispatch::OpenMsgHook()
{
    AutoObjLock(this);

    m_dwMsgHookFlag = DCOP_OPEN;
}

/*******************************************************
  �� �� ��: CDispatch::CloseMsgHook
  ��    ��: �ر���Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDispatch::CloseMsgHook()
{
    AutoObjLock(this);

    m_dwMsgHookFlag = DCOP_CLOSE;
}

/*******************************************************
  �� �� ��: CDispatch::GetMsgHookFlag
  ��    ��: ��ȡ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::GetMsgHookFlag()
{
    AutoObjLock(this);

    return m_dwMsgHookFlag;
}

/*******************************************************
  �� �� ��: CDispatch::StackRecvMsg
  ��    ��: �ֲ�ʽЭ��ջ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::StackRecvMsg(
                    DWORD dwSrcNodeID, 
                    void *pMsgBuf, 
                    DWORD dwBufLength, 
                    void *pUserArg
                    )
{
    return 0;
}

