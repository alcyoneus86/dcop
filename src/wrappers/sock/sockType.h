/// -------------------------------------------------
/// sockType.h : socket��װ�๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SOCKTYPE_IF_H_
#define _SOCKTYPE_IF_H_

#include "../osBase.h"
#include "sock.h"

#ifdef _MSC_VER
    #pragma warning(disable:4005)
    #pragma warning(disable:4244)
    #pragma warning(disable:4311)
    #pragma warning(disable:4312)
#endif

#if DCOP_OS == DCOP_OS_WINDOWS
#define FD_SETSIZE 1024 // windows��selectĬ����64����������Ϊ����1024
#include <winsock2.h>

    /// ---------------------------------------------
    /// windows�����÷����� : ioctlsocket(server_socket, FIONBIO, &ul); int ul = 1
    /// windows��send�������һ������ : һ������Ϊ0
    /// windows�º��뼶ʱ���ȡ : GetTickCount()
    /// windows�±������ӿ�ws2_32.lib
    /// ---------------------------------------------

    #ifndef MSG_NOSIGNAL
        #define MSG_NOSIGNAL 0
    #endif

    #define close closesocket
    #define errno WSAGetLastError()
    #define ioctl(s, cmd, pt) ioctlsocket(s, cmd, (unsigned long *)(pt))

    inline int setnonblock(int sockFd)
    {
        int optVal = 1;
        return ioctl(sockFd, (int)FIONBIO, (int)(char *)&optVal);
    }

    #define socklen_t int

#elif DCOP_OS == DCOP_OS_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

    /// ---------------------------------------------
    /// linux�����÷����� : fcntl(server_socket, F_SETFL, O_NONBLOCK); <fcntl.h>
    /// linux��send�������һ������ : �����Ǻ󼸸������õ���socket�У�ֵ�����������ͣ���1
    ///      ��һ��˵��������ΪMSG_NOSIGNAL : ��ֹsend()������ϵͳ�����쳣��Ϣ��ϵͳ�������BrokePipe
    /// linux�º��뼶ʱ���ȡ : gettimeofday()
    /// linux��������ʹ�ò��� : -lstdc (����ʱ��Ҫlibstdc++.so.5������/usr/libĿ¼�д���һ������)
    /// ---------------------------------------------

    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET (SOCKET)(~0)
    #endif

    #ifndef SOCKET_ERROR
        #define SOCKET_ERROR   (-1)
    #endif

    inline int setnonblock(int sockFd)
    {
        int optVal = fcntl(sockFd, F_GETFL, 0);
        return fcntl(sockFd, F_SETFL, optVal|O_NONBLOCK);
    }

#endif


class CSockBase : public objSock, private osBase
{
public:
    CSockBase();
    ~CSockBase();

    /// ---------------------------------------------
    /// ��ȡ�׽��ֲ���
    /// ---------------------------------------------
    DWORD           dwGetSockFlag() {return m_dwSockFlag;}
    DWORD           dwGetHostID() {return m_dwHostID;}
    const char *    cszGetHostIP() {return m_szHostIP;}
    WORD            wGetHostPort() {return m_wHostPort;}
    int             iGetSendBufLen() {return m_iSendBufLen;}
    int             iGetRecvBufLen() {return m_iRecvBufLen;}
    int             iGetLingerTime() {return m_iLingerTime;}
    int             iGetListenNum() {return m_iListenNum;}
    bool            bGetTcpNoDelay() {return m_bTcpNoDelay;}
    bool            bGetUdpBind() {return m_bUdpBind;}
    bool            bGetUdpBoardcast() {return m_bUdpBoardcast;}
    int             iGetSendTryTimes() {return m_iSendTryTimes;}
    SOCKET          sGetSock() {return (SOCKET)(size_t)(hGetHandle());}
    const void *    cpGetSockAddr() {return &m_HostSockAddr;}
    DWORD           dwGetSockAddrSize() {return sizeof(m_HostSockAddr);}

    /// ---------------------------------------------
    /// �����׽��ֲ���
    /// ---------------------------------------------
    void            vSetSockFlag(DWORD dwFlag) {m_dwSockFlag = dwFlag;}
    void            vSetHostID(DWORD dwHostID) {m_dwHostID = dwHostID;}
    void            vSetHostIP(const char *cpszIP)
                    {
                        (void)memset(m_szHostIP, 0, sizeof(m_szHostIP));
                        if (cpszIP)
                        {
                            (void)strncpy(m_szHostIP, cpszIP, sizeof(m_szHostIP));
                            m_szHostIP[sizeof(m_szHostIP) - 1] = '\0';
                        }
                    }
    void            vSetHostPort(WORD wPort) {m_wHostPort = wPort;}
    void            vSetSendBufLen(int iSendBufLen) {m_iSendBufLen = iSendBufLen;}
    void            vSetRecvBufLen(int iRecvBufLen) {m_iRecvBufLen = iRecvBufLen;}
    void            vSetLingerTime(int iLingerTime) {m_iLingerTime = iLingerTime;}
    void            vSetListenNum(int iListenNum) {m_iListenNum = iListenNum;}
    void            vSetTcpNoDelay(bool bTcpNoDelay) {m_bTcpNoDelay = bTcpNoDelay;}
    void            vSetUdpBind(bool bUdpBind) {m_bUdpBind = bUdpBind;}
    void            vSetUdpBoardcast(bool bUdpBoardcast) {m_bUdpBoardcast = bUdpBoardcast;}
    void            vSetSendTryTimes(int iSendTryTimes) {m_iSendTryTimes = iSendTryTimes;}
    void            vSetSock(SOCKET s) {vSetHandle((OSHANDLE)s);}
    void            vSetSockAddr(const void *cpsockaddr)
                    {
                        m_HostSockAddr.sin_family = ((sockaddr_in *)cpsockaddr)->sin_family;
                        m_HostSockAddr.sin_port = ((sockaddr_in *)cpsockaddr)->sin_port;
                        m_HostSockAddr.sin_addr.s_addr = ((sockaddr_in *)cpsockaddr)->sin_addr.s_addr;
                        for (int i = 0; i < 8; i++)
                        {
                            m_HostSockAddr.sin_zero[i] = 0;
                        }
                    }

    DWORD Open();

    /// ---------------------------------------------
    ///    TCP Server ʹ�� - begin
    /// ---------------------------------------------

    DWORD Accept(
            objSock *& rpCltSock,           // ���Զ��Sock����
            bool bTimeControl = false,      // �Ƿ����ʱ�����
            DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
            /////////////////////////////////////////
            /// bTimeControl == false ������ʱ�����
            ///     �������׽��ֱ��������ʱ��
            /////////////////////////////////////////
            );

    /// ---------------------------------------------
    ///    TCP Server ʹ�� - end
    /// ---------------------------------------------


    /// ---------------------------------------------
    ///     TCP Client ʹ�� - begin
    /// ---------------------------------------------

    /// ����Զ�˷�����
    DWORD Connect(
            const char *pDstIP,             // ����Ŀ�ĵ�ַ
            WORD wDstPort,                  // ����Ŀ�Ķ˿�
            bool bAsynCall = false,         // �첽����connect
            bool bTimeControl = false,      // �Ƿ����ʱ�����
            DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
            );

    /// �첽����connect������������
    DWORD FinishConnect();

    /// ---------------------------------------------
    ///     TCP Client ʹ�� - end
    /// ---------------------------------------------


    /// ---------------------------------------------
    ///     TCP ����ʹ�� - begin
    /// ---------------------------------------------

    /// TCP���ͺ���
    DWORD Send(
            const void *cpBuf,              // �������ݻ�������ַ
            DWORD dwBufLen,                 // �������ݻ������Ĵ�С
            DWORD *pdwSent = 0,             // �ѷ��ͳ�ȥ������
            bool bTimeControl = false,      // �Ƿ����ʱ�����
            DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
            );

    /// TCP�������ݺ���
    DWORD Recv(
            void *pBuf,                     // �������
            DWORD dwBufLen,                 // ���뱾�������Ĵ�С
            DWORD& rdwRecvLen,              // ����յ���Ϣ�Ĵ�С
            bool bTimeControl = false,      // �Ƿ����ʱ�����
            DWORD dwWaitTime = 0            // �ȴ�ʱ��ֵ
            );

    /// ---------------------------------------------
    ///     TCP ����ʹ�� - end
    /// ---------------------------------------------


    /// ---------------------------------------------
    ///     UDP ʹ�� - begin
    /// ---------------------------------------------

    /// UDP���ͺ���
    DWORD Sendto(
            const void *cpBuf,              // �������ݻ�������ַ
            DWORD dwBufLen,                 // �������ݻ������Ĵ�С
            const char *pDstIP,             // ����Ŀ�ĵ�ַ
            WORD wDstPort                   // ����Ŀ�Ķ˿�
            );

    /// UDP���պ���
    DWORD Recvfrom(
            void *pBuf,                     // �������
            DWORD dwBufLen,                 // ���뱾�������Ĵ�С
            DWORD& rdwRecvLen,              // ����յ���Ϣ�Ĵ�С
            char szSrcIP[OSSOCK_IPSIZE],    // ����Դ��ַ
            WORD &rwSrcPort                 // ����Դ�˿�
            );

    /// ---------------------------------------------
    ///     UDP ʹ�� - end
    /// ---------------------------------------------

    DWORD Close();
    DWORD Shut();

private:

    /// ---------------------------------------------
    ///     �׽��ֱ�ʶ
    /// ---------------------------------------------
    /// δ����      : OSSOCK_NONE
    /// UDP         : OSSOCK_UDP
    /// TCP Server  : OSSOCK_TCPSERVER
    /// TCP Accept  : OSSOCK_TCPACCEPT
    /// TCP Client  : OSSOCK_TCPCLIENT
    /// ---------------------------------------------
    DWORD m_dwSockFlag;                     // �׽��ֶ����ʶ

    DWORD m_dwHostID;                       // ����ID

    char m_szHostIP[OSSOCK_IPSIZE];         // ����IP��ַ
    WORD m_wHostPort;                       // �����˿�
    WORD m_wReserved;

    /// ---------------------------------------------
    /// �׽���ѡ��
    /// ---------------------------------------------
    int m_iSendBufLen;
    int m_iRecvBufLen;
    int m_iLingerTime;
    int m_iListenNum;
    bool m_bTcpNoDelay;
    bool m_bUdpBind;
    bool m_bUdpBoardcast;

    /// ---------------------------------------------
    /// δ��ɷ�������ʱ�����Դ���
    /// ---------------------------------------------
    int m_iSendTryTimes;

    struct sockaddr_in m_HostSockAddr;      // ���׽�����Ϣ

};


#endif // #ifndef _SOCKTYPE_IF_H_

