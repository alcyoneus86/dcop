/// -------------------------------------------------
/// ObjProxy_main.cpp : �������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjProxy_main.h"
#include "Factory_if.h"
#include "Manager_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CProxy, "proxy")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CProxy)
    DCOP_IMPLEMENT_INTERFACE(IProxy)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CProxy)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CProxy::CProxy
  ��    ��: CProxy����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CProxy::CProxy(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CProxy::~CProxy
  ��    ��: CProxy����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CProxy::~CProxy()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CProxy::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CProxy::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CProxy::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CProxy::Fini()
{
}

/*******************************************************
  �� �� ��: CProxy::Proc
  ��    ��: ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CProxy::Proc(objMsg *msg)
{
    if (!msg)
    {
        return;
    }
}

/*******************************************************
  �� �� ��: CProxy::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CProxy::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
}


