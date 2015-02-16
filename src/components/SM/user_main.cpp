/// -------------------------------------------------
/// user_main.cpp : �û�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "user_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "md5/md5.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CUser, "user")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CUser)
    DCOP_IMPLEMENT_INTERFACE(IUser)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CUser)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
    IMPLEMENT_CONFIG_DATATYPE("userdatatype", users)
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ������
/// -------------------------------------------------
IMPLEMENT_ATTRIBUTE(CUser, users, CUser::USER_TABLE_ID, IModel::TYPE_DATA)

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CUser)
    IMPLEMENT_ATTRIBUTE_MSG_PROC(userIndex)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END

/// -------------------------------------------------
/// �û�����
/// -------------------------------------------------
const char* CUser::USER_TABLE_NAME = "users";

/// -------------------------------------------------
/// �û��ֶ�����
/// -------------------------------------------------
IModel::Field CUser::UserFields[] = 
{
    {"username", IModel::KEY_NO, CUser::USER_NAME_TYPE, CUser::USER_NAME_SIZE, 0, 0, 0},
    {"userid", IModel::KEY_YES, CUser::USER_ID_TYPE, CUser::USER_ID_SIZE, 0, 0, 0},
    {"password", IModel::KEY_NO, CUser::USER_PASS_TYPE, CUser::USER_PASS_SIZE, 0, 0, 0},
    {"level", IModel::KEY_NO, CUser::USER_LEVEL_TYPE, CUser::USER_LEVEL_SIZE, 0, 0, 0},
    {"usergroup", IModel::KEY_NO, CUser::USER_GROUP_TYPE, CUser::USER_GROUP_SIZE, 0, 0, 0},
    {"info", IModel::KEY_NO, CUser::USER_INFO_TYPE, CUser::USER_INFO_SIZE, 0, 0, 0}
};

/// -------------------------------------------------
/// �û���������
/// -------------------------------------------------
DCOP_PARA_NODE CUser::UserParas[] = 
{
    {CUser::USER_NAME, 0, CUser::USER_NAME_TYPE, CUser::USER_NAME_SIZE},
    {CUser::USER_ID, 0, CUser::USER_ID_TYPE, CUser::USER_ID_SIZE},
    {CUser::USER_PASS, 0, CUser::USER_PASS_TYPE, CUser::USER_PASS_SIZE},
    {CUser::USER_LEVEL, 0, CUser::USER_LEVEL_TYPE, CUser::USER_LEVEL_SIZE},
    {CUser::USER_GROUP, 0, CUser::USER_GROUP_TYPE, CUser::USER_GROUP_SIZE},
    {CUser::USER_INFO, 0, CUser::USER_INFO_TYPE, CUser::USER_INFO_SIZE}
};

/// -------------------------------------------------
/// �û���У���Ĳ�������
/// -------------------------------------------------
DCOP_PARA_NODE CUser::UserNamePassParas[] = 
{
    {CUser::USER_NAME, 0, CUser::USER_NAME_TYPE, CUser::USER_NAME_SIZE},
    {CUser::USER_PASS, 0, CUser::USER_PASS_TYPE, CUser::USER_PASS_SIZE}
};

/// -------------------------------------------------
/// �û�����Ϊ�ؼ�������
/// -------------------------------------------------
DCOP_PARA_NODE CUser::UserKeyName[] = 
{
    {CUser::USER_NAME, 0, CUser::USER_NAME_TYPE, CUser::USER_NAME_SIZE}
};

/// -------------------------------------------------
/// �û�ID��Ϊ�ؼ�������
/// -------------------------------------------------
DCOP_PARA_NODE CUser::UserKeyID[] = 
{
    {CUser::USER_ID, 0, CUser::USER_ID_TYPE, CUser::USER_ID_SIZE}
};


/*******************************************************
  �� �� ��: CUser::BytesChangeRecord
  ��    ��: ת����¼�ֽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CUser::BytesChangeRecord(NODE *pRec)
{
    const BYTES_CHANGE_RULE UserRecBORule[] = 
    {
        {SIZE_OF(IUser::NODE, UserID), OFFSET_OF(IUser::NODE, UserID)},
        {SIZE_OF(IUser::NODE, Level), OFFSET_OF(IUser::NODE, Level)},
        {SIZE_OF(IUser::NODE, Group), OFFSET_OF(IUser::NODE, Group)}
    };
    Bytes_ChangeOrderByRule(UserRecBORule, ARRAY_SIZE(UserRecBORule), pRec, sizeof(NODE));
}

/*******************************************************
  �� �� ��: CUser::CUser
  ��    ��: CUser���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CUser::CUser(Instance *piParent, int argc, char **argv) : 
    m_users(IData::TYPE_MYSQL)
{
    m_piModel = 0;
    m_piData = 0;

    m_piDispatch = 0;
    m_piNotify = 0;
    m_pNotifyPool = 0;

    m_piSecure = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CUser::~CUser
  ��    ��: CUser��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CUser::~CUser()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CUser::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::Init(IObject *root, int argc, void **argv)
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
        DCOP_QUERY_OBJECT_ITEM(ISecure,      DCOP_OBJECT_SECURE,     m_piSecure)
    DCOP_QUERY_OBJECT_END

    /// ���������
    DWORD adwEvents[] = 
    {
        DCOP_OBJATTR_USER_TABLE
    };
    m_pNotifyPool = m_piNotify->CreatePool(this, adwEvents, ARRAY_SIZE(adwEvents));
    if (!m_pNotifyPool)
    {
        return FAILURE;
    }

    /// ��ʼ������
    INIT_ATTRIBUTE_START(userIndex, m_piDispatch, m_pNotifyPool)
        INIT_ATTRIBUTE_MEMBER(users)
    INIT_ATTRIBUTE_END

    /// �������ݶ���
    m_users.SetDataObj(m_piData);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CUser::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CUser::Fini()
{
    if (m_piNotify && m_pNotifyPool)
    {
        m_piNotify->DestroyPool(m_pNotifyPool);
        m_pNotifyPool = 0;
    }

    DCOP_RELEASE_INSTANCE(m_piSecure);
    DCOP_RELEASE_INSTANCE(m_piNotify);
    DCOP_RELEASE_INSTANCE(m_piDispatch);
    DCOP_RELEASE_INSTANCE(m_piData);
    DCOP_RELEASE_INSTANCE(m_piModel);
}

/*******************************************************
  �� �� ��: CUser::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CUser::OnStart(objMsg *msg)
{
    DWORD dwRc = InitModelData();
    CHECK_ERRCODE(dwRc, "Init Model Data");

    if (m_piSecure)
    {
        ISecure::Node node[] = 
        {
            {m_users.GetAttribute()->GetID(), 
                0, 0, 0, 0, 0, 0, 
                DCOP_GROUP_ADMINISTRATOR},
        };
        dwRc = m_piSecure->RegRule(node, ARRAY_SIZE(node));
        CHECK_ERRCODE(dwRc, "Reg Sceure Rule");
    }
}

/*******************************************************
  �� �� ��: CUser::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CUser::OnFinish(objMsg *msg)
{
    (void)m_users.Destroy();
}

/*******************************************************
  �� �� ��: CUser::CreateUser
  ��    ��: �����û�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::CreateUser(const char *cszUserName,
                        const char *cszPassText,
                        DWORD dwLevel,
                        DWORD dwGroup,
                        DWORD &rdwUserID)
{
    /// ��Ӽ�¼
    NODE addUserData = 
    {
        "", 0, "", dwLevel, dwGroup, ""
    };
    (void)strncpy(addUserData.UserName, cszUserName, sizeof(addUserData.UserName));
    addUserData.UserName[sizeof(addUserData.UserName) - 1] = '\0';
    (void)strncpy(addUserData.PassText, cszPassText, sizeof(addUserData.PassText));
    addUserData.PassText[sizeof(addUserData.PassText) - 1] = '\0';
    BytesChangeRecord(&addUserData);

    AutoObjLock(this);

    DWORD dwRc = m_users.AddRecord(UserParas, ARRAY_SIZE(UserParas), 
                        &addUserData, sizeof(addUserData));
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// �����û������Ҽ�¼
    dwRc = FindUser(cszUserName, addUserData);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    rdwUserID = addUserData.UserID;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CUser::DeleteUser
  ��    ��: ɾ���û�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::DeleteUser(DWORD dwUserID)
{
    BYTE condData[4];
    Bytes_SetDword(condData, dwUserID);

    AutoObjLock(this);

    return m_users.DelRecord(DCOP_CONDITION_ONE, 
                        UserKeyID, ARRAY_SIZE(UserKeyID), 
                        condData, sizeof(condData));
}

/*******************************************************
  �� �� ��: CUser::FindUser
  ��    ��: �����û�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::FindUser(const char *cszUserName,
                        NODE &rNode)
{
    if (!cszUserName)
    {
        return FAILURE;
    }

    char szUserName[NAME_SIZE];
    (void)strncpy(szUserName, cszUserName, sizeof(szUserName));
    szUserName[sizeof(szUserName) - 1] = '\0';
    DCOP_PARA_NODE *pRspPara = 0;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;

    AutoObjLock(this);

    DWORD dwRc = m_piData->QueryRecord(m_users, 
                        DCOP_CONDITION_ONE, 
                        UserKeyName, ARRAY_SIZE(UserKeyName), 
                        szUserName, NAME_SIZE, 
                        UserParas, ARRAY_SIZE(UserParas), 
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
  �� �� ��: CUser::CheckPass
  ��    ��: ���У����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::CheckPass(const char *cszUserName,
                        const char *cszPassText,
                        NODE *pNode)
{
    if (!cszUserName || !cszPassText)
    {
        return FAILURE;
    }

    NODE nodeTmp;

    AutoObjLock(this);

    DWORD dwRc = FindUser(cszUserName, nodeTmp);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    MD5_CTX md5;
    MD5Init(&md5);

    char digest[16];
    MD5Update(&md5, (unsigned  char *)nodeTmp.PassText, 16);
    MD5Update(&md5, (unsigned  char *)cszPassText, (unsigned int)strlen(cszPassText));
    MD5Final(&md5, (unsigned  char *)digest);

    if (memcmp(digest, nodeTmp.PassText + 16, 16) != 0)
    {
        return FAILURE;
    }

    if (pNode)
    {
        (void)memcpy(pNode, &nodeTmp, sizeof(NODE));
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CUser::ChangePass
  ��    ��: �޸�У����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::ChangePass(DWORD dwUserID,
                        char szUserName[NAME_SIZE],
                        char szOldPass[PASS_SIZE],
                        char szNewPass[PASS_SIZE])
{
    AutoObjLock(this);

    /// �Ƚ���У��
    DWORD dwRc = CheckPass(szUserName, szOldPass);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ������У����
    BYTE condData[4];
    Bytes_SetDword(condData, dwUserID);

    DCOP_PARA_NODE UserPass[] = 
    {
        {CUser::USER_PASS, 0, CUser::USER_PASS_TYPE, CUser::USER_PASS_SIZE}
    };

    return m_users.EditRecord(DCOP_CONDITION_ONE, 
                        UserKeyID, ARRAY_SIZE(UserKeyID), 
                        condData, sizeof(condData),
                        UserPass, ARRAY_SIZE(UserPass),
                        szNewPass, PASS_SIZE);
}

/*******************************************************
  �� �� ��: CUser::GetUser
  ��    ��: ��ȡ�û�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::GetUser(DWORD dwUserID,
                        NODE &rNode)
{
    BYTE condData[4];
    Bytes_SetDword(condData, dwUserID);

    DCOP_PARA_NODE *pRspPara = 0;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;

    AutoObjLock(this);

    DWORD dwRc = m_piData->QueryRecord(m_users, 
                        DCOP_CONDITION_ONE, 
                        UserKeyID, ARRAY_SIZE(UserKeyID), 
                        condData, sizeof(condData), 
                        UserParas, ARRAY_SIZE(UserParas), 
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
  �� �� ��: CUser::InitModelData
  ��    ��: ��ʼ��ģ�ͺ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CUser::InitModelData()
{
    if (!m_piModel || !m_piData)
    {
        return FAILURE;
    }

    /// ע�����Խ�ģ
    DWORD dwRc = m_piModel->RegTable((char *)USER_TABLE_NAME, 
                        ID(), 
                        m_users.GetAttribute()->GetID(), 
                        m_users.GetModelType(), 
                        UserFields, 
                        ARRAY_SIZE(UserFields), 
                        USER_REC_DEF_COUNT);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ��������
    dwRc = m_users.Create();
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    /// ����û�����Ϊ�ؼ�������
    dwRc = m_piData->AddKeyIdx(m_users, UserKeyName, ARRAY_SIZE(UserKeyName));
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    return SUCCESS;
}

