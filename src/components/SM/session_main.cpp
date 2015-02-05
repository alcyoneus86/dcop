/// -------------------------------------------------
/// session_main.cpp : �Ự����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "session_main.h"
#include "Factory_if.h"
#include "Manager_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CSession, "session")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CSession)
    DCOP_IMPLEMENT_INTERFACE(ISession)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CSession)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ������
/// -------------------------------------------------
IMPLEMENT_ATTRIBUTE(CSession, sessions, CSession::SESS_TABLE_ID, IModel::TYPE_DATA)

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CSession)
    IMPLEMENT_ATTRIBUTE_MSG_PROC(sessionIndex)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_SESSION_TIMER_1S, OnTimer1s)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END

/// -------------------------------------------------
/// �Ự����
/// -------------------------------------------------
const char* CSession::SESS_TABLE_NAME = "sessions";

/// -------------------------------------------------
/// �Ự�ֶ�����
/// -------------------------------------------------
IModel::Field CSession::SessFields[] = 
{
    {"session", IModel::KEY_YES, CSession::SESS_ID_TYPE, CSession::SESS_ID_SIZE, 0, 0, 0},
    {"user", IModel::KEY_NO, CSession::SESS_USER_TYPE, CSession::SESS_USER_SIZE, 0, 0, 0},
    {"usergroup", IModel::KEY_NO, CSession::SESS_GROUP_TYPE, CSession::SESS_GROUP_SIZE, 0, 0, 0},
    {"tty", IModel::KEY_NO, CSession::SESS_TTY_TYPE, CSession::SESS_TTY_SIZE, 0, 0, 0},
    {"timer", IModel::KEY_NO, CSession::SESS_TIMER_TYPE, CSession::SESS_TIMER_SIZE, 0, 0, 0},
    {"IP", IModel::KEY_NO, CSession::SESS_IP_TYPE, CSession::SESS_IP_SIZE, 0, 0, 0},
    {"port", IModel::KEY_NO, CSession::SESS_PORT_TYPE, CSession::SESS_PORT_SIZE, 0, 0, 0},
    {"info", IModel::KEY_NO, CSession::SESS_INFO_TYPE, CSession::SESS_INFO_SIZE, 0, 0, 0}
};

/// -------------------------------------------------
/// �����ֶ�����
/// -------------------------------------------------
IModel::Relation CSession::SessShips[] = 
{
    {CSession::SESS_USER, DCOP_OBJATTR_USER_TABLE, CSession::RELATION_USER},
};

/// -------------------------------------------------
/// �Ự��������
/// -------------------------------------------------
DCOP_PARA_NODE CSession::SessParas[] = 
{
    {CSession::SESS_ID, 0, CSession::SESS_ID_TYPE, CSession::SESS_ID_SIZE},
    {CSession::SESS_USER, 0, CSession::SESS_USER_TYPE, CSession::SESS_USER_SIZE},
    {CSession::SESS_GROUP, 0, CSession::SESS_GROUP_TYPE, CSession::SESS_GROUP_SIZE},
    {CSession::SESS_TTY, 0, CSession::SESS_TTY_TYPE, CSession::SESS_TTY_SIZE},
    {CSession::SESS_TIMER, 0, CSession::SESS_TIMER_TYPE, CSession::SESS_TIMER_SIZE},
    {CSession::SESS_IP, 0, CSession::SESS_IP_TYPE, CSession::SESS_IP_SIZE},
    {CSession::SESS_PORT, 0, CSession::SESS_PORT_TYPE, CSession::SESS_PORT_SIZE}
};

/// -------------------------------------------------
/// �Ự�û�����
/// -------------------------------------------------
DCOP_PARA_NODE CSession::SessUser[] = 
{
    {CSession::SESS_USER, 0, CSession::SESS_USER_TYPE, CSession::SESS_USER_SIZE},
    {CSession::SESS_GROUP, 0, CSession::SESS_GROUP_TYPE, CSession::SESS_GROUP_SIZE}
};

/// -------------------------------------------------
/// �Ự��ʱ������
/// -------------------------------------------------
DCOP_PARA_NODE CSession::SessTimer[] = 
{
    {CSession::SESS_TIMER, 0, CSession::SESS_TIMER_TYPE, CSession::SESS_TIMER_SIZE}
};

/// -------------------------------------------------
/// �Ự��Ϣ����
/// -------------------------------------------------
DCOP_PARA_NODE CSession::SessInfo[] = 
{
    {CSession::SESS_INFO, 0, CSession::SESS_INFO_TYPE, CSession::SESS_INFO_SIZE}
};

/// -------------------------------------------------
/// IP�Ͷ˿���Ϊ�ؼ�������
/// -------------------------------------------------
DCOP_PARA_NODE CSession::SessKeyIPPort[] = 
{
    {CSession::SESS_IP, 0, CSession::SESS_IP_TYPE, CSession::SESS_IP_SIZE},
    {CSession::SESS_PORT, 0, CSession::SESS_PORT_TYPE, CSession::SESS_PORT_SIZE}
};

/// -------------------------------------------------
/// SessionID��Ϊ�ؼ�������
/// -------------------------------------------------
DCOP_PARA_NODE CSession::SessKeyID[] = 
{
    {CSession::SESS_ID, 0, CSession::SESS_ID_TYPE, CSession::SESS_ID_SIZE}
};


/*******************************************************
  �� �� ��: CSession::BytesChangeRecord
  ��    ��: ת����¼�ֽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSession::BytesChangeRecord(NODE *pRec)
{
    const BYTES_CHANGE_RULE SessRecBORule[] = 
    {
        {SIZE_OF(ISession::NODE, SessID), OFFSET_OF(ISession::NODE, SessID)},
        {SIZE_OF(ISession::NODE, UserID), OFFSET_OF(ISession::NODE, UserID)},
        {SIZE_OF(ISession::NODE, UserGroup), OFFSET_OF(ISession::NODE, UserGroup)},
        {SIZE_OF(ISession::NODE, TTY), OFFSET_OF(ISession::NODE, TTY)},
        {SIZE_OF(ISession::NODE, Port), OFFSET_OF(ISession::NODE, Port)}
    };
    Bytes_ChangeOrderByRule(SessRecBORule, ARRAY_SIZE(SessRecBORule), pRec, sizeof(NODE));
}

/*******************************************************
  �� �� ��: CSession::CSession
  ��    ��: CSession���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSession::CSession(Instance *piParent, int argc, char **argv) : 
    m_sessions(IData::TYPE_MEM)
{
    m_piModel = 0;
    m_piData = 0;

    m_piDispatch = 0;
    m_piNotify = 0;
    m_pNotifyPool = 0;

    m_piTimer = 0;
    m_hTimer1s = 0;
    m_pTimerWheel = 0;

    m_piSecure = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CSession::~CSession
  ��    ��: CSession��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSession::~CSession()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CSession::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IModel,       DCOP_OBJECT_MODEL,      m_piModel)
        DCOP_QUERY_OBJECT_ITEM(IData,        DCOP_OBJECT_DATA,       m_piData)
        DCOP_QUERY_OBJECT_ITEM(IDispatch,    DCOP_OBJECT_DISPATCH,   m_piDispatch)
        DCOP_QUERY_OBJECT_ITEM(INotify,      DCOP_OBJECT_NOTIFY,     m_piNotify)
        DCOP_QUERY_OBJECT_ITEM(ITimer,       DCOP_OBJECT_TIMER,      m_piTimer)
        DCOP_QUERY_OBJECT_ITEM(ISecure,      DCOP_OBJECT_SECURE,     m_piSecure)
    DCOP_QUERY_OBJECT_END

    /// ���������
    DWORD adwEvents[] = 
    {
        DCOP_OBJATTR_SESSION_TABLE
    };
    m_pNotifyPool = m_piNotify->CreatePool(this, adwEvents, ARRAY_SIZE(adwEvents));
    if (!m_pNotifyPool)
    {
        return FAILURE;
    }

    /// ��ʼ������
    INIT_ATTRIBUTE_START(sessionIndex, m_piDispatch, m_pNotifyPool)
        INIT_ATTRIBUTE_MEMBER(sessions)
    INIT_ATTRIBUTE_END

    /// �������ݶ���
    m_sessions.SetDataObj(m_piData);

    /// ������ʱ��
    m_pTimerWheel = ITimer::IWheel::CreateInstance(WHEEL_S_SEC_ID, 
                        WHEEL_S_SEC_SLOT_COUNT, 
                        NULL, NULL, 
                        WHEEL_S_HASH_BASE, 
                        OnWheelTimeout, this);
    if (!m_pTimerWheel)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSession::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSession::Fini()
{
    if (m_pTimerWheel)
    {
        delete m_pTimerWheel;
        m_pTimerWheel = 0;
    }

    if (m_piNotify && m_pNotifyPool)
    {
        m_piNotify->DestroyPool(m_pNotifyPool);
        m_pNotifyPool = 0;
    }

    DCOP_RELEASE_INSTANCE(m_piSecure);
    DCOP_RELEASE_INSTANCE(m_piTimer);
    DCOP_RELEASE_INSTANCE(m_piNotify);
    DCOP_RELEASE_INSTANCE(m_piDispatch);
    DCOP_RELEASE_INSTANCE(m_piData);
    DCOP_RELEASE_INSTANCE(m_piModel);
}

/*******************************************************
  �� �� ��: CSession::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSession::OnStart(objMsg *msg)
{
    DWORD dwRc = InitModelData();
    CHECK_ERRCODE(dwRc, "Init Model Data");

    if (m_piTimer)
    {
        m_hTimer1s = m_piTimer->Start(ITimer::TYPE_LOOP, DCOP_MSG_SESSION_TIMER_1S, 1000, this);
    }

    if (m_piSecure)
    {
        ISecure::Node node[] = 
        {
            {m_sessions.GetAttribute()->GetID(), 
                0, 0, 0, 0, 0, 0, 0, 0, 0, 
                DCOP_GROUP_ADMINISTRATOR},
        };
        dwRc = m_piSecure->RegRule(node, ARRAY_SIZE(node));
        CHECK_ERRCODE(dwRc, "Reg Sceure Rule");
    }
}

/*******************************************************
  �� �� ��: CSession::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSession::OnFinish(objMsg *msg)
{
    if (m_piTimer && m_hTimer1s)
    {
        m_piTimer->Stop(m_hTimer1s);
        m_hTimer1s = 0;
    }

    (void)m_sessions.Destroy();
}

/*******************************************************
  �� �� ��: CSession::OnTimer1s
  ��    ��: 1S��ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSession::OnTimer1s(objMsg *msg)
{
    if (m_pTimerWheel)
    {
        m_pTimerWheel->OnTick();
    }
}

/*******************************************************
  �� �� ��: CSession::CreateSession
  ��    ��: �����Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::CreateSession(DWORD dwUserID,
                        DWORD dwUserGroup,
                        DWORD dwTTY,
                        DWORD dwRemoteIP,
                        WORD wRemotePort,
                        DWORD &rdwSessionID)
{
    /// ������ʱ��
    TimerValue timerValue = {0, dwTTY};
    ITimer::Handle hTimer = ITimer::IWheel::Alloc(&timerValue, sizeof(timerValue));
    if (!hTimer)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    DWORD dwRc = InsertToWheel(SESS_TIMEOUT, hTimer);
    if (dwRc != SUCCESS)
    {
        ITimer::IWheel::Free(hTimer);
        return dwRc;
    }

    /// ��Ӽ�¼
    NODE addSessData = 
    {
        0, dwUserID, dwUserGroup, dwTTY, *hTimer, dwRemoteIP, wRemotePort
    };
    BytesChangeRecord(&addSessData);

    dwRc = m_sessions.AddRecord(SessParas, ARRAY_SIZE(SessParas), 
                        &addSessData, sizeof(addSessData));
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ����IP�Ͷ˿ڲ��Ҽ�¼
    dwRc = FindSession(dwRemoteIP, wRemotePort, addSessData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    timerValue.m_dwSessID = addSessData.SessID;
    ITimer::IWheel::SetValue(hTimer, &timerValue, sizeof(timerValue));

    rdwSessionID = addSessData.SessID;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSession::DeleteSession
  ��    ��: ɾ���Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::DeleteSession(DWORD dwSessionID)
{
    AutoObjLock(this);

    NODE rNode;
    DWORD dwRc = GetSession(dwSessionID, rNode);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    BYTE condData[4];
    Bytes_SetDword(condData, dwSessionID);

    ITimer::Handle hTimer = ITimer::IWheel::GetHandle(&rNode.Timer);
    if (hTimer)
    {
        /// ͣ����ʱ��
        DelFromWheel(hTimer);

        /// Ϊ��ֹ��ʱ��������¼��б����ã�����ɾ����ʱ��
        /// ʹ��iDataԭ��API����ֹɾ����ʱ��ʱ�����ϱ��¼�
        (void)m_piData->EditRecord(m_sessions, 
                            DCOP_CONDITION_ONE, 
                            SessKeyID, ARRAY_SIZE(SessKeyID), 
                            condData, sizeof(condData), 
                            SessTimer, ARRAY_SIZE(SessTimer), 
                            hTimer, sizeof(*hTimer));

        /// ɾ����ʱ�����
        ITimer::IWheel::Free(hTimer);
    }

    return m_sessions.DelRecord(DCOP_CONDITION_ONE, 
                        SessKeyID, ARRAY_SIZE(SessKeyID), 
                        condData, sizeof(condData));
}

/*******************************************************
  �� �� ��: CSession::UpdateSession
  ��    ��: ���»Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::UpdateSession(DWORD dwSessionID)
{
    NODE rNode;

    AutoObjLock(this);

    DWORD dwRc = GetSession(dwSessionID, rNode);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    BYTE condData[4];
    Bytes_SetDword(condData, dwSessionID);

    ITimer::Handle hTimer = ITimer::IWheel::GetHandle(&rNode.Timer);
    if (!hTimer)
    {
        return FAILURE;
    }

    (void)DelFromWheel(hTimer);
    dwRc = InsertToWheel(SESS_TIMEOUT, hTimer);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    return m_piData->EditRecord(m_sessions, 
                        DCOP_CONDITION_ONE, 
                        SessKeyID, ARRAY_SIZE(SessKeyID), 
                        condData, sizeof(condData), 
                        SessTimer, ARRAY_SIZE(SessTimer), 
                        hTimer, sizeof(*hTimer));
}

/*******************************************************
  �� �� ��: CSession::UpdateUserID
  ��    ��: �����û�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::UpdateUserID(DWORD dwSessionID,
                        DWORD dwUserID,
                        DWORD dwUserGroup)
{
    BYTE condData[4];
    Bytes_SetDword(condData, dwSessionID);

    BYTE reqData[8];
    Bytes_SetDword(reqData, dwUserID);
    Bytes_SetDword(reqData + 4, dwUserGroup);

    AutoObjLock(this);

    return m_sessions.EditRecord(DCOP_CONDITION_ONE, 
                        SessKeyID, ARRAY_SIZE(SessKeyID), 
                        condData, sizeof(condData), 
                        SessUser, ARRAY_SIZE(SessUser), 
                        reqData, sizeof(reqData));
}

/*******************************************************
  �� �� ��: CSession::FindSession
  ��    ��: ���һỰ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::FindSession(DWORD dwRemoteIP,
                WORD wRemotePort,
                NODE &rNode)
{
    BYTE condData[6];
    *(DWORD *)condData = dwRemoteIP;
    Bytes_SetWord(condData + 4, wRemotePort);

    DCOP_PARA_NODE *pRspPara = 0;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;

    AutoObjLock(this);

    DWORD dwRc = m_piData->QueryRecord(m_sessions, 
                        DCOP_CONDITION_ONE, 
                        SessKeyIPPort, ARRAY_SIZE(SessKeyIPPort), 
                        condData, sizeof(condData), 
                        SessParas, ARRAY_SIZE(SessParas), 
                        pRspPara, 
                        dwRspParaCount, 
                        aRspData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ���ﲻ��Ҫ��Ӧ�ֶΣ�ֱ���ͷ�
    if (pRspPara) DCOP_Free(pRspPara);

    if (aRspData.Count() == 0)
    {
        return FAILURE;
    }

    /// ֻȡ��һ����¼
    NODE *pNode = (NODE *)aRspData.Pos(0);
    if (!pNode)
    {
        return FAILURE;
    }

    BytesChangeRecord(pNode);
    rNode = *pNode;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSession::GetSession
  ��    ��: ��ȡ�Ự
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::GetSession(DWORD dwSessionID, NODE &rNode)
{
    BYTE condData[4];
    Bytes_SetDword(condData, dwSessionID);

    DCOP_PARA_NODE *pRspPara = 0;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;

    AutoObjLock(this);

    DWORD dwRc = m_piData->QueryRecord(m_sessions, 
                        DCOP_CONDITION_ONE, 
                        SessKeyID, ARRAY_SIZE(SessKeyID), 
                        condData, sizeof(condData), 
                        SessParas, ARRAY_SIZE(SessParas), 
                        pRspPara, 
                        dwRspParaCount, 
                        aRspData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ���ﲻ��Ҫ��Ӧ�ֶΣ�ֱ���ͷ�
    if (pRspPara) DCOP_Free(pRspPara);

    if (aRspData.Count() == 0)
    {
        return FAILURE;
    }

    /// ֻȡ��һ����¼
    NODE *pNode = (NODE *)aRspData.Pos(0);
    if (!pNode)
    {
        return FAILURE;
    }

    BytesChangeRecord(pNode);
    rNode = *pNode;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSession::SetSessionInfo
  ��    ��: ���ûỰ��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::SetSessionInfo(DWORD dwSessionID,
                        char szInfo[INFO_SIZE])
{
    BYTE condData[4];
    Bytes_SetDword(condData, dwSessionID);

    AutoObjLock(this);

    return m_sessions.EditRecord(DCOP_CONDITION_ONE, 
                        SessKeyID, ARRAY_SIZE(SessKeyID), 
                        condData, sizeof(condData), 
                        SessInfo, ARRAY_SIZE(SessInfo), 
                        szInfo, INFO_SIZE);
}

/*******************************************************
  �� �� ��: CSession::GetSessionInfo
  ��    ��: ��ȡ�Ự��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::GetSessionInfo(DWORD dwSessionID,
                        char szInfo[INFO_SIZE])
{
    BYTE condData[4];
    Bytes_SetDword(condData, dwSessionID);

    DCOP_PARA_NODE *pRspPara = 0;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;

    AutoObjLock(this);

    DWORD dwRc = m_piData->QueryRecord(m_sessions, 
                        DCOP_CONDITION_ONE, 
                        SessKeyID, ARRAY_SIZE(SessKeyID), 
                        condData, sizeof(condData), 
                        SessInfo, ARRAY_SIZE(SessInfo), 
                        pRspPara, 
                        dwRspParaCount, 
                        aRspData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ���ﲻ��Ҫ��Ӧ�ֶΣ�ֱ���ͷ�
    if (pRspPara) DCOP_Free(pRspPara);

    if (aRspData.Count() == 0)
    {
        return FAILURE;
    }

    /// ֻȡ��һ����¼
    char *pInfo = (char *)aRspData.Pos(0);
    if (!pInfo)
    {
        return FAILURE;
    }

    (void)strncpy(szInfo, pInfo, INFO_SIZE);
    szInfo[INFO_SIZE - 1] = '\0';

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSession::InitModelData
  ��    ��: ��ʼ��ģ�ͺ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::InitModelData()
{
    if (!m_piModel || !m_piData)
    {
        return FAILURE;
    }

    /// ע�����Խ�ģ
    DWORD dwRc = m_piModel->RegTable((char *)SESS_TABLE_NAME, 
                        ID(), 
                        m_sessions.GetAttribute()->GetID(), 
                        m_sessions.GetModelType(), 
                        SessFields, 
                        ARRAY_SIZE(SessFields), 
                        SESS_REC_DEF_COUNT, 
                        SessShips, 
                        ARRAY_SIZE(SessShips));
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ��������
    dwRc = m_sessions.Create();
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ���IP�Ͷ˿���Ϊ�ؼ�������
    dwRc = m_piData->AddKeyIdx(m_sessions, SessKeyIPPort, ARRAY_SIZE(SessKeyIPPort));
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSession::OnWheelTimeout
  ��    ��: ʱ���ֳ�ʱʱ
            [ֻҪ�������ϵĽڵ㳬ʱʱ���Ǹýڵ㱣��ĻỰ��ʱ��]
            [Ȼ��Ըýڵ�����ĻỰ���ڵ��ն�ģ�鷢�ͳ�ʱ��Ϣ]
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSession::OnWheelTimeout(ITimer::Handle handle, void *para)
{
    CSession *pThis = (CSession *)para;
    OSASSERT(pThis != 0);

    IDispatch *pDispatch = pThis->GetDispatch();
    if (!pDispatch)
    {
        return;
    }

    TimerValue *pValue = (TimerValue *)ITimer::IWheel::GetValue(handle);
    if (!pValue)
    {
        return;
    }

    objMsg *pMsg = DCOP_CreateMsg(sizeof(DWORD), DCOP_MSG_SESSION_TIMEOUT, pThis->ID());
    if (!pMsg)
    {
        return;
    }

    DWORD *pdwSessID = (DWORD *)pMsg->GetDataBuf();
    if (!pdwSessID)
    {
        delete pMsg;
        return;
    }

    *pdwSessID = pValue->m_dwSessID;

    pMsg->MsgHead().m_dwDstID = pValue->m_dwTtyID;
    DWORD dwRc = pDispatch->Send(pMsg);
    if (dwRc)
    {
        CHECK_RETCODE(dwRc, STR_FORMAT("Send Timeout Msg To Obj:%d Fail(0x%x)!", pValue->m_dwTtyID, dwRc));
        delete pMsg;
    }

    /// Ȼ��Ƚ���ģ������ɾ���Ự
}

/*******************************************************
  �� �� ��: CSession::InsertToWheel
  ��    ��: ���뵽��ʱ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSession::InsertToWheel(DWORD dwTimeout, ITimer::Handle hTimer)
{
    if (!m_pTimerWheel)
    {
        return FAILURE;
    }

    ITimer::IWheel::SetTimeBase(hTimer, dwTimeout);
    return m_pTimerWheel->Add(hTimer);
}

/*******************************************************
  �� �� ��: CSession::DelFromWheel
  ��    ��: �Ӷ�ʱ������ɾ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSession::DelFromWheel(ITimer::Handle hTimer)
{
    if (!m_pTimerWheel)
    {
        return;
    }

    m_pTimerWheel->Del(hTimer);
}

