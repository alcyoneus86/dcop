/// -------------------------------------------------
/// ObjAttribute_if.h : �������Թ���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJATTRIBUTE_IF_H_
#define _OBJATTRIBUTE_IF_H_

#include "Object_if.h"
#include "Factory_if.h"
#include "BaseMessage.h"
#include "ObjModel_if.h"
#include "ObjData_if.h"
#include "ObjNotify_if.h"
#include "ObjDispatch_if.h"
#include "ObjResponse_if.h"
#include "ObjControl_if.h"
#include "stream/dstream.h"


/// -------------------------------------------------
/// ������
/// -------------------------------------------------
class CAttribute
{
public:
    enum TYPE
    {
        TYPE_VAR = IModel::TYPE_EVENT + 1,      // ����ֵ
        TYPE_BUF,                               // ������
        TYPE_STR,                               // �ַ���
        TYPE_OBJ                                // �Ӷ���
    };

public:
    CAttribute()
    {
        m_attrName = NULL;
        m_attrID   = 0;
    }
    CAttribute(const char *attrName, DWORD attrID, DWORD attrType)
    {
        m_attrName = attrName;
        m_attrID   = attrID;
        m_attrType = attrType;
    }
    ~CAttribute() {}

    void SetName(const char *attrName) {m_attrName = attrName;}
    void SetID(DWORD attrID) {m_attrID = attrID;}
    void SetType(DWORD attrType) {m_attrType = attrType;}

    const char *GetName() {return m_attrName;}
    DWORD GetID() {return m_attrID;}
    DWORD GetType() {return m_attrType;}

public:
    /// ��װ�Ự��Ϣͷ
    struct PACK_SESS_HEAD : public tagDCOP_SESSION_HEAD
    {
        PACK_SESS_HEAD(const DCOP_SESSION_HEAD *pSessHead = 0);
    };

    /// ��װ������Ϣͷ
    struct PACK_COND_HEAD : public tagDCOP_CONDITION_HEAD
    {
        PACK_COND_HEAD();
    };

    /// ��װ������Ϣͷ
    struct PACK_REQ_HEAD : public tagDCOP_REQUEST_HEAD
    {
        PACK_REQ_HEAD();
    };

    /// ��װ��Ӧ��Ϣͷ
    struct PACK_RSP_HEAD : public tagDCOP_RESPONSE_HEAD
    {
        PACK_RSP_HEAD();
    };

    /// ��װ�¼���Ϣͷ
    struct PACK_EVT_HEAD : public tagDCOP_EVENT_HEAD
    {
        PACK_EVT_HEAD();
    };

    /// ��װ��Ϣ�ڵ�
    struct PACK_MSG_NODE
    {
        DCOP_MSG_HEAD *m_pMsgHead;
        DCOP_PARA_NODE *m_pMsgPara;
        DWORD m_dwMsgParaCount;
        const CDStream &m_sMsgParaData;
    };

private:
    const char *m_attrName;                     // ��������
    DWORD m_attrID;                             // ����ID
    DWORD m_attrType;                           // ��������
};


/// -------------------------------------------------
/// �����Ա����
/// -------------------------------------------------
class IObjectMember
{
public:
    IObjectMember()
    {
        m_pOwner = NULL;
        m_pAttribute = NULL;
        m_pDispatch = NULL;
        m_pNotifyPool = NULL;
    }
    virtual ~IObjectMember() {}

    void SetOwner(IObject *pOwner) {m_pOwner = pOwner;}
    IObject *GetOwner() {return m_pOwner;}

    void SetAttribute(CAttribute *pAttr) {m_pAttribute = pAttr;}
    CAttribute *GetAttribute() {return m_pAttribute;}

    void SetDispatch(IDispatch *pDispatch) {m_pDispatch = pDispatch;}
    IDispatch *GetDispatch() {return m_pDispatch;}

    void SetNotifyPool(INotify::IPool *pNotifyPool) {m_pNotifyPool = pNotifyPool;}
    INotify::IPool *GetNotifyPool() {return m_pNotifyPool;}

    virtual IObjectMember *Init(IObject *pOwner, CAttribute *pAttr)
    {
        SetOwner(pOwner);
        SetAttribute(pAttr);

        return (IObjectMember *)this;
    }

    virtual DWORD Set(void *pData, DWORD dwSize) {return SUCCESS;}
    virtual void *Get(DWORD &rdwSize) {rdwSize = 0; return NULL;}

    virtual void Action(objMsg *pMsg, const DCOP_SESSION_HEAD &sessionHead, const CDArray &aCondHeads, DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, void *pReqData, DWORD dwReqDataLen);
    virtual void Notify(DCOP_MSG_HEAD *pHead, const CDArray &aData, DCOP_SESSION_HEAD *pSessionHead = 0, DWORD dwDstID = 0, DCOP_PARA_NODE *pPara = 0, DWORD dwParaCount = 0);

public:
    /// ��ȡָ������Ϣͷ
    static void GetMsgHead(void *pBuf, DWORD dwLen, 
                        CDArray *paSessHeads, 
                        CDArray *paCondHeads, 
                        CDArray *paReqHeads, 
                        CDArray *paRspHeads, 
                        CDArray *paEvtHeads);

    /// ��ȡ��Ϣ����������
    static void *GetMsgParaData(void *pMsgParaData, 
                        DWORD dwParaCount, 
                        DWORD dwDataLen, 
                        DCOP_PARA_NODE *pPara = 0);

    /// ��װ��Ϣ�����зַ� - ��������ͬ���͵Ķ�����¼(��Ӧ���¼�)
    /// [���ٿɷ���'һ��SessionHead + һ���հ�MsgHead']
    static DWORD PackMsg(IDispatch *pDispatch, 
                        DWORD dwSrcID, 
                        DWORD dwDstID, 
                        DWORD dwMsgType, 
                        DCOP_SESSION_HEAD *pSessionHead, 
                        DCOP_MSG_HEAD *pMsgHead, 
                        DCOP_PARA_NODE *pMsgPara, 
                        DWORD dwMsgParaCount, 
                        const CDArray &aMsgParaData, 
                        DWORD dwSendTryTimes = 0);

    /// ��װ��Ϣ�����зַ� - �����ڲ�ͬ���͵ĵ�����¼(����)
    /// [���ٿɷ���'һ��SessionHead']
    static DWORD PackMsg(IDispatch *pDispatch, 
                        objMsg **ppOutMsg, 
                        DWORD dwSrcID, 
                        DWORD dwDstID, 
                        DWORD dwMsgType, 
                        DCOP_SESSION_HEAD *pSessionHead, 
                        CAttribute::PACK_MSG_NODE *pPackNode, 
                        DWORD dwPackNodeCount, 
                        IResponse::IPool *pReqPool = 0, 
                        DWORD dwRspMsgType = 0, 
                        DWORD dwTimeout = 0, 
                        DWORD dwSendTryTimes = 0);

protected:
    void OnChanged()
    {
        DWORD dwSize = 0;
        void *pAddr = Get(dwSize);
        if (!pAddr || !dwSize) return;

        CDArray aEvtData;
        aEvtData.SetNodeSize(dwSize);
        if (aEvtData.Append(pAddr) != SUCCESS)
        {
            return;
        }

        CAttribute::PACK_EVT_HEAD evtHead;
        evtHead.m_recordCount = 1;
        Notify((DCOP_MSG_HEAD *)&evtHead, aEvtData);
    }

private:
    IObject *m_pOwner;                          // ���������Ķ���
    CAttribute *m_pAttribute;                   // ��Ա��Ӧ������
    IDispatch *m_pDispatch;                     // ��Ϣ�ַ���
    INotify::IPool *m_pNotifyPool;              // ���ķ����������
};


/// -------------------------------------------------
/// �����Ա��
/// -------------------------------------------------
template<class T>
class CObjectMember : public IObjectMember
{
public:
    CObjectMember()
    {
        m_var = 0;
    }
    CObjectMember(T t)
    {
        m_var = t;
    }
    ~CObjectMember() {}

    CObjectMember& operator=(T t)
    {
        if (m_var != t)
        {            
            m_var = t;
            OnChanged();
        }
        
        return *this;
    }
    operator T()
    {
        return m_var;
    }

    DWORD Set(void *pData, DWORD dwSize)
    {
        if (!pData || !dwSize)
            return FAILURE;

        *this = (T)Bytes_GetDwordValue((BYTE *)pData, dwSize);
        return SUCCESS;
    }
    void *Get(DWORD &rdwSize) {rdwSize = sizeof(m_var); return &m_var;}

private:
    T m_var;
};


/// -------------------------------------------------
/// �����Ա��(ƫ�ػ�: ���ָ��)
/// -------------------------------------------------
template<class T>
class CObjectMember<T*> : public IObjectMember
{
public:
    CObjectMember()
    {
        m_ptr = NULL;
        m_dwCount = 0;
    }
    CObjectMember(T* p, DWORD dwCount = 0)
    {
        m_ptr = NULL;
        m_dwCount = dwCount;
        (void)Set(p, dwCount * sizeof(T));
    }
    ~CObjectMember() {if (m_ptr) DCOP_Free(m_ptr);}

    CObjectMember& operator=(T *p)
    {
        (void)Set(p, m_dwCount * sizeof(T));
        return *this;
    }
    operator T*()
    {
        return m_ptr;
    }

    DWORD Set(void *pData, DWORD dwSize)
    {
        if (!pData)
            return FAILURE;

        if (!m_dwCount)
        {
            /// ���m_dwCountΪ�㣬˵����ֱ��ʹ��ָ��
            m_ptr = pData;
            return SUCCESS;
        }

        DWORD dwLen = m_dwCount * sizeof(T);
        if (dwSize != dwLen)
            return FAILURE;

        if (m_ptr && !memcmp(m_ptr, pData, dwSize))
            return SUCCESS;

        if (!m_ptr)
        {
            void *pBuf = DCOP_Malloc(dwLen);
            if (!pBuf) return FAILURE;
            (void)memset(pBuf, 0, dwLen);
            m_ptr = (T *)pBuf;
        }

        (void)memcpy(m_ptr, pData, dwSize);
        OnChanged();
        return SUCCESS;
    }
    void *Get(DWORD &rdwSize) {rdwSize = m_dwCount * sizeof(m_dwCount); return m_ptr;}

private:
    T *m_ptr;
    DWORD m_dwCount;
};


/// -------------------------------------------------
/// �����Ա��(�ػ�: ����ַ�������)
/// -------------------------------------------------
template<>
class CObjectMember<char*> : public IObjectMember
{
public:
    CObjectMember()
    {
        m_str = NULL;
    }
    CObjectMember(const char *pStr)
    {
        m_str = NULL;
        (void)Set((void *)pStr, (DWORD)strlen(pStr) + 1);
    }
    ~CObjectMember() {if (m_str) DCOP_Free(m_str);}

    CObjectMember& operator=(const char *pStr)
    {
        (void)Set((void *)pStr, (DWORD)strlen(pStr) + 1);
        return *this;
    }
    operator const char*()
    {
        return m_str;
    }

    DWORD Set(void *pData, DWORD dwSize)
    {
        if (!pData || !dwSize)
            return FAILURE;

        char *pStr = (char *)DCOP_Malloc(dwSize);
        if (!pStr) return FAILURE;
        (void)snprintf(pStr, dwSize, "%s", (char *)pData);
        pStr[dwSize - 1] = '\0';
        if (m_str) DCOP_Free(m_str);
        m_str = pStr;

        OnChanged();
        return SUCCESS;
    }
    void *Get(DWORD &rdwSize) {rdwSize = (DWORD)strlen(m_str) + 1; return m_str;}

private:
    char *m_str;
};


/// -------------------------------------------------
/// �����Ա��(�ػ�: ��Զ�������)
/// -------------------------------------------------
template<>
class CObjectMember<IObject*> : public IObjectMember
{
public:
    CObjectMember()
    {
        m_child = NULL;
    }
    ~CObjectMember() {DCOP_RELEASE_INSTANCE_REFER(GetOwner(), m_child);}

    CObjectMember &operator=(IObject *p)
    {
        m_child = p;
        return *this;
    }
    operator IObject*()
    {
        return m_child;
    }

    void Action(objMsg *pMsg, const DCOP_SESSION_HEAD &sessionHead, const CDArray &aCondHeads, DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, void *pReqData, DWORD dwReqDataLen)
    {
        if (m_child) m_child->Proc(pMsg);
    }

private:
    IObject *m_child;
};


/// -------------------------------------------------
/// �����Ա��(�ػ�: �����������)
/// -------------------------------------------------
template<>
class CObjectMember<IData*> : public IObjectMember
{
public:
    CObjectMember()
    {
        m_pData = NULL;
        m_hData = NULL;
        m_tData = IData::TYPE_MEM;
    }
    CObjectMember(IData::TYPE tData)
    {
        m_pData = NULL;
        m_hData = NULL;
        m_tData = tData;
    }
    ~CObjectMember() {}

    void SetDataObj(IData *pData)
    {
        m_pData = pData;
    }

    void SetDataType(IData::TYPE tData)
    {
        m_tData = tData;
    }

    operator IData::Handle&()
    {
        return m_hData;
    }

    IData *GetData() {return m_pData;}
    IData::Handle GetDataHandle() {return m_hData;}
    IData::TYPE GetDataType() {return m_tData;}
    IModel::TYPE GetModelType() {return IModel::TYPE_DATA;}

    void Action(objMsg *pMsg, const DCOP_SESSION_HEAD &sessionHead, const CDArray &aCondHeads, DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, void *pReqData, DWORD dwReqDataLen);

    DWORD Create(const DCOP_SESSION_HEAD *pSessionHead = 0);

    DWORD Destroy(const DCOP_SESSION_HEAD *pSessionHead = 0);

    DWORD AddRecord(DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                void *pReqData, 
                DWORD dwReqDataLen, 
                const DCOP_SESSION_HEAD *pSessionHead = 0);

    DWORD DelRecord(BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                const DCOP_SESSION_HEAD *pSessionHead = 0);

    DWORD EditRecord(BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                void *pReqData, 
                DWORD dwReqDataLen, 
                const DCOP_SESSION_HEAD *pSessionHead = 0);

private:
    IData *m_pData;
    IData::Handle m_hData;
    IData::TYPE m_tData;
};


/// -------------------------------------------------
/// ���󷽷���
/// -------------------------------------------------
class CMethod
{
public:
    typedef void (*ACTION)(IObject *pOwner, objMsg *pMsg, 
                        const DCOP_SESSION_HEAD &sessionHead, 
                        const CDArray &aCondHeads, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen);

public:
    CMethod() {m_action = NULL;}
    ~CMethod() {}

    void SetProc(ACTION action) {m_action = action;}
    ACTION GetAction() {return m_action;}

private:
    ACTION m_action;
};


/// -------------------------------------------------
/// �����Ա��(�ػ�: ��Է�������)
/// -------------------------------------------------
template<>
class CObjectMember<CMethod> : public IObjectMember
{
public:
    CObjectMember() {}
    ~CObjectMember() {}

    CObjectMember &operator=(CMethod::ACTION action)
    {
        m_method.SetProc(action);
        return *this;
    }

    void Action(objMsg *pMsg, const DCOP_SESSION_HEAD &sessionHead, const CDArray &aCondHeads, DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, void *pReqData, DWORD dwReqDataLen);

private:
    CMethod m_method;
};


/// -------------------------------------------------
/// �����Ա������
/// -------------------------------------------------
class CObjectMemberIndex
{
public:
    CObjectMemberIndex();
    ~CObjectMemberIndex();

    DWORD Init(IObjectMember **ppMembers, DWORD dwCount, IDispatch *pDispatch, INotify::IPool *pNotifyPool);

    void Dispatch(objMsg *pMsg);

private:
    IObjectMember **m_ppIndexTable;
    DWORD m_dwIndexCount;
};


/// -------------------------------------------------
/// ������������
/// -------------------------------------------------
#define DECLARE_ATTRIBUTE_INDEX(Index)              \
    CObjectMemberIndex m_attribute_##Index


/// -------------------------------------------------
/// ��������
/// -------------------------------------------------
#define DECLARE_ATTRIBUTE(Type, Member)             \
    CObjectMember<Type> m_##Member;                 \
    static CAttribute m_attribute_##Member


/// -------------------------------------------------
/// ʵ������
/// -------------------------------------------------
#define IMPLEMENT_ATTRIBUTE(CMyClass, Member, ID, Type) \
    CAttribute CMyClass::m_attribute_##Member(#Member, ID, Type);


/// -------------------------------------------------
/// ��ʼ�����Կ�ʼ
/// -------------------------------------------------
#define INIT_ATTRIBUTE_START(Index, iDispatch, pNotifyPool) \
    CObjectMemberIndex *__memberIndex = &m_attribute_##Index; \
    IDispatch *__memberDispatch = iDispatch;        \
    INotify::IPool *__memberNotifyPool = pNotifyPool; \
    IObjectMember *__memberTable[] = {


/// -------------------------------------------------
/// ��ʼ�����Գ�Ա
/// -------------------------------------------------
#define INIT_ATTRIBUTE_MEMBER(Member)               \
    m_##Member.Init(this, &m_attribute_##Member),


/// -------------------------------------------------
/// ��ʼ�����Խ���
/// -------------------------------------------------
#define INIT_ATTRIBUTE_END                          \
    };                                              \
    DWORD __rcIndexInit = __memberIndex->Init(      \
                        __memberTable,              \
                        ARRAY_SIZE(__memberTable),  \
                        __memberDispatch,           \
                        __memberNotifyPool);        \
    if (__rcIndexInit != SUCCESS) return __rcIndexInit;


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObjectģ�� - ������������
/// -------------------------------------------------
#define IMPLEMENT_CONFIG_DATATYPE(Config, Member)   \
    char __type##Member[16] = {0, };                \
    DCOP_IMPLEMENT_CONFIG_STRING(Config, __type##Member) \
    if (!strcmp(__type##Member, "mem"))             \
    {                                               \
        m_##Member.SetDataType(IData::TYPE_MEM);    \
    }                                               \
    else if (!strcmp(__type##Member, "file"))       \
    {                                               \
        m_##Member.SetDataType(IData::TYPE_FILE);   \
    }                                               \
    else if (!strcmp(__type##Member, "mysql"))      \
    {                                               \
        m_##Member.SetDataType(IData::TYPE_MYSQL);  \
    }                                               \
    else                                            \
    {                                               \
    }


/// -------------------------------------------------
/// �ҽ�ʵ���ൽIObject��Ϣ����ģ�� - �ҽӱ���
/// -------------------------------------------------
#define IMPLEMENT_ATTRIBUTE_MSG_PROC(Index)         \
            case DCOP_MSG_OBJECT_REQUEST:           \
                m_attribute_##Index.Dispatch(msg);  \
                break;


#endif // #ifndef _OBJATTRIBUTE_IF_H_

