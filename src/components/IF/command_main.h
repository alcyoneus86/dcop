/// -------------------------------------------------
/// command_main.h : �����н���˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _COMMAND_MAIN_H_
#define _COMMAND_MAIN_H_

#define INC_STRING
#define INC_MAP

#include "command_if.h"
#include "stream/dstream.h"
#include "string/tablestring.h"
#include "Manager_if.h"
#include "ObjDispatch_if.h"
#include "ObjNotify_if.h"
#include "ObjModel_if.h"
#include "ObjStatus_if.h"
#include "ObjResponse_if.h"
#include "user_if.h"
#include "session_if.h"
#include "access_if.h"
#include "sock.h"


/// -------------------------------------------------
/// �����н������ʵ����
/// -------------------------------------------------
class CCommand : public ICommand
{
public:
    /// ������ɶ���
    enum ELEMENT
    {
        ELEMENT_OPERATION = 0,
        ELEMENT_ATTRIBUTE,
        ELEMENT_REQ_ARG_LIST,
        ELEMENT_COND_ARG_LIST,
    };

    /// ����ָ����б�
    static const char *SplitCharList;

    /// ����������Ͷ���
    static const char *Operation[];

    /// �����������ַ���
    static const char *ArgOpCodeCharList;

    /// �������������Ͷ���
    static const char *ArgOpCode[];

    /// ���󻺴�����
    static const DWORD REQ_POOL_COUNT = 2048;

    /// ���ؽ����IP�Ͷ˿�
    static const DWORD LOCAL_IP = 0x7F000001;
    static const WORD LOCAL_PORT = 0;

    /// Telnet Serverͨ���Ͷ˿�
    static const DWORD TELNET_CHANNEL = 1;
    static const WORD TELNET_PORT = 23;

    /// ������������
    static const DWORD LAN_TASK_COUNT = 1;

    /// �Ự��½����������
    static const BYTE SESS_LOGIN_TRY_TIMES_MAX = 10;

    /// �Ự״̬
    enum SESS_STATE
    {
        SESS_STATE_INPUT_USERNAME,
        SESS_STATE_INPUT_PASSTEXT,
        SESS_STATE_LOGIN_FAILURE,
        SESS_STATE_LOGIN_SUCCESS,
    };

    /// �Ựģʽ
    enum SESS_MODEL
    {
        SESS_MODEL_TEXT,
        SESS_MODEL_CTRL,
        SESS_MODEL_IAC,
    };

    /// �Ự�ڵ�
    struct SessionNode
    {
        char m_szUserName[IUser::NAME_SIZE];
        DWORD m_dwUserID;
        DWORD m_dwUserGroup;
        DWORD m_dwSessID;
        objSock *m_pSock;
        CDStream m_sInput;
        BYTE m_byState;
        BYTE m_byModel;
        BYTE m_byTryTimes;
        BYTE m_reserved;
        CTableString *m_pRspTable;
    };

    /// ģ���б�MAP
    typedef std::map<std::string, IObject *> MAP_MODULES;
    typedef MAP_MODULES::iterator IT_MODULES;

    /// �����б�MAP
    typedef std::map<std::string, DWORD> MAP_OBJATTRS;
    typedef MAP_OBJATTRS::iterator IT_OBJATTRS;

    /// �Ự�б�
    typedef std::map<DWORD, SessionNode> MAP_SESSIONS;
    typedef MAP_SESSIONS::iterator IT_SESSIONS;

public:
    static DCOP_PARA_NODE *GetArgList(const CDArray &strArgList, 
                        IModel::Field *pField, 
                        DWORD dwFieldCount, 
                        DWORD &rdwParaCount, 
                        CDStream &rsParaData, 
                        bool bOneValue = true, 
                        IModel *piModel = NULL, 
                        DWORD dwAttrID = 0);

    static BYTE GetArgOpCode(const CDArray &strNameValue);

    static DWORD GetFieldID(IModel::Field *pField, 
                        DWORD dwFieldCount, 
                        const char *cpszFieldName, 
                        BYTE &rbyParaType, 
                        WORD &rwParaSize, 
                        IModel *piModel = NULL, 
                        DWORD dwAttrID = 0);

    static DWORD GetFieldID(IModel::Field *pField, 
                        DWORD dwFieldCount, 
                        DWORD dwFieldIdx, 
                        BYTE &rbyParaType, 
                        WORD &rwParaSize);

    static DWORD GetFieldValue(BYTE &rbyParaType, 
                        WORD &rwParaSize, 
                        const char *cpszParaValue, 
                        CDStream &rsParaData);

    static const char *GetRelFieldName(IModel *piModel, 
                        DWORD dwAttrID, 
                        DWORD dwFieldID,
                        DWORD dwFieldCount);

public:
    CCommand(Instance *piParent, int argc, char **argv);
    ~CCommand();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    void Welcome(const char *username, DWORD session);
    void Line(const char *command, DWORD session);
    void Out(LOG_PRINT fnOut, LOG_PARA pPara);

    void OnStart(objMsg *msg);
    void OnFinish(objMsg *msg);
    void OnModuleLoad(objMsg *msg);
    void OnModuleUnload(objMsg *msg);
    void OnModelReg(objMsg *msg);
    void OnSessionTimeout(objMsg *msg);
    void OnResponse(objMsg *msg);
    void OnEvent(objMsg *msg);
    void OnDefault(objMsg *msg);

    static void OnLogPrint(const char *cszLogInfo,
                        const char *pFile,
                        DWORD dwLine,
                        void *pUserArg);

    static DWORD OnAccept(DWORD dwChannelID,
                        objSock *pServerSock,
                        objSock *pAcceptSock,
                        void *pUserArg);

    static void OnDisconnect(DWORD dwChannelID,
                        objSock *pSock,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg);

    static void OnRecv(DWORD dwChannelID,
                        objSock *pSock,
                        void *pFrameBuf,
                        DWORD dwFrameLen,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg);

    static int bFrame(void *pBuf,
                        DWORD dwLen,
                        void *pUserArg);

    static void BytesOrder(void *pBuf,
                        DWORD dwLen,
                        void *pUserArg);

    struct OutputPara
    {
        CCommand *m_pThis;
        DWORD m_dwSessID;
    };

    static void Output(const char *text, void *para)
    {
        OutputPara *pPara = (OutputPara *)para;
        if (!pPara || !pPara->m_pThis) return;

        AutoObjLock(pPara->m_pThis);

        pPara->m_pThis->TextOut(text, pPara->m_pThis->FindSession(pPara->m_dwSessID));
    }

private:
    void AddModule(DWORD dwModuleID);
    void DelModule(DWORD dwModuleID);
    void RegModel(DWORD dwAttrID);

    IObject *GetModule(const char *cpszModName);
    BYTE GetOperation(const char *cpszOpName);
    DWORD GetAttribute(const char *cpszAttrName);

    DWORD Analyze(const char *command, SessionNode *pSessNode);
    
    void DumpModObj(const CDArray &strList, SessionNode *pSessNode);
    DWORD GetRspTableTitle(CTableString &tableStr, DWORD dwAttrID, DCOP_PARA_NODE *pPara, DWORD dwParaCount);
    DWORD GetRspTableContent(CTableString &tableStr, const CDArray &aRspHeads, DCOP_PARA_NODE *pPara, DWORD dwParaCount);
    DWORD GetRspTableLine(CTableString &tableStr, DCOP_PARA_NODE *pPara, DWORD dwParaCount, void *pData, DWORD dwDataLen);

    void GetDateTime(char *szStr, int strLen);

    void TextOut(const char *text, SessionNode *pSessNode = NULL);
    void TextEnd(SessionNode *pSessNode = NULL);
    void Help(SessionNode *pSessNode = NULL);

    void StartLanApp();
    void StopLanApp();

    DWORD CreateSession(objSock *pSock);
    void DeleteSession(DWORD dwIP, WORD wPort);
    SessionNode *FindSession(DWORD dwIP, WORD wPort);
    SessionNode *FindSession(DWORD dwSessionID);

    void ProcData(SessionNode &sessNode, void *pFrameBuf, DWORD dwFrameLen);

    void InputUserName(SessionNode &sessNode, const char *cszText);
    void InputPassText(SessionNode &sessNode, const char *cszText);

private:
    MAP_MODULES m_modules;                          // ģ���б�
    MAP_OBJATTRS m_objattrs;                        // �����б�

    WORD m_wTelnetdPort;                            // Telnet����˿�

    IManager *m_piManager;                          // ������
    IDispatch *m_piDispatch;                        // �ַ���
    INotify *m_piNotify;                            // ������
    IModel *m_piModel;                              // ģ��
    IStatus *m_piStatus;                            // ״̬

    IResponse *m_piResponse;                        // ��Ӧ��
    IResponse::IPool *m_pReqPool;                   // ���󻺳��

    IUser *m_piUser;                                // �û�����
    ISession *m_piSession;                          // �Ự����
    IAccess *m_piAccess;                            // �������

    LOG_PRINT m_fnOut;                              // ����ص�
    LOG_PARA  m_pPara;                              // �������

    char m_szSysInfo[OSNAME_LENGTH];                // ϵͳ��Ϣ
    char m_szUserName[IUser::NAME_SIZE];            // Ĭ���û���
    CTableString *m_pRspTable;                      // Ĭ���û���Ӧ��

    objLanApp *m_pLanApp;                           // ����Ӧ��
    MAP_SESSIONS m_sessions;                        // ���ػỰ��Ϣ
};


#endif // #ifndef _COMMAND_MAIN_H_

