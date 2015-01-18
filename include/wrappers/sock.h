/// -------------------------------------------------
/// sockType.h : socket��װ�๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SOCK_H_
#define _SOCK_H_

#include "dcop.h"


/// IP��ַ��С(IPv4)
#define OSSOCK_IPSIZE                       16

/// Ĭ��Socket�˿�
#define OSSOCK_DEFAULT_PORT                 34418

/// Ĭ��Socket����䵥Ԫ
#define OSSOCK_DEFAULT_MTU                  (16*1024)

/// Ĭ��Socketÿ������ʱ��
#define OSSOCK_DEFAULT_BLOCKTIME            1000

/// Ĭ��Socket����ջ��С
#define OSSOCK_DEFAULT_TASK_STACKSIZE       (32*1024)

/// Ĭ��Socket����Buf��С
#define OSSOCK_DEFAULT_SENDBUFLEN           40960

/// Ĭ��Socket����Buf��С
#define OSSOCK_DEFAULT_RECVBUFLEN           40960

/// Ĭ��Socket�������Դ���
#define OSSOCK_DEFAULT_SENDRETRYTIMES       20

/// Ĭ��Socket�������и���(TCPServer)
#define OSSOCK_DEFAULT_LISTENNUM            10

/// ��������Ӧ�������������
#define OSSOCK_LANAPP_TASK_MAX_COUNT        255


/// ����Sock����
#define DCOP_CreateSock(dwSockFlag, cpszIP, wPort) \
    objSock::CreateInstance(dwSockFlag, cpszIP, wPort, __FILE__, __LINE__)


/// -------------------------------------------------
/// sock����
/// -------------------------------------------------
class objSock
{
public:

    /// Socket�������
    typedef unsigned int SOCKET;                        // SOCK���

    /// SOCK����
    typedef enum tagOSSOCK_FLAG                         // Socket����
    {
        OSSOCK_NONE = 0,                                // û�д����׽���
        OSSOCK_UDP,                                     // UDP��ʽ���׽���
        OSSOCK_TCPSERVER,                               // TCP����˴������׽���
        OSSOCK_TCPACCEPT,                               // TCP����˽��տͻ������ӵ��׽���
        OSSOCK_TCPCLIENT                                // TCP�ͻ��˴������׽���
    }OSSOCK_FLAG;

public:
    static objSock *CreateInstance(DWORD dwSockFlag, 
                        const char *cpszIP, WORD wPort, 
                        const char *file,
                        int line);
    virtual ~objSock() = 0;

    /// ---------------------------------------------
    /// ��ȡ�׽��ֲ���
    /// ---------------------------------------------
    virtual DWORD       dwGetSockFlag() = 0;
    virtual DWORD       dwGetHostID() = 0;
    virtual const char *cszGetHostIP() = 0;
    virtual WORD        wGetHostPort() = 0;
    virtual int         iGetSendBufLen() = 0;
    virtual int         iGetRecvBufLen() = 0;
    virtual int         iGetLingerTime() = 0;
    virtual int         iGetListenNum() = 0;
    virtual bool        bGetTcpNoDelay() = 0;
    virtual bool        bGetUdpBind() = 0;
    virtual bool        bGetUdpBoardcast() = 0;
    virtual int         iGetSendTryTimes() = 0;
    virtual SOCKET      sGetSock() = 0;
    virtual const void *cpGetSockAddr() = 0;
    virtual DWORD       dwGetSockAddrSize() = 0;

    /// ---------------------------------------------
    /// �����׽��ֲ���
    /// ---------------------------------------------
    virtual void        vSetSockFlag(DWORD dwFlag) = 0;
    virtual void        vSetHostID(DWORD dwHostID) = 0;
    virtual void        vSetHostIP(const char *cpszIP) = 0;
    virtual void        vSetHostPort(WORD wPort) = 0;
    virtual void        vSetSendBufLen(int iSendBufLen) = 0;
    virtual void        vSetRecvBufLen(int iRecvBufLen) = 0;
    virtual void        vSetLingerTime(int iLingerTime) = 0;
    virtual void        vSetListenNum(int iListenNum) = 0;
    virtual void        vSetTcpNoDelay(bool bTcpNoDelay) = 0;
    virtual void        vSetUdpBind(bool bUdpBind) = 0;
    virtual void        vSetUdpBoardcast(bool bUdpBoardcast) = 0;
    virtual void        vSetSendTryTimes(int iSendTryTimes) = 0;
    virtual void        vSetSock(SOCKET s) = 0;
    virtual void        vSetSockAddr(const void *cpsockaddr) = 0;


    /// ---------------------------------------------
    ///    �� �ر� - begin
    /// ---------------------------------------------

    virtual DWORD Open() = 0;
    virtual DWORD Close() = 0;
    virtual DWORD Shut() = 0;

    /// ---------------------------------------------
    ///    �� �ر� - end
    /// ---------------------------------------------


    /// ---------------------------------------------
    ///    TCP Server ʹ�� - begin
    /// ---------------------------------------------

    virtual DWORD Accept(
                        objSock *& rpCltSock,           // ���Զ��Sock����
                        bool bTimeControl = false,      // �Ƿ����ʱ�����
                        DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
                        /////////////////////////////////////////
                        /// bTimeControl == false ������ʱ�����
                        ///     �������׽��ֱ��������ʱ��
                        /////////////////////////////////////////
                        ) = 0;

    /// ---------------------------------------------
    ///    TCP Server ʹ�� - end
    /// ---------------------------------------------


    /// ---------------------------------------------
    ///     TCP Client ʹ�� - begin
    /// ---------------------------------------------

    /// ����Զ�˷�����
    virtual DWORD Connect(
                        const char *pDstIP,             // ����Ŀ�ĵ�ַ
                        WORD wDstPort,                  // ����Ŀ�Ķ˿�
                        bool bAsynCall = false,         // �첽����connect
                        bool bTimeControl = false,      // �Ƿ����ʱ�����
                        DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
                        ) = 0;

    /// �첽����connect������������
    virtual DWORD FinishConnect() = 0;

    /// ---------------------------------------------
    ///     TCP Client ʹ�� - end
    /// ---------------------------------------------


    /// ---------------------------------------------
    ///     TCP ����ʹ�� - begin
    /// ---------------------------------------------

    /// TCP���ͺ���
    virtual DWORD Send(
                        const void *cpBuf,              // �������ݻ�������ַ
                        DWORD dwBufLen,                 // �������ݻ������Ĵ�С
                        DWORD *pdwSent = 0,             // �ѷ��ͳ�ȥ������
                        bool bTimeControl = false,      // �Ƿ����ʱ�����
                        DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
                        ) = 0;

    /// TCP�������ݺ���
    virtual DWORD Recv(
                        void *pBuf,                     // �������
                        DWORD dwBufLen,                 // ���뱾�������Ĵ�С
                        DWORD& rdwRecvLen,              // ����յ���Ϣ�Ĵ�С
                        bool bTimeControl = false,      // �Ƿ����ʱ�����
                        DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
                        ) = 0;

    /// ---------------------------------------------
    ///     TCP ����ʹ�� - end
    /// ---------------------------------------------


    /// ---------------------------------------------
    ///     UDP ʹ�� - begin
    /// ---------------------------------------------

    /// UDP���ͺ���
    virtual DWORD Sendto(
                        const void *cpBuf,              // �������ݻ�������ַ
                        DWORD dwBufLen,                 // �������ݻ������Ĵ�С
                        const char *pDstIP,             // ����Ŀ�ĵ�ַ
                        WORD wDstPort                   // ����Ŀ�Ķ˿�
                        ) = 0;

    /// UDP���պ���
    virtual DWORD Recvfrom(
                        void *pBuf,                     // �������
                        DWORD dwBufLen,                 // ���뱾�������Ĵ�С
                        DWORD& rdwRecvLen,              // ����յ���Ϣ�Ĵ�С
                        char szSrcIP[OSSOCK_IPSIZE],    // ����Դ��ַ
                        WORD &rwSrcPort                 // ����Դ�˿�
                        ) = 0;

    /// ---------------------------------------------
    ///     UDP ʹ�� - end
    /// ---------------------------------------------

    /// ---------------------------------------------
    /// ��̬���� - begin
    /// ---------------------------------------------

    /// ������������ȡIP��ַ�ַ���
    static void GetIPStringByName(const char *cszName, char szIP[OSSOCK_IPSIZE]);

    /// ������������ȡIP��ֵַ
    static DWORD GetIPValueByName(const char *cszName);

    /// ����IP��ַ�ַ�����ȡIP��ֵַ
    static DWORD GetIPValueByString(const char *cszIP);

    /// ����IP��ֵַ��ȡIP��ַ�ַ���
    static void GetIPStringByValue(DWORD dwIP, char szIP[OSSOCK_IPSIZE]);
    
    /// ---------------------------------------------
    /// ��̬���� - end
    /// ---------------------------------------------

                    
};


/// -------------------------------------------------
/// ����Ӧ�ö���
/// -------------------------------------------------
class objLanApp
{
public:
    static objLanApp *CreateInstance();                         // ��ȡʵ��
    virtual ~objLanApp() = 0;                                   // �ͷ�ʵ��

    /// �������յ�һ���ͻ�������
    typedef DWORD (*FUNC_ON_ACCEPT)(
                        DWORD dwChannelID,
                        objSock *pServerSock,
                        objSock *pAcceptSock,
                        void *pUserArg);

    /// �ͻ��˳ɹ������Ϸ�����
    typedef DWORD (*FUNC_ON_CONNECT)(
                        DWORD dwChannelID,
                        objSock *pClientSock,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg);

    /// ��Զ�����ӶϿ�
    typedef void (*FUNC_ON_DISCONNECT)(
                        DWORD dwChannelID,
                        objSock *pSock,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg);

    /// �յ�����
    typedef void (*FUNC_ON_RECV)(
                        DWORD dwChannelID,
                        objSock *pSock,
                        void *pFrameBuf,
                        DWORD dwFrameLen,
                        const char *cszRemoteIP,
                        WORD wRemotePort,
                        void *pUserArg);

    /// �����¼�����
    struct LanEventProc
    {
        FUNC_ON_ACCEPT fnAcceptProc;
        void *pAcceptProcPara;
        FUNC_ON_CONNECT fnConnectProc;
        void *pConnectProcPara;
        FUNC_ON_DISCONNECT fnDisconnectProc;
        void *pDisconnectProcPara;
        FUNC_ON_RECV fnRecvProc;
        void *pRecvProcPara;
    };

    /// �ж��Ƿ�������֡
    typedef int (*FUNC_B_FRAME)(
                        void *pBuf,
                        DWORD dwLen,
                        void *pUserArg);

    /// ת���ֽ���
    typedef void (*FUNC_CHANGE_BYTES_ORDER)(
                        void *pBuf,
                        DWORD dwLen,
                        void *pUserArg);

    /// ��������֡����
    struct LanFrameProc
    {
        FUNC_B_FRAME fnBFrameProc;
        void *pBFrameProcPara;
        FUNC_CHANGE_BYTES_ORDER fnChangeBytesOrderProc;
        void *pChangeBytesOrderProcPara;
    };

    /// ��־��ӡ�ص�
    typedef void (*FUNC_LOG_PRINT)(
                        const char *cszLogInfo,
                        const char *pFile,
                        DWORD dwLine,
                        void *pUserArg);

    /// ������־����
    struct LanLogProc
    {
        FUNC_LOG_PRINT fnLogProc;
        void *pLogProcPara;
    };

public:

    /// ��ȡ����ID
    virtual DWORD GetLocalID(
                        ) = 0;

    /// ��ȡͨ���׽���
    virtual objSock *GetChannel(
                        DWORD dwChannelID
                        ) = 0;

    /// ���TCPServerͨ��
    virtual DWORD AddTCPServer(
                        DWORD dwChannelID,
                        WORD wLocalPort
                        ) = 0;

    /// ���TCPClientͨ��
    virtual DWORD AddTCPClient(
                        DWORD dwChannelID,
                        const char *szRemoteIP,
                        WORD wRemotePort
                        ) = 0;

    /// ���UDPͨ��
    virtual DWORD AddUDP(
                        DWORD dwChannelID,
                        bool bBind,
                        WORD wLocalPort,
                        bool bBoardcast
                        ) = 0;

    /// ɾ��ͨ��
    virtual void DelChannel(
                        DWORD dwChannelID
                        ) = 0;

    /// ����Ӧ��
    virtual DWORD Start(
                        const char *cszAppName,
                        DWORD dwLocalID,
                        DWORD dwTaskCount,
                        LanEventProc *pEventProc,
                        LanFrameProc *pFrameProc = 0,
                        LanLogProc *pLogProc = 0
                        ) = 0;

    /// ֹͣӦ��
    virtual void Stop() = 0;

};


#endif // #ifndef _SOCK_H_

