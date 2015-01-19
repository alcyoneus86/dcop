/// -------------------------------------------------
/// test_data.cpp : ��Ҫ�������ݲ���
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "test_data.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "cpu/bytes.h"
#include "sock.h"


/// -------------------------------------------------
/// DATA ��������
/// -------------------------------------------------
TEST_SUITE_TABLE(DATA)
    TEST_SUITE_ITEM(CTestSuite_DATA)
        TEST_CASE_ITEM(1)
            "1"
        TEST_CASE_ITEM_END
    TEST_SUITE_ITEM_END
TEST_SUITE_TABLE_END

/// -------------------------------------------------
/// DATA ������
/// -------------------------------------------------
IMPLEMENT_REGTESTSUITE_FUNC(DATA)

/// -------------------------------------------------
/// �û�����
/// -------------------------------------------------
char* CTestSuite_DATA::USER_TABLE_NAME = "�û���";

/// -------------------------------------------------
/// �û��ֶ�����
/// -------------------------------------------------
IModel::Field CTestSuite_DATA::UserFields[] = 
{
    {"�û���", IModel::KEY_NO, CTestSuite_DATA::USER_NAME_TYPE, CTestSuite_DATA::USER_NAME_SIZE, 0, 0, 0},
    {"�û�ID", IModel::KEY_YES, CTestSuite_DATA::USER_ID_TYPE, CTestSuite_DATA::USER_ID_SIZE, 0, 0, 0},
    {"����", IModel::KEY_NO, CTestSuite_DATA::USER_PASS_TYPE, CTestSuite_DATA::USER_PASS_SIZE, 0, 0, 0},
    {"�ȼ�", IModel::KEY_NO, CTestSuite_DATA::USER_LEVEL_TYPE, CTestSuite_DATA::USER_LEVEL_SIZE, 0, 0, 0},
    {"Ⱥ��", IModel::KEY_NO, CTestSuite_DATA::USER_GROUP_TYPE, CTestSuite_DATA::USER_GROUP_SIZE, 0, 0, 0},
    {"��Ϣ", IModel::KEY_NO, CTestSuite_DATA::USER_INFO_TYPE, CTestSuite_DATA::USER_INFO_SIZE, 0, 0, 0},
};

/// -------------------------------------------------
/// �û���������
/// -------------------------------------------------
DCOP_PARA_NODE CTestSuite_DATA::UserParas[] = 
{
    {CTestSuite_DATA::USER_NAME, 0, CTestSuite_DATA::USER_NAME_TYPE, CTestSuite_DATA::USER_NAME_SIZE},
    {CTestSuite_DATA::USER_ID, 0, CTestSuite_DATA::USER_ID_TYPE, CTestSuite_DATA::USER_ID_SIZE},
    {CTestSuite_DATA::USER_PASS, 0, CTestSuite_DATA::USER_PASS_TYPE, CTestSuite_DATA::USER_PASS_SIZE},
    {CTestSuite_DATA::USER_LEVEL, 0, CTestSuite_DATA::USER_LEVEL_TYPE, CTestSuite_DATA::USER_LEVEL_SIZE},
    {CTestSuite_DATA::USER_GROUP, 0, CTestSuite_DATA::USER_GROUP_TYPE, CTestSuite_DATA::USER_GROUP_SIZE},
    {CTestSuite_DATA::USER_INFO, 0, CTestSuite_DATA::USER_INFO_TYPE, CTestSuite_DATA::USER_INFO_SIZE},
};

/// -------------------------------------------------
/// �Ự����
/// -------------------------------------------------
char* CTestSuite_DATA::SESS_TABLE_NAME = "�Ự��";

/// -------------------------------------------------
/// �Ự�ֶ�����
/// -------------------------------------------------
IModel::Field CTestSuite_DATA::SessFields[] = 
{
    {"�ỰID", IModel::KEY_YES, CTestSuite_DATA::SESS_ID_TYPE, CTestSuite_DATA::SESS_ID_SIZE, 0, 0, 0},
    {"�û�ID", IModel::KEY_NO, CTestSuite_DATA::SESS_USER_TYPE, CTestSuite_DATA::SESS_USER_SIZE, 0, 0, 0},
    {"�ն�ID", IModel::KEY_NO, CTestSuite_DATA::SESS_TTY_TYPE, CTestSuite_DATA::SESS_TTY_SIZE, 0, 0, 0},
    {"IP��ַ", IModel::KEY_NO, CTestSuite_DATA::SESS_IP_TYPE, CTestSuite_DATA::SESS_IP_SIZE, 0, 0, 0},
    {"�˿ں�", IModel::KEY_NO, CTestSuite_DATA::SESS_PORT_TYPE, CTestSuite_DATA::SESS_PORT_SIZE, 0, 0, 0},
    {"��Ϣ", IModel::KEY_NO, CTestSuite_DATA::SESS_INFO_TYPE, CTestSuite_DATA::SESS_INFO_SIZE, 0, 0, 0},
};

/// -------------------------------------------------
/// �Ự��������
/// -------------------------------------------------
DCOP_PARA_NODE CTestSuite_DATA::SessParas[] = 
{
    {CTestSuite_DATA::SESS_ID, 0, CTestSuite_DATA::SESS_ID_TYPE, CTestSuite_DATA::SESS_ID_SIZE},
    {CTestSuite_DATA::SESS_USER, 0, CTestSuite_DATA::SESS_USER_TYPE, CTestSuite_DATA::SESS_USER_SIZE},
    {CTestSuite_DATA::SESS_TTY, 0, CTestSuite_DATA::SESS_TTY_TYPE, CTestSuite_DATA::SESS_TTY_SIZE},
    {CTestSuite_DATA::SESS_IP, 0, CTestSuite_DATA::SESS_IP_TYPE, CTestSuite_DATA::SESS_IP_SIZE},
    {CTestSuite_DATA::SESS_PORT, 0, CTestSuite_DATA::SESS_PORT_TYPE, CTestSuite_DATA::SESS_PORT_SIZE},
    {CTestSuite_DATA::SESS_INFO, 0, CTestSuite_DATA::SESS_INFO_TYPE, CTestSuite_DATA::SESS_INFO_SIZE},
};

/// -------------------------------------------------
/// �û�ID���ն�ID��Ϊ�ؼ�������
/// -------------------------------------------------
DCOP_PARA_NODE CTestSuite_DATA::SessKeyUidTei[] = 
{
    {CTestSuite_DATA::SESS_USER, 0, CTestSuite_DATA::SESS_USER_TYPE, CTestSuite_DATA::SESS_USER_SIZE},
    {CTestSuite_DATA::SESS_TTY, 0, CTestSuite_DATA::SESS_TTY_TYPE, CTestSuite_DATA::SESS_TTY_SIZE}
};

/// -------------------------------------------------
/// IP�Ͷ˿���Ϊ�ؼ�������
/// -------------------------------------------------
DCOP_PARA_NODE CTestSuite_DATA::SessKeyIPPort[] = 
{
    {CTestSuite_DATA::SESS_IP, 0, CTestSuite_DATA::SESS_IP_TYPE, CTestSuite_DATA::SESS_IP_SIZE},
    {CTestSuite_DATA::SESS_PORT, 0, CTestSuite_DATA::SESS_PORT_TYPE, CTestSuite_DATA::SESS_PORT_SIZE}
};


CTestSuite_DATA::CTestSuite_DATA()
{
    m_piManager = 0;
    m_pOwner = 0;
    m_piModel = 0;
    m_piDispatch = 0;
    m_piNotify = 0;
    m_piSchedule = 0;
    m_piData = 0;
    m_hUserData = 0;
    m_hSessData = 0;
}

CTestSuite_DATA::~CTestSuite_DATA()
{
    m_hUserData = 0;
    m_hSessData = 0;

    if (m_piManager) m_piManager->Fini();

    DCOP_RELEASE_INSTANCE_REFER(0, m_piManager);
}

void CTestSuite_DATA::InitModule()
{
    TEST_LOAD_OBJECT(m_piManager, IObject, TA, DCOP_OBJECT_CUSTOM + 1, m_pOwner);
    TEST_LOAD_OBJECT(m_piManager, IModel, model, DCOP_OBJECT_MODEL, m_piModel);
    TEST_LOAD_OBJECT(m_piManager, IData, data, DCOP_OBJECT_DATA, m_piData);
    TEST_LOAD_OBJECT(m_piManager, IDispatch, dispatch, DCOP_OBJECT_DISPATCH, m_piDispatch);
    TEST_LOAD_OBJECT(m_piManager, INotify, notify, DCOP_OBJECT_NOTIFY, m_piNotify);
    TEST_LOAD_OBJECT(m_piManager, ISchedule, schedule, DCOP_OBJECT_SCHEDULE, m_piSchedule);

    (void)m_piManager->Init(NULL, 0, 0);
}

void CTestSuite_DATA::BeforeTest()
{
    DCOP_CREATE_INSTANCE(IManager, "manager", NULL, 0, 0, m_piManager);
    if (!m_piManager)
    {
        return;
    }

    InitModule();

    InitModelData();
}

void CTestSuite_DATA::InitModelData()
{
    if (!m_piModel || !m_piData)
    {
        return;
    }

    /// ע�����Խ�ģ
    DWORD dwRc = m_piModel->RegTable(USER_TABLE_NAME, 
                        m_pOwner->ID(), 
                        USER_TABLE_ID, 
                        IModel::TYPE_DATA, 
                        UserFields, 
                        ARRAY_SIZE(UserFields), 
                        USER_REC_DEF_COUNT);
    printf("Reg Table('%s') Rc(0x%x)! \n", USER_TABLE_NAME, dwRc);
    if (dwRc != SUCCESS)
    {
        return;
    }

    /// ע�����Խ�ģ
    dwRc = m_piModel->RegTable(SESS_TABLE_NAME, 
                        m_pOwner->ID(), 
                        SESS_TABLE_ID, 
                        IModel::TYPE_DATA, 
                        SessFields, 
                        ARRAY_SIZE(SessFields), 
                        SESS_REC_DEF_COUNT);
    printf("Reg Table('%s') Rc(0x%x)! \n", SESS_TABLE_NAME, dwRc);
    if (dwRc != SUCCESS)
    {
        return;
    }

}

int CTestSuite_DATA::TestEntry(int argc, char* argv[])
{
    if (!m_piModel || !m_piData || !m_pOwner)
    {
        return -1;
    }

    /// �����û�����
    DWORD dwRc = m_piData->Create(IData::TYPE_FILE, 
                        USER_TABLE_ID, 
                        m_pOwner, 
                        m_hUserData);
    printf("Create Table(%d) Rc(0x%x)! \n", USER_TABLE_ID, dwRc);
    if (dwRc != SUCCESS)
    {
        return -3;
    }

    if (!m_hUserData)
    {
        return -4;
    }

    /// �����Ự����
    dwRc = m_piData->Create(IData::TYPE_MEM, 
                        SESS_TABLE_ID, 
                        m_pOwner, 
                        m_hSessData);
    printf("Create Table(%d) Rc(0x%x)! \n", SESS_TABLE_ID, dwRc);
    if (dwRc != SUCCESS)
    {
        return -5;
    }

    if (!m_hSessData)
    {
        return -6;
    }

    /// ����û�ID���ն�ID��Ϊ�ؼ�������
    dwRc = m_piData->AddKeyIdx(m_hSessData, SessKeyUidTei, ARRAY_SIZE(SessKeyUidTei));
    printf("AddKeyIdx Table(%d) Rc(0x%x)! \n", SESS_TABLE_ID, dwRc);
    if (dwRc != SUCCESS)
    {
        return -7;
    }

    /// ���IP�Ͷ˿���Ϊ�ؼ�������
    dwRc = m_piData->AddKeyIdx(m_hSessData, SessKeyIPPort, ARRAY_SIZE(SessKeyIPPort));
    printf("AddKeyIdx Table(%d) Rc(0x%x)! \n", SESS_TABLE_ID, dwRc);
    if (dwRc != SUCCESS)
    {
        return -8;
    }

    /// ����û�����
    USER_RECORD_NODE addUserData[] = 
    {
        {"����", 0, "HaHaHa", 100, 1, "����"},
        {"�����", 1, "HaHaHa", 98, 2, "��ʦ"},
        {"����", 2, "HaHaHa", 97, 3, "����"},
        {"�ŷ�", 3, "HaHaHa", 96, 3, "����"},
        {"����", 4, "HaHaHa", 96, 3, "����"},
    };

    const BYTES_CHANGE_RULE UserRecBORule[] = 
    {
        {SIZE_OF(CTestSuite_DATA::USER_RECORD_NODE, UserID), OFFSET_OF(CTestSuite_DATA::USER_RECORD_NODE, UserID)},
        {SIZE_OF(CTestSuite_DATA::USER_RECORD_NODE, Level), OFFSET_OF(CTestSuite_DATA::USER_RECORD_NODE, Level)},
        {SIZE_OF(CTestSuite_DATA::USER_RECORD_NODE, Group), OFFSET_OF(CTestSuite_DATA::USER_RECORD_NODE, Group)}
    };

    for (DWORD i = 0; i < ARRAY_SIZE(addUserData); ++i)
    {
        Bytes_ChangeOrderByRule(UserRecBORule, ARRAY_SIZE(UserRecBORule), &(addUserData[i]), sizeof(addUserData[i]));
        dwRc = m_piData->AddRecord(m_hUserData, 
                        UserParas, 
                        ARRAY_SIZE(UserParas), 
                        &(addUserData[i]), 
                        sizeof(addUserData[i]));
        printf("Add User '%s' Rc(0x%x)! \n", addUserData[i].UserName, dwRc);
    }

    /// ��ӻỰ����
    SESS_RECORD_NODE addSessData[] = 
    {
        {0, 0, 0x101, objSock::GetIPValueByString("1.1.1.1"), 200, "����"},
        {0, 1, 0x102, objSock::GetIPValueByString("1.1.1.2"), 200, "����"},
        {0, 2, 0x101, objSock::GetIPValueByString("1.1.1.3"), 200, "����"},
        {0, 3, 0x101, objSock::GetIPValueByString("1.1.1.3"), 201, "����"},
        {0, 4, 0x103, objSock::GetIPValueByString("1.1.1.3"), 202, "����"},
    };

    const BYTES_CHANGE_RULE SessRecBORule[] = 
    {
        {SIZE_OF(CTestSuite_DATA::SESS_RECORD_NODE, SessID), OFFSET_OF(CTestSuite_DATA::SESS_RECORD_NODE, SessID)},
        {SIZE_OF(CTestSuite_DATA::SESS_RECORD_NODE, UserID), OFFSET_OF(CTestSuite_DATA::SESS_RECORD_NODE, UserID)},
        {SIZE_OF(CTestSuite_DATA::SESS_RECORD_NODE, TTY), OFFSET_OF(CTestSuite_DATA::SESS_RECORD_NODE, TTY)},
        {SIZE_OF(CTestSuite_DATA::SESS_RECORD_NODE, Port), OFFSET_OF(CTestSuite_DATA::SESS_RECORD_NODE, Port)}
    };

    for (DWORD i = 0; i < ARRAY_SIZE(addUserData); ++i)
    {
        Bytes_ChangeOrderByRule(SessRecBORule, ARRAY_SIZE(SessRecBORule), &(addSessData[i]), sizeof(addSessData[i]));
        dwRc = m_piData->AddRecord(m_hSessData, 
                        SessParas, 
                        ARRAY_SIZE(SessParas), 
                        &(addSessData[i]), 
                        sizeof(addSessData[i]));
        printf("Add Sess '%s' Rc(0x%x)! \n", addUserData[i].UserName, dwRc);
    }

    m_piData->Dump(PrintToConsole, 0, 0, 0);

    BYTE condSessIPPortData[6];
    *(DWORD *)(condSessIPPortData) = objSock::GetIPValueByString("1.1.1.3");
    Bytes_SetWord(condSessIPPortData + 4, 200);
    DCOP_PARA_NODE *pRspPara = 0;
    DWORD dwRspParaCount = 0;
    CDArray aRspData;
    dwRc = m_piData->QueryRecord(m_hSessData, 
                        DCOP_CONDITION_ONE, 
                        SessKeyIPPort, 
                        ARRAY_SIZE(SessKeyIPPort), 
                        condSessIPPortData, 
                        sizeof(condSessIPPortData), 
                        SessParas, 
                        ARRAY_SIZE(SessParas), 
                        pRspPara, 
                        dwRspParaCount, 
                        aRspData);
    if (dwRc)
    {
        return -6;
    }

    for (DWORD i = 0; i < aRspData.Count(); ++i)
    {
        SESS_RECORD_NODE *pSessRec = (SESS_RECORD_NODE *)aRspData.Pos(i);
        Bytes_ChangeOrderByRule(SessRecBORule, ARRAY_SIZE(SessRecBORule), pSessRec, sizeof(SESS_RECORD_NODE));
        char szIP[OSSOCK_IPSIZE];
        (void)memset(szIP, 0, sizeof(szIP));
        objSock::GetIPStringByValue(pSessRec->IP, szIP);
        printf("����: %s��ID:%d���Ự:%d���ն�:0x%x��IP:%s��Port:%d, ��Ϣ:%s \n", 
                        addUserData[pSessRec->UserID].UserName, 
                        pSessRec->UserID, 
                        pSessRec->SessID, 
                        pSessRec->TTY, 
                        szIP, 
                        pSessRec->Port, 
                        pSessRec->Info);
    }

    if (pRspPara) DCOP_Free(pRspPara);

    return 0;
}

