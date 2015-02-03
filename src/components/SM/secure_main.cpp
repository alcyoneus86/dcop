/// -------------------------------------------------
/// secure_main.cpp : ��ȫ����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "secure_main.h"
#include "Factory_if.h"
#include "Manager_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CSecure, "secure")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CSecure)
    DCOP_IMPLEMENT_INTERFACE(ISecure)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CSecure)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CSecure)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END


/*******************************************************
  �� �� ��: CSecure::CSecure
  ��    ��: CSecure���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSecure::CSecure(Instance *piParent, int argc, char **argv)
{
    m_piControl = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CSecure::~CSecure
  ��    ��: CSecure��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSecure::~CSecure()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CSecure::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IControl,     DCOP_OBJECT_CONTROL,    m_piControl)
    DCOP_QUERY_OBJECT_END

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSecure::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::Fini()
{
    DCOP_RELEASE_INSTANCE(m_piControl);
}

/*******************************************************
  �� �� ��: CSecure::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
}

/*******************************************************
  �� �� ��: CSecure::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::OnStart(objMsg *msg)
{
    /// ע�ᰲȫ����
    if (m_piControl)
    {
        IControl::Node node[] = 
        {
            {0, DCOP_CTRL_NULL, DCOP_REQ, DCOP_GROUP_VISITOR, 0, InputCtrl},
        };
        DWORD dwRc = m_piControl->RegCtrlNode(this, 
                        DCOP_OBJECT_ACCESS, 
                        node, 
                        ARRAY_SIZE(node));
        CHECK_ERRCODE(dwRc, "Reg Ctrl Node To Access");
    }
}

/*******************************************************
  �� �� ��: CSecure::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::OnFinish(objMsg *msg)
{
}

/*******************************************************
  �� �� ��: CSecure::InputCtrl
  ��    ��: �������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::InputCtrl(objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        IObject *piCtrler)
{
    /// ��ȡ��Ϣͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(pInput->GetDataBuf(), pInput->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    if (!aSessHeads.Count())
    {
        return FAILURE;
    }

    /// ��ȡ�Ựͷ(ֻ��ȡ��һ���Ựͷ)
    DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(0);
    if (!pSessionHead)
    {
        return FAILURE;
    }

    PrintLog(STR_FORMAT("group:%d\r\n session:%d\r\n user:%d\r\n tty:%d\r\n attribute:0x%x\r\n",
                        pSessionHead->m_group,
                        pSessionHead->m_session,
                        pSessionHead->m_user,
                        pSessionHead->m_tty,
                        pSessionHead->m_attribute), 
                        PrintToConsole, 0);

    return SUCCESS;
}

