/// -------------------------------------------------
/// ObjKernel_main.cpp : object��ܺ�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjKernel_main.h"
#include "Factory_if.h"


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

    osBase::Dump(PrintToConsole, 0, 0, 0);

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

    return SUCCESS;
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

