/// -------------------------------------------------
/// user_main.h : �û�����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _USER_MAIN_H_
#define _USER_MAIN_H_

#include "user_if.h"
#include "ObjAttribute_if.h"
#include "secure_if.h"


/// -------------------------------------------------
/// �û�����ʵ����
/// -------------------------------------------------
class CUser : public IUser
{
public:
    /////////////////////////////////////////////////////
    /// �û���
    /////////////////////////////////////////////////////

    /// �û�����������ID��ȱʡ��¼��
    static const char* USER_TABLE_NAME;
    static const DWORD USER_TABLE_ID = DCOP_OBJATTR_USER_TABLE;
    static const DWORD USER_REC_DEF_COUNT = 100;

    /// �û��ֶ�����
    static const BYTE USER_NAME_TYPE = IModel::FIELD_STRING;
    static const BYTE USER_ID_TYPE = IModel::FIELD_IDENTIFY;
    static const BYTE USER_PASS_TYPE = IModel::FIELD_PASS;
    static const BYTE USER_LEVEL_TYPE = IModel::FIELD_NUMBER;
    static const BYTE USER_GROUP_TYPE = IModel::FIELD_NUMBER;
    static const BYTE USER_INFO_TYPE = IModel::FIELD_STRING;

    /// �û��ֶδ�С
    static const WORD USER_NAME_SIZE = SIZE_OF(NODE, UserName);
    static const WORD USER_ID_SIZE = SIZE_OF(NODE, UserID);
    static const WORD USER_PASS_SIZE = SIZE_OF(NODE, PassText);
    static const WORD USER_LEVEL_SIZE = SIZE_OF(NODE, Level);
    static const WORD USER_GROUP_SIZE = SIZE_OF(NODE, Group);
    static const WORD USER_INFO_SIZE = SIZE_OF(NODE, Info);

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

    /// �û��ֶ�����
    static IModel::Field UserFields[];

    /// �û���������
    static DCOP_PARA_NODE UserParas[];

    /// �û���У���Ĳ�������
    static DCOP_PARA_NODE UserNamePassParas[];

    /// �û���У���Ĳ���
    struct USER_NAME_PASS
    {
        char UserName[NAME_SIZE];
        char PassText[PASS_SIZE];
    };

    /// �û�����Ϊ�ؼ�������
    static DCOP_PARA_NODE UserKeyName[];

    /// �û�ID��Ϊ�ؼ�������
    static DCOP_PARA_NODE UserKeyID[];

    /// �û�������ͬһ����¼����Ҫת���ֽ���Ĺ���
    static void BytesChangeRecord(NODE *pRec);

public:
    CUser(Instance *piParent, int argc, char **argv);
    ~CUser();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    void OnStart(objMsg *msg);
    void OnFinish(objMsg *msg);

    DWORD CreateUser(const char *cszUserName,
                        const char *cszPassText,
                        DWORD dwLevel,
                        DWORD dwGroup,
                        DWORD &rdwUserID);

    DWORD DeleteUser(DWORD dwUserID);

    DWORD FindUser(const char *cszUserName,
                        NODE &rNode);

    DWORD CheckPass(const char *cszUserName,
                        const char *cszPassText,
                        NODE *pNode = 0);

    DWORD ChangePass(DWORD dwUserID,
                        char szUserName[NAME_SIZE],
                        char szOldPass[PASS_SIZE],
                        char szNewPass[PASS_SIZE]);

    DWORD GetUser(DWORD dwUserID,
                        NODE &rNode);

    DECLARE_ATTRIBUTE_INDEX(userIndex);
    DECLARE_ATTRIBUTE(IData*, users);

private:
    DWORD InitModelData();

private:
    IModel *m_piModel;                              // ģ�͹���
    IData *m_piData;                                // ��������
    IData::TYPE m_userDataType;                     // ��������

    IDispatch *m_piDispatch;                        // ��Ϣ�ַ���
    INotify *m_piNotify;                            // �¼�֪ͨ��
    INotify::IPool *m_pNotifyPool;                  // �¼������

    ISecure *m_piSecure;                            // ��ȫ����
};


#endif // #ifndef _USER_MAIN_H_

