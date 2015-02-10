/// -------------------------------------------------
/// access_main.cpp : �ֲ�ʽ�������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "access_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "BaseMessage.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CAccess, "access")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CAccess)
    DCOP_IMPLEMENT_INTERFACE(IAccess)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CAccess)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
    DCOP_IMPLEMENT_CONFIG_INTEGER("lantaskcount", m_dwLanTaskCount)
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ������
/// -------------------------------------------------
IMPLEMENT_ATTRIBUTE(CAccess, loginProc, DCOP_OBJATTR_ACCESS_LOGIN, IModel::TYPE_METHOD)
IMPLEMENT_ATTRIBUTE(CAccess, logoutProc, DCOP_OBJATTR_ACCESS_LOGOUT, IModel::TYPE_METHOD)

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CAccess)
    IMPLEMENT_ATTRIBUTE_MSG_PROC(accessIndex)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_SESSION_TIMEOUT, OnSessionTimeout)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_OBJECT_RESPONSE, OnResponse)
    DCOP_IMPLEMENT_IOBJECT_MSG_DEFAULT(OnDefault)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END


/*******************************************************
  �� �� ��: CAccess::CAccess
  ��    ��: CAccess���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CAccess::CAccess(Instance *piParent, int argc, char **argv)
{
    m_dwLanTaskCount = LAN_TASK_COUNT;

    m_piUser = 0;
    m_piSession = 0;
    m_piDispatch = 0;
    m_pNotifyPool = 0;

    m_piControl = 0;
    m_pCtrlChain = 0;

    m_piResponse = 0;
    m_pReqPool = 0;

    m_pLanApp = 0;

    m_loginProc = OnLogin;
    m_logoutProc = OnLogout;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CAccess::~CAccess
  ��    ��: CAccess��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CAccess::~CAccess()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CAccess::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CAccess::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IUser, DCOP_OBJECT_USER, m_piUser)
        DCOP_QUERY_OBJECT_ITEM(ISession, DCOP_OBJECT_SESSION, m_piSession)
        DCOP_QUERY_OBJECT_ITEM(IDispatch, DCOP_OBJECT_DISPATCH, m_piDispatch)
        DCOP_QUERY_OBJECT_ITEM(IControl, DCOP_OBJECT_CONTROL, m_piControl)
        DCOP_QUERY_OBJECT_ITEM(IResponse, DCOP_OBJECT_RESPONSE, m_piResponse)
    DCOP_QUERY_OBJECT_END

    /// ����������
    m_pCtrlChain = m_piControl->CreateChain(this);
    if (!m_pCtrlChain)
    {
        return FAILURE;
    }

    /// ��ʼ������
    INIT_ATTRIBUTE_START(accessIndex, m_piDispatch, m_pNotifyPool)
        INIT_ATTRIBUTE_MEMBER(loginProc)
        INIT_ATTRIBUTE_MEMBER(logoutProc)
    INIT_ATTRIBUTE_END

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CAccess::Fini
  ��    ��: ����ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::Fini()
{
    OnFinish(NULL);

    /// ɾ��������
    if (m_piControl && m_pCtrlChain)
    {
        (void)m_piControl->DestroyChain(m_pCtrlChain);
        m_pCtrlChain = 0;
    }

    DCOP_RELEASE_INSTANCE(m_piResponse);
    DCOP_RELEASE_INSTANCE(m_piControl);
    DCOP_RELEASE_INSTANCE(m_piDispatch);
    DCOP_RELEASE_INSTANCE(m_piSession);
    DCOP_RELEASE_INSTANCE(m_piUser);
}

/*******************************************************
  �� �� ��: CAccess::Input
  ��    ��: �ⲿ���ڲ�������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CAccess::Input(objMsg *pMsg, DWORD dwRemoteIP, WORD wRemotePort)
{
    if (!pMsg) return FAILURE;

    ISession::NODE sessNode;
    (void)memset(&sessNode, 0, sizeof(sessNode));

    AutoObjLock(this);

    /// ������ĻỰ
    DWORD dwRc = CheckLogin(dwRemoteIP, wRemotePort, sessNode);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ����������
    if (m_pCtrlChain)
    {
        objMsg *pCtrlMsg = 0;
        bool bContinue = true;

        dwRc = m_pCtrlChain->OnCtrl(pMsg, pCtrlMsg, bContinue);
        if (!bContinue)
        {
            return dwRc;
        }

        if (dwRc == SUCCESS)
        {
            if (pCtrlMsg)
            {
                delete pMsg;
                pMsg = pCtrlMsg;
            }
        }
    }

    /// ���ڲ��ַ���Ϣ
    return DispatchMsg(pMsg, &sessNode);
}

/*******************************************************
  �� �� ��: CAccess::Output
  ��    ��: �ڲ����ⲿ�����Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CAccess::Output(objMsg *pMsg, DWORD dwRemoteIP, WORD wRemotePort)
{
    if (!pMsg || !dwRemoteIP || !wRemotePort)
    {
        return FAILURE;
    }

    /// GetDataChannel�����л��������Ȼ�����û�з��ʳ�Ա����������Ͳ��û��������
    objSock *pSock = GetDataChannel();
    if (!pSock)
    {
        return FAILURE;
    }

    DWORD dwFrameLen = 0;
    void *pFrameBuf = DCOP_PackMsg(pMsg, dwFrameLen);
    if (!pFrameBuf)
    {
        return FAILURE;
    }

    char szRemoteIP[OSSOCK_IPSIZE];
    (void)memset(szRemoteIP, 0, sizeof(szRemoteIP));
    objSock::GetIPStringByValue(dwRemoteIP, szRemoteIP);
    DWORD dwRc = pSock->Sendto(pFrameBuf, dwFrameLen, szRemoteIP, wRemotePort);
    DCOP_Free(pFrameBuf);
    if (dwRc == SUCCESS)
    {
        /// ���ͳɹ�������Ҫ�ͷ���Ϣ������ʧ�ܣ����ⲿ�������ͷ�
        delete pMsg;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CAccess::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnStart(objMsg *msg)
{
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
  �� �� ��: CAccess::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnFinish(objMsg *msg)
{
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
  �� �� ��: CAccess::OnSessionTimeout
  ��    ��: �Ự��ʱ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnSessionTimeout(objMsg *msg)
{
}

/*******************************************************
  �� �� ��: CAccess::OnResponse
  ��    ��: ��Ӧ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnResponse(objMsg *msg)
{
    if (!msg || !m_piDispatch) return;

    /// �������󻺳�ؽ���Ԥ����
    if (m_pReqPool && (m_pReqPool->OnRsp((DCOP_SESSION_HEAD *)msg->GetDataBuf()) != SUCCESS))
    {
        return;
    }

    /// �����Ựͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(msg->GetDataBuf(), msg->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(0);
    if (!pSessionHead)
    {
        return;
    }

    /// ����Ǳ���ն˵���Ϣ��ת����Ϣ����Ӧ�Ľ����ն�
    DWORD dwRc = SUCCESS;
    if (pSessionHead->m_tty != ID())
    {
        objMsg *pOtherMsg = DCOP_CloneMsg(msg);
        if (pOtherMsg)
        {
            pOtherMsg->MsgHead().m_dwDstID = pSessionHead->m_tty;
            dwRc = m_piDispatch->Send(pOtherMsg);
            if (dwRc != SUCCESS) delete pOtherMsg;
        }
        return;
    }

    if (!m_piSession) return;

    ISession::NODE sessNode;
    (void)memset(&sessNode, 0, sizeof(sessNode));

    if (m_piSession->GetSession(pSessionHead->m_session, sessNode) != SUCCESS)
    {
        return;
    }

    if (pSessionHead->m_user != sessNode.UserID)
    {
        return;
    }

    /// ʣ�����Լ��նˣ�������ⲿ��������
    objMsg *pOutputMsg = DCOP_CloneMsg(msg);
    if (pOutputMsg)
    {
        dwRc = Output(pOutputMsg, sessNode.IP, sessNode.Port);
        if (dwRc != SUCCESS) delete pOutputMsg;
    }
}

/*******************************************************
  �� �� ��: CAccess::OnDefault
  ��    ��: Ĭ����Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnDefault(objMsg *msg)
{
}

/*******************************************************
  �� �� ��: CAccess::OnLogPrint
  ��    ��: ��־��ӡ�ص�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnLogPrint(const char *cszLogInfo,
                        const char *pFile,
                        DWORD dwLine,
                        void *pUserArg)
{
    TraceLogEx(cszLogInfo, pFile, dwLine);
}

/*******************************************************
  �� �� ��: CAccess::OnAccept
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CAccess::OnAccept(DWORD dwChannelID,
                        objSock *pServerSock,
                        objSock *pAcceptSock,
                        void *pUserArg)
{
    return FAILURE;
}

/*******************************************************
  �� �� ��: CAccess::OnRecv
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnRecv(DWORD dwChannelID,
                        objSock *pSock,
                        void *pFrameBuf,
                        DWORD dwFrameLen,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg)
{
    /// ֻ��������ͨ������Ϣ
    if (dwChannelID != CHANNEL_DATA)
    {
        return;
    }

    CAccess *pThis = (CAccess *)pUserArg;
    if (!pThis)
    {
        return;
    }

    /// ������Ϣ
    objMsg *pMsg = DCOP_ParseMsg(pFrameBuf, dwFrameLen);
    if (!pMsg)
    {
        return;
    }

    if (pThis->Input(pMsg, 
                        objSock::GetIPValueByString(cszRemoteIP), 
                        wRemotePort) != SUCCESS)
    {
        delete pMsg;
    }
}

/*******************************************************
  �� �� ��: CAccess::OnLogin
  ��    ��: �յ���¼��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnLogin(IObject *pOwner, 
                        objMsg *pMsg, 
                        const DCOP_SESSION_HEAD &sessionHead, 
                        const CDArray &aCondHeads, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen)
{
    if (!pOwner || !pMsg || 
        !pReqPara || !dwReqParaCount || 
        !pReqData || !dwReqDataLen)
    {
        return;
    }

    /// ��ȡ�Ự��Ϣ
    CAccess *pThis = (CAccess *)pOwner;
    ISession *piSession = pThis->GetSession();
    if (!piSession) return;
    ISession::NODE sessNode;
    (void)memset(&sessNode, 0, sizeof(sessNode));
    if (piSession->GetSession(sessionHead.m_session, sessNode) != SUCCESS)
    {
        return;
    }

    /// �����������¼��Ϣ
    char *pUserName = 0;
    char *pPassText = 0;
    IUser::NODE userNode;
    (void)memset(&userNode, 0, sizeof(userNode));
    DWORD dwDataPos = 0;
    DWORD dwRc = FAILURE;
    do
    {
        /// ��ȡ��Ϣ���û�����У����
        for (DWORD dwParaIdx = 0; dwParaIdx < dwReqParaCount; ++dwParaIdx)
        {
            if (pReqPara[dwParaIdx].m_paraID == LOGIN_USERNAME)
            {
                pUserName = (char *)((BYTE *)pReqData + dwDataPos);
            }

            if (pReqPara[dwParaIdx].m_paraID == LOGIN_PASSTEXT)
            {
                pPassText = (char *)((BYTE *)pReqData + dwDataPos);
            }

            /// ����ƫ��
            dwDataPos += pReqPara[dwParaIdx].m_paraSize;
            if (dwDataPos > dwReqDataLen)
            {
                break;
            }
        }

        /// ��ȡУ����ʧ��
        if (!pUserName || !pPassText) break;

        /// ��ȡ�û���Ϣ
        IUser *piUser = pThis->GetUser();
        if (!piUser) break;
        if (piUser->FindUser(pUserName, userNode) != SUCCESS)
        {
            break;
        }

        /// У��У����
        if (memcmp(pPassText, userNode.PassText, IUser::PASS_SIZE) != 0)
        {
            break;
        }

        /// �����û�
        if (piSession->UpdateUserID(sessionHead.m_session, 
                        userNode.UserID, userNode.Group) != SUCCESS)
        {
            break;
        }

        dwRc = SUCCESS;
    } while (0);

    /// ������Ӧ��Ϣ
    objMsg *pRespMsg = DCOP_CreateMsg(DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_SESSION] + 
                        DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_RESPONSE], 
                        DCOP_MSG_OBJECT_RESPONSE, pThis->ID());
    if (!pRespMsg)
    {
        return;
    }

    void *pRespData = pRespMsg->GetDataBuf();
    if (!pRespData)
    {
        delete pRespMsg;
        return;
    }

    DCOP_SESSION_HEAD *pRespSession = (DCOP_SESSION_HEAD *)pRespData;
    pRespSession->m_type.m_headType = DCOP_MSG_HEAD_SESSION;
    pRespSession->m_type.m_headSize = DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_SESSION];
    pRespSession->m_type.m_valueLen = DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_RESPONSE];
    pRespSession->m_ver = DCOP_SESSION_VER;
    pRespSession->m_count = 1;
    pRespSession->m_group = (BYTE)userNode.Group;
    pRespSession->m_session = sessionHead.m_session;
    pRespSession->m_user = userNode.UserID;
    pRespSession->m_tty = pThis->ID();
    pRespSession->m_attribute = DCOP_OBJATTR_ACCESS_LOGIN;
    pRespSession->m_index = sessionHead.m_index;
    pRespSession->m_ctrl = DCOP_CTRL_METHOD;
    pRespSession->m_ack = DCOP_RSP_END;
    BYTES_CHANGE_SESSION_HEAD_ORDER(pRespSession);

    DCOP_RESPONSE_HEAD *pRespHead = (DCOP_RESPONSE_HEAD *)(pRespSession + 1);
    pRespHead->m_type.m_headType = DCOP_MSG_HEAD_RESPONSE;
    pRespHead->m_type.m_headSize = DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_RESPONSE];
    pRespHead->m_type.m_valueLen = 0;
    pRespHead->m_retCode = dwRc;
    pRespHead->m_recordCount = 0;
    pRespHead->m_recordIndex = 0;
    pRespHead->m_paraCount = 0;
    pRespHead->m_paraLen = 0;
    BYTES_CHANGE_RESPONSE_HEAD_ORDER(pRespHead);

    if (pThis->Output(pRespMsg, sessNode.IP, sessNode.Port) != SUCCESS)
    {
        delete pRespMsg;
    }
}

/*******************************************************
  �� �� ��: CAccess::OnLogout
  ��    ��: �յ��˳���Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::OnLogout(IObject *pOwner, 
                        objMsg *pMsg, 
                        const DCOP_SESSION_HEAD &sessionHead, 
                        const CDArray &aCondHeads, 
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen)
{
    
}

/*******************************************************
  �� �� ��: CAccess::StartLanApp
  ��    ��: ��������Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::StartLanApp()
{
    TRACE_LOG("Access Network Service Start ...");

    m_pLanApp = objLanApp::CreateInstance();
    if (!m_pLanApp)
    {
        TRACE_LOG("Access Service Init Fail!");
    }

    DWORD dwRc = m_pLanApp->AddTCPServer(CHANNEL_CTRL, PORT_CTRL);
    TRACE_LOG(STR_FORMAT("Access Service Add TCPServer(channel:%d,port:%d) rc:0x%x!", CHANNEL_CTRL, PORT_CTRL, dwRc));

    dwRc = m_pLanApp->AddUDP(CHANNEL_DATA, true, PORT_DATA, false);
    TRACE_LOG(STR_FORMAT("Access Service Add UDP(channel:%d,port:%d) rc:0x%x!", CHANNEL_DATA, PORT_DATA, dwRc));

    objLanApp::LanEventProc theEventProc = 
    {
        OnAccept, this,
        NULL, 0,
        NULL, 0,
        OnRecv, this
    };

    objLanApp::LanLogProc theLogProc = 
    {
        OnLogPrint, this
    };

    IObject *piParent = Parent();
    dwRc = m_pLanApp->Start("Access",
                        (piParent)? piParent->ID() : 0,
                        ID(),
                        m_dwLanTaskCount,
                        &theEventProc,
                        0,
                        &theLogProc);
    TRACE_LOG(STR_FORMAT("Access Service Start Rc:0x%x!", dwRc));
}

/*******************************************************
  �� �� ��: CAccess::StopLanApp
  ��    ��: ֹͣ����Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CAccess::StopLanApp()
{
    if (!m_pLanApp)
    {
        return;
    }

    m_pLanApp->Stop();
    delete m_pLanApp;
    m_pLanApp = 0;

    TRACE_LOG("Access Network Service Stop!");
}

/*******************************************************
  �� �� ��: CAccess::CheckLogin
  ��    ��: ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CAccess::CheckLogin(DWORD dwRemoteIP, WORD wRemotePort, ISession::NODE &rSessNode)
{
    if (!m_piSession)
    {
        return FAILURE;
    }

    /// ���һỰ�Ƿ����
    DWORD dwRc = m_piSession->FindSession(dwRemoteIP, wRemotePort, rSessNode);
    if (dwRc == SUCCESS)
    {
        /// �Ѿ���ӵ��Ự����
        return SUCCESS;
    }

    /// ��ʱ���Ϊ������
    DWORD dwSessionID = 0;
    if (m_piSession->CreateSession(DCOP_USER_UNLOGIN, 
                        DCOP_GROUP_VISITOR, 
                        ID(), 
                        dwRemoteIP, 
                        wRemotePort, 
                        dwSessionID) != SUCCESS)
    {
        return FAILURE;
    }

    rSessNode.SessID = dwSessionID;
    rSessNode.UserID = DCOP_USER_UNLOGIN;
    rSessNode.TTY = ID();
    rSessNode.IP = dwRemoteIP;
    rSessNode.Port = wRemotePort;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CAccess::DispatchMsg
  ��    ��: �ַ���Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CAccess::DispatchMsg(objMsg *pMsg, ISession::NODE *pSessNode)
{
    if (!pMsg || !pSessNode || !m_piDispatch)
    {
        return FAILURE;
    }

    /// ��ȡ�Ựͷ
    DCOP_SESSION_HEAD *pSessHead = (DCOP_SESSION_HEAD *)pMsg->GetDataBuf();
    if (!pSessHead || 
        (pSessHead->m_type.m_headType != DCOP_MSG_HEAD_SESSION) || 
        (pSessHead->m_type.m_headSize != DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_SESSION]))
    {
        /// ������ȷ�ĻỰͷ
        return FAILURE;
    }

    /// ��ȡ�Ự����
    DWORD dwSessLen = pMsg->GetDataLen();
    DWORD dwPackLen = pSessHead->m_type.m_headSize + 
                        Bytes_GetWord((BYTE *)&(pSessHead->m_type.m_valueLen));
    if (dwSessLen < dwPackLen)
    {
        /// �Ự���Ȳ���
        return FAILURE;
    }

    /// ���ûỰ��Ϣ
    pSessHead->m_group = (BYTE)pSessNode->UserGroup;
    Bytes_SetDword((BYTE *)&(pSessHead->m_session), pSessNode->SessID);
    Bytes_SetDword((BYTE *)&(pSessHead->m_user), pSessNode->UserID);
    Bytes_SetDword((BYTE *)&(pSessHead->m_tty), pSessNode->TTY);

    /// ��Դ��ַ��Ϊ�Լ���Ȼ��ת����Ϣ
    pMsg->MsgHead().m_dwSrcID = ID();
    DWORD dwRc = FAILURE;
    DWORD dwHaveTryTimes = 0;
    while ((dwRc != SUCCESS) && (dwHaveTryTimes++ < SEND_TRY_TIMES))
    {
        if (m_pReqPool)
        {
            DCOP_SESSION_HEAD *pBuf = (DCOP_SESSION_HEAD *)pMsg->GetDataBuf();
            if (!pBuf)
            {
                return FAILURE;
            }

            if (pBuf->m_ack == DCOP_REQ)
            {
                dwRc = m_pReqPool->OnReq(pBuf, pMsg->GetMsgType(), pMsg->GetSrcID(), pMsg->GetDstID(), 
                        DCOP_MSG_OBJECT_RESPONSE, SEND_TIMEOUT, SEND_TRY_TIMES);
                if (dwRc)
                {
                    continue;
                }
            }
        }

        dwRc = m_piDispatch->Send(pMsg);
    }

    return dwRc;
}

