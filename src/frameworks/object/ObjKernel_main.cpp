/// -------------------------------------------------
/// ObjKernel_main.cpp : object��ܺ�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjKernel_main.h"
#include "Factory_if.h"
#include "BaseID.h"


/// -------------------------------------------------
/// ȫ���ں���Դ��ʵ��
/// -------------------------------------------------
CFrameKernel    CFrameKernel::sm_instance;
void          (*g_onInstanceQueryInterface)(
                        Instance *piThis, 
                        Instance *piRefer, 
                        void *pPara) = 0;
void *          g_onInstanceQueryInterfacePara = 0;
void          (*g_onInstanceRelease)(
                        Instance *piThis, 
                        Instance *piRefer, 
                        void *pPara) = 0;
void *          g_onInstanceReleasePara = 0;
void          (*g_onInstanceGetRef)(
                        Instance *piThis, 
                        Instance ***pppiRefers, 
                        DWORD *pdwReferCount, 
                        void *pPara) = 0;
void *          g_onInstanceGetRefPara = 0;


/*******************************************************
  �� �� ��: CFrameKernel::CReferNode::OnReferto
  ��    ��: ������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::CReferNode::OnReferto(Instance *refer)
{
    IT_COUNT it = m_count.find(refer);
    if (it != m_count.end())
    {
        ((*it).second)++;
        return;
    }

    it = m_count.insert(m_count.end(), MAP_COUNT::value_type(refer, 1));
    if (it == m_count.end())
    {
        return;
    }

    /// �ߵ������ʾ������������ã���������������
    m_refer.Clear();
    m_refcnt.Clear();
    if (!m_count.size())
    {
        return;
    }

    (void)m_refer.Set(m_count.size() - 1, 0);
    (void)m_refcnt.Set(m_count.size() - 1, 0);
    for (it = m_count.begin(); it != m_count.end(); ++it)
    {
        (void)m_refer.Append((void *)&((*it).first));
        (void)m_refcnt.Append((void *)&((*it).second));
    }
}

/*******************************************************
  �� �� ��: CFrameKernel::CReferNode::OnRelease
  ��    ��: ���ͷ�ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::CReferNode::OnRelease(Instance *refer)
{
    IT_COUNT it = m_count.find(refer);
    if (it == m_count.end())
    {
        return;
    }

    if ((*it).second)
    {
        ((*it).second)--;
    }

    if ((*it).second)
    {
        return;
    }

    (void)m_count.erase(it);

    /// �ߵ������ʾ����ɾ�������ã���������������
    m_refer.Clear();
    m_refcnt.Clear();
    if (!m_count.size())
    {
        return;
    }

    (void)m_refer.Set(m_count.size() - 1, 0);
    (void)m_refcnt.Set(m_count.size() - 1, 0);
    for (it = m_count.begin(); it != m_count.end(); ++it)
    {
        (void)m_refer.Append((void *)&((*it).first));
        (void)m_refcnt.Append((void *)&((*it).second));
    }
}

/*******************************************************
  �� �� ��: CFrameKernel::CReferNode::OnGetRefer
  ��    ��: ��ȡ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::CReferNode::OnGetRefer(Instance ***refers, DWORD *count)
{
    if (!refers || !count)
    {
        return;
    }

    *refers = (Instance **)m_refer.Get();
    *count  = m_refer.Count();
}

/*******************************************************
  �� �� ��: CFrameKernel::CReferNode::OnGetReferCount
  ��    ��: ��ȡ���ü���ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::CReferNode::OnGetReferCount(DWORD **refercount, DWORD *count)
{
    if (!refercount || !count)
    {
        return;
    }

    *refercount = (DWORD *)m_refcnt.Get();
    *count      = m_refcnt.Count();
}

/*******************************************************
  �� �� ��: objBase::GetInstance
  ��    ��: ��ȡ�ں�ʵ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objBase *objBase::GetInstance()
{
    return &CFrameKernel::sm_instance;
}

/*******************************************************
  �� �� ��: objBase::~objBase
  ��    ��: �鹹ʵ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objBase::~objBase()
{
}

/*******************************************************
  �� �� ��: CFrameKernel::CFrameKernel
  ��    ��: CFrameKernel����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CFrameKernel::CFrameKernel()
{
    m_pLock = DCOP_CreateLock();
    m_pTask = 0;

    g_onInstanceQueryInterface          = OnInstanceQueryInterface;
    g_onInstanceQueryInterfacePara      = this;
    g_onInstanceRelease                 = OnInstanceRelease;
    g_onInstanceReleasePara             = this;
    g_onInstanceGetRef                  = OnInstanceGetRef;
    g_onInstanceGetRefPara              = this;
}

/*******************************************************
  �� �� ��: CFrameKernel::~CFrameKernel
  ��    ��: CFrameKernel����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CFrameKernel::~CFrameKernel()
{
    if (m_pLock)
    {
        delete m_pLock;
        m_pLock = 0;
    }

    if (m_pTask)
    {
        delete m_pTask;
        m_pTask = 0;
    }

    g_onInstanceQueryInterface          = 0;
    g_onInstanceQueryInterfacePara      = 0;
    g_onInstanceRelease                 = 0;
    g_onInstanceReleasePara             = 0;
    g_onInstanceGetRef                  = 0;
    g_onInstanceGetRefPara              = 0;
}

/*******************************************************
  �� �� ��: CFrameKernel::Enter
  ��    ��: ʵ�ֽ����������ȫ���ٽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::Enter()
{
    if (m_pLock)
    {
        m_pLock->Enter();
    }
}

/*******************************************************
  �� �� ��: CFrameKernel::Leave
  ��    ��: ʵ���˳���������ȫ���ٽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::Leave()
{
    if (m_pLock)
    {
        m_pLock->Leave();
    }
}

/*******************************************************
  �� �� ��: CFrameKernel::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);

    for (IT_REFERS it = m_refers.begin(); it != m_refers.end(); ++it)
    {
        Instance *piThis = ((*it).first);
        if (!piThis)
        {
            continue;
        }

        logPrint(STR_FORMAT("'%s'(id:%d)[inst:%p] Be Refered(count:%d) By: \r\n", 
                        piThis->Name(), piThis->ID(), piThis, piThis->GetRef()), logPara);

        Instance **ppiRefers = 0;
        DWORD dwReferCount = 0;
        ((*it).second).OnGetRefer(&ppiRefers, &dwReferCount);
        if (!ppiRefers || !dwReferCount)
        {
            logPrint("    No Refered! \r\n", logPara);
            continue;
        }

        DWORD *pdwReferCount = 0;
        DWORD dwCounterCount = 0;
        ((*it).second).OnGetReferCount(&pdwReferCount, &dwCounterCount);

        for (DWORD i = 0; i < dwReferCount; ++i)
        {
            Instance *piRefer = ppiRefers[i];
            if (!piRefer)
            {
                continue;
            }

            logPrint(STR_FORMAT("    '%s'(id:%d)[inst:%p](count:%d) \r\n", 
                        piRefer->Name(), piRefer->ID(), piRefer, 
                        (i < dwCounterCount)? pdwReferCount[i] : 0), logPara);
        }
    }

    osBase::Dump(logPrint, logPara, argc, argv);
}

/*******************************************************
  �� �� ��: CFrameKernel::Start
  ��    ��: ����Ӧ��ʵ�������
  ��    ��: cfgDeploy   - ����Ĳ��������ļ�
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
objBase *CFrameKernel::Start(const char *cfgDeploy)
{
    if (!m_pTask) m_pTask = DCOP_CreateTask("EntryTask", NULL, 0, 0, 0);

    /////////////////////////////////////////////////
    /// ���ع����������ж���
    /////////////////////////////////////////////////
    IManager *piManager = LoadAllObjects(cfgDeploy);
    if (!piManager)
    {
        return NULL;
    }

    /////////////////////////////////////////////////
    /// ��ʼ�������������ж���
    /////////////////////////////////////////////////
    DWORD dwRc = piManager->Init(NULL, 0, 0);
    if (dwRc)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("System(%d) InitAllObjects Fail(0x%x)!", 
                piManager->GetSystemID(), dwRc));
        (void)piManager->Release();
        return NULL;
    }

    TRACE_LOG(STR_FORMAT("System(%d) InitAllObjects OK!", piManager->GetSystemID()));
    piManager->Dump(PrintToConsole, 0, 0, 0);
    Dump(PrintToConsole, 0, 0, 0);

    return piManager;
}

/*******************************************************
  �� �� ��: CFrameKernel::End
  ��    ��: ����Ӧ��ʵ���ĳ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::End(objBase *pBase)
{
    /////////////////////////////////////////////////
    /// ���������������ж���
    /////////////////////////////////////////////////
    IManager *piManager = (IManager *)pBase;
    if (piManager)
    {
        piManager->Fini();
        DCOP_RELEASE_INSTANCE_REFER(0, piManager);
    }
}

/*******************************************************
  �� �� ��: CFrameKernel::LoadAllObjects
  ��    ��: ���������ļ��е����ж���
  ��    ��: xmlFile     - �����xml�����ļ�
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IManager *CFrameKernel::LoadAllObjects(const char *xmlFile)
{
    DWORD dwRc;
    XMLDocument doc;

    /////////////////////////////////////////////////
    /// ����xml�ļ�
    /////////////////////////////////////////////////
    dwRc = (DWORD)doc.LoadFile(xmlFile);
    if (dwRc)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("System('%s') LoadFile Fail!", xmlFile));
        return NULL;
    }

    IFactory *piFactory = IFactory::GetInstance();
    if (piFactory)
    {
        piFactory->Dump(PrintToConsole, 0, 0, 0);
        (void)piFactory->Release();
    }

    /////////////////////////////////////////////////
    /// ��ȡXml���ڵ�
    /////////////////////////////////////////////////
    XMLElement *pXmlSystem = doc.RootElement();
    if (!pXmlSystem)
    {
        CHECK_RETCODE(FAILURE, STR_FORMAT("System('%s') LoadFile Fail!", xmlFile));
        return NULL;
    }

    TRACE_LOG(STR_FORMAT("System('%s') LoadFile OK!", xmlFile));

    /////////////////////////////////////////////////
    /// ����ϵͳID�����������
    /////////////////////////////////////////////////
    objTask *pTask = objTask::Current();
    if (pTask)
    {
        DWORD dwSysID = 0;
        const char *sysID = pXmlSystem->Attribute("id");
        dwSysID = (sysID)? (DWORD)atoi(sysID) : 0;
        dwRc = pTask->SetLocal(TASK_LOCAL_SYSTEM, &dwSysID, sizeof(dwSysID));
        CHECK_ERRCODE(dwRc, "Set Sys ID To Task Local");
        DWORD dwObjID = DCOP_OBJECT_KERNEL;
        dwRc = pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
        CHECK_ERRCODE(dwRc, "Set Obj ID To Task Local");
    }

    /////////////////////////////////////////////////
    /// ʵ�������������
    /////////////////////////////////////////////////
    CDArray szArgs(DCOP_STRING_ARG_LEM_MAX, DCOP_SYSTEM_ARG_MAX_COUNT);
    char *argv[DCOP_SYSTEM_ARG_MAX_COUNT];
    DWORD argc = GetXmlAttribute(pXmlSystem, szArgs);
    GetArgList(argc, argv, szArgs);
    IManager *piManager = NULL;
    DCOP_CREATE_INSTANCE(IManager, "manager", NULL, argc, argv, piManager);
    if (!piManager)
    {
        CHECK_RETCODE(FAILURE, STR_FORMAT("System('%s') CreateManager Fail!", xmlFile));
        return NULL;
    }

    TRACE_LOG(STR_FORMAT("System('%s') CreateManager OK!", xmlFile));

    /////////////////////////////////////////////////
    /// �����������������ж���
    /////////////////////////////////////////////////
    dwRc = CreateAllObjects(piManager, pXmlSystem);
    if (dwRc)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("System(%d) CreateAllObjects Fail(0x%x)!", 
                piManager->GetSystemID(), dwRc));
        (void)piManager->Release();
        return NULL;
    }

    TRACE_LOG(STR_FORMAT("System(%d) CreateAllObjects OK!", piManager->GetSystemID()));

    if (pTask)
    {
        DWORD dwObjID = DCOP_OBJECT_KERNEL;
        dwRc = pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
        CHECK_ERRCODE(dwRc, "Set Obj ID To Task Local");
    }

    return piManager;
}

/*******************************************************
  �� �� ��: CFrameKernel::Load
  ��    ��: ���ص�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IObject *CFrameKernel::Load(const char *cszName, IManager *piManager, int argc, char **argv)
{
    IObject *piObject = 0;
    DCOP_CREATE_INSTANCE(IObject, cszName, piManager, argc, argv, piObject);
    if (!piObject)
    {
        CHECK_RETCODE(FAILURE, STR_FORMAT("Create Object: '%s' Fail!", cszName));
        return NULL;
    }

    DWORD dwRc = piManager->InsertObject(piObject);
    if (dwRc != SUCCESS)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("Insert Object: '%s'|%d Fail(0x%x)!", 
                        piObject->Name(), piObject->ID(), dwRc));
        DCOP_RELEASE_INSTANCE_REFER(piManager, piObject);
        return NULL;
    }

    return piObject;
}

/*******************************************************
  �� �� ��: CFrameKernel::DynamicLoad
  ��    ��: ���ض�̬��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objDynamicLoader *CFrameKernel::DynamicLoad(const char *dllFile)
{
    objDynamicLoader *pLoader = DCOP_CreateDynamicLoader();
    if (!pLoader)
    {
        return 0;
    }

    DWORD dwRc = pLoader->Load(dllFile);
    if (dwRc != SUCCESS)
    {
        delete pLoader;
        return 0;
    }

    return pLoader;
}

/*******************************************************
  �� �� ��: CFrameKernel::AddRefer
  ��    ��: �������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CFrameKernel::AddRefer(Instance *piThis, Instance *piRefer, objDynamicLoader *pLoader)
{
    if (!piThis)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    IT_REFERS it = m_refers.find(piThis);
    if (it == m_refers.end())
    {
        CReferNode refNode;
        it = m_refers.insert(m_refers.end(), MAP_REFERS::value_type(piThis, refNode));
        if (it == m_refers.end())
        {
            return FAILURE;
        }
    }

    if (pLoader)
    {
        ((*it).second).SetLoader(pLoader);
    }
    else
    {
        ((*it).second).OnReferto(piRefer);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CFrameKernel::DelRefer
  ��    ��: ɾ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CFrameKernel::DelRefer(Instance *piThis, Instance *piRefer)
{
    if (!piThis)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    IT_REFERS it = m_refers.find(piThis);
    if (it == m_refers.end())
    {
        return FAILURE;
    }

    ((*it).second).OnReferto(piRefer);

    if (!piThis->GetRef())
    {
        (void)m_refers.erase(it);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CFrameKernel::GetRefer
  ��    ��: ��ȡ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CFrameKernel::GetRefer(Instance *piThis, Instance ***pppiRefers)
{
    if (!piThis)
    {
        return 0;
    }

    AutoObjLock(this);

    IT_REFERS it = m_refers.find(piThis);
    if (it == m_refers.end())
    {
        return 0;
    }

    DWORD dwCount = 0;
    ((*it).second).OnGetRefer(pppiRefers, &dwCount);

    return dwCount;
}

/*******************************************************
  �� �� ��: CFrameKernel::GetXmlAttribute
  ��    ��: CFrameKernel��ȡXMLԪ������ֵ
  ��    ��: pXMLElement - XMLԪ��
  ��    ��: rArgs       - ��������
  ��    ��: �����������
  �޸ļ�¼: 
 *******************************************************/
DWORD CFrameKernel::GetXmlAttribute(const XMLElement *pXMLElement, CDArray &rArgs)
{
    DWORD dwArgCount = rArgs.Count();
    const XMLAttribute *pXMLAttribute = pXMLElement->FirstAttribute();
    while (pXMLAttribute)
    {
        char szConfig[DCOP_STRING_ARG_LEM_MAX];
        char szValue[DCOP_STRING_ARG_LEM_MAX];

        /// ����������(Ϊ�վͲ�������)
        const char *pszName = pXMLAttribute->Name();
        if (!pszName || !(*pszName))
        {
            pXMLAttribute = pXMLAttribute->Next();
            continue;
        }

        (void)snprintf(szConfig, sizeof(szConfig), "-%s", pszName);
        szConfig[sizeof(szConfig) - 1] = '\0';
        if (rArgs.Append(szConfig) != SUCCESS)
        {
            pXMLAttribute = pXMLAttribute->Next();
            continue;
        }

        /// ������ֵ(Ϊ�վ���������Ϊ����: ֻ�����ƣ���û��ֵ)
        const char *pszValue = pXMLAttribute->Value();
        if (!pszValue || !(*pszValue))
        {
            pXMLAttribute = pXMLAttribute->Next();
            continue;
        }

        (void)snprintf(szValue, sizeof(szValue), "%s", pszValue);
        szValue[sizeof(szValue) - 1] = '\0';
        (void)rArgs.Append(szValue);

        pXMLAttribute = pXMLAttribute->Next();
    }

    if (rArgs.Count() < dwArgCount)
    {
        dwArgCount = 0;
    }
    else
    {
        dwArgCount = rArgs.Count() - dwArgCount;
    }

    return dwArgCount;
}

/*******************************************************
  �� �� ��: CFrameKernel::GetXmlChildValue
  ��    ��: CFrameKernel��ȡXMLԪ���µ���Ԫ��ֵ(ֻ��DWORD)
  ��    ��: pXMLElement - XMLԪ��
  ��    ��: rArgs       - ��������
  ��    ��: ��Ԫ�ص�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CFrameKernel::GetXmlChildValue(const XMLElement *pXMLElement, CDArray &rArgs)
{
    DWORD dwArgCount = rArgs.Count();
    const XMLElement *pXMLChildElement = pXMLElement->FirstChildElement();
    while (pXMLChildElement)
    {
        char szConfig[DCOP_STRING_ARG_LEM_MAX];
        char szValue[DCOP_STRING_ARG_LEM_MAX];

        /// ��Ԫ������(Ϊ�վͲ�������)
        const char *pszName = pXMLChildElement->Name();
        if (!pszName || !(*pszName))
        {
            pXMLChildElement = pXMLChildElement->NextSiblingElement();
            continue;
        }

        (void)snprintf(szConfig, sizeof(szConfig), "-%s", pszName);
        szConfig[sizeof(szConfig) - 1] = '\0';
        if (rArgs.Append(szConfig) != SUCCESS)
        {
            pXMLChildElement = pXMLChildElement->NextSiblingElement();
            continue;
        }

        /// ������ֵ(Ϊ�վ���������Ϊ����: ֻ�����ƣ���û��ֵ)
        const char *pszValue = pXMLChildElement->GetText();
        if (!pszValue || !(*pszValue))
        {
            pXMLChildElement = pXMLChildElement->NextSiblingElement();
            continue;
        }

        (void)snprintf(szValue, sizeof(szValue), "%s", pszValue);
        szValue[sizeof(szValue) - 1] = '\0';
        (void)rArgs.Append(szValue);

        pXMLChildElement = pXMLChildElement->NextSiblingElement();
    }

    if (rArgs.Count() < dwArgCount)
    {
        dwArgCount = 0;
    }
    else
    {
        dwArgCount = rArgs.Count() - dwArgCount;
    }

    return dwArgCount;
}

/*******************************************************
  �� �� ��: CFrameKernel::GetArgList
  ��    ��: CFrameKernel��ȡchar **argv���ֲ�����ʽ
  ��    ��: crArgs      - ��������(��ð�argc��Ϊ��crArgs�������)
  ��    ��: argv        - �����б�
  ��    ��: ��Ԫ�ص�����
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::GetArgList(DWORD argc, char **argv, const CDArray &crArgs)
{
    for (DWORD i = 0; i < argc; ++i)
    {
        char *pStr = (char *)crArgs.Pos(i);
        if (!pStr)
        {
            break;
        }

        argv[i] = pStr;
    }
}

/*******************************************************
  �� �� ��: CFrameKernel::CreateAllObjects
  ��    ��: CFrameKernel��������XML�����еĶ���
  ��    ��: piManager   - ������
            pXMLElement - XMLԪ��
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CFrameKernel::CreateAllObjects(IManager *piManager, const XMLElement *pXMLElement)
{
    if (!piManager) return FAILURE;

    const XMLElement *pXmlObjects = pXMLElement->FirstChildElement("objects");
    if (!pXmlObjects)
    {
        return FAILURE;
    }

    const XMLElement *pXmlObject = pXmlObjects->FirstChildElement();
    DWORD dwRc = SUCCESS;
    while (pXmlObject)
    {
        /// ��ȡ���ò���
        CDArray szArgs(DCOP_STRING_ARG_LEM_MAX, DCOP_OBJECT_ARG_MAX_COUNT);
        char *argv[DCOP_OBJECT_ARG_MAX_COUNT];
        DWORD argc = 0;
        argc  = GetXmlAttribute (pXmlObject, szArgs);
        argc += GetXmlChildValue(pXmlObject, szArgs);
        GetArgList(argc, argv, szArgs);

        /// �������������
        objTask *pTask = objTask::Current();
        if (pTask)
        {
            DWORD dwObjID = 0;
            const char *objID = pXmlObject->Attribute("id");
            dwObjID = (objID)? (DWORD)atoi(objID) : 0;
            (void)pTask->SetLocal(TASK_LOCAL_HANDLER, &dwObjID, sizeof(dwObjID));
        }

        /// ����Ƕ�̬�⣬���Ƚ��ж�̬����
        objDynamicLoader *pLoader = 0;
        const char *dllFile = pXmlObject->Attribute("dll");
        if (dllFile)
        {
            pLoader = CFrameKernel::DynamicLoad(dllFile);
        }

        /// ���ص�������
        const char *cszName = pXmlObject->Attribute("inst");
        if (!cszName) cszName = pXmlObject->Attribute("name");
        IObject *piObject = Load(cszName, piManager, argc, argv);
        if (!piObject)
        {
            delete pLoader;
            pLoader = 0;
            dwRc |= FAILURE;
        }

        /// ��Ӷ�̬���ؾ��
        if (pLoader)
        {
            if (AddRefer(piObject, 0, pLoader) != SUCCESS)
            {
                delete pLoader;
                pLoader = 0;
            }
        }

        /// ����һ������Ԫ��
        pXmlObject = pXmlObject->NextSiblingElement();
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CFrameKernel::OnInstanceQueryInterface
  ��    ��: ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::OnInstanceQueryInterface(
                        Instance *piThis, 
                        Instance *piRefer, 
                        void *pPara)
{
    CFrameKernel *pKernel = (CFrameKernel *)pPara;
    if (!pKernel)
    {
        return;
    }

    (void)pKernel->AddRefer(piThis, piRefer);
}

/*******************************************************
  �� �� ��: CFrameKernel::OnInstanceRelease
  ��    ��: �ͷ�����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::OnInstanceRelease(
                    Instance *piThis, 
                    Instance *piRefer, 
                    void *pPara)
{
    CFrameKernel *pKernel = (CFrameKernel *)pPara;
    if (!pKernel)
    {
        return;
    }

    (void)pKernel->DelRefer(piThis, piRefer);
}

/*******************************************************
  �� �� ��: CFrameKernel::OnInstanceGetRef
  ��    ��: ��ȡ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CFrameKernel::OnInstanceGetRef(
                    Instance *piThis, 
                    Instance ***pppiRefers, 
                    DWORD *pdwReferCount, 
                    void *pPara)
{
    CFrameKernel *pKernel = (CFrameKernel *)pPara;
    if (!pKernel)
    {
        return;
    }

    DWORD dwReferCount = pKernel->GetRefer(piThis, pppiRefers);
    if (pdwReferCount)
    {
        *pdwReferCount = dwReferCount;
    }
}

