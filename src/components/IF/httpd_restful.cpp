/// -------------------------------------------------
/// httpd_restful.cpp : ���ı�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "httpd_restful.h"
#include "httpd_protocol.h"
#include "Factory_if.h"
#include "BaseID.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CHttpProcess, "restful")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CHttpProcess)
    DCOP_IMPLEMENT_INTERFACE(IHttpProcess)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CHttpProcess)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_STRING("dir", m_szDir)
    DCOP_IMPLEMENT_CONFIG_STRING("home", m_szHome)
    DCOP_IMPLEMENT_CONFIG_THREADSAFE_ENABLE()
    DCOP_IMPLEMENT_CONFIG_CONCURRENT_ENABLE()
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CHttpProcess::CHttpProcess
  ��    ��: CHttpProcess���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpProcess::CHttpProcess(Instance *piParent, int argc, char **argv)
{
    (void)memset(m_szDir, 0, sizeof(m_szDir));
    (void)memset(m_szHome, 0, sizeof(m_szHome));

    m_pPathNode = 0;
    m_dwPathCount = 0;
    m_pTypeNode = 0;
    m_dwTypeCount = 0;

    m_piManager = 0;
    m_piModel = 0;
    m_piAccess = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    if (!(*m_szDir)) (void)snprintf(m_szDir, sizeof(m_szDir), ".");
    if (!(*m_szHome)) (void)snprintf(m_szHome, sizeof(m_szHome), "index.html");
}

/*******************************************************
  �� �� ��: CHttpProcess::~CHttpProcess
  ��    ��: CHttpProcess��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpProcess::~CHttpProcess()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CHttpProcess::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpProcess::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IManager,     DCOP_OBJECT_MANAGER,    m_piManager)
        DCOP_QUERY_OBJECT_ITEM(IModel,       DCOP_OBJECT_MODEL,      m_piModel)
        DCOP_QUERY_OBJECT_ITEM(IAccess,      DCOP_OBJECT_ACCESS,     m_piAccess)
    DCOP_QUERY_OBJECT_END

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpProcess::Fini
  ��    ��: ����ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpProcess::Fini()
{
    AutoObjLock(this);

    if (m_pPathNode)
    {
        DCOP_Free(m_pPathNode);
        m_pPathNode = 0;
    }

    m_dwPathCount = 0;

    if (m_pTypeNode)
    {
        DCOP_Free(m_pTypeNode);
        m_pTypeNode = 0;
    }

    m_dwPathCount = 0;

    DCOP_RELEASE_INSTANCE(m_piAccess);
    DCOP_RELEASE_INSTANCE(m_piModel);
    DCOP_RELEASE_INSTANCE(m_piManager);
}

/*******************************************************
  �� �� ��: CHttpProcess::RegResPath
  ��    ��: ע����Դ·��������ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpProcess::RegResPath(ResPathNode *pNode, DWORD dwCount)
{
    if (!pNode || !dwCount)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    DWORD dwNewCount = m_dwPathCount + dwCount;
    ResPathNode *pNewNode = (ResPathNode *)DCOP_Malloc(dwNewCount * sizeof(ResPathNode));
    if (!pNewNode)
    {
        return FAILURE;
    }

    if (m_pPathNode)
    {
        (void)memcpy(pNewNode, m_pPathNode, m_dwPathCount * sizeof(ResPathNode));
        DCOP_Free(m_pPathNode);
    }

    (void)memcpy(pNewNode + m_dwPathCount, pNode, dwCount * sizeof(ResPathNode));

    m_pPathNode = pNewNode;
    m_dwPathCount = dwNewCount;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpProcess::RegResType
  ��    ��: ע����Դ���ͺʹ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpProcess::RegResType(ResTypeNode *pNode, DWORD dwCount)
{
    if (!pNode || !dwCount)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    DWORD dwNewCount = m_dwTypeCount + dwCount;
    ResTypeNode *pNewNode = (ResTypeNode *)DCOP_Malloc(dwNewCount * sizeof(ResTypeNode));
    if (!pNewNode)
    {
        return FAILURE;
    }

    if (m_pTypeNode)
    {
        (void)memcpy(pNewNode, m_pTypeNode, m_dwTypeCount * sizeof(ResTypeNode));
        DCOP_Free(m_pTypeNode);
    }

    (void)memcpy(pNewNode + m_dwTypeCount, pNode, dwCount * sizeof(ResTypeNode));

    m_pTypeNode = pNewNode;
    m_dwTypeCount = dwNewCount;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpProcess::GetResOwner
  ��    ��: ��ȡ��Դ·����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IObject *CHttpProcess::GetResOwner(const char *URI, DWORD &rdwAttrID)
{
    AutoObjLock(this);

    if (!m_pPathNode || !m_piManager || !m_piModel)
    {
        return NULL;
    }

    CDString str(URI);
    if (!str.Length())
    {
        return NULL;
    }

    CDArray strList;
    str.Split("./", strList);
    if (!strList.Count())
    {
        return NULL;
    }

    /// ����·��֮�������"rest"Ŀ¼
    CDString *pStr = (CDString *)strList.Pos(1);
    if (!pStr)
    {
        return NULL;
    }

    if (stricmp("rest", *pStr))
    {
        return NULL;
    }

    pStr = (CDString *)strList.Pos(2);
    if (!pStr)
    {
        return NULL;
    }

    DWORD dwAttrID = 0;
    for (DWORD i = 0; i < m_dwPathCount; ++i)
    {
        if (!stricmp(m_pPathNode[i].m_resPath, *pStr))
        {
            dwAttrID = m_pPathNode[i].m_attrID;
            break;
        }
    }

    if (!dwAttrID)
    {
        return NULL;
    }

    DWORD dwObjID = m_piModel->GetObjID(dwAttrID);
    if (!dwObjID)
    {
        return NULL;
    }

    IObject *piOwner = m_piManager->Child(dwObjID);
    if (!piOwner)
    {
        return NULL;
    }

    rdwAttrID = dwAttrID;
    return piOwner;
}

/*******************************************************
  �� �� ��: CHttpProcess::Input
  ��    ��: ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpProcess::Input(objMsg *msg, DWORD clientIP, WORD clientPort)
{
    AutoObjLock(this);

    if (!m_piAccess)
    {
        return FAILURE;
    }

    return m_piAccess->Input(msg, clientIP, clientPort);
}

/*******************************************************
  �� �� ��: CHttpProcess::Proc
  ��    ��: ����HTTP���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpProcess::Proc(objMsg *msg)
{
    if (!msg)
    {
        return;
    }

    /////////////////////////////////////////////////
    /// ��Ҫ֧����Ϣ��������Ϣ��ڲ���ʹ�ô������б���
    /////////////////////////////////////////////////

    /// ��Ϣ�Ŀ��Ʋ�����������ǻỰ��Ϣ
    DWORD dwCtrlLen = 0;
    IHttpServer::SessionNode *pSessNode = (IHttpServer::SessionNode *)msg->GetCtrl(dwCtrlLen);
    if (!pSessNode || (dwCtrlLen < sizeof(IHttpServer::SessionNode)))
    {
        return;
    }

    /// ��ȡ�Ự��Ϣ�е�HTTP���
    IHttpHandle *http = pSessNode->m_pHttp;
    if (!http)
    {
        return;
    }

    /// ��Դ�ַ�����
    const char *resType = strrchr(http->GetURI(), '.');
    if (!resType)
    {
        resType = strrchr(CHttpResponse::GetContentType(http), '/');
    }

    if (resType && (Dispatch(resType + 1, msg) == SUCCESS))
    {
        return;
    }

    /// ����Ĭ����Դ
    DefaultProc(http);
}

/*******************************************************
  �� �� ��: CHttpProcess::Dispatch
  ��    ��: �ַ�HTTP����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpProcess::Dispatch(const char *resType, objMsg *msg)
{
    if (!resType || !msg)
    {
        return FAILURE;
    }

    IObject *objProc = NULL;
    {
        AutoObjLock(this);

        if (!m_pTypeNode)
        {
            return FAILURE;
        }

        for (DWORD i = 0; i < m_dwTypeCount; ++i)
        {
            if (!stricmp(m_pTypeNode[i].m_resType, resType))
            {
                if (m_pTypeNode[i].m_objProc)
                {
                    objProc = m_pTypeNode[i].m_objProc;
                }

                break;
            }
        }
    }

    if (!objProc)
    {
        return FAILURE;
    }

    objProc->Proc(msg);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpProcess::DefaultProc
  ��    ��: ����Ĭ����Դ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpProcess::DefaultProc(IHttpHandle *http)
{
    CDString strURI(m_szDir);
    strURI.TrimRight("/");

    strURI << http->GetURI();
    if (strURI.Get(CDString::TAIL) == '/')
    {
        strURI << m_szHome;
    }

    /// ����ļ������ھ���Ӧ����
    if (DCOP_IsFileExist(strURI) != TRUE)
    {
        http->SetStatus(IHttpHandle::STATUS_NOT_FOUND);
        return;
    }

    /// �ļ����ݲ��ܳ���������С
    DWORD dwFileLen = DCOP_GetFileLen(strURI);
    http->SetContentLength(dwFileLen);
    if (dwFileLen > DSTREAM_DMEM_MAX_LEN)
    {
        http->SetURI(strURI);
        http->Content().Clear();

        /// ��Ҫ��Ӧ�����жϲ�����ʱ���з�Ƭ����
        http->SetStatus(IHttpHandle::STATUS_RESPONSE);
        return;
    }

    /// ���ؾ�̬�ļ�
    if (http->Content().LoadFile(strURI) != SUCCESS)
    {
        http->SetStatus(IHttpHandle::STATUS_NOT_FOUND);
        return;
    }

    http->SetStatus(IHttpHandle::STATUS_RESPONSE);
}

