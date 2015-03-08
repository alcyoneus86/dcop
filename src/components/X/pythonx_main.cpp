/// -------------------------------------------------
/// pythonx_main.cpp : python��չʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "pythonx_main.h"
#include "Factory_if.h"
#include "Manager_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CPythonX, "pythonx")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CPythonX)
    DCOP_IMPLEMENT_INTERFACE(IPythonX)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CPythonX)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CPythonX::CPythonX
  ��    ��: CPythonX����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CPythonX::CPythonX(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CPythonX::~CPythonX
  ��    ��: CPythonX����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CPythonX::~CPythonX()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CPythonX::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CPythonX::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    PythonInit();

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CPythonX::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CPythonX::Fini()
{
    PythonFini();
}

/*******************************************************
  �� �� ��: CPythonX::Proc
  ��    ��: ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CPythonX::Proc(objMsg *msg)
{
    if (!msg)
    {
        return;
    }
}

/*******************************************************
  �� �� ��: CPythonX::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CPythonX::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
}

/*******************************************************
  �� �� ��: CPythonX::PythonInit
  ��    ��: Python��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CPythonX::PythonInit()
{
    /// Python�������ĳ�ʼ��
    Py_Initialize();

    CHECK_ERRCODE(Py_IsInitialized()? SUCCESS : FAILURE, "Init Python Interpreter");
}

/*******************************************************
  �� �� ��: CPythonX::PythonFini
  ��    ��: Python���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CPythonX::PythonFini()
{
    /// ����Python������
    Py_Finalize();
}

