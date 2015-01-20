/// -------------------------------------------------
/// ObjFactory_main.cpp : �೧ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjFactory_main.h"
#include "string/tablestring.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CClassFactory)
    DCOP_IMPLEMENT_INTERFACE(IFactory)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// �Զ�����
/// -------------------------------------------------
void vConstructFactoryFunc()
{
}

/// -------------------------------------------------
/// �Զ�����
/// -------------------------------------------------
void vReleaseFactoryFunc()
{
    if (CClassFactory::sm_pClassFactory)
    {
        (void)CClassFactory::sm_pClassFactory->Release();
        CClassFactory::sm_pClassFactory = 0;
    }
}

/// -------------------------------------------------
/// �Զ�����
/// -------------------------------------------------
CPPBUILDUNIT_AUTO(vConstructFactoryFunc, vReleaseFactoryFunc);

/// -------------------------------------------------
/// ȫ���������
/// -------------------------------------------------
CClassFactory *CClassFactory::sm_pClassFactory = 0;


/*******************************************************
  �� �� ��: IFactory::GetInstance
  ��    ��: ��ȡ�೧��ʵ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
            IFactory *          - �๤��ָ��
  �޸ļ�¼: 
 *******************************************************/
IFactory *IFactory::GetInstance()
{
    if (!CClassFactory::sm_pClassFactory)
    {
        CClassFactory::sm_pClassFactory = new CClassFactory();
        if (!CClassFactory::sm_pClassFactory)
        {
            /// һ��ʼ������ʧ�ܣ��ڴ�������
            return NULL;
        }
    }

    IFactory *piFactoryRc = NULL;
    if (CClassFactory::sm_pClassFactory->QueryInterface(ID_INTF(IFactory), REF_PTR(piFactoryRc)) != SUCCESS)
    {
        /// ����ʧ��ֻ���أ��������˳�ʱ�Զ�����
        return NULL;
    }

    return piFactoryRc;
}

/*******************************************************
  �� �� ��: CClassFactory::CClassFactory
  ��    ��: �๤��ʵ���๹��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CClassFactory::CClassFactory()
{
    m_pLock = DCOP_CreateLock();

    DCOP_CONSTRUCT_INSTANCE(NULL);
}

/*******************************************************
  �� �� ��: CClassFactory::~CClassFactory
  ��    ��: �๤��ʵ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CClassFactory::~CClassFactory()
{
    {
        AutoObjLock(this);
        m_instances.clear();
    }

    if (m_pLock)
    {
        delete m_pLock;
        m_pLock = 0;
    }

    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CClassFactory::InsertClass
  ��    ��: ����һ���ൽ�๤����
  ��    ��: 
            cpszClassName       - ����
            fnConstruct         - ��Ĺ�������
  ��    ��: 
  ��    ��: 
            SUCCESS             - �ɹ�
            FAILURE             - ʧ��
  �޸ļ�¼: 
 *******************************************************/
DWORD CClassFactory::InsertClass(const char *cszName,
                        DCOP_CREATE_INSTANCE_FUNC fnConstruct)
{
    if (!cszName)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    std::string strKey = cszName;
    IT_INSTANCES it_ins = m_instances.find(cszName);
    if (it_ins != m_instances.end())
    {
        return FAILURE;
    }

    (void)m_instances.insert(
                MAP_INSTANCES::value_type(
                strKey, fnConstruct));

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CClassFactory::CreateInstance
  ��    ��: ���๤���д���һ��ʵ��
  ��    ��: 
            cpszClassName       - Ҫ�������������
            piParent            - ʵ������(��ʵ��)
            argc                - ��Ĺ�����������
            argv                - ��Ĺ��������б�
  ��    ��: 
  ��    ��: 
            Instance *          - ������ʵ��
  �޸ļ�¼: 
 *******************************************************/
Instance *CClassFactory::CreateInstance(const char *cszName, 
                        Instance *piParent, 
                        int argc, 
                        char **argv)
{
    if (!cszName)
    {
        return 0;
    }

    AutoObjLock(this);

    std::string strKey = cszName;
    IT_INSTANCES it_ins = m_instances.find(cszName);
    if (it_ins == m_instances.end())
    {
        return 0;
    }

    Instance *pBaseRc = ((*it_ins).second)(piParent, argc, argv);

    return pBaseRc;
}

/*******************************************************
  �� �� ��: CClassFactory::Enter
  ��    ��: �����๤���ٽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CClassFactory::Enter()
{
    if (m_pLock)
    {
        m_pLock->Enter();
    }
}

/*******************************************************
  �� �� ��: CClassFactory::Leave
  ��    ��: �˳��๤���ٽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CClassFactory::Leave()
{
    if (m_pLock)
    {
        m_pLock->Leave();
    }
}

/*******************************************************
  �� �� ��: CClassFactory::Dump
  ��    ��: Dump���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CClassFactory::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);
    logPrint("-----------------------------------------------\r\n", logPara);
    logPrint("Factory Dump: \r\n", logPara);

    CTableString tableStr(2, (DWORD)m_instances.size(), "  ");

    for (IT_INSTANCES it_ins = m_instances.begin();
        it_ins != m_instances.end(); ++it_ins)
    {
        tableStr << STR_FORMAT("insName:'%s'", ((*it_ins).first).c_str());
        tableStr << STR_FORMAT("insFunc:'%p'", (*it_ins).second);
    }

    tableStr.Show(logPrint, logPara);

    logPrint(STR_FORMAT("Classes Count: %d\r\n", m_instances.size()), logPara);
    logPrint("-----------------------------------------------\r\n", logPara);
}

