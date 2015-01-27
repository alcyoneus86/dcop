/// -------------------------------------------------
/// Object_if.h : ������๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJECT_IF_H_
#define _OBJECT_IF_H_

#include "BaseClass.h"
#include "sem.h"
#include "msg.h"


/////////////////////////////////////////////////////
///                   IObjectʹ��˵��
/// -------------------------------------------------
/// 1.  h�ļ�CMyClass�н���:
///         DCOP_DECLARE_INSTANCE;
///         DCOP_DECLARE_IOBJECT;
/// -------------------------------------------------
/// 2.  cpp�ļ�CMyClass�����:
///     DCOP_IMPLEMENT_INSTANCE(CMyClass)
///         ... ...
///         DCOP_IMPLEMENT_INTERFACE(IA)                    // �ӿ�A��ʵ��(�������δ����ൽ����Instance)
///         DCOP_IMPLEMENT_INTERFACE(IObject)
///         DCOP_IMPLEMENT_INTERFACE(Instance)
///     DCOP_IMPLEMENT_INSTANCE_END
/// 
///     DCOP_IMPLEMENT_IOBJECT(CMyClass)
///         DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")   // �Ӳ���-name��-id�����ֵ��ȡ���ֺ�ID���Ƽ�ʹ��
///         DCOP_IMPLEMENT_IDENTIFY_STATIC("A", 1)          // Ҳ����Ӳ����NAME��ID�������ڵ�ʵ��(����)ʱ��ʹ��
///         DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")  // �����̰߳�ȫ����ͨ����������Ƕ��̻߳���(���Ӷ����鲻����)
///         DCOP_IMPLEMENT_CUSTOM(argc, argv)               // �û����Թҽ��Լ��ĺ괦�������ֻ�Ǿ���
///         ... ...
///     DCOP_IMPLEMENT_IOBJECT_END
/// -------------------------------------------------
/// 3.  CMyClass���캯����ʼ:
///     CMyClass(Instance *piParent, int argc, char **argv)
///     {
///         ... ...
/// 
///         DCOP_CONSTRUCT_INSTANCE(piParent);
///         DCOP_CONSTRUCT_IOBJECT(argc, argv);
///     }
/// -------------------------------------------------
/// 4.  CMyClass������������:
///     ~CMyClass()
///     {
///         ... ...
/// 
///         DCOP_DESTRUCT_IOBJECT();
///         DCOP_DESTRUCT_INSTANCE();
///     }
/// -------------------------------------------------
/// 5.  ʵ���๤���������ģ��, λ�ò��̶�, cpp�м���
///     DCOP_IMPLEMENT_FACTORY(CMyClass, "A")
/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
/// ������˵��
/// -------------------------------------------------
/// 1) ������һ�ֱ���ʽ����ֻ��Ҫ�󱻶������ⲿ����
/// 2) ������ṩ"�����ӿ�"��"��Ϣ�ӿ�"���������Ҫ��
///    �ػ��Ķ�����ṩ"�����ӿ�"�⣬��������Ӧ������
///    �ṩ"��Ϣ�ӿ�"�����ڷֲ�ʽ����
/// 3) ������ⲿ�Ľ�������Ҫͨ��ʹ��"���������"�е�
///    ����������ʵ�� (�������ڳ�ʼ�������root������
///    �ݴ��룬�����������εĸ�һ���ǹ�����)��
///    ������ⲿ������:
///    a) ���ⲿ�ַ���Ϣ(���������Ӧ - ��Ŀ�ĵ�ַ)��
///       ��Ҫʹ��IDispatch����
///    b) ���ⲿ�ַ��¼�(�ڲ������¼� - ��Ŀ�ĵ�ַ)��
///       ��Ҫʹ��INotify����
///    c) ���������������Ҫʹ����������ӿ� ... ...
/// 4) ����ı���: ����ʵ�ֵ�����ɶ������ṩ������
///    ������ʹ��"�Զ�������"����ڴ������Զ���
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// ����IObject�汾��
/// -------------------------------------------------
INTF_VER(IObject, 1, 0, 0);


/// -------------------------------------------------
/// �������ӿ�
/// -------------------------------------------------
interface IObject : public Instance
{
    /// ����Ϣ
    virtual const char *Class() = 0;
    virtual DWORD Size() = 0;

    /// ��ȡ�Ƿ�'�̰߳�ȫ'��'��Ϣ����'
    ///     �̰߳�ȫ: ͬ�����������ź�������ֹ������ڱ����룬����ر�'�̰߳�ȫ'������������'��Ϣ����'(��Ϊ��������)
    ///     ��Ϣ����: ����Ϣ��ڲ����б�����'Proc(msg)'�ɱ�����߳����룬�������Ϣ�����м���ʶ������������Ҫ������
    virtual bool bThreadSafe() = 0;
    virtual bool bConcurrent() = 0;

    /// ��ȡ��������Ӷ���
    ///     ������: ��Դ��Instance����ĸ�����ֻ����IObject����ʱ����ת��������
    ///     �Ӷ���: ���ɾ�������Լ������������������Ӷ���Ĺ����ṩ��ѯ����
    virtual IObject *Parent() = 0;
    virtual IObject *Child(DWORD dwID) {return 0;}

    /// ��ʼ�����
    ///     root����: һ�����ڸ����󣬿��Ը���������ݾ���������������Ը�����
    ///     ���ʹ��: �����ǵ�ַָ��(void**)����ͬ�ڹ��캯�����ַ���ָ��(char**)
    virtual DWORD Init(IObject *root, int argc, void **argv) {return SUCCESS;}

    /// ����ʱ���
    virtual void Fini() {}

    /// ��Ϣ���
    ///     ��Ϣ�ͷ�: ����������Ϣ��ֻ�������ͷ�
    virtual void Proc(objMsg *msg) {}
};


/// -------------------------------------------------
/// ʵ��������IObjectģ��
/// -------------------------------------------------
#define DCOP_DECLARE_IOBJECT                        \
public:                                             \
    char m_szObjName[OSNAME_LENGTH];                \
    DWORD m_dwObjID;                                \
    bool m_bThreadSafe;                             \
    bool m_bConcurrent;                             \
    objLock *m_pLock;                               \
    IObject *m_piObjParent;                         \
    const char *Class();                            \
    DWORD Size();                                   \
    const char *Name();                             \
    DWORD ID();                                     \
    bool bThreadSafe();                             \
    bool bConcurrent();                             \
    void Enter();                                   \
    void Leave();                                   \
    IObject *Parent();                              \
    void ConstructClass(int argc, char **argv)


/// -------------------------------------------------
/// ʵ����ʵ��IObjectģ��
/// -------------------------------------------------
#define DCOP_CONSTRUCT_IOBJECT(argc, argv)          \
    *m_szObjName = 0;                               \
    m_dwObjID = 0;                                  \
    m_bThreadSafe = false;                          \
    m_bConcurrent = false;                          \
    m_pLock = 0;                                    \
    m_piObjParent = 0;                              \
    ConstructClass(argc, argv);                     \
    if (m_bThreadSafe)                              \
        m_pLock = DCOP_CreateLock()


/// -------------------------------------------------
/// ʵ�����ͷ�IObjectģ��
/// -------------------------------------------------
#define DCOP_DESTRUCT_IOBJECT()                     \
    m_piObjParent = 0;                              \
    if (m_pLock)                                    \
        delete m_pLock;                             \
    m_pLock = 0


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽӱ�ͷ
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IOBJECT(CMyClass)            \
const char *CMyClass::Class()                       \
{                                                   \
    return #CMyClass;                               \
}                                                   \
DWORD CMyClass::Size()                              \
{                                                   \
    return (DWORD)sizeof(CMyClass);                 \
}                                                   \
const char *CMyClass::Name()                        \
{                                                   \
    return m_szObjName;                             \
}                                                   \
DWORD CMyClass::ID()                                \
{                                                   \
    return m_dwObjID;                               \
}                                                   \
bool CMyClass::bThreadSafe()                        \
{                                                   \
    return m_bThreadSafe;                           \
}                                                   \
bool CMyClass::bConcurrent()                        \
{                                                   \
    return m_bConcurrent;                           \
}                                                   \
void CMyClass::Enter()                              \
{                                                   \
    if (m_pLock)                                    \
        m_pLock->Enter();                           \
}                                                   \
void CMyClass::Leave()                              \
{                                                   \
    if (m_pLock)                                    \
        m_pLock->Leave();                           \
}                                                   \
IObject *CMyClass::Parent()                         \
{                                                   \
    return m_piObjParent;                           \
}                                                   \
void CMyClass::ConstructClass(int argc, char **argv) \
{                                                   \
    if (m_piParent && (m_piParent->QueryInterface(  \
            ID_INTF(IObject),                       \
            REF_PTR(m_piObjParent),                 \
            this) == SUCCESS))                      \
    {                                               \
        (void)m_piObjParent->Release(this);         \
    }                                               \
    bool __staticInit = false;                      \
    int __idx = 0;                                  \
    while ((argv && (__idx < argc)) || !__staticInit) \
    {


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽ�'�ַ�������'����
/// -------------------------------------------------
#define DCOP_IMPLEMENT_CONFIG_STRING(Config, Value) \
    if (argv && (__idx < argc) &&                   \
        (*(argv[__idx]) == '-') &&                  \
        !strcmp(argv[__idx] + 1, Config) &&         \
        (__idx < (argc - 1)))                       \
    {                                               \
        (void)strncpy(Value, argv[__idx + 1],       \
                        sizeof(Value));             \
        Value[sizeof(Value) - 1] = '\0';            \
        __idx += 2;                                 \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽ�'��������'����
/// -------------------------------------------------
#define DCOP_IMPLEMENT_CONFIG_INTEGER(Config, Value) \
    if (argv && (__idx < argc) &&                   \
        (*(argv[__idx]) == '-') &&                  \
        !strcmp(argv[__idx] + 1, Config) &&         \
        (__idx < (argc - 1)))                       \
    {                                               \
        void *__ptr = (void *)&Value;               \
        if (sizeof(Value) == 1)                     \
        {                                           \
            *(BYTE *)__ptr = (BYTE)atoi(            \
                        argv[__idx + 1]);           \
        }                                           \
        else if (sizeof(Value) == 2)                \
        {                                           \
            *(WORD *)__ptr = (WORD)atoi(            \
                        argv[__idx + 1]);           \
        }                                           \
        else                                        \
        {                                           \
            *(DWORD *)__ptr = (DWORD)atoi(          \
                        argv[__idx + 1]);           \
        }                                           \
        __idx += 2;                                 \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽ�'��������'����
/// -------------------------------------------------
#define DCOP_IMPLEMENT_CONFIG_SWITCH(Config, Value) \
    if (argv && (__idx < argc) &&                   \
        (*(argv[__idx]) == '-') &&                  \
        !strcmp(argv[__idx] + 1, Config))           \
    {                                               \
        if ((__idx < (argc - 1)) &&                 \
                       (*(argv[__idx + 1]) != '-')) \
        {                                           \
            if (!strcmp(argv[__idx + 1], "true"))   \
            {                                       \
                Value = true;                       \
            }                                       \
            __idx += 2;                             \
        }                                           \
        else                                        \
        {                                           \
            Value = true;                           \
            __idx += 1;                             \
        }                                           \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽ�'��̬��ʶ����'���� (���鵥ʵ��ʹ��)
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IDENTIFY_STATIC(NAME, ID)    \
    if (!__staticInit)                              \
    {                                               \
        (void)strncpy(m_szObjName, NAME,            \
                        sizeof(m_szObjName));       \
        m_szObjName[sizeof(m_szObjName) - 1] = '\0'; \
        m_dwObjID = ID;                             \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽ�'��̬��ʶ����'���� (�����ʵ��ʹ��)
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IDENTIFY_DYNAMIC(name, id)   \
    DCOP_IMPLEMENT_CONFIG_STRING(name, m_szObjName) \
    DCOP_IMPLEMENT_CONFIG_INTEGER(id, m_dwObjID)


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽ�'�̰߳�ȫ��ʶ����'���� (��ͨ��������;�Ӷ���ͷǶ��̻߳�������������)
/// -------------------------------------------------
#define DCOP_IMPLEMENT_CONFIG_THREADSAFE(ThreadSafe) \
    DCOP_IMPLEMENT_CONFIG_SWITCH(ThreadSafe, m_bThreadSafe)


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - ��'�̰߳�ȫ'���� (������Ĭ���ǹرյ�״̬;�򿪺���޷�ʹ�ò�������)
/// -------------------------------------------------
#define DCOP_IMPLEMENT_CONFIG_THREADSAFE_ENABLE()   \
    if (!__staticInit)                              \
    {                                               \
        m_bThreadSafe = true;                       \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - ��'��Ϣ����'���� (Ĭ�Ϲر�;��֧�����ò���;��������Լ������ѡ���)
/// -------------------------------------------------
#define DCOP_IMPLEMENT_CONFIG_CONCURRENT_ENABLE()   \
    if (!__staticInit)                              \
    {                                               \
        m_bConcurrent = true;                       \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - �ҽӱ�β
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IOBJECT_END                  \
        ++__idx;                                    \
        __staticInit = true;                        \
    }                                               \
}


/// -------------------------------------------------
/// ʵ��������IObject��Ϣ����ģ��
/// -------------------------------------------------
#define DCOP_DECLARE_IOBJECT_MSG_HANDLE             \
    void Proc(objMsg *msg)                          \


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObject��Ϣ����ģ�� - �ҽӱ�ͷ
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CMyClass) \
    void CMyClass::Proc(objMsg *msg)                \
    {                                               \
        if (!msg) return;                           \
        if (!m_bConcurrent) Enter();                \
        switch (msg->GetMsgType())                  \
        {


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObject��Ϣ����ģ�� - �ҽӱ���
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IOBJECT_MSG_PROC(Msg, OnProc) \
            case Msg:                               \
                OnProc(msg);                        \
                break;


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObject��Ϣ����ģ�� - �ҽӱ�β
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IOBJECT_MSG_DEFAULT(OnDefault) \
            default:                                \
                OnDefault(msg);                     \
                break;                              \


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObject��Ϣ����ģ�� - �ҽӱ�β
/// -------------------------------------------------
#define DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END       \
        }                                           \
        if (!m_bConcurrent) Leave();                \
    }


/// -------------------------------------------------
/// ��ѯһ��ָ�����͵Ķ��� - ����������
/// -------------------------------------------------
#define DCOP_QUERY_OBJECT(Type, ID, Parent, objPtr) \
do {                                                \
    IObject *piObjParent = Parent;                  \
    if (!piObjParent) break;                        \
    IObject *piObject = piObjParent->Child(ID);     \
    if (!piObject) break;                           \
    (void)piObject->QueryInterface(                 \
                        ID_INTF(Type),              \
                        REF_PTR(objPtr),            \
                        this);                      \
} while (0)


/// -------------------------------------------------
/// ��ѯһ��ָ�����͵Ķ��� - ����������(��ָ��������)
/// -------------------------------------------------
#define DCOP_QUERY_OBJECT_REFER(Type, ID, Parent, Refer, objPtr) \
do {                                                \
    IObject *piObjParent = Parent;                  \
    if (!piObjParent) break;                        \
    IObject *piObject = piObjParent->Child(ID);     \
    if (!piObject) break;                           \
    (void)piObject->QueryInterface(                 \
                        ID_INTF(Type),              \
                        REF_PTR(objPtr),            \
                        Refer);                     \
} while (0)


/// -------------------------------------------------
/// ��ѯ���ָ�����͵Ķ��� - ����
/// -------------------------------------------------
#define DCOP_QUERY_OBJECT_START(Parent)             \
    DWORD __rcQueryObj = SUCCESS;                   \
    IObject *__objParent = Parent;                  \
    if (!__objParent)                               \
    {                                               \
        return FAILURE;                             \
    }                                               \
    do {


/// -------------------------------------------------
/// ��ѯ���ָ�����͵Ķ��� - ����
/// -------------------------------------------------
#define DCOP_QUERY_OBJECT_ITEM(Type, ID, objPtr)    \
    IObject *pi##objPtr = __objParent->Child(ID);   \
    if (!pi##objPtr)                                \
    {                                               \
        __rcQueryObj = FAILURE;                     \
        break;                                      \
    }                                               \
    __rcQueryObj = pi##objPtr->QueryInterface(      \
                        ID_INTF(Type),              \
                        REF_PTR(objPtr),            \
                        this);                      \
    if (__rcQueryObj != SUCCESS)                    \
    {                                               \
        break;                                      \
    }


/// -------------------------------------------------
/// ��ѯ���ָ�����͵Ķ��� - ����
/// -------------------------------------------------
#define DCOP_QUERY_OBJECT_END                       \
    } while (0);                                    \
    if (__rcQueryObj != SUCCESS) return __rcQueryObj;


#endif // ifndef _OBJECT_IF_H_

