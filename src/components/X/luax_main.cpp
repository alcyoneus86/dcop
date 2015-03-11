/// -------------------------------------------------
/// luax_main.cpp : lua��չʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "luax_main.h"
#include "Factory_if.h"
#include "Manager_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CLuaX, "luax")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CLuaX)
    DCOP_IMPLEMENT_INTERFACE(ILuaX)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CLuaX)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CLuaX::CLuaX
  ��    ��: CLuaX����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLuaX::CLuaX(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    m_L = NULL;
}

/*******************************************************
  �� �� ��: CLuaX::~CLuaX
  ��    ��: CLuaX����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLuaX::~CLuaX()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CLuaX::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CLuaX::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    LuaInit();

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CLuaX::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLuaX::Fini()
{
    LuaFini();
}

/*******************************************************
  �� �� ��: CLuaX::Proc
  ��    ��: ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLuaX::Proc(objMsg *msg)
{
    if (!msg)
    {
        return;
    }
}

/*******************************************************
  �� �� ��: CLuaX::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLuaX::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    if (!m_L)
    {
        logPrint("[Lua Dump] No Lua Interpreter! \r\n", logPara);
        return;
    }

    if ((argc <= 2) || !argv)
    {
        logPrint("[Lua Dump] No Lua File! \r\n", logPara);
        return;
    }

    /// ����lua�ű�
    int rc = luaL_dofile(m_L, (char *)(argv[argc - 1]));
    if (rc) LuaError(logPrint, logPara, rc);
}

/*******************************************************
  �� �� ��: CLuaX::LuaInit
  ��    ��: Lua��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLuaX::LuaInit()
{
    /// Lua�������ĳ�ʼ��
    m_L = luaL_newstate();
    if (!m_L)
    {
        CHECK_ERRCODE(FAILURE, "Init Lua Interpreter");
        return;
    }

    luaL_openlibs(m_L);
}

/*******************************************************
  �� �� ��: CLuaX::LuaFini
  ��    ��: Lua���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLuaX::LuaFini()
{
    /// ����Lua������
    if (m_L)
    {
        lua_close(m_L);
        m_L = NULL;
    }
}

/*******************************************************
  �� �� ��: CLuaX::LuaError
  ��    ��: Lua������Ϣ��ȡ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLuaX::LuaError(LOG_PRINT logPrint, LOG_PARA logPara, int errNo)
{
    if (!m_L || !logPrint || !errNo) return;

    switch (errNo)
    {
        case LUA_ERRSYNTAX:     // ����ʱ����
            logPrint("[syntax error during pre-compilation] \r\n", logPara);
            break;
        case LUA_ERRMEM:        // �ڴ����
            logPrint("[memory allocation error] \r\n", logPara);
            break;
        case LUA_ERRRUN:        // ����ʱ����
            logPrint("[a runtime error] \r\n", logPara);
            break;
        case LUA_YIELD:         // �̱߳��������
            logPrint("[Thread has Suspended] \r\n", logPara);
            break;
        case LUA_ERRERR:        // �ڽ��д�����ʱ��������
            logPrint("[error while running the error handler function] \r\n", logPara);
            break;
        default:
            break;
    }

    /// ��ӡ������
    const char *errInfo = lua_tostring(m_L, -1);
    if (errInfo)
    {
        logPrint(errInfo, logPara);
        logPrint("\r\n", logPara);
    }
    lua_pop(m_L, 1);
}

