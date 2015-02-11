/// -------------------------------------------------
/// httpd_main.cpp : ���ı�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "httpd_main.h"
#include "Factory_if.h"
#include "BaseMessage.h"
#include "ObjAttribute_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CHttpServer, "httpd")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CHttpServer)
    DCOP_IMPLEMENT_INTERFACE(IHttpServer)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CHttpServer)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
    DCOP_IMPLEMENT_CONFIG_CONCURRENT_ENABLE()
    DCOP_IMPLEMENT_CONFIG_INTEGER("lantaskcount", m_dwHttpdLanTaskCount)
    DCOP_IMPLEMENT_CONFIG_STRING("proctaskcount", m_szHttpdProcTaskCount)
    DCOP_IMPLEMENT_CONFIG_INTEGER("httpdport", m_wHttpdPort)
    DCOP_IMPLEMENT_CONFIG_STRING("httpddir", m_szHttpdDir)
    DCOP_IMPLEMENT_CONFIG_STRING("httpdhome", m_szHttpdHome)
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CHttpServer)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MODEL_REG, OnModelReg)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_SESSION_TIMEOUT, OnSessionTimeout)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_OBJECT_RESPONSE, OnResponse)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_OBJECT_EVENT, OnEvent)
    DCOP_IMPLEMENT_IOBJECT_MSG_DEFAULT(OnDefault)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END


/*******************************************************
  �� �� ��: CHttpServer::CHttpServer
  ��    ��: CHttpServer���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpServer::CHttpServer(Instance *piParent, int argc, char **argv)
{
    m_dwHttpdLanTaskCount = LAN_TASK_COUNT;
    (void)memset(m_szHttpdProcTaskCount, 0, sizeof(m_szHttpdProcTaskCount));
    m_wHttpdPort = HTTP_PORT;
    (void)snprintf(m_szHttpdDir, sizeof(m_szHttpdDir), ".");
    (void)snprintf(m_szHttpdHome, sizeof(m_szHttpdHome), "index.html");

    m_piManager = 0;
    m_piDispatch = 0;
    m_piNotify = 0;
    m_piModel = 0;
    m_piStatus = 0;

    m_piResponse = 0;
    m_pReqPool = 0;

    m_piUser = 0;
    m_piSession = 0;
    m_piAccess = 0;

    m_pLanApp = 0;

    m_pHttpRequest = 0;
    m_pHttpProcess = 0;
    m_pHttpResponse = 0;

    m_pHttpJson = 0;

    m_pHttpSchedule = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CHttpServer::~CHttpServer
  ��    ��: CHttpServer��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpServer::~CHttpServer()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CHttpServer::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpServer::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IManager,     DCOP_OBJECT_MANAGER,    m_piManager)
        DCOP_QUERY_OBJECT_ITEM(IDispatch,    DCOP_OBJECT_DISPATCH,   m_piDispatch)
        DCOP_QUERY_OBJECT_ITEM(INotify,      DCOP_OBJECT_NOTIFY,     m_piNotify)
        DCOP_QUERY_OBJECT_ITEM(IModel,       DCOP_OBJECT_MODEL,      m_piModel)
        DCOP_QUERY_OBJECT_ITEM(IStatus,      DCOP_OBJECT_STATUS,     m_piStatus)
        DCOP_QUERY_OBJECT_ITEM(IResponse,    DCOP_OBJECT_RESPONSE,   m_piResponse)
        DCOP_QUERY_OBJECT_ITEM(IUser,        DCOP_OBJECT_USER,       m_piUser)
        DCOP_QUERY_OBJECT_ITEM(ISession,     DCOP_OBJECT_SESSION,    m_piSession)
        DCOP_QUERY_OBJECT_ITEM(IAccess,      DCOP_OBJECT_ACCESS,     m_piAccess)
    DCOP_QUERY_OBJECT_END

    /// �����¼�
    SUBSCRIBE_EVENT_START(m_piNotify)
        SUBSCRIBE_EVENT_ITEM(DCOP_OBJECT_MODEL,     DCOP_MSG_MODEL_REG)
        SUBSCRIBE_EVENT_ITEM(DCOP_OBJECT_USER,      DCOP_OBJATTR_USER_TABLE)
        SUBSCRIBE_EVENT_ITEM(DCOP_OBJECT_SESSION,   DCOP_OBJATTR_SESSION_TABLE)
    SUBSCRIBE_EVENT_END

    /// ����HTTP����/����/��Ӧ����
    const char *httpdProcName = "restful";
    char httpdProcID[16];
    (void)snprintf(httpdProcID, sizeof(httpdProcID), "%d", DCOP_OBJECT_RESTFUL);
    char *httpdProcArg[] = 
    {
        (char *)"-name", (char *)httpdProcName, 
        (char *)"-id", httpdProcID, 
        (char *)"-dir", m_szHttpdDir, (char *)"-home", m_szHttpdHome
    };
    DCOP_CREATE_INSTANCE(IHttpRequest, "HttpRequest", this, 0, 0, m_pHttpRequest);
    DCOP_CREATE_INSTANCE(IHttpProcess, httpdProcName, this, ARRAY_SIZE(httpdProcArg), httpdProcArg, m_pHttpProcess);
    DCOP_CREATE_INSTANCE(IHttpResponse, "HttpResponse", this, 0, 0, m_pHttpResponse);
    if (!m_pHttpRequest || !m_pHttpProcess || !m_pHttpResponse)
    {
        return FAILURE;
    }

    /// �����������뵽��������
    DWORD dwRc = m_piManager->InsertObject(m_pHttpProcess);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ����HTTP����JSON����(��������'HttpProcess')
    DCOP_CREATE_INSTANCE(IObject, "HttpJson", m_pHttpProcess, 0, 0, m_pHttpJson);
    if (!m_pHttpJson)
    {
        return FAILURE;
    }

    /// ע����Դ���ͺʹ������
    IHttpProcess::ResTypeNode procArg[] = 
    {
        {"json", m_pHttpJson},
    };
    dwRc = m_pHttpProcess->RegResType(procArg, ARRAY_SIZE(procArg));
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ����HTTP�����̳߳ص�����
    char *schdArg[] = 
    {
        (char *)"-taskname", (char *)"HttpdProc", 
        (char *)"-taskcount", m_szHttpdProcTaskCount
    };
    DCOP_CREATE_INSTANCE(ISchedule, "schedule", this, ARRAY_SIZE(schdArg), schdArg, m_pHttpSchedule);
    if (!m_pHttpSchedule)
    {
        return FAILURE;
    }

    /// ��ʼ��HTTP�����̳߳ص�����
    dwRc = m_pHttpSchedule->Init(root, 0, 0);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ����HTTP�����̳߳ص�����
    m_pHttpSchedule->SetProc(ProcHttp, this);
    m_pHttpSchedule->SetSameDst(false);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpServer::Fini
  ��    ��: ����ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::Fini()
{
    OnFinish(NULL);

    if (m_pHttpProcess)
    {
        m_pHttpProcess->Fini();
    }

    DCOP_RELEASE_INSTANCE(m_pHttpJson);
    DCOP_RELEASE_INSTANCE(m_pHttpSchedule);
    DCOP_RELEASE_INSTANCE(m_pHttpRequest);
    DCOP_RELEASE_INSTANCE(m_pHttpProcess);
    DCOP_RELEASE_INSTANCE(m_pHttpResponse);

    DCOP_RELEASE_INSTANCE(m_piAccess);
    DCOP_RELEASE_INSTANCE(m_piSession);
    DCOP_RELEASE_INSTANCE(m_piUser);
    DCOP_RELEASE_INSTANCE(m_piResponse);
    DCOP_RELEASE_INSTANCE(m_piStatus);
    DCOP_RELEASE_INSTANCE(m_piModel);
    DCOP_RELEASE_INSTANCE(m_piNotify);
    DCOP_RELEASE_INSTANCE(m_piDispatch);
    DCOP_RELEASE_INSTANCE(m_piManager);
}

/*******************************************************
  �� �� ��: CHttpServer::Dump
  ��    ��: DUMP���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);

    if (m_pHttpSchedule)
    {
        logPrint(">>>>>>>>> Http Scheduler Dump: \r\n", logPara);
        m_pHttpSchedule->Dump(logPrint, logPara, argc, argv);
    }

    logPrint(STR_FORMAT(">>>>>>>>> Http Dir: %s \r\n", m_szHttpdDir), logPara);
    logPrint(STR_FORMAT(">>>>>>>>> Http Home: %s \r\n", m_szHttpdHome), logPara);
}

/*******************************************************
  �� �� ��: CHttpServer::SaveHttpToSession
  ��    ��: ����HTTP������Ự��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::SaveHttpToSession(DWORD dwSessionID, IHttpHandle *pHttp)
{
    /// �����Ǵ���HTTP�����SOCK���񶼿��ܷ��ʣ�����Ҫʹ�ñ���
    AutoObjLock(this);

    SessionNode *pSessNode = FindSession(dwSessionID);
    if (!pSessNode)
    {
        return;
    }

    if (pHttp == pSessNode->m_pHttp)
    {
        return;
    }

    /// ���������HTTP���������������
    if (pHttp)
    {
        (void)pHttp->QueryInterface(ID_INTF(IHttpHandle));
        if (pSessNode->m_pHttp)
        {
            /// ����˵���Ǹ�����HTTP�������Ҫ��ԭHTTP��������ͷ�
            (void)pSessNode->m_pHttp->Release();
        }
    }

    /// �����ȡ��HTTP��������ͷ�����
    if (!pHttp && pSessNode->m_pHttp)
    {
        (void)pSessNode->m_pHttp->Release();
    }

    pSessNode->m_pHttp = pHttp;
}

/*******************************************************
  �� �� ��: CHttpServer::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnStart(objMsg *msg)
{
    /// ������Ϣ��ڿ��Բ������룬���Է����ڲ�����ʱ��Ҫ����
    AutoObjLock(this);

    /// �������󻺳��
    if (m_piResponse && !m_pReqPool)
    {
        m_pReqPool = m_piResponse->CreatePool(this, REQ_POOL_COUNT);
        if (!m_pReqPool) CHECK_ERRCODE(FAILURE, "Create Response Pool");
    }

    /// ��������
    StartLanApp();
}

/*******************************************************
  �� �� ��: CHttpServer::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnFinish(objMsg *msg)
{
    /// ������Ϣ��ڿ��Բ������룬���Է����ڲ�����ʱ��Ҫ����
    AutoObjLock(this);

    /// ֹͣ����
    StopLanApp();

    /// ɾ�����󻺳��
    if (m_piResponse && m_pReqPool)
    {
        m_piResponse->DestroyPool(m_pReqPool);
        m_pReqPool = 0;
    }
}

/*******************************************************
  �� �� ��: CHttpServer::OnModelReg
  ��    ��: ģ��ע���¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnModelReg(objMsg *msg)
{
    if (!msg || !m_piModel || !m_pHttpProcess)
    {
        return;
    }

    DWORD *pdwPara = (DWORD *)msg->GetDataBuf();
    if (!pdwPara) return;

    DWORD dwCount = pdwPara[0];
    if (!dwCount)
    {
        return;
    }

    IHttpProcess::ResPathNode *pNode = (IHttpProcess::ResPathNode *)DCOP_Malloc(
                        dwCount * sizeof(IHttpProcess::ResPathNode));
    if (!pNode)
    {
        return;
    }

    for (DWORD i = 0; i < dwCount; ++i)
    {
        DWORD dwAttrID = pdwPara[i + 1];

        pNode[i].m_resPath = m_piModel->GetTableName(dwAttrID);
        pNode[i].m_attrID = dwAttrID;

        RegModel(dwAttrID);
    }

    DWORD dwRc = m_pHttpProcess->RegResPath(pNode, dwCount);
    CHECK_ERRCODE(dwRc, "Reg Res Path");

    DCOP_Free(pNode);
}

/*******************************************************
  �� �� ��: CHttpServer::OnSessionTimeout
  ��    ��: �Ự��ʱ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnSessionTimeout(objMsg *msg)
{
    if (!msg) return;

    DWORD *pdwSessID = (DWORD *)msg->GetDataBuf();
    if (!pdwSessID) return;

    /// ������Ϣ��ڿ��Բ������룬���Է����ڲ�����ʱ��Ҫ����
    AutoObjLock(this);

    if (!m_piModel || !m_pHttpProcess)
    {
        return;
    }

    SessionNode *pSessNode = FindSession(*pdwSessID);
    if (!pSessNode)
    {
        return;
    }

    objSock *pSock = pSessNode->m_pSock;
    if (!pSock)
    {
        return;
    }

#if _HTTPD_DEBUG_
    PrintLog(STR_FORMAT("<Session(%d) Shutdown> %s:%d", pSessNode->m_dwSessID, 
                        pSock->cszGetHostIP(), pSock->wGetHostPort()), 
                        PrintToConsole, 0);
#endif

    /// �ɷ����߳̽������Źرգ��Ժ���ɽ����߳��Լ��ر�
    (void)pSock->Shut();
}

/*******************************************************
  �� �� ��: CHttpServer::OnResponse
  ��    ��: ��Ӧ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnResponse(objMsg *msg)
{
    if (!msg) return;

    /// �����Ựͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(msg->GetDataBuf(), msg->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(0);
    if (!pSessionHead)
    {
        return;
    }

    /// ������Ϣ��ڿ��Բ������룬���Է����ڲ�����ʱ��Ҫ����
    AutoObjLock(this);

#if _HTTPD_DEBUG_ && _HTTPD_DEBUG_DETAIL_
    PrintBuffer(STR_FORMAT("<Recv Ack Msg> len:%d type:0x%x src:%d, dst:%d", 
                        msg->GetDataLen(), msg->GetMsgType(), msg->GetSrcID(), msg->GetDstID()), 
                        msg->GetDataBuf(), msg->GetDataLen(), 
                        PrintToConsole, 0);
#endif

    /// ���ұ��ػỰ
    SessionNode *pSessNode = FindSession(pSessionHead->m_session);
    if (!pSessNode)
    {
        return;
    }

    /// �������ݴ���
    ProcData(*pSessNode, DCOP_MSG_HTTPD_PROCESS, msg->GetDataBuf(), msg->GetDataLen());
}

/*******************************************************
  �� �� ��: CHttpServer::OnEvent
  ��    ��: �¼���Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnEvent(objMsg *msg)
{
}

/*******************************************************
  �� �� ��: CHttpServer::OnHttpRequest
  ��    ��: Http������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnHttpRequest(objMsg *msg)
{
    if (!msg || !m_pHttpRequest)
    {
        return;
    }

    /// ��Ϣ�Ŀ��Ʋ�����������ǻỰ��Ϣ
    DWORD dwCtrlLen = 0;
    SessionNode *pSessNode = (SessionNode *)msg->GetCtrl(dwCtrlLen);
    if (!pSessNode || (dwCtrlLen < sizeof(SessionNode)))
    {
        return;
    }

    /// ���������ӾͲ��ܴ���������
    objSock *pSock = pSessNode->m_pSock;
    if (!pSock)
    {
        return;
    }

    /// ��������Ϣת��Ϊ�ַ����������н���
    CDString strReq((const char *)msg->GetDataBuf(), msg->GetDataLen());
    if (!strReq.Length())
    {
        return;
    }

    CHttpRequest::StrDecode(strReq.Get());

#if _HTTPD_DEBUG_
    PrintLog(STR_FORMAT("<Session(%d) Recv Http Req> Len:%d From:'%s:%d'", 
                        pSessNode->m_dwSessID, msg->GetDataLen(), pSock->cszGetHostIP(), pSock->wGetHostPort()), 
                        PrintToConsole, 0);
    if ((const char *)strReq) PrintToConsole((const char *)strReq, 0);
#endif

    /// ��ȡ�Ự��Ϣ�е�HTTP�����û�о��´���һ��
    IHttpHandle *pHttp = pSessNode->m_pHttp;
    if (!pHttp)
    {
        pHttp = CREATE_HTTP_HANDLE();
        if (!pHttp) return;
        pSessNode->m_pHttp = pHttp;
    }

    /// �������봦��
    m_pHttpRequest->Input(pHttp, (const char *)strReq);

    /// ���������������Ҫ���صȴ���һ������
    if (!pHttp->bCompleted())
    {
        /// �����´�Ҫʹ�ã�ֻ���ȱ����ԭʼ�Ự��(��Ϊ����׶εľ����ȡ��ʱ���ᱻ���)
        SaveHttpToSession(pSessNode->m_dwSessID, pHttp);
        return;
    }

    /// ���ûỰ��Ϣ
    if (!pSessNode->m_bSetClientInfo)
    {
        SetSessionInfo(pSessNode->m_dwSessID, pHttp->GetUserAgent());
    }

    /// ���Ž��д���
    if (pHttp->GetStatus() == IHttpHandle::STATUS_PROCESS)
    {
        msg->MsgHead().m_dwMsgType = DCOP_MSG_HTTPD_PROCESS;
    }
    else
    {
        msg->MsgHead().m_dwMsgType = DCOP_MSG_HTTPD_RESPONSE;
    }
}

/*******************************************************
  �� �� ��: CHttpServer::OnHttpProcess
  ��    ��: Http������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnHttpProcess(objMsg *msg)
{
    if (!msg || !m_pHttpProcess)
    {
        return;
    }

    /// ��Ϣ�Ŀ��Ʋ�����������ǻỰ��Ϣ
    DWORD dwCtrlLen = 0;
    SessionNode *pSessNode = (SessionNode *)msg->GetCtrl(dwCtrlLen);
    if (!pSessNode || (dwCtrlLen < sizeof(SessionNode)))
    {
        return;
    }

    /// ��ȡ�Ự��Ϣ�е�HTTP���
    IHttpHandle *pHttp = pSessNode->m_pHttp;
    if (!pHttp)
    {
        return;
    }

    /// �������ݴ���
    m_pHttpProcess->Proc(msg);

    /// ���Ž�����Ӧ
    if (pHttp->GetStatus() >= IHttpHandle::STATUS_RESPONSE)
    {
        msg->MsgHead().m_dwMsgType = DCOP_MSG_HTTPD_RESPONSE;
    }
}

/*******************************************************
  �� �� ��: CHttpServer::OnHttpResponse
  ��    ��: Http��Ӧ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnHttpResponse(objMsg *msg)
{
    if (!msg || !m_pHttpResponse)
    {
        return;
    }

    /// ��Ϣ�Ŀ��Ʋ�����������ǻỰ��Ϣ
    DWORD dwCtrlLen = 0;
    SessionNode *pSessNode = (SessionNode *)msg->GetCtrl(dwCtrlLen);
    if (!pSessNode || (dwCtrlLen < sizeof(SessionNode)))
    {
        return;
    }

    /// ���������ӾͲ��ܴ�����Ӧ��
    objSock *pSock = pSessNode->m_pSock;
    if (!pSock)
    {
        return;
    }

    /// ��ȡ�Ự��Ϣ�е�HTTP���
    IHttpHandle *pHttp = pSessNode->m_pHttp;
    if (!pHttp)
    {
        return;
    }

    /// ������Ӧ���
    CDStream sRsp;
    DWORD dwHeadSize = 0;
    m_pHttpResponse->Output(pHttp, sRsp, &dwHeadSize);

    /// Send�����������ԣ�����ֻ��Ҫ����ÿ�εȴ�ʱ�伴��
    DWORD dwSent = 0;
    DWORD dwRc = pSock->Send(sRsp.Buffer(), sRsp.Length(), &dwSent, true, RSP_WAIT_TIME);
#if _HTTPD_DEBUG_
    PrintLog(STR_FORMAT("<Session(%d) Send Http Rsp> Len:%d Sent:%d bCompleted:%d[%d/%d] Rc:%d To:'%s:%d'", 
                        pSessNode->m_dwSessID, sRsp.Length(), dwSent, pHttp->bCompleted(), 
                        pHttp->Content().Length(), pHttp->GetContentLength(), dwRc, 
                        pSock->cszGetHostIP(), pSock->wGetHostPort()), 
                        PrintToConsole, 0);
    CDString strRsp((const char *)sRsp.Buffer(), dwHeadSize);
    if ((const char *)strRsp) PrintToConsole((const char *)strRsp, 0);
#endif

    /// �������ʧ�ܣ������Ѿ�������ϣ��˳�
    if ((dwRc != SUCCESS) || (pHttp->bCompleted()))
    {
        return;
    }

    /// ������Ŵ���δ������ϵ���Ӧ����
    DWORD dwOffset = pHttp->Content().Length();
    DWORD dwLength = pHttp->GetContentLength();
    if (dwOffset >= dwLength) return;
    while (dwOffset < dwLength)
    {
        sRsp.Clear();
        if (sRsp.LoadFile(pHttp->GetURI(), dwOffset, DSTREAM_DMEM_MAX_LEN) != SUCCESS)
        {
            break;
        }

        /// Send�����������ԣ�����ֻ��Ҫ����ÿ�εȴ�ʱ�伴��
        dwSent = 0;
        dwRc = pSock->Send(sRsp.Buffer(), sRsp.Length(), &dwSent, true, RSP_WAIT_TIME);
    #if _HTTPD_DEBUG_
        PrintLog(STR_FORMAT("<Session(%d) Send Http Rsp Continue> Offset:%d Len:%d Sent:%d Rc:%d To:'%s:%d'", 
                        pSessNode->m_dwSessID, dwOffset, sRsp.Length(), dwSent, dwRc, 
                        pSock->cszGetHostIP(), pSock->wGetHostPort()), 
                        PrintToConsole, 0);
    #endif
        if (dwRc != SUCCESS)
        {
            /// Send��������Ժܶ�Σ�����ʧ��ֻ���˳���
            break;
        }

        dwOffset += sRsp.Length();
    }
}

/*******************************************************
  �� �� ��: CHttpServer::OnDefault
  ��    ��: Ĭ����Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnDefault(objMsg *msg)
{
}

/*******************************************************
  �� �� ��: CHttpServer::RegModel
  ��    ��: ע��ģ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::RegModel(DWORD dwAttrID)
{
    if (!m_piModel) return;

    /// ������Ϣ��ڿ��Բ������룬���Է����ڲ�����ʱ��Ҫ����
    AutoObjLock(this);

    std::string strKey = m_piModel->GetTableName(dwAttrID);
    (void)m_objattrs.insert(MAP_OBJATTRS::value_type(strKey, dwAttrID));
}

/*******************************************************
  �� �� ��: CHttpServer::StartLanApp
  ��    ��: ��������Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::StartLanApp()
{
    TRACE_LOG("HttpServer Network Service Start ...");

    m_pLanApp = objLanApp::CreateInstance();
    if (!m_pLanApp)
    {
        TRACE_LOG("HttpServer Service Init Fail!");
    }

    DWORD dwRc = m_pLanApp->AddTCPServer(HTTP_CHANNEL, m_wHttpdPort);
    TRACE_LOG(STR_FORMAT("HttpServer Service Add TCPServer(channel:%d,port:%d) rc:0x%x!", HTTP_CHANNEL, m_wHttpdPort, dwRc));

    objLanApp::LanEventProc theEventProc = 
    {
        OnAccept, this,
        NULL, 0,
        OnDisconnect, this,
        OnRecv, this
    };

    objLanApp::LanFrameProc theFrameProc = 
    {
        bFrame, this,
        BytesOrder, this
    };

    objLanApp::LanLogProc theLogProc = 
    {
        OnLogPrint, this
    };

    IObject *piParent = Parent();
    dwRc = m_pLanApp->Start("HttpdLan",
                        (piParent)? piParent->ID() : 0,
                        ID(),
                        m_dwHttpdLanTaskCount,
                        &theEventProc,
                        &theFrameProc,
                        &theLogProc);
    TRACE_LOG(STR_FORMAT("HttpServer Service Start Rc:0x%x!", dwRc));
}

/*******************************************************
  �� �� ��: CHttpServer::StopLanApp
  ��    ��: ֹͣ����Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::StopLanApp()
{
    if (!m_pLanApp)
    {
        return;
    }

    m_pLanApp->Stop();
    delete m_pLanApp;
    m_pLanApp = 0;

    TRACE_LOG("HttpServer Network Service Stop!");
}

/*******************************************************
  �� �� ��: CHttpServer::OnLogPrint
  ��    ��: ��־��ӡ�ص�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnLogPrint(const char *cszLogInfo,
                        const char *pFile,
                        DWORD dwLine,
                        void *pUserArg)
{
    TraceLogEx(cszLogInfo, pFile, dwLine);
}

/*******************************************************
  �� �� ��: CHttpServer::OnAccept
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpServer::OnAccept(DWORD dwChannelID,
                        objSock *pServerSock,
                        objSock *pAcceptSock,
                        void *pUserArg)
{
    CHttpServer *pThis = (CHttpServer *)pUserArg;
    if (!pThis)
    {
        return FAILURE;
    }

    AutoObjLock(pThis);

    DWORD dwRc = pThis->CreateSession(pAcceptSock);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpServer::OnDisconnect
  ��    ��: �Ͽ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnDisconnect(DWORD dwChannelID,
                        objSock *pSock,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg)
{
    CHttpServer *pThis = (CHttpServer *)pUserArg;
    if (!pThis)
    {
        return;
    }

    AutoObjLock(pThis);

    pThis->DeleteSession(objSock::GetIPValueByString(cszRemoteIP), wRemotePort);
}

/*******************************************************
  �� �� ��: CHttpServer::OnRecv
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::OnRecv(DWORD dwChannelID,
                        objSock *pSock,
                        void *pFrameBuf,
                        DWORD dwFrameLen,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg)
{
    CHttpServer *pThis = (CHttpServer *)pUserArg;
    if (!pThis)
    {
        return;
    }

    AutoObjLock(pThis);

    SessionNode *pSessNode = pThis->FindSession(
                        objSock::GetIPValueByString(cszRemoteIP), 
                        wRemotePort);
#if _HTTPD_DEBUG_ && _HTTPD_DEBUG_DETAIL_
    PrintBuffer(STR_FORMAT("=== %s:%d -> Session(%d) \r\n", cszRemoteIP, wRemotePort, 
                        (pSessNode)? pSessNode->m_dwSessID : (-1)), pFrameBuf, dwFrameLen, 
                        PrintToConsole, 0);
#endif
    if (!pSessNode)
    {
        return;
    }

    pThis->ProcData(*pSessNode, DCOP_MSG_HTTPD_REQUEST, pFrameBuf, dwFrameLen);
}

/*******************************************************
  �� �� ��: CHttpServer::bFrame
  ��    ��: �ж�����֡
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int CHttpServer::bFrame(void *pBuf,
                        DWORD dwLen,
                        void *pUserArg)
{
    return dwLen;
}

/*******************************************************
  �� �� ��: CHttpServer::BytesOrder
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::BytesOrder(void *pBuf,
                    DWORD dwLen,
                    void *pUserArg)
{
}

/*******************************************************
  �� �� ��: CHttpServer::CreateSession
  ��    ��: �����Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpServer::CreateSession(objSock *pSock)
{
    if (!pSock || !m_piSession) return FAILURE;

    DWORD dwSessionID = 0;
    ISession::NODE sessNode;
    (void)memset(&sessNode, 0, sizeof(sessNode));

    /// ���һỰ�Ƿ����
    DWORD dwIP = objSock::GetIPValueByString(pSock->cszGetHostIP());
    WORD wPort = pSock->wGetHostPort();
    DWORD dwRc = m_piSession->FindSession(dwIP, wPort, sessNode);
    if (dwRc == SUCCESS)
    {
        /// �Ѿ���ӵ��Ự����
    #if _HTTPD_DEBUG_
        PrintLog(STR_FORMAT("<Session(%d) Exist> %s:%d", sessNode.SessID, 
                        pSock->cszGetHostIP(), pSock->wGetHostPort()), 
                        PrintToConsole, 0);
    #endif
        return SUCCESS;
    }

    /// �����ڵĻỰ�����Ϊδ��¼�û�
    dwRc = m_piSession->CreateSession(DCOP_USER_UNLOGIN, 
                        DCOP_GROUP_VISITOR, 
                        ID(), 
                        dwIP, 
                        wPort, 
                        dwSessionID);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    SessionNode sess;
    (void)memset(sess.m_szUserName, 0, sizeof(sess.m_szUserName));
    sess.m_dwUserID = DCOP_USER_UNLOGIN;
    sess.m_dwUserGroup = DCOP_GROUP_VISITOR;
    sess.m_dwSessID = dwSessionID;
    sess.m_pSock = pSock;
    sess.m_pHttp = 0;
    sess.m_bSetClientInfo = false;
    (void)m_sessions.insert(MAP_SESSIONS::value_type(dwSessionID, sess));

#if _HTTPD_DEBUG_
    PrintLog(STR_FORMAT("<Session(%d) Connected> %s:%d", dwSessionID, 
                        pSock->cszGetHostIP(), pSock->wGetHostPort()), 
                        PrintToConsole, 0);
#endif

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpServer::DeleteSession
  ��    ��: ɾ���Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::DeleteSession(DWORD dwIP, WORD wPort)
{
    if (!m_piSession) return;

    /// ���һỰ�Ƿ����
    ISession::NODE sessNode;
    DWORD dwRc = m_piSession->FindSession(dwIP, wPort, sessNode);
    if (dwRc != SUCCESS)
    {
        return;
    }

    (void)m_piSession->DeleteSession(sessNode.SessID);

    IT_SESSIONS it_sess = m_sessions.find(sessNode.SessID);
    if (it_sess == m_sessions.end())
    {
        return;
    }

    IHttpHandle *pHttp = ((*it_sess).second).m_pHttp;
    if (pHttp)
    {
        (void)pHttp->Release();
    }

    (void)m_sessions.erase(sessNode.SessID);

#if _HTTPD_DEBUG_
    char szIP[OSSOCK_IPSIZE] = {0,};
    objSock::GetIPStringByValue(dwIP, szIP);
    PrintLog(STR_FORMAT("<Session(%d) Disconnected> %s:%d", sessNode.SessID, 
                        szIP, wPort), 
                        PrintToConsole, 0);
#endif
}

/*******************************************************
  �� �� ��: CHttpServer::FindSession
  ��    ��: ���һỰ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpServer::SessionNode *CHttpServer::FindSession(DWORD dwIP, WORD wPort)
{
    ISession::NODE sessNode;

    /// ���һỰ�Ƿ����
    DWORD dwRc = m_piSession->FindSession(dwIP, wPort, sessNode);
    if (dwRc != SUCCESS)
    {
        return NULL;
    }

    IT_SESSIONS it_sess = m_sessions.find(sessNode.SessID);
    if (it_sess == m_sessions.end())
    {
        return NULL;
    }

    return &((*it_sess).second);
}

/*******************************************************
  �� �� ��: CHttpServer::FindSession
  ��    ��: ���һỰ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpServer::SessionNode *CHttpServer::FindSession(DWORD dwSessionID)
{
    IT_SESSIONS it_sess = m_sessions.find(dwSessionID);
    if (it_sess == m_sessions.end())
    {
        return NULL;
    }

    return &((*it_sess).second);
}

/*******************************************************
  �� �� ��: CHttpServer::SetSessionInfo
  ��    ��: ���ûỰ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::SetSessionInfo(DWORD dwSessionID, const char *cszUserAgent)
{
    if (!cszUserAgent) return;

    /// UA��Ϣ���ң�����ʷ�໥����ԭ���йأ���΢���õ�ϵͳ��Ϣ�ڵ�һ��������
    const char *pcszRealInfo = strchr(cszUserAgent, '(');
    if (!pcszRealInfo) pcszRealInfo = cszUserAgent;
    else pcszRealInfo++;

    AutoObjLock(this);

    if (!m_piSession)
    {
        return;
    }

    SessionNode *pSessNode = FindSession(dwSessionID);
    if (!pSessNode)
    {
        return;
    }

    char szInfo[ISession::INFO_SIZE];
    (void)snprintf(szInfo, sizeof(szInfo), "%s", pcszRealInfo);

    pSessNode->m_bSetClientInfo = (m_piSession->SetSessionInfo(dwSessionID, 
                        szInfo) == SUCCESS)? true : false;
}

/*******************************************************
  �� �� ��: CHttpServer::ProcData
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::ProcData(SessionNode &sessNode, DWORD dwMsgType, void *pFrameBuf, DWORD dwFrameLen)
{
    if (!sessNode.m_pSock || !pFrameBuf || !dwFrameLen) return;

    if (m_piSession)
    {
        m_piSession->UpdateSession(sessNode.m_dwSessID);
    }

    if (!m_pHttpSchedule)
    {
        return;
    }

    objMsg *pMsg = DCOP_LoadMsg(dwFrameLen, dwMsgType, ID(), pFrameBuf, dwFrameLen);
    if (!pMsg)
    {
        return;
    }

    /// �ѻỰ��Ϣ���õ���Ϣ������
    SessionNode *pSessNode = (SessionNode *)pMsg->SetCtrl(&sessNode, sizeof(SessionNode));
    if (!pSessNode)
    {
        delete pMsg;
        return;
    }

    /// ���ƻỰ��Ϣ�е�SOCK���Ա�����ڴ���������ֱ�ӻظ���Ӧ
    objSock *pCopySock = DCOP_CreateSock(objSock::OSSOCK_NONE, 
                        sessNode.m_pSock->cszGetHostIP(), 
                        sessNode.m_pSock->wGetHostPort());
    if (!pCopySock)
    {
        delete pMsg;
        return;
    }

    /// �Ը��ƺ�ĻỰ�е�HTTP�����������
    if (pSessNode->m_pHttp)
    {
        (void)pSessNode->m_pHttp->QueryInterface(ID_INTF(IHttpHandle));

        /// ȥ������״̬�ĻỰ�е�HTTP���
        if (dwMsgType == DCOP_MSG_HTTPD_REQUEST)
        {
            SaveHttpToSession(pSessNode->m_dwSessID, NULL);
        }
    }

    /// ��ؼ����ǰ�SOCK�������������������̷߳���ʹ��
    pCopySock->vSetSock(sessNode.m_pSock->sGetSock());
    pSessNode->m_pSock = pCopySock;

    pMsg->MsgHead().m_dwDstID = ID();
    if (m_pHttpSchedule->Join(pMsg) != SUCCESS)
    {
        delete pCopySock;
        delete pMsg;
    }
}

/*******************************************************
  �� �� ��: CHttpServer::ProcHttp
  ��    ��: ����HTTP����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpServer::ProcHttp(objMsg *pMsg, void *pUserArg)
{
    if (!pMsg || !pUserArg) return;

    CHttpServer *pThis = (CHttpServer *)pUserArg;
    if (!pThis)
    {
        return;
    }

#if _HTTPD_DEBUG_ && _HTTPD_DEBUG_DETAIL_
    DWORD dwCtrlLenTmp = 0;
    SessionNode *pSessNodeTmp = (SessionNode *)pMsg->GetCtrl(dwCtrlLenTmp);
    PrintBuffer(STR_FORMAT("... ProcHttp Session(%d) \r\n", pSessNodeTmp->m_dwSessID), 
                        pMsg->GetDataBuf(), pMsg->GetDataLen(), 
                        PrintToConsole, 0);
#endif

    /// ���ν������´���

    if (pMsg->GetMsgType() == DCOP_MSG_HTTPD_REQUEST)
    {
        pThis->OnHttpRequest(pMsg);
    }

    if (pMsg->GetMsgType() == DCOP_MSG_HTTPD_PROCESS)
    {
        pThis->OnHttpProcess(pMsg);
    }

    if (pMsg->GetMsgType() == DCOP_MSG_HTTPD_RESPONSE)
    {
        pThis->OnHttpResponse(pMsg);
    }

    /// ��Ϣ��������SOCK�����⸴�Ƶģ�֮�����ˣ���Ҫ�����ͷ�
    DWORD dwCtrlLen = 0;
    SessionNode *pSessNode = (SessionNode *)pMsg->GetCtrl(dwCtrlLen);
    if (!pSessNode || (dwCtrlLen < sizeof(SessionNode)))
    {
        return;
    }

    /// HTTP��������õģ��ͷ�����
    if (pSessNode->m_pHttp)
    {
        DWORD dwRefCount = pSessNode->m_pHttp->Release();

        /// ��������Ϊ�գ���ͬʱȥ����Ӧ״̬�ĻỰ�е�HTTP���
        if (dwRefCount && (pMsg->GetMsgType() == DCOP_MSG_HTTPD_RESPONSE))
        {
            pThis->SaveHttpToSession(pSessNode->m_dwSessID, NULL);
        }
    }

    /// SOCK����Ǹ��Ƶģ��ͷſռ�
    if (pSessNode->m_pSock)
    {
        delete pSessNode->m_pSock;
        pSessNode->m_pSock = 0;
    }
}

