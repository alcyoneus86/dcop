/// -------------------------------------------------
/// ObjManager_main.cpp : �������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjManager_main.h"
#include "Factory_if.h"
#include "ObjDispatch_if.h"
#include "msg.h"
#include "BaseMessage.h"
#include "BaseID.h"
#include "string/tablestring.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CObjectManager, "manager")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CObjectManager)
    DCOP_IMPLEMENT_INTERFACE(IManager)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CObjectManager)
    DCOP_IMPLEMENT_IDENTIFY_STATIC("manager", DCOP_OBJECT_MANAGER)
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
    IMPLEMENT_CONFIG_SYSTEM("id", "info")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CObjectManager::CObjectManager
  ��    ��: �������ʵ���๹��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CObjectManager::CObjectManager(Instance *piParent, int argc, char **argv)
{
    m_dwSystemID = 0;
    (void)memset(m_szSystemInfo, 0, sizeof(m_szSystemInfo));

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    m_bInit = false;
}

/*******************************************************
  �� �� ��: CObjectManager::~CObjectManager
  ��    ��: �������ʵ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CObjectManager::~CObjectManager()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CObjectManager::Init
  ��    ��: ��ʼ�����
  ��    ��: root - NULL, ������һ��û�и�����
            argc - һ���Դ���Ķ������
            argv - һ���Դ���Ķ���ָ��
            (ע��:��̳п�����ת��Ϊvoid*ʱ��ʧdynamic��Ϣ)
            (����:��̳еĶ��󾡿����ڳ�ʼ��ǰ����InsertObject)
  ��    ��: 
  ��    ��: SUCCESS             - �ɹ�
            FAILURE             - ʧ��
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectManager::Init(IObject *root, int argc, void **argv)
{
    AutoObjLock(this);

    if (m_bInit)
    {
        return FAILURE;
    }

    objTask *pTask = objTask::Current();
    DWORD dwObjID = 0;
    if (pTask)
    {
        dwObjID = DCOP_OBJECT_MANAGER;
        (void)pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
    }

    /////////////////////////////////////////////////
    /// �Ȳ������е��������
    /////////////////////////////////////////////////
    for (int i = 0; i < argc; ++i)
    {
        (void)InsertObject((IObject *)(argv[i]));
    }

    /////////////////////////////////////////////////
    /// ���ų�ʼ�����ж���
    /////////////////////////////////////////////////
    /// Ϊ�˷�ֹ�ڳ�ʼ�������в����¶���
    /// �����Ƚ����еĶ����ռ�����ʱ����
    CDArray aObjects(sizeof(IObject *), (DWORD)m_objects.size());
    for (IT_OBJECTS it_obj = m_objects.begin();
        it_obj != m_objects.end(); ++it_obj)
    {
        IObject *pObjTmp = ((*it_obj).second).GetObject();
        (void)aObjects.Append(&pObjTmp);
    }

    /////////////////////////////////////////////////
    /// �ó�ʼ����ʶ��ͬʱ���г�ʼ��
    /// (���ñ�ʶ��Ϊ�˷�ֹ�ڳ�ʼ���������ٲ����¶���)
    /////////////////////////////////////////////////
    m_bInit = true;

    IObject **ppiObject = (IObject **)aObjects.Get();
    if (!ppiObject)
    {
        return FAILURE;
    }

    DWORD dwRc = SUCCESS;
    for (DWORD i = 0; i < aObjects.Count(); ++i)
    {
        IObject *pObjTmp = ppiObject[i];
        if (!pObjTmp) continue;

        if (pTask)
        {
            dwObjID = pObjTmp->ID();
            (void)pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
        }

        dwRc = pObjTmp->Init(this, 0, 0);
        if (dwRc)
        {
            CHECK_RETCODE(dwRc, STR_FORMAT("Init Static Object: '%s'(ID:%d) Fail!", 
                        pObjTmp->Name(), pObjTmp->ID()));
            break;
        }
    }

    /////////////////////////////////////////////////
    /// �����ʼ��ʧ�ܷ��ش���
    /////////////////////////////////////////////////
    if (dwRc)
    {
        return dwRc;
    }

    /////////////////////////////////////////////////
    /// ���ͳ�ʼ���¼�
    /////////////////////////////////////////////////
    SendEvent(true);

    if (pTask)
    {
        dwObjID = DCOP_OBJECT_KERNEL;
        (void)pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectManager::Fini
  ��    ��: ����ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CObjectManager::Fini()
{
    AutoObjLock(this);

    if (!m_bInit)
    {
        return;
    }

    /////////////////////////////////////////////////
    /// ���ͽ����¼�
    /////////////////////////////////////////////////
    SendEvent(false);

    /////////////////////////////////////////////////
    /// �������еĶ���
    /////////////////////////////////////////////////
    for (IT_OBJECTS it_obj = m_objects.begin();
        it_obj != m_objects.end(); ++it_obj)
    {
        IObject *pObjTmp = ((*it_obj).second).GetObject();
        if (!pObjTmp) continue;

        pObjTmp->Fini();
    }

    /////////////////////////////////////////////////
    /// CObjectNode�������ͷŶ�������
    /////////////////////////////////////////////////
    m_objects.clear();

    /////////////////////////////////////////////////
    /// �ó�ʼ����ʶ
    /////////////////////////////////////////////////
    m_bInit = false;
}

/*******************************************************
  �� �� ��: CObjectManager::Proc
  ��    ��: ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CObjectManager::Proc(objMsg *msg)
{
    BroadcastMsg(msg);
}

/*******************************************************
  �� �� ��: CObjectManager::Dump
  ��    ��: Dump���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CObjectManager::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);

    DWORD dwCount = (DWORD)m_objects.size();
    CTableString tableStr(7, dwCount + 4, "  ");
    tableStr << "objKey";
    tableStr << "objID";
    tableStr << "objName";
    tableStr << "className";
    tableStr << "classSize";
    tableStr << "refCount";
    tableStr << "objPtr";

    tableStr << STR_FORMAT("%d", DCOP_OBJECT_KERNEL);
    tableStr << STR_FORMAT("%d", DCOP_OBJECT_KERNEL);
    tableStr << "kernel";
    tableStr << "";
    tableStr << "";
    tableStr << "";
    tableStr << STR_FORMAT("%p", objBase::GetInstance());

    tableStr << STR_FORMAT("%d", DCOP_OBJECT_FACTORY);
    tableStr << STR_FORMAT("%d", DCOP_OBJECT_FACTORY);
    tableStr << "factory";
    tableStr << "";
    tableStr << "";
    tableStr << "";
    tableStr << STR_FORMAT("%p", IFactory::GetInstance());

    tableStr << STR_FORMAT("%d", DCOP_OBJECT_MANAGER);
    tableStr << STR_FORMAT("%d", ID());
    tableStr << Name();
    tableStr << Class();
    tableStr << STR_FORMAT("%d", Size());
    tableStr << STR_FORMAT("%d", GetRef());
    tableStr << STR_FORMAT("%p", this);

    DCOP_START_TIME();

    for (IT_OBJECTS it_obj = m_objects.begin();
        it_obj != m_objects.end(); ++it_obj)
    {
        IObject *pObjTmp = ((*it_obj).second).GetObject();
        if (!pObjTmp) continue;

        tableStr << STR_FORMAT("%d", (*it_obj).first);
        tableStr << STR_FORMAT("%d", pObjTmp->ID());
        tableStr << pObjTmp->Name();
        tableStr << pObjTmp->Class();
        tableStr << STR_FORMAT("%d", pObjTmp->Size());
        tableStr << STR_FORMAT("%d", pObjTmp->GetRef());
        tableStr << STR_FORMAT("%p", pObjTmp);
    }

    DCOP_END_TIME();

    logPrint(STR_FORMAT("Manager Dump: (Objects Count: %d) \r\n", dwCount), logPara);
    tableStr.Show(logPrint, logPara, "=", "-");
    logPrint(STR_FORMAT("[cost time: %d ms] \r\n", DCOP_COST_TIME()), logPara);
}

/*******************************************************
  �� �� ��: CObjectManager::Child
  ��    ��: �Ӷ�����������ҵ�һ������
  ��    ��: dwID                - ����ID
  ��    ��: 
  ��    ��: 
            IObject *           - ����ָ��
  �޸ļ�¼: 
 *******************************************************/
IObject *CObjectManager::Child(DWORD dwID)
{
    if (dwID == DCOP_OBJECT_MANAGER)
    {
        return (IObject *)this;
    }

    AutoObjLock(this);

    IT_OBJECTS it_obj = m_objects.find(dwID);
    if (it_obj == m_objects.end())
    {
        return NULL;
    }

    return ((*it_obj).second).GetObject();
}

/*******************************************************
  �� �� ��: CObjectManager::InsertObject
  ��    ��: ����һ�����󵽶����������
  ��    ��: pObject             - ����
  ��    ��: 
  ��    ��: 
            SUCCESS             - �ɹ�
            FAILURE             - ʧ��
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectManager::InsertObject(IObject *pObject)
{
    if (!pObject)
    {
        return FAILURE;
    }

    DWORD dwID = pObject->ID();

    AutoObjLock(this);

    /////////////////////////////////////////////////
    /// �����Ƿ����ظ��Ķ���
    /////////////////////////////////////////////////
    IT_OBJECTS it_obj = m_objects.find(dwID);
    if (it_obj != m_objects.end())
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// ���ö���
    /////////////////////////////////////////////////
    if (pObject->QueryInterface(ID_INTF(IObject), 0, this) != SUCCESS)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// �Ѷ�����뵽MAP������
    /////////////////////////////////////////////////
    CObjectNode objNode;
    it_obj = m_objects.insert(m_objects.end(), MAP_OBJECTS::value_type(dwID, objNode));
    if (it_obj == m_objects.end())
    {
        (void)pObject->Release(this);
        return FAILURE;
    }

    ((*it_obj).second).SetObject(pObject, this);

    /////////////////////////////////////////////////
    /// ����ǳ�ʼ��֮ǰ����ģ���Ϊ��̬���󣬵��ܳ�ʼ�����һ���ʼ��
    /////////////////////////////////////////////////
    if (!m_bInit)
    {
        return SUCCESS;
    }

    /////////////////////////////////////////////////
    /// ����ǳ�ʼ��֮�����ģ���Ϊ��̬�������ﵥ�����г�ʼ��
    /////////////////////////////////////////////////
    DWORD dwRc = pObject->Init(this, 0, 0);
    if (dwRc)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("Init Dynamic Object: '%s'(ID:%d) Fail!", 
                        pObject->Name(), pObject->ID()));
        (void)m_objects.erase(it_obj);
        return dwRc;
    }

    SendEvent(true, pObject);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectManager::DeleteObject
  ��    ��: �Ӷ����������ɾ��һ������
  ��    ��: pObject             - ����
  ��    ��: 
  ��    ��: 
            SUCCESS             - �ɹ�
            FAILURE             - ʧ��
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectManager::DeleteObject(IObject *pObject)
{
    if (!pObject)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /////////////////////////////////////////////////
    /// �����Ƿ��ж���
    /////////////////////////////////////////////////
    IT_OBJECTS it_obj = m_objects.find(pObject->ID());
    if (it_obj == m_objects.end())
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// �ѳ�ʼ�����������н�������
    /////////////////////////////////////////////////
    if (m_bInit)
    {
        IObject *pObjTmp = ((*it_obj).second).GetObject();
        if (!pObjTmp) return FAILURE;

        pObjTmp->Fini();
        SendEvent(false, pObjTmp);
    }

    /////////////////////////////////////////////////
    /// CObjectNode�������ͷŶ�������
    /////////////////////////////////////////////////
    (void)m_objects.erase(it_obj);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CObjectManager::GetSystemID
  ��    ��: ��ȡϵͳID
  ��    ��: 
  ��    ��: 
  ��    ��: ϵͳID
  �޸ļ�¼: 
 *******************************************************/
DWORD CObjectManager::GetSystemID()
{
    AutoObjLock(this);

    return m_dwSystemID;
}

/*******************************************************
  �� �� ��: CObjectManager::GetSystemInfo
  ��    ��: ��ȡϵͳ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: ϵͳID
  �޸ļ�¼: 
 *******************************************************/
const char *CObjectManager::GetSystemInfo()
{
    AutoObjLock(this);

    return m_szSystemInfo;
}

/*******************************************************
  �� �� ��: CObjectManager::BroadcastMsg
  ��    ��: �㲥��Ϣmsg
  ��    ��: msg
  ��    ��: 
  ��    ��: ϵͳID
  �޸ļ�¼: 
 *******************************************************/
void CObjectManager::BroadcastMsg(objMsg *msg)
{
    AutoObjLock(this);

    for (IT_OBJECTS it_obj = m_objects.begin();
        it_obj != m_objects.end(); ++it_obj)
    {
        IObject *pObjTmp = ((*it_obj).second).GetObject();
        if (!pObjTmp) continue;

        pObjTmp->Enter();
        pObjTmp->Proc(msg);
        pObjTmp->Leave();
    }
}

/*******************************************************
  �� �� ��: CObjectManager::SendEvent
  ��    ��: ���ͳ�ʼ���¼�
  ��    ��: pDynamicObject - ��̬���صĶ���
            (��̬���صĶ���: �ڳ�ʼ��֮�����Ķ���)
            (0Ϊ�����������е����ж����ʼ��)
  ��    ��: 
  ��    ��: 
  ��    ע: �ڲ���������������
  �޸ļ�¼: 
 *******************************************************/
void CObjectManager::SendEvent(bool bInitOrFini, IObject *pDynamicObject)
{
    DWORD dwCount = (pDynamicObject)? 1 : (DWORD)m_objects.size();
    if (!dwCount)
    {
        return;
    }

    /////////////////////////////////////////////////
    /// 1.����Init��Ϣ
    /////////////////////////////////////////////////
    DWORD dwDataLen = sizeof(DWORD) * (dwCount + 1);
    DWORD dwMsgType = DCOP_MSG_MANAGER_START;
    if (bInitOrFini)
    {
        if (pDynamicObject)
        {
            TRACE_LOG(STR_FORMAT("Send Event: Load Dynamic Object:'%s'(%d)!", pDynamicObject->Name(), pDynamicObject->ID()));
            dwMsgType = DCOP_MSG_MANAGER_LOAD;
        }
        else
        {
            TRACE_LOG(STR_FORMAT("Send Event: Start Static Objects!"));
            dwMsgType = DCOP_MSG_MANAGER_START;
        }
    }
    else
    {
        if (pDynamicObject)
        {
            TRACE_LOG(STR_FORMAT("Send Event: Unload Dynamic Object:'%s'(%d)!", pDynamicObject->Name(), pDynamicObject->ID()));
            dwMsgType = DCOP_MSG_MANAGER_UNLOAD;
        }
        else
        {
            TRACE_LOG(STR_FORMAT("Send Event: Finish Static Objects!"));
            dwMsgType = DCOP_MSG_MANAGER_FINISH;
        }
    }
    objMsg *pMsg = DCOP_CreateMsg(dwDataLen,
                        dwMsgType,
                        ID());
    if (!pMsg)
    {
        CHECK_RETCODE(FAILURE, STR_FORMAT("CreateMsg(Len:%d) Fail!", dwDataLen));
        return;
    }

    DWORD *pdwPara = (DWORD *)pMsg->GetDataBuf();
    OSASSERT(pdwPara != NULL);

    /////////////////////////////////////////////////
    /// 2.�������б���Ϣ
    /////////////////////////////////////////////////
    pdwPara[0] = dwCount;
    if (pDynamicObject)
    {
        pdwPara[1] = pDynamicObject->ID();
    }
    else
    {
        DWORD i = 1;
        for (IT_OBJECTS it_obj = m_objects.begin();
            it_obj != m_objects.end(); ++it_obj)
        {
            IObject *pObjTmp = ((*it_obj).second).GetObject();
            if (!pObjTmp) continue;

            pdwPara[i++] = pObjTmp->ID();
            if (i > dwCount) break;
        }
    }

    /////////////////////////////////////////////////
    /// 3.�Թ����������ж���㲥��Ϣ
    /////////////////////////////////////////////////
    BroadcastMsg(pMsg);
    delete pMsg;
}

