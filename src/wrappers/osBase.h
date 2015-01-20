/// -------------------------------------------------
/// osBase.h : ����ϵͳ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _DCOP_OSBASE_H_
#define _DCOP_OSBASE_H_

#include "dcop.h"
#include "list/list.h"


/// -------------------------------------------------
/// [OS��������] ��Ҫ����:
/// task
/// lock event counter
/// sock lanapp
/// msg msgque
/// objbase
/// ���У�task lock event counter ����׮��
/// ���objbase�ǿ�ܲ�����
/// -------------------------------------------------
enum OSTYPE
{
    OSTYPE_TASK = 0,
    OSTYPE_LOCK,
    OSTYPE_EVENT,
    OSTYPE_COUNTER,
    OSTYPE_SOCK,
    OSTYPE_LANAPP,
    OSTYPE_MSG,
    OSTYPE_MSGQUE,
    OSTYPE_OBJBASE,

    OSTYPE_NUM,
    OSTYPE_NULL = 0xFFFFFFFF
};

static const char * const OSTYPE_INFO[] = 
{
    "task",
    "lock",
    "event",
    "counter",
    "sock",
    "lanapp",
    "msg",
    "msgque",
    "objbase"
};


/// -------------------------------------------------
/// [�з���ֵ���в����ĵ���] OS����API׮�������ú�
/// �÷�ʾ��: dwRc = OS_FUNC_CALL(Thread, Create)(...);
///     �൱�ڵ��� dwRc = g_ThreadFuncs.m_pCreate(...);
/// -------------------------------------------------
#define OS_FUNC_CALL(os, func)  \
    (!(g_##os##Funcs.m_p##func))? (FAILURE) : (g_##os##Funcs.m_p##func)

/// -------------------------------------------------
/// [�޷���ֵ���в�������] OS����API׮�������ú�
/// �÷�ʾ��: OS_VDFUNC_CALL(Thread, Create)(...);
///     �൱�ڵ��� g_ThreadFuncs.m_pCreate(...);
/// -------------------------------------------------
#define OS_VDFUNC_CALL(os, func)  \
    if (g_##os##Funcs.m_p##func) (g_##os##Funcs.m_p##func)

/// -------------------------------------------------
/// [�з���ֵ���޲�������] OS����API׮�������ú�
/// �÷�ʾ��: OS_FUNC_VDCALL(Thread, Create)(...);
///     �൱�ڵ��� (void)g_ThreadFuncs.m_pCreate(...);
/// -------------------------------------------------
#define OS_FUNC_VDCALL(os, func)  \
    if (g_##os##Funcs.m_p##func) (void)(g_##os##Funcs.m_p##func)


/// -------------------------------------------------
/// OS����
/// -------------------------------------------------
#ifdef __cplusplus
class osBase
{
public:
    osBase();
    ~osBase();

    OSHANDLE hGetHandle() const {return m_hHandle;}
    void vSetHandle(OSHANDLE hHandle) {m_hHandle = hHandle;}

    DWORD osGetType() {return m_osType;}
    void *objGetPtr() {return m_objPtr;}

    const char *cszGetName() {return m_szName;}
    void vSetName(const char *cszName);

    DWORD dwGetID() {return m_dwID;}
    void vSetID(DWORD dwID) {m_dwID = dwID;}

    void vAddToList(DWORD osType, void *objPtr);
    void vDelFromList();

    static osBase *First(DWORD osType);
    osBase *Next();
    static osBase *Find(DWORD osType, const char *cszName);
    static osBase *Find(DWORD osType, DWORD dwID);
    static void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

private:
    OSHANDLE m_hHandle;

    DWORD m_osType;
    void *m_objPtr;

    char m_szName[OSNAME_LENGTH];
    DWORD m_dwID;

    LIST_ENTRY(osBase) m_field;
};
#endif // #ifdef __cplusplus

#endif // #ifndef _DCOP_OSBASE_H_

