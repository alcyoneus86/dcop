/// -------------------------------------------------
/// httpd_main.h : ���ı�����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _HTTPD_MAIN_H_
#define _HTTPD_MAIN_H_

#define INC_STRING
#define INC_MAP

#include "httpd_if.h"
#include "httpd_protocol.h"
#include "httpd_restful.h"
#include "Manager_if.h"
#include "ObjDispatch_if.h"
#include "ObjNotify_if.h"
#include "ObjModel_if.h"
#include "ObjStatus_if.h"
#include "ObjResponse_if.h"
#include "ObjSchedule_if.h"
#include "session_if.h"
#include "access_if.h"


/// -------------------------------------------------
/// ���ı��������ʵ����
/// -------------------------------------------------
class CHttpServer : public IHttpServer
{
public:
    /// ���󻺴�����
    static const DWORD REQ_POOL_COUNT = 2048;

    /// Http Serverͨ���Ͷ˿�
    static const DWORD HTTP_CHANNEL = 1;
    static const WORD HTTP_PORT = 80;

    /// ������������
    static const DWORD LAN_TASK_COUNT = 8;

    /// ��Ӧ���͵ȴ�ʱ��
    static const DWORD RSP_WAIT_TIME = 500;

    /// �����б�MAP
    typedef std::map<std::string, DWORD> MAP_OBJATTRS;
    typedef MAP_OBJATTRS::iterator IT_OBJATTRS;

    /// �Ự�б�
    typedef std::map<DWORD, SessionNode> MAP_SESSIONS;
    typedef MAP_SESSIONS::iterator IT_SESSIONS;

public:
    CHttpServer(Instance *piParent, int argc, char **argv);
    ~CHttpServer();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();
    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

    void SaveHttpToSession(DWORD dwSessionID, IHttpHandle *pHttp);

    void OnStart(objMsg *msg);
    void OnFinish(objMsg *msg);
    void OnModelReg(objMsg *msg);
    void OnSessionTimeout(objMsg *msg);
    void OnResponse(objMsg *msg);
    void OnEvent(objMsg *msg);
    void OnHttpRequest(objMsg *msg);
    void OnHttpProcess(objMsg *msg);
    void OnHttpResponse(objMsg *msg);
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

    static void ProcHttp(objMsg *pMsg,
                        void *pUserArg);

private:
    void RegModel(DWORD dwAttrID);

    void StartLanApp();
    void StopLanApp();

    DWORD CreateSession(objSock *pSock);
    void DeleteSession(DWORD dwIP, WORD wPort);
    SessionNode *FindSession(DWORD dwIP, WORD wPort);
    SessionNode *FindSession(DWORD dwSessionID);
    void SetSessionInfo(DWORD dwSessionID, const char *cszUserAgent);

    void ProcData(SessionNode &sessNode, DWORD dwMsgType, void *pFrameBuf, DWORD dwFrameLen);

private:
    MAP_OBJATTRS m_objattrs;                        // �����б�

    DWORD m_dwHttpdLanTaskCount;                    // HTTP����������
    char m_szHttpdProcTaskCount[6];                 // HTTP����������
    WORD m_wHttpdPort;                              // HTTP����˿�
    char m_szHttpdDir[DCOP_FILE_NAME_LEN];          // HTTPĿ¼
    char m_szHttpdHome[DCOP_FILE_NAME_LEN];         // HTTP��ҳ

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

    objLanApp *m_pLanApp;                           // ����Ӧ��
    MAP_SESSIONS m_sessions;                        // ���ػỰ��Ϣ

    IHttpRequest *m_pHttpRequest;                   // HTTP�������
    IHttpProcess *m_pHttpProcess;                   // HTTP�������
    IHttpResponse *m_pHttpResponse;                 // HTTP��Ӧ����

    IObject *m_pHttpJson;                           // HTTP����Json����

    ISchedule *m_pHttpSchedule;                     // HTTP�����̳߳ص�����
};


#endif // #ifndef _HTTPD_MAIN_H_

