/// -------------------------------------------------
/// test_data.h : ��Ҫ�������ݲ���
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_DATA_H_
#define _TEST_DATA_H_

#include "test.h"
#include "ObjData_if.h"
#include "Object_if.h"
#include "Manager_if.h"
#include "ObjModel_if.h"
#include "ObjDispatch_if.h"
#include "ObjNotify_if.h"
#include "ObjSchedule_if.h"
#include "BaseMessage.h"
#include "cpu/bytes.h"


/// ����Data
class CTestSuite_DATA : public ITestSuite
{
public:
    /////////////////////////////////////////////////////
    /// �û���
    /////////////////////////////////////////////////////

    /// �û�����������ID��ȱʡ��¼��
    static char* USER_TABLE_NAME;
    static const DWORD USER_TABLE_ID = 1;
    static const DWORD USER_REC_DEF_COUNT = 100;

    /// �û��ֶ�����
    static const BYTE USER_NAME_TYPE = IModel::FIELD_STRING;
    static const BYTE USER_ID_TYPE = IModel::FIELD_NUMBER;
    static const BYTE USER_PASS_TYPE = IModel::FIELD_STRING;
    static const BYTE USER_LEVEL_TYPE = IModel::FIELD_NUMBER;
    static const BYTE USER_GROUP_TYPE = IModel::FIELD_NUMBER;
    static const BYTE USER_INFO_TYPE = IModel::FIELD_STRING;

    /// �û��ֶδ�С
    static const WORD USER_NAME_SIZE = 32;
    static const WORD USER_ID_SIZE = 4;
    static const WORD USER_PASS_SIZE = 32;
    static const WORD USER_LEVEL_SIZE = 4;
    static const WORD USER_GROUP_SIZE = 4;
    static const WORD USER_INFO_SIZE = 64;

    /// �û��ֶ�ID
    enum USER_FIELD_ID
    {
        USER_NAME = 1,
        USER_ID,
        USER_PASS,
        USER_LEVEL,
        USER_GROUP,
        USER_INFO
    };

    /// �û���¼�ڵ�
    struct USER_RECORD_NODE
    {
        char UserName[USER_NAME_SIZE];
        DWORD UserID;
        char PassText[USER_PASS_SIZE];
        DWORD Level;
        DWORD Group;
        char Info[USER_INFO_SIZE];
    };

    /// �û��ֶ�����
    static IModel::Field UserFields[];

    /// �û���������
    static DCOP_PARA_NODE UserParas[];


    /////////////////////////////////////////////////////
    /// �Ự��
    /////////////////////////////////////////////////////

    /// �Ự����������ID��ȱʡ��¼��
    static char* SESS_TABLE_NAME;
    static const DWORD SESS_TABLE_ID = 2;
    static const DWORD SESS_REC_DEF_COUNT = 100;

    /// �Ự�ֶ�����
    static const BYTE SESS_ID_TYPE = IModel::FIELD_IDENTIFY;
    static const BYTE SESS_USER_TYPE = IModel::FIELD_NUMBER;
    static const BYTE SESS_TTY_TYPE = IModel::FIELD_DWORD;
    static const BYTE SESS_IP_TYPE = IModel::FIELD_IP;
    static const BYTE SESS_PORT_TYPE = IModel::FIELD_NUMBER;
    static const BYTE SESS_INFO_TYPE = IModel::FIELD_STRING;

    /// �Ự�ֶδ�С
    static const WORD SESS_ID_SIZE = 4;
    static const WORD SESS_USER_SIZE = 4;
    static const WORD SESS_TTY_SIZE = 4;
    static const WORD SESS_IP_SIZE = 4;
    static const WORD SESS_PORT_SIZE = 2;
    static const WORD SESS_INFO_SIZE = 22;

    /// �Ự�ֶ�ID
    enum SESS_FIELD_ID
    {
        SESS_ID = 1,
        SESS_USER,
        SESS_TTY,
        SESS_IP,
        SESS_PORT,
        SESS_INFO
    };

    /// �Ự��¼�ڵ�
    struct SESS_RECORD_NODE
    {
        DWORD SessID;
        DWORD UserID;
        DWORD TTY;
        DWORD IP;
        WORD Port;
        char Info[SESS_INFO_SIZE];
    };

    /// �Ự�ֶ�����
    static IModel::Field SessFields[];

    /// �Ự��������
    static DCOP_PARA_NODE SessParas[];

    /// �û�ID���ն�ID��Ϊ�ؼ�������
    static DCOP_PARA_NODE SessKeyUidTei[];

    /// IP�Ͷ˿���Ϊ�ؼ�������
    static DCOP_PARA_NODE SessKeyIPPort[];


public:
    CTestSuite_DATA();
    ~CTestSuite_DATA();

    void BeforeTest();

    int TestEntry(int argc, char* argv[]);

private:
    void InitModule();
    void InitModelData();

private:
    IManager *m_piManager;
    IObject *m_pOwner;
    IModel *m_piModel;
    IDispatch *m_piDispatch;
    INotify *m_piNotify;
    ISchedule *m_piSchedule;
    IData *m_piData;
    IData::Handle m_hUserData;
    IData::Handle m_hSessData;
};


#endif // #ifndef _TEST_DATA_H_

