/// -------------------------------------------------
/// BaseClass.h : ʵ�����๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _BASECLASS_H_
#define _BASECLASS_H_

#include "task.h"


/////////////////////////////////////////////////////
///              Instanceʹ��˵��
/// -------------------------------------------------
/// 1.  h�ļ�CMyClass�н���:
///         DCOP_DECLARE_INSTANCE;              // ����Instance��Ա
/// -------------------------------------------------
/// 2.  cpp�ļ���CMyClass�����:
///     DCOP_IMPLEMENT_INSTANCE(CMyClass)       // ʵ��Instance��Ա
///         ... ...
///         DCOP_IMPLEMENT_INTERFACE(IA)        // �ӿ�A��ʵ��(�������δ����ൽ����Instance)
///         DCOP_IMPLEMENT_INTERFACE(Instance)
///     DCOP_IMPLEMENT_INSTANCE_END             // ����
/// -------------------------------------------------
/// 3.  CMyClass���캯����ʼ:
///     CMyClass(Instance *piParent, int argc, char **argv)
///     {
///         DCOP_CONSTRUCT_INSTANCE(piParent);  // ���븸����ָ��
/// 
///         ... ...
///     }
/// -------------------------------------------------
/// 4.  CMyClass������������:
///     ~CMyClass()
///     {
///         ... ...
/// 
///         DCOP_DESTRUCT_INSTANCE();
///     }
/// -------------------------------------------------
/// 5.  ʵ���๤���������ģ��, λ�ò��̶�, cpp�м���
///     DCOP_IMPLEMENT_FACTORY(CMyClass, "MyName") // ����ʵ���ൽ�೧����
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// �汾����(�ӿ���,��汾,С�汾,΢�汾)
/// -------------------------------------------------
#define INTF_VER(Interface, major, minor, micro)    \
    const DWORD ___VERSION_##Interface = (          \
        (((major) << 16) & 0xFFFF0000) |            \
        (((minor) <<  8) & 0x0000FF00) |            \
        ((micro)         & 0x000000FF))
/// -------------------------------------------------
#define INTF_VER_MAJOR(Interface)                   \
        ((___VERSION_##Interface >> 16 ) & 0x0000FFFF)
/// -------------------------------------------------
#define INTF_VER_MINOR(Interface)                   \
        ((___VERSION_##Interface >>   8) & 0x000000FF)
/// -------------------------------------------------
#define INTF_VER_MICRO(Interface)                   \
        ((___VERSION_##Interface)        & 0x000000FF)
/// -------------------------------------------------
INTF_VER(Instance, 1, 0, 0);
/// -------------------------------------------------


/// -------------------------------------------------
/// ʵ��������(������������Ϊ)
/// -------------------------------------------------
class objBase
{
public:
    /// �����û����ȫ��ʵ��(��:����ں˶���)
    static objBase *GetInstance();

    /// ���л����Ϊ������
    virtual ~objBase() = 0;

    /// ������Ϣ: ���ֺ�ID
    ///     Name: ֻ��������ʶʵ�����
    ///     ID  : ����Ψһ����һ��ʵ��
    virtual const char *Name() {return 0;}
    virtual DWORD ID() {return 0;}

    /// ������뿪�Ļ��Ᵽ��
    ///     ���Ᵽ��: ��ʵ��ʵ�����Լ�������ڽ��б���(Ĭ��Ϊ��ʵ��)
    ///     ʹ�÷���: ������ں�����ʼʹ��AutoObjLock(this)���򻯲���
    virtual void Enter() {}
    virtual void Leave() {}

    /// Dump���
    ///     ���ʹ��: ʵ��ʱ�밴Լ����˳���ȡ����ָ����������
    virtual void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv) {}

    /// ����һ���µ�ϵͳ(ʹ�ò��������ļ�)[���ظ�ϵͳ������(������)]
    virtual objBase *Deploy(const char *cfgDeploy) {return 0;}
};


/// -------------------------------------------------
/// �Զ�������������ʵ����ں����Զ������ͽ���
/// -------------------------------------------------
#define AutoObjLock(x) AutoObjLockLine(x, __LINE__)
#define AutoObjLockLine(x, line) AutoObjLockLineEx(x, line)
#define AutoObjLockLineEx(x, line) AutoObjLockEx __tmp_##line(x)
class AutoObjLockEx
{
public:
    AutoObjLockEx() {m_piBase = 0;}
    AutoObjLockEx(objBase *piBase) {m_piBase = piBase; if (m_piBase) m_piBase->Enter();}
    ~AutoObjLockEx() {if (m_piBase) m_piBase->Leave();}

private:
    objBase *m_piBase;
};


/// -------------------------------------------------
/// �����ѯ�ӿڵķ��غͲ���(���ֺͰ汾)
/// -------------------------------------------------
struct IntfNameVer
{
    const char *m_interfaceName;        // �ӿ���
    WORD m_majorVersion;                // ��汾
    BYTE m_minorVersion;                // С�汾
    BYTE m_microVersion;                // ΢�汾

    IntfNameVer()
    {
        m_interfaceName = 0;
        m_majorVersion  = 0;
        m_minorVersion  = 0;
        m_microVersion  = 0;
    }
    IntfNameVer(const char *cszInterfaceName, DWORD dwVersion)
    {
        m_interfaceName = cszInterfaceName;
        m_majorVersion  = (WORD)((dwVersion >> 16) & 0x0000FFFF);
        m_minorVersion  = (BYTE)((dwVersion >>  8) & 0x000000FF);
        m_microVersion  = (BYTE)((dwVersion)       & 0x000000FF);
    }
};
#define ID_INTF(Interface)  IntfNameVer(#Interface, ___VERSION_##Interface)
#define interface           struct
#define REF_PTR(objPtr)     ((void **)&(objPtr))
#ifdef _MSC_VER
    #pragma warning(disable:4005)
#endif


/// -------------------------------------------------
/// ʵ���ӿڻ�����(����������Ϊ)
/// -------------------------------------------------
interface Instance : public objBase
{
    /// ����ʵ��
    virtual DWORD QueryInterface(
                        const IntfNameVer& iid,     // �ӿ�ID
                        void **ppv = 0,             // ���ʵ��
                        Instance *pir = 0           // ��������ʵ��
                        ) = 0;

    /// �ͷ�����
    virtual DWORD Release(
                        Instance *pir = 0           // ��������ʵ��
                        ) = 0;

    /// ��ѯ����
    virtual DWORD GetRef(
                        Instance ***pppirs = 0,     // �������ʵ���б�
                        DWORD *count = 0            // �������ʵ������
                        ) = 0;
};


/// -------------------------------------------------
/// ʵ��������Instanceģ��
/// -------------------------------------------------
#define DCOP_DECLARE_INSTANCE                       \
public:                                             \
    Instance *m_piParent;                           \
    objAtomic::T m_refCount;                        \
    DWORD GetRef(Instance ***pppirs = 0, DWORD *count = 0); \
    DWORD Release(Instance *pir = 0);               \
    DWORD QueryInterface(const IntfNameVer& iid, void **ppv = 0, Instance *pir = 0)


/// -------------------------------------------------
/// ʵ����ʵ��Instanceģ��
/// -------------------------------------------------
#define DCOP_CONSTRUCT_INSTANCE(Parent)             \
    m_piParent = Parent;                            \
    if (m_piParent)                                 \
    {                                               \
        (void)m_piParent->QueryInterface(           \
                            ID_INTF(Instance),      \
                            0,                      \
                            this);                  \
    }                                               \
    (void)objAtomic::Set(m_refCount, 1)


/// -------------------------------------------------
/// ʵ�����ͷ�Instanceģ��
/// -------------------------------------------------
#define DCOP_DESTRUCT_INSTANCE()                    \
    m_piParent = 0;                                 \
    (void)objAtomic::Set(m_refCount, 0)


/// -------------------------------------------------
/// �ҽ�ʵ���ൽInstanceģ�� - �ҽӱ�ͷ
/// -------------------------------------------------
#define DCOP_IMPLEMENT_INSTANCE(CMyClass)           \
extern void (*g_onInstanceQueryInterface)(          \
                    Instance *piThis,               \
                    Instance *piRefer,              \
                    void *pPara);                   \
extern void * g_onInstanceQueryInterfacePara;       \
extern void (*g_onInstanceRelease)(                 \
                    Instance *piThis,               \
                    Instance *piRefer,              \
                    void *pPara);                   \
extern void * g_onInstanceReleasePara;              \
extern void (*g_onInstanceGetRef)(                  \
                    Instance *piThis,               \
                    Instance ***pppiRefers,         \
                    DWORD *pdwReferCount,           \
                    void *pPara);                   \
extern void * g_onInstanceGetRefPara;               \
DWORD CMyClass::GetRef(Instance ***pppirs, DWORD *count) \
{                                                   \
    if (g_onInstanceGetRef && pppirs && count)      \
        g_onInstanceGetRef(this, pppirs, count,     \
        g_onInstanceGetRefPara);                    \
    return (DWORD)m_refCount;                       \
}                                                   \
DWORD CMyClass::Release(Instance *pir)              \
{                                                   \
    if (m_piParent)                                 \
        (void)m_piParent->Release(this);            \
    DWORD dwRefCount = (DWORD)objAtomic::Dec(m_refCount); \
    if (g_onInstanceRelease && pir)                 \
        g_onInstanceRelease(this, pir,              \
        g_onInstanceGetRefPara);                    \
    if (!dwRefCount) delete this;                   \
    return dwRefCount;                              \
}                                                   \
DWORD CMyClass::QueryInterface(const IntfNameVer& iid, void **ppv, Instance *pir) \
{


/// -------------------------------------------------
/// �ҽ�ʵ���ൽInstanceģ�� - �ҽ�'�ӿ���'����
/// -------------------------------------------------
#define DCOP_IMPLEMENT_INTERFACE(Interface)         \
    if (!strcmp(#Interface, iid.m_interfaceName) && \
        (INTF_VER_MAJOR(Interface) >=               \
            iid.m_majorVersion))                    \
    {                                               \
        if (ppv)                                    \
            *ppv = static_cast<Interface *>(this);  \
        if (m_piParent)                             \
            (void)m_piParent->QueryInterface(       \
                        ID_INTF(Instance),          \
                        0,                          \
                        this);                      \
        (void)objAtomic::Inc(m_refCount);           \
        if (g_onInstanceQueryInterface && pir)      \
            g_onInstanceQueryInterface(this, pir,   \
            g_onInstanceQueryInterfacePara);        \
        return SUCCESS;                             \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽInstanceģ�� - �ҽӱ�β
/// -------------------------------------------------
#define DCOP_IMPLEMENT_INSTANCE_END                 \
    return FAILURE;                                 \
}


#endif // #ifndef _BASECLASS_H_

