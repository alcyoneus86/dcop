/// -------------------------------------------------
/// session_main.h : �Ự����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SESSION_MAIN_H_
#define _SESSION_MAIN_H_

#include "session_if.h"
#include "ObjAttribute_if.h"
#include "ObjResponse_if.h"
#include "cpu/bytes.h"


/// �Ự����ʵ����
class CSession : public ISession
{
public:
    /////////////////////////////////////////////////////
    /// �Ự��
    /////////////////////////////////////////////////////

    /// �Ự����������ID��ȱʡ��¼��
    static const char* SESS_TABLE_NAME;
    static const DWORD SESS_TABLE_ID = DCOP_OBJATTR_SESSION_TABLE;
    static const DWORD SESS_REC_DEF_COUNT = 100;
    static const DWORD RELATION_USER = 2;

    /// �Ự�ֶ�����
    static const BYTE SESS_ID_TYPE = IModel::FIELD_IDENTIFY;
    static const BYTE SESS_USER_TYPE = IModel::FIELD_NUMBER;
    static const BYTE SESS_GROUP_TYPE = IModel::FIELD_NUMBER;
    static const BYTE SESS_TTY_TYPE = IModel::FIELD_NUMBER;
    static const BYTE SESS_TIMER_TYPE = IModel::FIELD_TIMER;
    static const BYTE SESS_IP_TYPE = IModel::FIELD_IP;
    static const BYTE SESS_PORT_TYPE = IModel::FIELD_NUMBER;
    static const BYTE SESS_INFO_TYPE = IModel::FIELD_STRING;

    /// �Ự�ֶδ�С
    static const WORD SESS_ID_SIZE = SIZE_OF(NODE, SessID);
    static const WORD SESS_USER_SIZE = SIZE_OF(NODE, UserID);
    static const WORD SESS_GROUP_SIZE = SIZE_OF(NODE, UserGroup);
    static const WORD SESS_TTY_SIZE = SIZE_OF(NODE, TTY);
    static const WORD SESS_TIMER_SIZE = SIZE_OF(NODE, Timer);
    static const WORD SESS_IP_SIZE = SIZE_OF(NODE, IP);
    static const WORD SESS_PORT_SIZE = SIZE_OF(NODE, Port);
    static const WORD SESS_INFO_SIZE = INFO_SIZE;

    /// �Ự�ֶ�ID
    enum SESS_FIELD_ID
    {
        SESS_ID = 1,
        SESS_USER,
        SESS_GROUP,
        SESS_TTY,
        SESS_TIMER,
        SESS_IP,
        SESS_PORT,
        SESS_INFO
    };

    /// �Ự�ֶ�����
    static IModel::Field SessFields[];

    /// �����ֶ�����
    static IModel::Relation SessShips[];

    /// �Ự��������
    static DCOP_PARA_NODE SessParas[];

    /// �Ự�û�����
    static DCOP_PARA_NODE SessUser[];

    /// �Ự��ʱ������
    static DCOP_PARA_NODE SessTimer[];

    /// �Ự��Ϣ����
    static DCOP_PARA_NODE SessInfo[];

    /// IP�Ͷ˿���Ϊ�ؼ�������
    static DCOP_PARA_NODE SessKeyIPPort[];

    /// SessionID��Ϊ�ؼ�������
    static DCOP_PARA_NODE SessKeyID[];

    /// �Ự������ͬһ����¼����Ҫת���ֽ���Ĺ���
    static void BytesChangeRecord(NODE *pRec);

    /// �Ự�л�����������
    static const DWORD REQ_POOL_COUNT = 2048;

    /// ��ʱ����������ID����ֵ�Ͳ�λ����
    static const DWORD WHEEL_S_SEC = 0;
    static const DWORD WHEEL_S_SEC_ID = 1;
    static const DWORD WHEEL_S_HASH_BASE = 1;
    static const DWORD WHEEL_S_SEC_SLOT_COUNT = 32;

    /// TimerValue��ע��Ķ�ʱ���ֽڵ�ֵ
    struct TimerValue
    {
        DWORD m_dwSessID;                   // �ỰID
        DWORD m_dwTtyID;                    // ����ID
    };

    /// �Ự��ʱʱ��(��λ:��)
    static const DWORD SESS_TIMEOUT = 300;

public:
    CSession(Instance *piParent, int argc, char **argv);
    ~CSession();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    void OnStart(objMsg *msg);
    void OnFinish(objMsg *msg);
    void OnTimer1s(objMsg *msg);

    DWORD CreateSession(DWORD dwUserID,
                        DWORD dwUserGroup,
                        DWORD dwTTY,
                        DWORD dwRemoteIP,
                        WORD wRemotePort,
                        DWORD &rdwSessionID);

    DWORD DeleteSession(DWORD dwSessionID);

    DWORD UpdateSession(DWORD dwSessionID);

    DWORD UpdateUserID(DWORD dwSessionID,
                        DWORD dwUserID,
                        DWORD dwUserGroup);

    DWORD FindSession(DWORD dwRemoteIP,
                        WORD wRemotePort,
                        NODE &rNode);

    DWORD GetSession(DWORD dwSessionID,
                        NODE &rNode);

    DWORD SetSessionInfo(DWORD dwSessionID,
                        char szInfo[INFO_SIZE]);

    DWORD GetSessionInfo(DWORD dwSessionID,
                        char szInfo[INFO_SIZE]);

    DECLARE_ATTRIBUTE_INDEX(sessionIndex);
    DECLARE_ATTRIBUTE(IData*, sessions);

private:
    DWORD InitModelData();
    DWORD InsertToWheel(DWORD dwTimeOut, ITimer::Handle hTimer);
    void  DelFromWheel(ITimer::Handle hTimer);
    static void OnWheelTimeout(ITimer::Handle handle, void *para);
    IDispatch *GetDispatch() {return m_piDispatch;}

private:
    IModel *m_piModel;                              // ģ�͹���
    IData *m_piData;                                // ��������

    IDispatch *m_piDispatch;                        // ��Ϣ�ַ���
    INotify *m_piNotify;                            // �¼�֪ͨ��
    INotify::IPool *m_pNotifyPool;                  // �¼������

    ITimer *m_piTimer;                              // ��ʱ��
    ITimer::Handle m_hTimer1s;
    ITimer::IWheel *m_pTimerWheel;
};



#endif // #ifndef _SESSION_MAIN_H_

