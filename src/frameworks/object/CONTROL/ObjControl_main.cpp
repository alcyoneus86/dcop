/// -------------------------------------------------
/// ObjControl_main.cpp : ����������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjControl_main.h"
#include "Factory_if.h"
#include "Manager_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CControl, "control")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CControl)
    DCOP_IMPLEMENT_INTERFACE(IControl)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CControl)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CControl::CControl
  ��    ��: CControl����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CControl::CControl(Instance *piParent, int argc, char **argv)
{
    

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CControl::~CControl
  ��    ��: CControl����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CControl::~CControl()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CControl::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CControl::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CControl::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControl::Fini()
{
    
}

/*******************************************************
  �� �� ��: CControl::Proc
  ��    ��: ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControl::Proc(objMsg *msg)
{
    if (!msg)
    {
        return;
    }
}

/*******************************************************
  �� �� ��: CControl::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControl::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{

}

