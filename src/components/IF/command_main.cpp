/// -------------------------------------------------
/// command_main.cpp : �����н���ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "command_main.h"
#include "Factory_if.h"
#include "BaseMessage.h"
#include "ObjAttribute_if.h"
#include "ObjTimer_if.h"
#include "md5/md5.h"
#include "task.h"
#include <time.h>


#define _COMMAND_DEBUG_ 0


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CCommand, "command")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CCommand)
    DCOP_IMPLEMENT_INTERFACE(ICommand)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CCommand)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
    DCOP_IMPLEMENT_CONFIG_INTEGER("telnetdport", m_wTelnetdPort)
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CCommand)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_LOAD, OnModuleLoad)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_UNLOAD, OnModuleUnload)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MODEL_REG, OnModelReg)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_SESSION_TIMEOUT, OnSessionTimeout)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_OBJECT_RESPONSE, OnResponse)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_OBJECT_EVENT, OnEvent)
    DCOP_IMPLEMENT_IOBJECT_MSG_DEFAULT(OnDefault)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END

/// -------------------------------------------------
/// ����ָ����б�
/// -------------------------------------------------
const char *CCommand::SplitCharList = "-:@$";

/// -------------------------------------------------
/// ����������Ͷ���(��DCOP_CTRL_TYPEһһ��Ӧ)
/// -------------------------------------------------
const char *CCommand::Operation[] = 
{
    "",
    "create",
    "destroy",
    "add",
    "del",
    "set",
    "get",
    "dump",
    "action",
    "event",
};

/// -------------------------------------------------
/// �����������ַ���
/// -------------------------------------------------
const char *CCommand::ArgOpCodeCharList = "+-*/=><!#";

/// -------------------------------------------------
/// �������������Ͷ���(��DCOP_OPCODE_TYPEһһ��Ӧ)
/// -------------------------------------------------
const char *CCommand::ArgOpCode[] = 
{
    "",
    "+",
    "-",
    "*",
    "/",
    "=",
    ">=",
    ">",
    "<",
    "<=",
    "!=",
    "#",
    "!#",
};


/*******************************************************
  �� �� ��: CCommand::CCommand
  ��    ��: CCommand���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CCommand::CCommand(Instance *piParent, int argc, char **argv)
{
    m_wTelnetdPort = TELNET_PORT;

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

    m_fnOut = 0;
    m_pPara = 0;

    (void)memset(m_szSysInfo, 0, sizeof(m_szSysInfo));
    (void)memset(m_szUserName, 0, sizeof(m_szUserName));
    m_pRspTable = NULL;

    m_pLanApp = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CCommand::~CCommand
  ��    ��: CCommand��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CCommand::~CCommand()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CCommand::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::Init(IObject *root, int argc, void **argv)
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

    /// ��ӹ���������
    AddModule(DCOP_OBJECT_MANAGER);

    /// ��ȡ��Ϣϵͳ
    const char *pszSysInfo = m_piManager->GetSystemInfo();
    if (!pszSysInfo || !(*pszSysInfo))
    {
        (void)snprintf(m_szSysInfo, sizeof(m_szSysInfo), "%d", m_piManager->GetSystemID());
    }
    else
    {
        (void)snprintf(m_szSysInfo, sizeof(m_szSysInfo), "%s/%d", pszSysInfo, m_piManager->GetSystemID());
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CCommand::Fini
  ��    ��: ����ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::Fini()
{
    OnFinish(NULL);

    DelModule(DCOP_OBJECT_MANAGER);

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
  �� �� ��: CCommand::Welcome
  ��    ��: ��ӭ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::Welcome(const char *username, DWORD session)
{
    SessionNode *pSessNode = NULL;

    AutoObjLock(this);

    if (session)
    {
        /// Զ�̵�½ʱ���һỰ
        pSessNode = FindSession(session);
        if (!pSessNode) return;

        /// ���δ�ɹ���½����Ҫ�����û������е�½
        if (pSessNode->m_byState != SESS_STATE_LOGIN_SUCCESS)
        {
            TextOut("Please input your username and password. \r\n", pSessNode);
            TextOut("username:", pSessNode);
            return;
        }

        /// ʹ���ѳɹ���½���û���
        username = pSessNode->m_szUserName;
    }
    else
    {
        if (!username) return;
        (void)snprintf(m_szUserName, sizeof(m_szUserName), "%s", username);
    }

    /// ��������ʾ��ӭ��Ϣ
    TextOut(STR_FORMAT("\r\n  %s, Welcome to the system! \r\n", username), pSessNode);
    TextOut("\r\nPlease input command and press <Enter> \r\n", pSessNode);
    TextOut("\r\n  ('?' or 'help' for help) \r\n", pSessNode);
    TextEnd(pSessNode);
}

/*******************************************************
  �� �� ��: CCommand::Line
  ��    ��: �����������ַ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::Line(const char *command, DWORD session)
{
    if (!command) return;

    SessionNode *pSessNode = NULL;

    AutoObjLock(this);

    /// ����Ǳ��ػỰ����Ҫ���н����������û�����У����
    if (session)
    {
        pSessNode = FindSession(session);
        if (!pSessNode)
        {
            return;
        }

        if (pSessNode->m_byState == SESS_STATE_INPUT_USERNAME)
        {
            InputUserName(*pSessNode, command);
            return;
        }

        if (pSessNode->m_byState == SESS_STATE_INPUT_PASSTEXT)
        {
            InputPassText(*pSessNode, command);
            return;
        }

        if (pSessNode->m_byState != SESS_STATE_LOGIN_SUCCESS)
        {
            return;
        }

        if (pSessNode->m_pRspTable != NULL)
        {
            return;
        }
    }
    else
    {
        if (m_pRspTable != NULL)
        {
            return;
        }
    }

    TextOut("\r\n", pSessNode);

    /// ����������������������
    DWORD dwRc = Analyze(command, pSessNode);
    if (dwRc != SUCCESS)
    {
        TextOut(STR_FORMAT("  ERROR! Analyze Failed(%d)! \r\n", dwRc), pSessNode);
        TextEnd(pSessNode);
    }
}

/*******************************************************
  �� �� ��: CCommand::Out
  ��    ��: �������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::Out(LOG_PRINT fnOut, LOG_PARA pPara)
{
    AutoObjLock(this);

    m_fnOut = (fnOut)? fnOut : PrintToConsole;
    m_pPara = pPara;
}

/*******************************************************
  �� �� ��: CCommand::Help
  ��    ��: ��ʾ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::Help(SessionNode *pSessNode)
{
    TextOut("<Operation>-<Attribute>: ReqArgList @CondArgList/$OnlyCondArgList <Enter> \r\n", pSessNode);
    TextOut("Example: \r\n", pSessNode);
    TextOut("  add-users: username=test,password=1234,usergroup=1 \r\n", pSessNode);
    TextOut("  del-users: @username=test \r\n", pSessNode);
    TextOut("  edit-users: info=abcd @userid=19 \r\n", pSessNode);
    TextOut("  get-users: @usergroup=1 \r\n", pSessNode);
    TextOut("  get-users \r\n", pSessNode);
    TextOut("  get-users: @usergroup=1,offset=1,limit=2 \r\n", pSessNode);
    TextOut("  get-users: @limit=2 \r\n", pSessNode);
    TextOut("  get-users: username,userid @usergroup=2,usergroup>=5 \r\n", pSessNode);
    TextOut("  count-users: @usergroup=1 \r\n", pSessNode);
    TextOut("  dump-data \r\n", pSessNode);
    TextOut("  dump \r\n", pSessNode);

    TextOut("<Operation> List: \r\n", pSessNode);
    for (DWORD i = 0; i < ARRAY_SIZE(Operation); ++i)
    {
        if ( !(*(Operation[i])) ) continue;
        TextOut(STR_FORMAT("  %s \r\n", Operation[i]), pSessNode);
    }

    TextOut("<Attribute> List: \r\n", pSessNode);
    for (IT_OBJATTRS it_attr = m_objattrs.begin();
        it_attr != m_objattrs.end(); ++it_attr)
    {
        IObject *pObj = NULL;
        if (m_piManager && m_piModel)
        {
            pObj = m_piManager->Child(m_piModel->GetObjID((*it_attr).second));
        }
        TextOut(STR_FORMAT("  %s (owner:'%s') \r\n", ((*it_attr).first).c_str(), ((pObj)? pObj->Name() : "none")), pSessNode);
    }

    TextEnd(pSessNode);
}

/*******************************************************
  �� �� ��: CCommand::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnStart(objMsg *msg)
{
    /// ģ������¼�
    OnModuleLoad(msg);

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
  �� �� ��: CCommand::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnFinish(objMsg *msg)
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
  �� �� ��: CCommand::OnModuleLoad
  ��    ��: ����ģ������¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnModuleLoad(objMsg *msg)
{
    if (!msg) return;

    DWORD *pdwPara = (DWORD *)msg->GetDataBuf();
    if (!pdwPara) return;

    for (DWORD i = 0; i < pdwPara[0]; ++i)
    {
        AddModule(pdwPara[i + 1]);
    }
}

/*******************************************************
  �� �� ��: CCommand::OnModuleUnload
  ��    ��: ж��ģ������¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnModuleUnload(objMsg *msg)
{
    if (!msg) return;

    DWORD *pdwPara = (DWORD *)msg->GetDataBuf();
    if (!pdwPara) return;

    for (DWORD i = 0; i < pdwPara[0]; ++i)
    {
        DelModule(pdwPara[i + 1]);
    }
}

/*******************************************************
  �� �� ��: CCommand::OnModelReg
  ��    ��: ģ��ע���¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnModelReg(objMsg *msg)
{
    if (!msg) return;

    DWORD *pdwPara = (DWORD *)msg->GetDataBuf();
    if (!pdwPara) return;

    for (DWORD i = 0; i < pdwPara[0]; ++i)
    {
        RegModel(pdwPara[i + 1]);
    }
}

/*******************************************************
  �� �� ��: CCommand::OnSessionTimeout
  ��    ��: �Ự��ʱ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnSessionTimeout(objMsg *msg)
{
    if (!msg) return;

    DWORD *pdwSessID = (DWORD *)msg->GetDataBuf();
    if (!pdwSessID) return;

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

    #if _COMMAND_DEBUG_
    PrintLog(STR_FORMAT("<Session(%d) Shutdown> %s:%d", pSessNode->m_dwSessID, 
                        pSock->cszGetHostIP(), pSock->wGetHostPort()), m_fnOut, m_pPara);
    #endif

    /// �ɷ����߳̽������Źرգ��Ժ���ɽ����߳��Լ��ر�
    (void)pSock->Shut();
}

/*******************************************************
  �� �� ��: CCommand::OnResponse
  ��    ��: ��Ӧ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnResponse(objMsg *msg)
{
    if (!msg) return;

    #if _COMMAND_DEBUG_
    PrintBuffer(STR_FORMAT("<Recv Ack Msg> len:%d type:0x%x src:%d, dst:%d", 
                        msg->GetDataLen(), msg->GetMsgType(), msg->GetSrcID(), msg->GetDstID()), 
                        msg->GetDataBuf(), msg->GetDataLen(), m_fnOut, m_pPara);
    #endif

    /// �����Ựͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(msg->GetDataBuf(), msg->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(0);
    if (!pSessionHead)
    {
        return;
    }

    /// ���ұ��ػỰ
    SessionNode *pSessNode = NULL;
    DWORD dwSessionID = pSessionHead->m_session;
    if (dwSessionID)
    {
        pSessNode = FindSession(dwSessionID);
        if (!pSessNode)
        {
            return;
        }

        if (pSessionHead->m_user != pSessNode->m_dwUserID)
        {
            return;
        }
    }
    else
    {
        /// �������󻺳�ؽ���Ԥ����
        if (m_pReqPool && (m_pReqPool->OnRsp((DCOP_SESSION_HEAD *)msg->GetDataBuf()) != SUCCESS))
        {
            return;
        }
    }

    /// ��ȡ�Ự����
    void *pSessionData = *(void **)(pSessionHead + 1);
    if (!pSessionData)
    {
        TextOut("  ERROR! Recv Null Session Head! \r\n", pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// ��ȡ��Ӧ����ͷ
    CDArray aRspHeads;
    IObjectMember::GetMsgHead(pSessionData, pSessionHead->m_type.m_valueLen, 0, 0, 0, &aRspHeads, 0);
    if (!aRspHeads.Count())
    {
        TextOut("  ERROR! Recv Null Response Count! \r\n", pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// ��Ӧ������ͷ������(��ʽ����)
    if (aRspHeads.Count() != pSessionHead->m_count)
    {
        TextOut("  ERROR! Recv Wrong Response Count! \r\n", pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// ��Ӧ�ֶ��ڵ�һ����Ϣ�ڵ���
    DCOP_RESPONSE_HEAD *pRspHead = (DCOP_RESPONSE_HEAD *)aRspHeads.Pos(0);
    if (!pRspHead)
    {
        TextOut("  ERROR! Recv Null Response Head! \r\n", pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// ʧ�ܵ���Ӧ
    if (pRspHead->m_retCode != SUCCESS)
    {
        TextOut(STR_FORMAT("  ERROR! Operate Failed(%d)! \r\n", pRspHead->m_retCode), pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// �ǲ�ѯ�����Ӧ
    if (!pRspHead->m_recordCount && (pSessionHead->m_ctrl != DCOP_CTRL_GET))
    {
        TextOut("  OK! \r\n", pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// ��ѯ��������Ӧ
    if (!pRspHead->m_paraCount)
    {
        TextOut(STR_FORMAT("  OK! %d Records! \r\n", pRspHead->m_recordCount), pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// ������Ӧ��������
    CDStream sRspPara(pRspHead->m_paraCount * sizeof(DCOP_PARA_NODE));
    DCOP_PARA_NODE *pRspPara = (DCOP_PARA_NODE *)sRspPara.Buffer();
    (void)IObjectMember::GetMsgParaData(*(void **)(pRspHead + 1), pRspHead->m_paraCount, pRspHead->m_paraLen, pRspPara);
    CTableString *pRspTable = NULL;
    if (!pRspHead->m_recordIndex)
    {
        pRspTable = new CTableString(pRspHead->m_paraCount, pRspHead->m_recordCount + 1, "  ");
        if (pSessNode)
        {
            if (pSessNode->m_pRspTable) delete pSessNode->m_pRspTable;
            pSessNode->m_pRspTable = pRspTable;
        }
        else
        {
            if (m_pRspTable) delete m_pRspTable;
            m_pRspTable = pRspTable;
        }
    }
    else
    {
        pRspTable = (pSessNode)? pSessNode->m_pRspTable : m_pRspTable;
    }

    /// ��Ӧ������ʧ��
    if (!pRspTable)
    {
        TextOut("  ERROR! No Rsp Table! \r\n");
        TextEnd(pSessNode);
        return;
    }

    CTableString &tableStr = *pRspTable;
    DWORD dwRc = SUCCESS;

    /// ������Ӧ����
    if (!pRspHead->m_recordIndex)
    {
        dwRc = GetRspTableTitle(tableStr, pSessionHead->m_attribute, pRspPara, pRspHead->m_paraCount);
        if (dwRc != SUCCESS)
        {
            TextOut(STR_FORMAT("  ERROR! Get Response Title Failed(%d)! \r\n", dwRc), pSessNode);
            TextEnd(pSessNode);
            return;
        }
    }

    /// ������Ӧ����
    dwRc = GetRspTableContent(tableStr, aRspHeads, pRspPara, pRspHead->m_paraCount);
    if (dwRc != SUCCESS)
    {
        TextOut(STR_FORMAT("  ERROR! Get Response Content Failed(%d)! \r\n", dwRc), pSessNode);
        TextEnd(pSessNode);
        return;
    }

    /// ��ʾ��Ӧ���ݺͽ�����־
    if (pSessionHead->m_ack == DCOP_RSP_END)
    {
        OutputPara outPara = {this, dwSessionID};
        tableStr.Show((LOG_PRINT)Output, (LOG_PARA)&outPara);
        TextOut(STR_FORMAT("  OK! %d Records! \r\n", pRspHead->m_recordCount), pSessNode);
        TextEnd(pSessNode);
    }
}

/*******************************************************
  �� �� ��: CCommand::OnEvent
  ��    ��: �¼���Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnEvent(objMsg *msg)
{
    if (!msg) return;

    #if _COMMAND_DEBUG_
    PrintBuffer(STR_FORMAT("<Recv Evt Msg> len:%d type:0x%x src:%d, dst:%d", 
                        msg->GetDataLen(), msg->GetMsgType(), msg->GetSrcID(), msg->GetDstID()), 
                        msg->GetDataBuf(), msg->GetDataLen(), m_fnOut, m_pPara);
    #endif

    /// �����Ựͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(msg->GetDataBuf(), msg->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(0);
    if (!pSessionHead)
    {
        return;
    }

    /// ��ȡ�ѽ����ĻỰͷ
    void *pSessionData = *(void **)(pSessionHead + 1);
    if (!pSessionData)
    {
        return;
    }

    /// ��ȡ�¼�����ͷ
    CDArray aEvtHeads;
    IObjectMember::GetMsgHead(pSessionData, pSessionHead->m_type.m_valueLen, 0, 0, 0, 0, &aEvtHeads);
    if (!aEvtHeads.Count())
    {
        return;
    }

    if (aEvtHeads.Count() != pSessionHead->m_count)
    {
        return;
    }

    /// �¼��ֶ��ڵ�һ����Ϣ�ڵ���
    DCOP_EVENT_HEAD *pEvtHead = (DCOP_EVENT_HEAD *)aEvtHeads.Pos(0);
    if (!pEvtHead)
    {
        return;
    }

    /// �����¼����ݸ�ʽ
    CDStream sEvtPara(pEvtHead->m_paraCount * sizeof(DCOP_PARA_NODE));
    DCOP_PARA_NODE *pEvtPara = (DCOP_PARA_NODE *)sEvtPara.Buffer();
    (void)IObjectMember::GetMsgParaData(*(void **)(pEvtHead + 1), pEvtHead->m_paraCount, pEvtHead->m_paraLen, pEvtPara);

    CTableString tableStr(pEvtHead->m_paraCount, pEvtHead->m_recordCount + 1, "  ");

    (void)GetRspTableTitle(tableStr, pSessionHead->m_attribute, pEvtPara, pEvtHead->m_paraCount);
    DWORD dwRc = GetRspTableContent(tableStr, aEvtHeads, pEvtPara, pEvtHead->m_paraCount);
    if (dwRc != SUCCESS)
    {
        return;
    }

    /// ��ȡ�¼�����
    const char *pcszOperName = (pSessionHead->m_ctrl <= DCOP_CTRL_EVENT)? Operation[pSessionHead->m_ctrl] : "null";
    const char *pcszAttrName = (m_piModel)? m_piModel->GetTableName(pSessionHead->m_attribute) : "null";
    const char *pcszUserName = m_szUserName;
    SessionNode *pSessNode = (pSessionHead->m_session != 0)? FindSession(pSessionHead->m_session) : NULL;
    if (pSessNode) pcszUserName = pSessNode->m_szUserName;
    if (!pSessionHead->m_session && !pSessionHead->m_user && !pSessionHead->m_tty) pcszUserName = "null";
    const char *pcszTermName = Name();
    IObject *piTerminal = (m_piManager)? m_piManager->Child(pSessionHead->m_tty) : NULL;
    if (piTerminal) pcszTermName = piTerminal->Name();
    if (!pSessionHead->m_session && !pSessionHead->m_user && !pSessionHead->m_tty) pcszTermName = "null";

    /// ��ʾ�¼�
    if (pSessionHead->m_session || pSessionHead->m_user || pSessionHead->m_tty)
    {
        for (IT_SESSIONS it_sess = m_sessions.begin();
            it_sess != m_sessions.end(); ++it_sess)
        {
            SessionNode *pSessNode = &((*it_sess).second);
            if (pSessNode->m_dwSessID == pSessionHead->m_session)
            {
                continue;
            }

            if (pSessNode->m_byState != SESS_STATE_LOGIN_SUCCESS)
            {
                continue;
            }

            TextOut("\r\n\r\n", pSessNode);
            OutputPara outPara = {this, pSessNode->m_dwSessID};
            tableStr.Show((LOG_PRINT)Output, (LOG_PARA)&outPara);
            TextOut(STR_FORMAT("  Event '%s-%s' by '%s' from '%s'! %d Records! \r\n", 
                            pcszOperName, pcszAttrName, 
                            pcszUserName, pcszTermName, 
                            pEvtHead->m_recordCount), pSessNode);
            TextEnd(pSessNode);
        }
    }

    if ((!pSessionHead->m_session && !pSessionHead->m_user && !pSessionHead->m_tty) || 
        (pSessionHead->m_session != 0))
    {
        TextOut("\r\n\r\n");
        OutputPara outPara = {this, 0};
        tableStr.Show((LOG_PRINT)Output, (LOG_PARA)&outPara);
        TextOut(STR_FORMAT("  Event '%s-%s' by '%s' from '%s'! %d Records! \r\n", 
                        pcszOperName, pcszAttrName, 
                        pcszUserName, pcszTermName, 
                        pEvtHead->m_recordCount));
        TextEnd();
    }
}

/*******************************************************
  �� �� ��: CCommand::OnDefault
  ��    ��: Ĭ����Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnDefault(objMsg *msg)
{
}

/*******************************************************
  �� �� ��: CCommand::AddModule
  ��    ��: ���ģ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::AddModule(DWORD dwModuleID)
{
    if (!m_piManager) return;

    IObject *pObject = m_piManager->Child(dwModuleID);
    if (!pObject)
    {
        return;
    }

    std::string strKey = pObject->Name();
    (void)m_modules.insert(MAP_MODULES::value_type(strKey, pObject));
}

/*******************************************************
  �� �� ��: CCommand::DelModule
  ��    ��: ɾ��ģ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::DelModule(DWORD dwModuleID)
{
    if (!m_piManager) return;

    IObject *pObject = m_piManager->Child(dwModuleID);
    if (!pObject)
    {
        return;
    }

    std::string strKey = pObject->Name();
    (void)m_modules.erase(strKey);
}

/*******************************************************
  �� �� ��: CCommand::RegModel
  ��    ��: ע��ģ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::RegModel(DWORD dwAttrID)
{
    if (!m_piModel) return;

    std::string strKey = m_piModel->GetTableName(dwAttrID);
    (void)m_objattrs.insert(MAP_OBJATTRS::value_type(strKey, dwAttrID));
}

/*******************************************************
  �� �� ��: CCommand::GetModule
  ��    ��: ��ȡģ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IObject *CCommand::GetModule(const char *cpszModName)
{
    if (!cpszModName) return NULL;

    std::string strKey = cpszModName;
    IT_MODULES it_module = m_modules.find(strKey);
    if (it_module == m_modules.end())
    {
        return NULL;
    }

    return (*it_module).second;
}

/*******************************************************
  �� �� ��: CCommand::GetOperation
  ��    ��: ��ȡ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
BYTE CCommand::GetOperation(const char *cpszOpName)
{
    if (!cpszOpName) return DCOP_CTRL_NULL;

    for (DWORD i = 0; i < ARRAY_SIZE(Operation); ++i)
    {
        if (!stricmp(Operation[i], cpszOpName))
        {
            return (BYTE)i;
        }
    }

    return DCOP_CTRL_NULL;
}

/*******************************************************
  �� �� ��: CCommand::GetAttribute
  ��    ��: ��ȡ����ID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::GetAttribute(const char *cpszAttrName)
{
    if (!cpszAttrName) return 0;

    std::string strKey = cpszAttrName;
    IT_OBJATTRS it_attr = m_objattrs.find(strKey);
    if (it_attr == m_objattrs.end())
    {
        return 0;
    }

    return (*it_attr).second;
}

/*******************************************************
  �� �� ��: CCommand::GetArgList
  ��    ��: ��ȡ�����б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DCOP_PARA_NODE *CCommand::GetArgList(const CDArray &strArgList, IModel::Field *pField, DWORD dwFieldCount, 
                        DWORD &rdwParaCount, CDStream &rsParaData, bool bOneValue, IModel *piModel, DWORD dwAttrID)
{
    DWORD dwArgCount = strArgList.Count();
    if (!dwArgCount)
    {
        return NULL;
    }

    /// ��������ڵ�
    DCOP_PARA_NODE *pParaNode = (DCOP_PARA_NODE *)DCOP_Malloc(dwArgCount * sizeof(DCOP_PARA_NODE));
    if (!pParaNode)
    {
        return NULL;
    }

    (void)memset(pParaNode, 0, dwArgCount * sizeof(DCOP_PARA_NODE));

    /// ��������ÿ������
    DWORD dwParaCount = 0;
    bool bSpec = false;
    for (DWORD i = 0; i < dwArgCount; ++i)
    {
        CDString *pStr = (CDString *)strArgList.Pos(i);
        if (!pStr) break;

        /// ��ȡ�ֶε�����ֵ
        CDString *pStrName = 0;
        CDString *pStrValue = 0;
        CDArray aNameValue;
        pStr->Split(ArgOpCodeCharList, aNameValue);
        DWORD dwNameValueCount = aNameValue.Count();
        if (dwNameValueCount > 1)
        {
            pStrName = (CDString *)aNameValue.Pos(0);
            pStrValue = (CDString *)aNameValue.Pos(dwNameValueCount - 1);
            pParaNode[i].m_opCode = GetArgOpCode(aNameValue);
        }
        else if (dwNameValueCount == 1)
        {
            if (bOneValue)
            {
                pStrValue = (CDString *)aNameValue.Pos(0);
            }
            else
            {
                pStrName = (CDString *)aNameValue.Pos(0);
            }
            pParaNode[i].m_opCode = DCOP_OPCODE_NONE;
        }
        else
        {
            break;
        }

        /// ��ȡ��Ӧ���ֶ���
        if (!pStrName || !pStrName->Length())
        {
            if (bSpec) break; // ָ���ֶεĲ���ֻ�ܳ��������Ҳ�
            bSpec = false;
            pParaNode[i].m_paraID = GetFieldID(pField, dwFieldCount, i, 
                        pParaNode[i].m_paraType, pParaNode[i].m_paraSize);
            if (!pParaNode[i].m_paraID) break;
        }
        else
        {
            bSpec = true;
            pStrName->Trim(" \r\n\t\"\'");
            pParaNode[i].m_paraID = GetFieldID(pField, dwFieldCount, *pStrName, 
                        pParaNode[i].m_paraType, pParaNode[i].m_paraSize, piModel, dwAttrID);
            if (!pParaNode[i].m_paraID) break;
        }

        /// ��ȡ��Ӧ���ֶ�ֵ
        if (pStrValue && pStrValue->Length())
        {
            pStrValue->Trim(" \r\n\t\"\'");
            if (GetFieldValue(pParaNode[i].m_paraType, pParaNode[i].m_paraSize, 
                            *pStrValue, rsParaData) != SUCCESS)
            {
                break;
            }
        }

        ++dwParaCount;
    }

    if (dwParaCount != dwArgCount)
    {
        DCOP_Free(pParaNode);
        return NULL;
    }

    rdwParaCount = dwParaCount;
    return pParaNode;
}

/*******************************************************
  �� �� ��: CCommand::GetArgOpCode
  ��    ��: ��ȡ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
BYTE CCommand::GetArgOpCode(const CDArray &aNameValue)
{
    DWORD dwNameValueCount = aNameValue.Count();
    if ((dwNameValueCount <= 1) ||
        (dwNameValueCount >= 4)) // Ŀǰ���ֻ�������ַ���ɵĲ�����
    {
        return DCOP_OPCODE_NONE;
    }

    char szOpCode[4];
    DWORD dwOffset = 0;
    for (DWORD i = 0; i < (dwNameValueCount - 1); ++i)
    {
        CDString *pStr = (CDString *)aNameValue.Pos(i);
        if (!pStr)
        {
            return DCOP_OPCODE_NONE;
        }

        char ch = *(char *)(pStr + 1);
        if (strchr(ArgOpCodeCharList, ch) == NULL)
        {
            return DCOP_OPCODE_NONE;
        }

        szOpCode[dwOffset++] = ch;
    }

    if (!dwOffset) return DCOP_OPCODE_NONE;

    szOpCode[dwOffset] = 0;
    for (DWORD i = 0; i < ARRAY_SIZE(ArgOpCode); ++i)
    {
        if (!stricmp(ArgOpCode[i], szOpCode))
        {
            return (BYTE)i;
        }
    }

    return DCOP_OPCODE_NONE;
}

/*******************************************************
  �� �� ��: CCommand::GetFieldID
  ��    ��: ��ȡ�ֶ�ID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::GetFieldID(IModel::Field *pField, DWORD dwFieldCount, const char *cpszFieldName, 
                        BYTE &rbyParaType, WORD &rwParaSize, IModel *piModel, DWORD dwAttrID)
{
    if (!pField || !dwFieldCount) return 0;

    CDString str(cpszFieldName);
    if (!str.Length())
    {
        return 0;
    }

    /// �ָ�������ֶ���(��ʽ:'����.�ֶ���')
    CDArray strList;
    str.Split(".", strList);
    if (!strList.Count() || (strList.Count() > 2))
    {
        return 0;
    }

    /// ��ȡ�������ֶ���
    CDString *pStrTableName = NULL;
    CDString *pStrFieldName = NULL;
    if (strList.Count() == 1)
    {
        pStrFieldName = (CDString *)strList.Pos(0);
    }
    else
    {
        pStrTableName = (CDString *)strList.Pos(0);
        pStrFieldName = (CDString *)strList.Pos(1);
    }

    /// �ֶ�������Ϊ��
    if (!pStrFieldName || !pStrFieldName->Length())
    {
        return 0;
    }

    /// ���ҹ������ֶ�ID
    if (pStrTableName)
    {
        if (!pStrTableName->Length())
        {
            return 0;
        }

        if (!piModel || !dwAttrID)
        {
            return 0;
        }

        DWORD dwShipCount = 0;
        IModel::Relation *pShip = piModel->GetShips(dwAttrID, dwShipCount);
        if (!pShip || !dwShipCount)
        {
            return 0;
        }

        IModel::Relation *pShipOne = NULL;
        for (DWORD i = 0; i < dwShipCount; ++i)
        {
            const char *pcszRelTableName = piModel->GetTableName(pShip[i].m_attrID);
            if (!pcszRelTableName)
            {
                continue;
            }

            if (!stricmp(pcszRelTableName, *pStrTableName))
            {
                pShipOne = &(pShip[i]);
                break;
            }
        }

        if (!pShipOne)
        {
            return 0;
        }

        DWORD dwRelFieldCount = 0;
        IModel::Field *pRelField = piModel->GetFields(pShipOne->m_attrID, dwRelFieldCount);
        if (!pRelField || !dwRelFieldCount)
        {
            return 0;
        }

        DWORD dwRelFieldID = GetFieldID(pRelField, dwRelFieldCount, *pStrFieldName, 
                        rbyParaType, rwParaSize);
        if (!dwRelFieldID)
        {
            return 0;
        }

        return  (DCOP_FIELD_HIGH & (DCOP_FIELD_RELATION << 16)) |
                (DCOP_FIELD_LOW1 & (pShipOne->m_relID) << 8) |
                (DCOP_FIELD_LOW0 & dwRelFieldID);
    }

    /// ���������ֶ�
    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        /// ��ȡһ���ֶ�
        if (!stricmp(pField[i].m_fieldName, *pStrFieldName))
        {
            rbyParaType = pField[i].m_fieldType;
            rwParaSize  = pField[i].m_fieldSize;
            return i + 1;
        }

        /// ��ȡ�����ֶ� - count
        if (!stricmp("count", *pStrFieldName))
        {
            rbyParaType = IModel::FIELD_DWORD;
            rwParaSize  = (WORD)sizeof(DWORD);
            return DCOP_SPECPARA_COUNT;
        }

        /// ��ȡ�����ֶ� - offset
        if (!stricmp("offset", *pStrFieldName))
        {
            rbyParaType = IModel::FIELD_DWORD;
            rwParaSize  = (WORD)sizeof(DWORD);
            return DCOP_SPECPARA_OFFSET;
        }

        /// ��ȡ�����ֶ� - limit
        if (!stricmp("limit", *pStrFieldName))
        {
            rbyParaType = IModel::FIELD_DWORD;
            rwParaSize  = (WORD)sizeof(DWORD);
            return DCOP_SPECPARA_LIMIT;
        }
    }

    return 0;
}

/*******************************************************
  �� �� ��: CCommand::GetFieldID
  ��    ��: ��ȡ�ֶ�ID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::GetFieldID(IModel::Field *pField, DWORD dwFieldCount, DWORD dwFieldIdx, BYTE &rbyParaType, WORD &rwParaSize)
{
    if (!pField || !dwFieldCount) return 0;

    if (dwFieldIdx >= dwFieldCount) return 0;

    rbyParaType = pField[dwFieldIdx].m_fieldType;
    rwParaSize  = pField[dwFieldIdx].m_fieldSize;

    return dwFieldIdx + 1;
}

/*******************************************************
  �� �� ��: CCommand::GetFieldValue
  ��    ��: ��ȡ�ֶ�ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::GetFieldValue(BYTE &rbyParaType, WORD &rwParaSize, const char *cpszParaValue, CDStream &rsParaData)
{
    if (!cpszParaValue) return FAILURE;

    DWORD dwRc = SUCCESS;
    switch (rbyParaType)
    {
        case IModel::FIELD_IDENTIFY:
        case IModel::FIELD_BYTE:
        case IModel::FIELD_WORD:
        case IModel::FIELD_DWORD:
        case IModel::FIELD_SHORT:
        case IModel::FIELD_INTEGER:
        case IModel::FIELD_NUMBER:
        {
            if (rwParaSize == 1)
            {
                BYTE byValue = (BYTE)atoi(cpszParaValue);
                rsParaData << byValue;
            }
            else if (rwParaSize == 2)
            {
                WORD wValue = (WORD)atoi(cpszParaValue);
                rsParaData << wValue;
            }
            else
            {
                DWORD dwValue = (DWORD)atoi(cpszParaValue);
                rsParaData << dwValue;
            }
        }
            break;
        case IModel::FIELD_CHAR:
        {
            BYTE byValue = (BYTE)*cpszParaValue;
            rsParaData << byValue;
        }
            break;
        case IModel::FIELD_STRING:
        {
            CDString strValue = cpszParaValue;
            strValue.Trim(" \r\n\t\"\'");
            if (strValue.Length() >= rwParaSize)
            {
                dwRc = FAILURE;
                break;
            }
            rsParaData << strValue;
            rwParaSize = (WORD)strValue.Length();
        }
            break;
        case IModel::FIELD_BUFFER:
        case IModel::FIELD_PTR:
        case IModel::FIELD_TIMER:
        {
            const char *pszStrTmp = cpszParaValue;
            if ((pszStrTmp[0] == '0') && (pszStrTmp[0] == 'x')) pszStrTmp += 2;
            CBufferPara bufPara(pszStrTmp, "%02x", 2);
            if (bufPara.GetBufLen() > rwParaSize)
            {
                dwRc = FAILURE;
                break;
            }
            rsParaData << bufPara;
            rwParaSize = (WORD)bufPara.GetBufLen();
        }
            break;
        case IModel::FIELD_DATE:
            break;
        case IModel::FIELD_TIME:
            break;
        case IModel::FIELD_IP:
        {
            DWORD dwIP = objSock::GetIPValueByString(cpszParaValue);
            dwIP = Bytes_GetDword((BYTE *)&dwIP);
            rsParaData << dwIP;
        }
            break;
        case IModel::FIELD_PASS:
        {
            objRandom *pRand = DCOP_CreateRandom();
            if (!pRand)
            {
                dwRc = FAILURE;
                break;
            }

            char salt[16];
            pRand->Gen(salt, sizeof(salt));
            delete pRand;

            MD5_CTX md5;
            char digest[16];
            (void)memset(digest, 0, 16);
            MD5Init(&md5);
            MD5Update(&md5, (unsigned  char *)salt, (unsigned int)sizeof(salt));
            MD5Update(&md5, (unsigned  char *)cpszParaValue, (unsigned int)strlen((char *)cpszParaValue));
            MD5Final(&md5, (unsigned  char *)digest);
            rsParaData << CBufferPara((void *)salt, sizeof(salt));
            rsParaData << CBufferPara((void *)digest, sizeof(digest));
        }
            break;
        default:
            break;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CCommand::GetRelFieldName
  ��    ��: ��ȡ�����ֶ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
const char *CCommand::GetRelFieldName(IModel *piModel, DWORD dwAttrID, DWORD dwFieldID, DWORD dwFieldCount)
{
    DWORD dwShipCount = 0;
    IModel::Relation *pShip = piModel->GetShips(dwAttrID, dwShipCount);
    if (!pShip || !dwShipCount)
    {
        return NULL;
    }

    DWORD dwLocalFieldID = (DWORD)((dwFieldID & DCOP_FIELD_LOW1) >> 8);
    if (!dwLocalFieldID || (dwLocalFieldID > dwFieldCount))
    {
        return NULL;
    }

    IModel::Relation *pShipOne = NULL;
    for (DWORD i = 0; i < dwShipCount; ++i)
    {
        if (pShip[i].m_relID == dwLocalFieldID)
        {
            pShipOne = &(pShip[i]);
            break;
        }
    }

    if (!pShipOne)
    {
        return NULL;
    }

    DWORD dwRelFieldCount = 0;
    IModel::Field *pRelField = piModel->GetFields(pShipOne->m_attrID, dwRelFieldCount);
    if (!pRelField || !dwRelFieldCount)
    {
        return NULL;
    }

    DWORD dwRelFieldID = (DWORD)(dwFieldID & DCOP_FIELD_LOW0);
    if (!dwRelFieldID || (dwRelFieldID > dwRelFieldCount))
    {
        return NULL;
    }

    return pRelField[dwRelFieldID - 1].m_fieldName;
}

/*******************************************************
  �� �� ��: CCommand::Analyze
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::Analyze(const char *command, SessionNode *pSessNode)
{
    CDString str(command);
    str.Trim(" \r\n\t;");
    if (!str.Length())
    {
        TextEnd(pSessNode);
        return SUCCESS;
    }

    CDArray strList;
    str.Split(SplitCharList, strList);
    if (!strList.Count())
    {
        return FAILURE;
    }

    /// ��ȡ��������
    CDString *pStr = (CDString *)strList.Pos(ELEMENT_OPERATION);
    if (!pStr) return FAILURE;
    if ((*pStr == "?") || (*pStr == "help"))
    {
        Help(pSessNode);
        return SUCCESS;
    }

    BYTE byOpType = GetOperation(*pStr);
    if (byOpType == DCOP_CTRL_NULL)
    {
        return FAILURE;
    }

    /// ��Dump�����������⴦��
    if (byOpType == DCOP_CTRL_DUMP)
    {
        DumpModObj(strList, pSessNode);
        return SUCCESS;
    }

    /// �������ͺ���ķָ���������'-'
    char ch = *(char *)(pStr + 1);
    if (ch != '-')
    {
        return FAILURE;
    }

    /// ��ȡ��������
    pStr = (CDString *)strList.Pos(ELEMENT_ATTRIBUTE);
    if (!pStr) return FAILURE;
    DWORD dwAttrID = GetAttribute(*pStr);
    if (!dwAttrID)
    {
        return FAILURE;
    }

    if (!m_piModel) return FAILURE;
    DWORD dwFieldCount = 0;
    IModel::Field *pField = m_piModel->GetFields(dwAttrID, dwFieldCount);
    if (!pField || !dwFieldCount)
    {
        return FAILURE;
    }

    /// ��ȡ����ID
    DWORD dwObjID = m_piModel->GetObjID(dwAttrID);

    /// ��ȡ�������
    DCOP_PARA_NODE *pReqParaNode = 0;
    DWORD dwReqParaCount = 0;
    CDStream sReqParaData;
    ch = *(char *)(pStr + 1);
    pStr = (CDString *)strList.Pos(ELEMENT_REQ_ARG_LIST);
    if (pStr) pStr->Trim(" \r\n\t");
    if (pStr && pStr->Length())
    {
        /// ���������ǰ�������':'�ָ�
        if (ch != ':') return FAILURE;

        /// ��ѯ�����£����������Ψһֵ����Ϊ�ֶ���
        bool bOneValue = (byOpType != DCOP_CTRL_GET)? true : false;

        CDArray strReqArgList;
        pStr->Split(",", strReqArgList);

        /// ��ȡ��������б�
        pReqParaNode = GetArgList(strReqArgList, pField, dwFieldCount, dwReqParaCount, sReqParaData, 
                        bOneValue, m_piModel, dwAttrID);
        if (!pReqParaNode) return FAILURE;
    }

    /// ��ȡ��������
    DCOP_PARA_NODE *pCondParaNode = 0;
    DWORD dwCondParaCount = 0;
    CDStream sCondParaData;
    BYTE condition = DCOP_CONDITION_ANY;
    ch = (!pStr)? '\0' : *(char *)(pStr + 1);
    pStr = (CDString *)strList.Pos(ELEMENT_COND_ARG_LIST);
    if (pStr) pStr->Trim(" \r\n\t");
    if (pStr && pStr->Length())
    {
        /// ����������ǰ�������'@/$'�ָ�
        /// "@����"����������������"$����"�ǹؼ���ƥ���Ψһ����
        if ((ch != '@') && (ch != '$'))
        {
            if (pReqParaNode) DCOP_Free(pReqParaNode);
            return FAILURE;
        }

        if (ch == '$') condition = DCOP_CONDITION_ONE;
        CDArray strCondArgList;
        pStr->Split(",", strCondArgList);

        /// ��ȡ���������б�
        pCondParaNode = GetArgList(strCondArgList, pField, dwFieldCount, dwCondParaCount, sCondParaData);
        if (!pCondParaNode)
        {
            if (pReqParaNode) DCOP_Free(pReqParaNode);
            return FAILURE;
        }
    }

    /// ��װ�Ự��Ϣ
    CAttribute::PACK_SESS_HEAD sessHead;
    sessHead.m_group = (pSessNode)? (BYTE)pSessNode->m_dwUserGroup : DCOP_GROUP_VISITOR;
    sessHead.m_session = (pSessNode)? pSessNode->m_dwSessID : 0;
    sessHead.m_user = (pSessNode)? pSessNode->m_dwUserID : DCOP_USER_UNLOGIN;
    sessHead.m_tty = ID();
    sessHead.m_attribute = dwAttrID;
    sessHead.m_ctrl = byOpType;

    CAttribute::PACK_REQ_HEAD reqHead;
    reqHead.m_paraCount = (WORD)dwReqParaCount;
    reqHead.m_paraLen = (WORD)sReqParaData.Length();

    CAttribute::PACK_COND_HEAD condHead;
    condHead.m_condition = condition;
    condHead.m_paraCount = (BYTE)dwCondParaCount;
    condHead.m_paraLen = (WORD)sCondParaData.Length();

    CAttribute::PACK_MSG_NODE packNodes[] = 
    {
        {(DCOP_MSG_HEAD *)&condHead, pCondParaNode, dwCondParaCount, sCondParaData},
        {(DCOP_MSG_HEAD *)&reqHead, pReqParaNode, dwReqParaCount, sReqParaData},
    };

    DWORD dwRc = SUCCESS;
    do
    {
        if (!pSessNode)
        {
            dwRc = IObjectMember::PackMsg(m_piDispatch, NULL, ID(), dwObjID, DCOP_MSG_OBJECT_REQUEST, 
                        &sessHead, packNodes, ARRAY_SIZE(packNodes), m_pReqPool, DCOP_MSG_OBJECT_RESPONSE);
            break;
        }

        if (!pSessNode->m_pSock || !m_piAccess)
        {
            dwRc = FAILURE;
            break;
        }

        objMsg *pMsg = NULL;
        dwRc = IObjectMember::PackMsg(NULL, &pMsg, ID(), dwObjID, DCOP_MSG_OBJECT_REQUEST, 
                        &sessHead, packNodes, ARRAY_SIZE(packNodes));
        if (dwRc != SUCCESS)
        {
            break;
        }

        if (!pMsg)
        {
            dwRc = FAILURE;
            break;
        }

        dwRc = m_piAccess->Input(pMsg, 
                        objSock::GetIPValueByString(pSessNode->m_pSock->cszGetHostIP()), 
                        pSessNode->m_pSock->wGetHostPort());
        if (dwRc != SUCCESS)
        {
            delete pMsg;
        }
    } while (0);

    if (pCondParaNode) DCOP_Free(pCondParaNode);
    if (pReqParaNode) DCOP_Free(pReqParaNode);
    return dwRc;
}

/*******************************************************
  �� �� ��: CCommand::DumpModObj
  ��    ��: Dumpģ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::DumpModObj(const CDArray &strList, SessionNode *pSessNode)
{
    OutputPara outPara = {this, (pSessNode)? pSessNode->m_dwSessID : 0};

    IObject *pModObj = NULL;
    CDString *pModStr = (CDString *)strList.Pos(ELEMENT_ATTRIBUTE);
    if (pModStr) pModObj = GetModule(*pModStr);

    /// ��ӡ�ص��Ͳ����п��ܱ�Dump��������(�������ط����д�ӡ)
    /// ������������һ��������ʾlogPara�ĳ���(logparalen)
    CDString strArg(STR_FORMAT("logparalen=%d,", sizeof(OutputPara)));
    CDArray strArgList;
    CDString *pArgStr = (CDString *)strList.Pos(ELEMENT_REQ_ARG_LIST);
    if (pArgStr)
    {
        pArgStr->Trim(" \r\n\t\"\'");
        strArg << (const char *)(*pArgStr);
    }

    /// �Ѳ�����ֳɶ���ַ����б�
    strArg.Split(" ,=", strArgList, false);

    /// ģ�鲻Ϊ�գ���Dumpָ��ģ�����
    if (pModObj)
    {
        pModObj->Dump((LOG_PRINT)Output, (LOG_PARA)&outPara, 
                        (int)strArgList.Count(), (void **)strArgList.Get());
        TextEnd(pSessNode);
        return;
    }

    /// ģ��Ϊ�գ���Dump�๤���͹�����
    IFactory *piFactory = IFactory::GetInstance();
    if (piFactory)
    {
        piFactory->Dump((LOG_PRINT)Output, (LOG_PARA)&outPara, 
                        (int)strArgList.Count(), (void **)strArgList.Get());
        (void)piFactory->Release();
    }

    if (m_piManager)
    {
        m_piManager->Dump((LOG_PRINT)Output, (LOG_PARA)&outPara, 
                        (int)strArgList.Count(), (void **)strArgList.Get());
    }

    objBase *pBase = objBase::GetInstance();
    if (pBase)
    {
        pBase->Dump((LOG_PRINT)Output, (LOG_PARA)&outPara, 
                        (int)strArgList.Count(), (void **)strArgList.Get());
    }

    TextEnd(pSessNode);
}

/*******************************************************
  �� �� ��: CCommand::GetRspTableTitle
  ��    ��: ��ȡ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::GetRspTableTitle(CTableString &tableStr, DWORD dwAttrID, DCOP_PARA_NODE *pPara, DWORD dwParaCount)
{
    if (!pPara || !dwParaCount)
    {
        return FAILURE;
    }

    if (!m_piModel)
    {
        return FAILURE;
    }

    DWORD dwFieldCount = 0;
    IModel::Field *pField = m_piModel->GetFields(dwAttrID, dwFieldCount);
    if (!pField || !dwFieldCount)
    {
        return FAILURE;
    }

    for (DWORD i = 0; i < dwParaCount; ++i)
    {
        DWORD dwFieldID = pPara[i].m_paraID;
        if (DCOP_SPECPARA(dwFieldID, DCOP_FIELD_RELATION))
        {
            const char *pRelFieldName = GetRelFieldName(m_piModel, dwAttrID, dwFieldID, dwFieldCount);
            if (!pRelFieldName)
            {
                return FAILURE;
            }

            tableStr << pRelFieldName;
            continue;
        }

        if (!dwFieldID || (dwFieldID > dwFieldCount))
        {
            return FAILURE;
        }

        tableStr << pField[dwFieldID - 1].m_fieldName;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CCommand::GetRspTableContent
  ��    ��: ��ȡ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::GetRspTableContent(CTableString &tableStr, const CDArray &aRspHeads, 
                        DCOP_PARA_NODE *pPara, DWORD dwParaCount)
{
    for (DWORD i = 0; i < aRspHeads.Count(); ++i)
    {
        DCOP_MSG_HEAD *pMsgHead = (DCOP_MSG_HEAD *)aRspHeads.Pos(i);
        if (!pMsgHead) return FAILURE;

        DWORD dwMsgParaCount = 0;
        DWORD dwMsgDataLen = 0;
        if (pMsgHead->m_headType == DCOP_MSG_HEAD_RESPONSE)
        {
            DCOP_RESPONSE_HEAD *pRspHead = (DCOP_RESPONSE_HEAD *)pMsgHead;
            dwMsgParaCount = pRspHead->m_paraCount;
            dwMsgDataLen = pRspHead->m_paraLen;
        }
        else if (pMsgHead->m_headType == DCOP_MSG_HEAD_EVENT)
        {
            DCOP_EVENT_HEAD *pEvtHead = (DCOP_EVENT_HEAD *)pMsgHead;
            dwMsgParaCount = pEvtHead->m_paraCount;
            dwMsgDataLen = pEvtHead->m_paraLen;
        }
        else
        {
            return FAILURE;
        }

        void *pMsgData = IObjectMember::GetMsgParaData(
                        *(void **)((BYTE *)pMsgHead + pMsgHead->m_headSize), 
                        dwMsgParaCount, dwMsgDataLen);
        if (!pMsgData) return FAILURE;

        DWORD dwRc = GetRspTableLine(tableStr, pPara, dwParaCount, pMsgData, dwMsgDataLen);
        if (dwRc != SUCCESS) return dwRc;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CCommand::GetRspTableLine
  ��    ��: ��ȡ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::GetRspTableLine(CTableString &tableStr, DCOP_PARA_NODE *pPara, DWORD dwParaCount, void *pData, DWORD dwDataLen)
{
    if (!pPara || !dwParaCount || !pData || !dwDataLen) return FAILURE;

    BYTE *pbyRec = (BYTE *)pData;
    DWORD dwOffset = 0;
    for (DWORD i = 0; i < dwParaCount; ++i)
    {
        switch (pPara[i].m_paraType)
        {
            case IModel::FIELD_BYTE:
                tableStr << STR_FORMAT("0x%02x", *(pbyRec + dwOffset));
                break;
            case IModel::FIELD_WORD:
                tableStr << STR_FORMAT("0x%04x", Bytes_GetWord(pbyRec + dwOffset));
                break;
            case IModel::FIELD_DWORD:
                tableStr << STR_FORMAT("0x%08lx", Bytes_GetDword(pbyRec + dwOffset));
                break;
            case IModel::FIELD_CHAR:
                tableStr << STR_FORMAT("%c", *(char *)(pbyRec + dwOffset));
                break;
            case IModel::FIELD_SHORT:
            case IModel::FIELD_INTEGER:
                tableStr << STR_FORMAT("%d", (int)Bytes_GetDwordValue(pbyRec + dwOffset, pPara[i].m_paraSize));
                break;
            case IModel::FIELD_IDENTIFY:
            case IModel::FIELD_NUMBER:
                tableStr << STR_FORMAT("%d", Bytes_GetDwordValue(pbyRec + dwOffset, pPara[i].m_paraSize));
                break;
            case IModel::FIELD_STRING:
            {
                CDString strTmp((char *)pbyRec + dwOffset, pPara[i].m_paraSize);
                tableStr << (const char *)strTmp;
            }
                break;
            case IModel::FIELD_BUFFER:
                tableStr << CBufferString(pbyRec + dwOffset, pPara[i].m_paraSize);
                break;
            case IModel::FIELD_DATE:
                break;
            case IModel::FIELD_TIME:
                break;
            case IModel::FIELD_IP:
            {
                char szIP[OSSOCK_IPSIZE];
                (void)memset(szIP, 0, sizeof(szIP));
                objSock::GetIPStringByValue(*(DWORD *)(pbyRec + dwOffset), szIP);
                tableStr << szIP;
            }
                break;
            case IModel::FIELD_PTR:
                tableStr << STR_FORMAT("%p", *(void **)(pbyRec + dwOffset));
                break;
            case IModel::FIELD_TIMER:
            {
                CDString strTimer;
                ITimer::IWheel::GetString((ITimer::Handle)(pbyRec + dwOffset), strTimer);
                tableStr << (const char *)strTimer;
            }
                break;
            case IModel::FIELD_PASS:
                tableStr << "********";
                break;
            default:
                break;
        }

        dwOffset += pPara[i].m_paraSize;
        if (dwOffset >= dwDataLen)
        {
            break;
        }
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CCommand::GetDateTime
  ��    ��: �ı��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::GetDateTime(char *szStr, int strLen)
{
    struct tm *newtime;
    time_t ltime;

    time(&ltime);
    newtime = localtime(&ltime);

    (void)snprintf(szStr, strLen, "[%02d-%02d %02d:%02d:%02d] ", 
        newtime->tm_mon + 1, newtime->tm_mday, 
        newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
}

/*******************************************************
  �� �� ��: CCommand::TextOut
  ��    ��: �ı����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::TextOut(const char *text, SessionNode *pSessNode)
{
    if (!text) return;

    if (!pSessNode)
    {
        if (!m_fnOut) m_fnOut = PrintToConsole;
        m_fnOut(text, m_pPara);
        return;
    }

    if (!pSessNode->m_pSock)
    {
        return;
    }

    (void)pSessNode->m_pSock->Send(text, (DWORD)strlen(text));
}

/*******************************************************
  �� �� ��: CCommand::TextEnd
  ��    ��: �ı��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::TextEnd(SessionNode *pSessNode)
{
    char szDateTime[18];
    GetDateTime(szDateTime, sizeof(szDateTime));

    if (!pSessNode)
    {
        if (m_pRspTable)
        {
            delete m_pRspTable;
            m_pRspTable = NULL;
        }

        CStrFormat strLocal("\r\n%s[%s@%s] # ", szDateTime, m_szUserName, m_szSysInfo);
        if (!m_fnOut) m_fnOut = PrintToConsole;
        m_fnOut((const char *)strLocal, m_pPara);

        return;
    }

    if (!pSessNode->m_pSock)
    {
        return;
    }

    if (pSessNode->m_byState != SESS_STATE_LOGIN_SUCCESS)
    {
        return;
    }

    if (pSessNode->m_pRspTable != NULL)
    {
        delete pSessNode->m_pRspTable;
        pSessNode->m_pRspTable = NULL;
    }

    CStrFormat strRemote("\r\n%s[%s@%s] # ", szDateTime, pSessNode->m_szUserName, m_szSysInfo);
    (void)pSessNode->m_pSock->Send((const char *)strRemote, (DWORD)strlen((const char *)strRemote));
}

/*******************************************************
  �� �� ��: CCommand::StartLanApp
  ��    ��: ��������Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::StartLanApp()
{
    TRACE_LOG("Command Network Service Start ...");

    m_pLanApp = objLanApp::CreateInstance();
    if (!m_pLanApp)
    {
        TRACE_LOG("Command Service Init Fail!");
    }

    DWORD dwRc = m_pLanApp->AddTCPServer(TELNET_CHANNEL, m_wTelnetdPort);
    TRACE_LOG(STR_FORMAT("Command Service Add TCPServer(channel:%d,port:%d) rc:0x%x!", TELNET_CHANNEL, m_wTelnetdPort, dwRc));

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
    dwRc = m_pLanApp->Start("Command",
                        (piParent)? piParent->ID() : 0,
                        ID(),
                        LAN_TASK_COUNT,
                        &theEventProc,
                        &theFrameProc,
                        &theLogProc);
    TRACE_LOG(STR_FORMAT("Command Service Start Rc:0x%x!", dwRc));
}

/*******************************************************
  �� �� ��: CCommand::StopLanApp
  ��    ��: ֹͣ����Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::StopLanApp()
{
    if (!m_pLanApp)
    {
        return;
    }

    m_pLanApp->Stop();
    delete m_pLanApp;
    m_pLanApp = 0;

    TRACE_LOG("Command Network Service Stop!");
}

/*******************************************************
  �� �� ��: CCommand::OnLogPrint
  ��    ��: ��־��ӡ�ص�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnLogPrint(const char *cszLogInfo,
                        const char *pFile,
                        DWORD dwLine,
                        void *pUserArg)
{
    TraceLogEx(cszLogInfo, pFile, dwLine);
}

/*******************************************************
  �� �� ��: CCommand::OnAccept
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::OnAccept(DWORD dwChannelID,
                        objSock *pServerSock,
                        objSock *pAcceptSock,
                        void *pUserArg)
{
    CCommand *pThis = (CCommand *)pUserArg;
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

    /// IAC DONT ECHO
    /// ��ֹ�ͻ����ڵ��ַ�ģʽ�½��б��ػ���(����Windows Telent Client)
    /// ����û��ǿ����ģʽ�����Զ���ģʽ�Ŀͻ��˲�������(����secureCRT)
    BYTE byTmp[] = {0xFF, 0xFE, 0x01};
    (void)pAcceptSock->Send(byTmp, (DWORD)sizeof(byTmp));

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CCommand::OnDisconnect
  ��    ��: �Ͽ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnDisconnect(DWORD dwChannelID,
                        objSock *pSock,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg)
{
    CCommand *pThis = (CCommand *)pUserArg;
    if (!pThis)
    {
        return;
    }

    AutoObjLock(pThis);

    pThis->DeleteSession(objSock::GetIPValueByString(cszRemoteIP), wRemotePort);
}

/*******************************************************
  �� �� ��: CCommand::OnRecv
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::OnRecv(DWORD dwChannelID,
                        objSock *pSock,
                        void *pFrameBuf,
                        DWORD dwFrameLen,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg)
{
    CCommand *pThis = (CCommand *)pUserArg;
    if (!pThis)
    {
        return;
    }

    AutoObjLock(pThis);

    SessionNode *pSessNode = pThis->FindSession(
                        objSock::GetIPValueByString(cszRemoteIP), 
                        wRemotePort);
    if (!pSessNode)
    {
        return;
    }

    pThis->ProcData(*pSessNode, pFrameBuf, dwFrameLen);
}

/*******************************************************
  �� �� ��: CCommand::bFrame
  ��    ��: �ж�����֡
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int CCommand::bFrame(void *pBuf,
                        DWORD dwLen,
                        void *pUserArg)
{
    return dwLen;
}

/*******************************************************
  �� �� ��: CCommand::BytesOrder
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::BytesOrder(void *pBuf,
                    DWORD dwLen,
                    void *pUserArg)
{
}

/*******************************************************
  �� �� ��: CCommand::CreateSession
  ��    ��: �����Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CCommand::CreateSession(objSock *pSock)
{
    if (!pSock || !m_piSession) return FAILURE;

    DWORD dwSessionID = 0;
    ISession::NODE sessNode;
    (void)memset(&sessNode, 0, sizeof(sessNode));

    /// ���һỰ�Ƿ����
    DWORD dwIP = objSock::GetIPValueByString(pSock->cszGetHostIP());
    WORD wPort = pSock->wGetHostPort();
    DWORD dwRc = m_piSession->FindSession(dwIP, wPort, sessNode);
    if (dwRc != SUCCESS)
    {
        /// �����ڵĻỰ�����Ϊδ��¼�û�
        dwRc = m_piSession->CreateSession(DCOP_USER_UNLOGIN, 
                        DCOP_GROUP_VISITOR, 
                        ID(), 
                        dwIP, 
                        wPort, 
                        dwSessionID);
    }
    else
    {
        /// �Ѿ����ڻỰ������Ϊδ��¼�û�
        dwSessionID = sessNode.SessID;
        dwRc = m_piSession->UpdateUserID(dwSessionID, DCOP_USER_UNLOGIN, DCOP_GROUP_VISITOR);
    }

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
    sess.m_byState = SESS_STATE_INPUT_USERNAME;
    sess.m_byModel = SESS_MODEL_TEXT;
    sess.m_byTryTimes = 0;
    sess.m_pRspTable = NULL;
    (void)m_sessions.insert(MAP_SESSIONS::value_type(dwSessionID, sess));

    #if _COMMAND_DEBUG_
    PrintLog(STR_FORMAT("<Session(%d) Connected> %s:%d", dwSessionID, 
                        pSock->cszGetHostIP(), pSock->wGetHostPort()), m_fnOut, m_pPara);
    #endif

    Welcome("", dwSessionID);
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CCommand::DeleteSession
  ��    ��: ɾ���Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::DeleteSession(DWORD dwIP, WORD wPort)
{
    if (!m_piSession) return;

    ISession::NODE sessNode;

    /// ���һỰ�Ƿ����
    DWORD dwRc = m_piSession->FindSession(dwIP, wPort, sessNode);
    if (dwRc != SUCCESS)
    {
        return;
    }

    (void)m_piSession->DeleteSession(sessNode.SessID);

    (void)m_sessions.erase(sessNode.SessID);

    #if _COMMAND_DEBUG_
    char szIP[OSSOCK_IPSIZE] = {0,};
    objSock::GetIPStringByValue(dwIP, szIP);
    PrintLog(STR_FORMAT("<Session(%d) Disconnected> %s:%d", sessNode.SessID, 
                        szIP, wPort), m_fnOut, m_pPara);
    #endif
}

/*******************************************************
  �� �� ��: CCommand::FindSession
  ��    ��: ���һỰ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CCommand::SessionNode *CCommand::FindSession(DWORD dwIP, WORD wPort)
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
  �� �� ��: CCommand::FindSession
  ��    ��: ���һỰ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CCommand::SessionNode *CCommand::FindSession(DWORD dwSessionID)
{
    IT_SESSIONS it_sess = m_sessions.find(dwSessionID);
    if (it_sess == m_sessions.end())
    {
        return NULL;
    }

    return &((*it_sess).second);
}

/*******************************************************
  �� �� ��: CCommand::ProcData
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::ProcData(SessionNode &sessNode, void *pFrameBuf, DWORD dwFrameLen)
{
    if (!sessNode.m_pSock || !pFrameBuf || !dwFrameLen) return;

    if (m_piSession)
    {
        m_piSession->UpdateSession(sessNode.m_dwSessID);
    }

    if ((*(char *)pFrameBuf == '\r') || 
        (*(char *)pFrameBuf == '\n'))
    {
        /// [���ַ�ģʽ] ����'�س�����'
        (void)sessNode.m_pSock->Send("\r\n", 2);

        /// ���ı�ģʽ�£��л����ı�ģʽ
        if (sessNode.m_byModel != SESS_MODEL_TEXT)
        {
            sessNode.m_byModel = SESS_MODEL_TEXT;
            return;
        }

        /// ������������
        sessNode.m_sInput << (BYTE)'\0';
        Line((const char *)sessNode.m_sInput.Buffer(), sessNode.m_dwSessID);
        (void)sessNode.m_sInput.Remove(0, CDString::WHOLE);
    }
    else if (*(char *)pFrameBuf == 8)
    {
        /// ���ı�ģʽ�£��л����ı�ģʽ
        if (sessNode.m_byModel != SESS_MODEL_TEXT)
        {
            return;
        }

        /// [���ַ�ģʽ] ����'�˸�ɾ��'
        if (sessNode.m_sInput.Remove(CDString::TAIL, 1) == SUCCESS)
        {
            BYTE byTmp[] = {0x08, ' ', 0x08};
            (void)sessNode.m_pSock->Send(byTmp, (DWORD)sizeof(byTmp));
        }
    }
    else if (isprint(*(char *)pFrameBuf))
    {
        if (dwFrameLen == 1)
        {
            /// ���ı�ģʽ�£��л����ı�ģʽ
            if (sessNode.m_byModel != SESS_MODEL_TEXT)
            {
                if (((*(char *)pFrameBuf >= 'a') && (*(char *)pFrameBuf <= 'z')) || 
                    ((*(char *)pFrameBuf >= 'A') && (*(char *)pFrameBuf <= 'Z')))
                {
                    sessNode.m_byModel = SESS_MODEL_TEXT;
                }

                return;
            }

            /// [���ַ�ģʽ] ����'�����ַ�'
            if (sessNode.m_byState == SESS_STATE_INPUT_PASSTEXT)
            {
                BYTE byTmp[] = {'*'}; // �����ַ�ģʽ��'*'����У����
                (void)sessNode.m_pSock->Send(byTmp, 1);
            }
            else
            {
                (void)sessNode.m_pSock->Send(pFrameBuf, dwFrameLen);
            }
        }

        if (sessNode.m_byModel == SESS_MODEL_TEXT)
        {
            sessNode.m_sInput << CBufferPara(pFrameBuf, dwFrameLen);
        }

        if (dwFrameLen > 1)
        {
            /// ���ı�ģʽ�£��л����ı�ģʽ
            if (sessNode.m_byModel != SESS_MODEL_TEXT)
            {
                sessNode.m_byModel = SESS_MODEL_TEXT;
                return;
            }

            /// [��ģʽ] ֻ����������
            if (sessNode.m_byState == SESS_STATE_INPUT_PASSTEXT)
            {
                char szTmp[8]; // ��ģʽ����У���� : �ȷ�����һ�У�Ȼ�����Ƶ�"password:"֮��ɾ��У����
                DWORD dwLenTmp = (DWORD)snprintf(szTmp, sizeof(szTmp), "\033[1A");
                (void)sessNode.m_pSock->Send(szTmp, dwLenTmp);
                dwLenTmp = (DWORD)snprintf(szTmp, sizeof(szTmp), "\033[9C");
                (void)sessNode.m_pSock->Send(szTmp, dwLenTmp);
                dwLenTmp = (DWORD)snprintf(szTmp, sizeof(szTmp), "\033[K");
                (void)sessNode.m_pSock->Send(szTmp, dwLenTmp);
                dwLenTmp = (DWORD)snprintf(szTmp, sizeof(szTmp), "\033[1B");
                (void)sessNode.m_pSock->Send(szTmp, dwLenTmp);
                dwLenTmp = (DWORD)snprintf(szTmp, sizeof(szTmp), "\033[9D");
                (void)sessNode.m_pSock->Send(szTmp, dwLenTmp);
            }

            if ((*((char *)pFrameBuf + dwFrameLen - 1) == '\r') || 
                (*((char *)pFrameBuf + dwFrameLen - 1) == '\n'))
            {
                sessNode.m_sInput << (BYTE)'\0';
                Line((const char *)sessNode.m_sInput.Buffer(), sessNode.m_dwSessID);
                (void)sessNode.m_sInput.Remove(0, CDString::WHOLE);
            }
        }
    }
    else
    {
        if (*(BYTE *)pFrameBuf == 255)
        {
            sessNode.m_byModel = SESS_MODEL_IAC;
            #if _COMMAND_DEBUG_
            PrintBuffer(STR_FORMAT("<Session(%d) IAC> len:%d", sessNode.m_dwSessID, dwFrameLen), 
                        pFrameBuf, dwFrameLen, m_fnOut, m_pPara);
            #endif
            /// ������TELNETѡ������
            sessNode.m_byModel = SESS_MODEL_TEXT;
        }

        if (*(BYTE *)pFrameBuf == 0x1b)
        {
            sessNode.m_byModel = SESS_MODEL_CTRL;
            #if _COMMAND_DEBUG_
            PrintBuffer(STR_FORMAT("<Session(%d) Ctrl> len:%d", sessNode.m_dwSessID, dwFrameLen), 
                        pFrameBuf, dwFrameLen, m_fnOut, m_pPara);
            #endif
            /// ������VT100��������
            if (dwFrameLen >= 3)
            {
                /// 3���ֽ����ϲŻ������ȫ��������ֻ����a-zA-Z����
                sessNode.m_byModel = SESS_MODEL_TEXT;
            }
        }
    }
}

/*******************************************************
  �� �� ��: CCommand::InputUserName
  ��    ��: ���������û���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::InputUserName(SessionNode &sessNode, const char *cszText)
{
    if (!m_piUser) return;

    CDString str(cszText);
    str.Trim(" \r\n\t");
    if (!str.Length())
    {
        if (sessNode.m_pSock) (void)sessNode.m_pSock->Shut();
        return;
    }

    if (str == "guest")
    {
        (void)snprintf(sessNode.m_szUserName, sizeof(sessNode.m_szUserName), "guest");
        sessNode.m_dwUserID = DCOP_USER_UNLOGIN;
        sessNode.m_dwUserGroup = DCOP_GROUP_VISITOR;
        sessNode.m_byState = SESS_STATE_LOGIN_SUCCESS;
        Welcome("", sessNode.m_dwSessID);
        return;
    }

    (void)snprintf(sessNode.m_szUserName, sizeof(sessNode.m_szUserName), "%s", (const char *)str);
    sessNode.m_szUserName[sizeof(sessNode.m_szUserName) - 1] = '\0';
    sessNode.m_byState = SESS_STATE_INPUT_PASSTEXT;
    TextOut("password:", &sessNode);
    TextOut("\033[8m", &sessNode);
}

/*******************************************************
  �� �� ��: CCommand::InputPassText
  ��    ��: ��������У����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CCommand::InputPassText(SessionNode &sessNode, const char *cszText)
{
    if (!m_piUser || !m_piSession) return;

    CDString str(cszText);
    str.Trim(" \r\n\t");
    if (!str.Length())
    {
        if (sessNode.m_pSock) (void)sessNode.m_pSock->Shut();
        return;
    }

    IUser::NODE userNode;
    DWORD dwRc = m_piUser->CheckPass(sessNode.m_szUserName, (const char *)str, &userNode);
    if (dwRc != SUCCESS)
    {
        sessNode.m_dwUserID = DCOP_USER_UNLOGIN;
        sessNode.m_dwUserGroup = DCOP_GROUP_VISITOR;
        sessNode.m_byState = SESS_STATE_INPUT_USERNAME;
        sessNode.m_byTryTimes++;
        if ((sessNode.m_byTryTimes >= SESS_LOGIN_TRY_TIMES_MAX) && 
            (sessNode.m_pSock))
        {
            TextOut("\033[0m", &sessNode);
            TextOut("Try Times Over Max, Bye! \r\n", &sessNode);
            (void)sessNode.m_pSock->Shut();
            return;
        }
        TextOut("\033[0m", &sessNode);
        TextOut("Wrong username or password, please input again. \r\n", &sessNode);
        TextOut("username:", &sessNode);
        return;
    }

    sessNode.m_dwUserID = userNode.UserID;
    sessNode.m_dwUserGroup = userNode.Group;
    sessNode.m_byState = SESS_STATE_LOGIN_SUCCESS;
    TextOut("\033[0m", &sessNode);
    Welcome("", sessNode.m_dwSessID);
    (void)m_piSession->UpdateUserID(sessNode.m_dwSessID, sessNode.m_dwUserID, sessNode.m_dwUserGroup);
}

