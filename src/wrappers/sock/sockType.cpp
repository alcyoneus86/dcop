/// -------------------------------------------------
/// sockType.cpp : socket��װ��ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "sockType.h"

#if DCOP_OS == DCOP_OS_WINDOWS
#pragma comment(lib, "ws2_32")
#endif


/*******************************************************
  �� �� ��: objSock::CreateInstance
  ��    ��: ����sock
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objSock *objSock::CreateInstance(DWORD dwSockFlag, const char *cpszIP, WORD wPort, 
                        const char *file, int line)
{
    #undef new
    CSockBase *pSockBase =  new (file, line) CSockBase();
    #define new new(__FILE__, __LINE__)
    if (pSockBase)
    {
        pSockBase->vSetSockFlag(dwSockFlag);
        pSockBase->vSetHostIP(cpszIP);
        pSockBase->vSetHostPort(wPort);
    }

    return pSockBase;
}

/*******************************************************
  �� �� ��: objSock::~objSock
  ��    ��: sock����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objSock::~objSock()
{
}

/*******************************************************
  �� �� ��: objSock::GetIPStringByName
  ��    ��: ��ȡIP
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void objSock::GetIPStringByName(const char *cszName, char szIP[OSSOCK_IPSIZE])
{
    struct hostent *host = gethostbyname(cszName);

    (void)strncpy(szIP, (char *)inet_ntoa(*(struct in_addr *)(host->h_addr)), OSSOCK_IPSIZE);

    szIP[OSSOCK_IPSIZE - 1] = '\0';
}

/*******************************************************
  �� �� ��: objSock::GetIPValueByName
  ��    ��: ��ȡIP
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD objSock::GetIPValueByName(const char *cszName)
{
    struct hostent *host = gethostbyname(cszName);

    return (*(struct in_addr *)(host->h_addr)).s_addr;
}

/*******************************************************
  �� �� ��: objSock::GetIPValueByString
  ��    ��: IP�ַ���תֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD objSock::GetIPValueByString(const char *cszIP)
{
    return inet_addr((char *)cszIP);
}

/*******************************************************
  �� �� ��: objSock::GetIPStringByValue
  ��    ��: IPֵת�ַ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void objSock::GetIPStringByValue(DWORD dwIP, char szIP[OSSOCK_IPSIZE])
{
    struct in_addr in;
    in.s_addr = dwIP;

    (void)strncpy(szIP, (char *)inet_ntoa(in), OSSOCK_IPSIZE);

    szIP[OSSOCK_IPSIZE - 1] = '\0';
}

/*******************************************************
  �� �� ��: CSockBase::CSockBase
  ��    ��: CSockBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSockBase::CSockBase()
{
#if DCOP_OS == DCOP_OS_WINDOWS
    static bool dwWinSockInit = false;
    if (!dwWinSockInit)
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 2 );

        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            return;
        }

        /* Confirm that the WinSock DLL supports 2.2.*/
        /* Note that if the DLL supports versions greater    */
        /* than 2.2 in addition to 2.2, it will still return */
        /* 2.2 in wVersion since that is the version we      */
        /* requested.                                        */
         
        if ( LOBYTE( wsaData.wVersion ) != 2 ||
                HIBYTE( wsaData.wVersion ) != 2 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            WSACleanup( );
            return; 
        }

        /* The WinSock DLL is acceptable. Proceed. */

        dwWinSockInit = true;
    }
#endif

    /// ��ʼ��

    vSetSockFlag(OSSOCK_NONE);
    vSetHostID(0);
    vSetHostIP(0);
    vSetHostPort(OSSOCK_DEFAULT_PORT);
    vSetSendBufLen(OSSOCK_DEFAULT_SENDBUFLEN);
    vSetRecvBufLen(OSSOCK_DEFAULT_RECVBUFLEN);
    vSetLingerTime(0);
    vSetListenNum(OSSOCK_DEFAULT_LISTENNUM);
    vSetTcpNoDelay(false);
    vSetUdpBind(true);
    vSetUdpBoardcast(false);
    vSetSendTryTimes(OSSOCK_DEFAULT_SENDRETRYTIMES);
    vSetSock(INVALID_SOCKET);

    struct sockaddr_in __tmp;
    __tmp.sin_family            = AF_INET;
    __tmp.sin_port              = htons(wGetHostPort());
    __tmp.sin_addr.s_addr       = htonl(INADDR_ANY);
    vSetSockAddr(&__tmp);
}

/*******************************************************
  �� �� ��: CSockBase::~CSockBase
  ��    ��: CSockBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSockBase::~CSockBase()
{
    /// ע: ���Ƶ�SOCK��������ΪOSSOCK_NONE��ֻ�����ڷ�����Ϣ
    ///     (������ʱ�޷��Զ��رգ�����Ҫʱ���Ե���Shut/Close)

    if (dwGetSockFlag() != OSSOCK_NONE)
    {
        (void)Shut();
        (void)Close();
        vSetSockFlag(OSSOCK_NONE);
    }

    /// windowsҪ�����е�socket�˳�ʱʹ��WSACleanup
    /// ��������ֻ�Ƿ�װһ���⣬�޷�֪���˳��ĵط�
    /// �����Ȳ������˳�����(ĿǰҲ��û�з��ִ���)
    /// (���캯�����ڵ�һ�γ�ʼ��ʱ������WSAStartup)
}

/*******************************************************
  �� �� ��: CSockBase::Open
  ��    ��: ��sock
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Open()
{
    int iRc = 0;
    DWORD dwRc = SUCCESS;
    int optVal;
    linger lingerOpt;
    SOCKET hostSock = sGetSock();

    struct sockaddr_in __tmp;
    __tmp.sin_family            = AF_INET;
    __tmp.sin_port              = htons(wGetHostPort());
    if (*cszGetHostIP() == '\0')
        __tmp.sin_addr.s_addr   = htonl(INADDR_ANY);
    vSetSockAddr(&__tmp);

    if ((dwGetSockFlag() == OSSOCK_NONE) || (hostSock != INVALID_SOCKET))
    {
        return ERRCODE_SOCK_FLAG_NON_NONE;
    }

    ////////////////////////////////////////////////////
    /// ��ʼ���׽��֣������ò�ͬ��ѡ��
    ////////////////////////////////////////////////////

    switch (dwGetSockFlag())
    {
        case OSSOCK_TCPSERVER :
        case OSSOCK_TCPCLIENT :
        {
            hostSock = socket(AF_INET, SOCK_STREAM, 0);
            ERROR_CHECK(hostSock != INVALID_SOCKET);

            /// �����ظ�ʹ��ѡ��
            optVal = 1;
            iRc = setsockopt(hostSock,SOL_SOCKET, SO_REUSEADDR, (char *)&optVal, sizeof(optVal));
            ERROR_CHECK(iRc != SOCKET_ERROR);

            /// ���ñ�������ѡ��
            iRc = setsockopt(hostSock, SOL_SOCKET, SO_KEEPALIVE, (char *)&optVal, sizeof(optVal));
            ERROR_CHECK(iRc != SOCKET_ERROR);

            /// �����ӳ�ѡ��
            optVal = (bGetTcpNoDelay())? 1 : 0;
            iRc = setsockopt(hostSock, IPPROTO_TCP, TCP_NODELAY, (char *)&optVal, sizeof(optVal));
            ERROR_CHECK(iRc != SOCKET_ERROR);

            /// ����lingerѡ��
            lingerOpt.l_onoff = 1;
            lingerOpt.l_linger = (u_short)iGetLingerTime();
            iRc = setsockopt(hostSock, SOL_SOCKET, SO_LINGER, (char *)&lingerOpt, sizeof(lingerOpt));
            ERROR_CHECK(iRc != SOCKET_ERROR);
        }
            break;
        case OSSOCK_UDP :
        {
            hostSock = socket(AF_INET, SOCK_DGRAM, 0);

            /// �����ظ�ʹ��ѡ��
            optVal = 1;
            iRc = setsockopt(hostSock,SOL_SOCKET, SO_REUSEADDR, (char *)&optVal, sizeof(optVal));
            ERROR_CHECK(iRc != SOCKET_ERROR);

            /// ����UDP �Ĺ㲥����
            if (bGetUdpBoardcast())
            {
                optVal = 1;
                iRc = setsockopt(hostSock, SOL_SOCKET, SO_BROADCAST, (char *)&optVal, sizeof(optVal));
                ERROR_CHECK(iRc != SOCKET_ERROR);
            }
        }
            break;
        default :
            /// ��Ϊ��, �����κδ���(����TCPServer�յ��ͻ������ӵ��׽��ָ���)
            return SUCCESS;
    }

    ERROR_CHECK(hostSock > 0);


    ////////////////////////////////////////////////////
    /// �������е��׽��ֹ��е�ѡ��
    ////////////////////////////////////////////////////

    /// ���÷��ͻ�������С
    optVal = iGetSendBufLen();
    iRc = setsockopt(hostSock, SOL_SOCKET, SO_SNDBUF, (char *)&optVal, sizeof(optVal));
    ERROR_CHECK(iRc != SOCKET_ERROR);

    /// ���÷��ͻ�������С
    optVal = iGetRecvBufLen();
    iRc = setsockopt(hostSock, SOL_SOCKET, SO_RCVBUF, (char *)&optVal, sizeof(optVal));
    ERROR_CHECK(iRc != SOCKET_ERROR);


    ////////////////////////////////////////////////////
    /// �󶨵���ַ�Ͷ˿ڣ������ò�ͬ��ѡ��
    ////////////////////////////////////////////////////

    switch (dwGetSockFlag())
    {
        case OSSOCK_TCPSERVER :
        {
            /// �󶨵����ص�ַ�Ͷ˿�
            iRc = bind(hostSock, (struct sockaddr *)cpGetSockAddr(), dwGetSockAddrSize());
            ERROR_CHECK(iRc != SOCKET_ERROR);
 
            iRc = listen(hostSock, iGetListenNum());
            ERROR_CHECK(iRc != SOCKET_ERROR);
        }
            break;
        case OSSOCK_TCPCLIENT :
        {
        }
            break;
        case OSSOCK_UDP :
        {
            if (bGetUdpBind())
            {
                iRc = bind(hostSock, (struct sockaddr *)cpGetSockAddr(), dwGetSockAddrSize());
                ERROR_CHECK(iRc != SOCKET_ERROR);
            }
        }
            break;
        default :
        {
            iRc = SOCKET_ERROR;
            ERROR_CHECK(iRc != SOCKET_ERROR);
        }
            break;
    }

    ERROR_CHECK(dwRc == SUCCESS);

    /// ����SockΪ������
    (void)setnonblock(hostSock);

    vSetSock(hostSock);
    return SUCCESS;


    ////////////////////////////////////////////////////
    /// ɾ��������ʧ�ܵ����Ѿ��������׽���
    ////////////////////////////////////////////////////

ERROR_LABEL :

    dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = ERRCODE_SOCK_CREATE_FAILURE;
    }

    if (hostSock > 0)
    {
        close(hostSock);
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSockBase::Accept
  ��    ��: TCPServer����TCPClient����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Accept(objSock *& rpCltSock, 
                    bool bTimeControl, 
                    DWORD dwWaitTime)
{
    struct timeval wait;
    fd_set fdread;
    fd_set fdexcept;
    int iRc;
    struct sockaddr_in client;
    int iAddrSize;
    SOCKET sClient;
    int optVal;
    SOCKET hostSock = sGetSock();
    rpCltSock = 0;

    if ((dwGetSockFlag() != OSSOCK_TCPSERVER) || (INVALID_SOCKET == hostSock))
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    if (bTimeControl)
    {
        FD_ZERO(&fdread);
        FD_ZERO(&fdexcept);
        FD_SET(hostSock, &fdread);
        FD_SET(hostSock, &fdexcept);

        struct timeval *pwait = NULL;
        if (dwWaitTime != OSWAIT_FOREVER)
        {
            wait.tv_sec = (dwWaitTime != 0)? (long)(dwWaitTime / THOUSAND) : 0;
            wait.tv_usec = (dwWaitTime != 0)? (long)((long)(dwWaitTime % THOUSAND) * THOUSAND) : 0;
            pwait = &wait;
        }

        iRc = select(hostSock + 1, &fdread, NULL, &fdexcept, pwait);
        ERROR_CHECK(iRc >= 0);
        if (iRc == 0)
        {
            return ERRCODE_SOCK_SELECT_TIMEOUT;
        }

        /// select����ֵ���յ����¼���socket����
        if (FD_ISSET(hostSock, &fdread))
        {
            bTimeControl = false; // ��������Ż��������Ĳ���
        }
        else
        {
            if (FD_ISSET(hostSock, &fdexcept))
            {
                return ERRCODE_SOCK_CONN_CLSOED;
            }

            ERROR_CHECK(false); // һ���ߵ�ERROR_LABEL���
        }
    }

    if (!bTimeControl)
    {
        iAddrSize = sizeof(client);
        sClient = accept(hostSock, (struct sockaddr *)&client, (socklen_t *)&iAddrSize);
        ERROR_CHECK(sClient != INVALID_SOCKET);

        rpCltSock = DCOP_CreateSock(OSSOCK_NONE,
                        (char *)inet_ntoa(client.sin_addr),
                        (WORD)ntohs(client.sin_port));
        if (!rpCltSock)
        {
            close(sClient);
            return FAILURE;
        }

        /// ʵ�������ڷ�����յ��Ŀͻ�������
        rpCltSock->vSetSockFlag(OSSOCK_TCPACCEPT);
        rpCltSock->vSetSock(sClient);
        rpCltSock->vSetSockAddr(&client);

        /// ���÷��ͻ�������С
        optVal = iGetSendBufLen();
        iRc = setsockopt(sClient, SOL_SOCKET, SO_SNDBUF, (char *)&optVal, sizeof(optVal));
        ERROR_CHECK(iRc != SOCKET_ERROR);

        /// ���÷��ͻ�������С
        optVal = iGetRecvBufLen();
        iRc = setsockopt(sClient, SOL_SOCKET, SO_RCVBUF, (char *)&optVal, sizeof(optVal));
        ERROR_CHECK(iRc != SOCKET_ERROR);

        /// ���ÿͻ��׽���Ϊ������
        iRc = setnonblock(sClient);
        ERROR_CHECK(iRc != SOCKET_ERROR);

        return SUCCESS;

    }

ERROR_LABEL :

    DWORD dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = ERRCODE_SOCK_ACCEPT_ERROR;
    }

    if (rpCltSock)
    {
        delete rpCltSock;
        rpCltSock = 0;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSockBase::Connect
  ��    ��: TCPClient����TCPServer
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Connect(const char *pDstIP, 
                    WORD wDstPort, 
                    bool bAsynCall, 
                    bool bTimeControl, 
                    DWORD dwWaitTime)
{
    struct timeval wait;
    fd_set fdwrite;
    int iRc = 0;
    struct sockaddr_in dstAddr;
    DWORD dwDstAddr;
    int optVal;
    SOCKET hostSock = sGetSock();

    if ((dwGetSockFlag() != OSSOCK_TCPCLIENT) || (INVALID_SOCKET == hostSock))
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    dwDstAddr = inet_addr((char *)pDstIP);
    dstAddr.sin_family          = AF_INET;
    dstAddr.sin_port            = htons(wDstPort);
    dstAddr.sin_addr.s_addr     = dwDstAddr;

    /// �첽��ʱ�����ģʽ�£�������connectΪ����������
    if (bAsynCall || bTimeControl)
    {
        iRc = setnonblock(hostSock);
        ERROR_CHECK(iRc != SOCKET_ERROR);
    }

    iRc = connect(hostSock, (struct sockaddr *)&dstAddr, sizeof(struct sockaddr));

    /// �첽����ʱ�ȷ��سɹ���������ҪFinishConnect
    if (bAsynCall) iRc = SUCCESS;

    /// ʧ��ʱ���Ҫʱ����ƵĻ�����Ҫ����״̬�ȴ�
    if ((iRc == SOCKET_ERROR) && bTimeControl)
    {
        FD_ZERO(&fdwrite);
        FD_SET(hostSock, &fdwrite);

        if (dwWaitTime != OSWAIT_FOREVER)
        {
            wait.tv_sec = (dwWaitTime != 0)? (long)(dwWaitTime / THOUSAND) : 0;
            wait.tv_usec = (dwWaitTime != 0)? (long)((long)(dwWaitTime % THOUSAND) * THOUSAND) : 0;
            iRc = select(hostSock + 1, NULL, &fdwrite, NULL, &wait);
            ERROR_CHECK(iRc >= 0);
        }
        else
        {
            iRc = select(hostSock + 1, NULL, &fdwrite, NULL, NULL);
            ERROR_CHECK(iRc >= 0);
        }

        if (iRc == 0)
        {
            return ERRCODE_SOCK_SELECT_TIMEOUT;
        }

        /// ʹ��getsockopt�����жϣ�����ʹ��ISSET����Ϊ׼ȷ
        optVal = sizeof(int);
        getsockopt(hostSock, SOL_SOCKET, SO_ERROR, (char *)&iRc, (socklen_t *)&optVal);
        if (iRc != 0) iRc = SOCKET_ERROR;
    }

    /// ��һ���ж�ǰ��Ĳ����Ƿ�ʧ��
    ERROR_CHECK(iRc != SOCKET_ERROR);

    /// ����SockΪ������
    (void)setnonblock(hostSock);

    return SUCCESS;

ERROR_LABEL :

    DWORD dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = ERRCODE_SOCK_CONN_ERROR;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSockBase::FinishConnect
  ��    ��: �첽����connect������������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::FinishConnect()
{
    int iRc = 0;
    SOCKET hostSock = sGetSock();

    if ((dwGetSockFlag() != OSSOCK_TCPCLIENT) ||
        (INVALID_SOCKET == hostSock))
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    /// ʹ��getsockopt�����жϣ�����ʹ��ISSET����Ϊ׼ȷ
    int optVal = sizeof(int);
    getsockopt(hostSock, SOL_SOCKET, SO_ERROR, (char *)&iRc, (socklen_t *)&optVal);
    if (iRc != 0)
    {
        DWORD dwRc = errno;
        if (dwRc == 0)
        {
            dwRc = ERRCODE_SOCK_CONN_ERROR;
        }

        return dwRc;
    }

    /// ����SockΪ������
    (void)setnonblock(hostSock);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSockBase::Send
  ��    ��: TCP����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Send(const void *cpBuf, 
                    DWORD dwBufLen, 
                    DWORD *pdwSent, 
                    bool bTimeControl, 
                    DWORD dwWaitTime)
{
    struct timeval wait;
    fd_set fdwrite;
    fd_set fdexcept;
    int iRc;
    DWORD dwHaveSent = 0;
    int iSendTryTimes = 0;
    SOCKET hostSock = sGetSock();

    if (pdwSent) *pdwSent = 0;

    if (!cpBuf || !dwBufLen)
    {
        return ERRCODE_SOCK_PARA_ERR_VALUE;
    }

    if (INVALID_SOCKET == hostSock)
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    if (bTimeControl && (dwWaitTime != OSWAIT_FOREVER))
    {
        wait.tv_sec = (dwWaitTime != 0)? (long)(dwWaitTime / THOUSAND) : 0;
        wait.tv_usec = (dwWaitTime != 0)? (long)((long)(dwWaitTime % THOUSAND) * THOUSAND) : 0;
    }

    while ((dwHaveSent < dwBufLen) && (iSendTryTimes <= iGetSendTryTimes()))
    {
        if (bTimeControl)
        {
            FD_ZERO(&fdwrite);
            FD_ZERO(&fdexcept);
            FD_SET(hostSock, &fdwrite);
            FD_SET(hostSock, &fdexcept);

            struct timeval *pwait = NULL;
            if (dwWaitTime != OSWAIT_FOREVER)
            {
                pwait = &wait;
            }

            iRc = select(hostSock + 1, NULL, &fdwrite, &fdexcept, pwait);
            if (iRc < 0) break;         // ����
            if (iRc == 0) continue;     // ��ʱ
            if (!FD_ISSET(hostSock, &fdwrite))
            {
                if (FD_ISSET(hostSock, &fdexcept))
                {
                    break;              // �쳣�¼�
                }

                continue;               // �����¼�
            }

            /// �ܼ�������ȥ�ģ������յ���д�¼������Խ���Send
        }

        iRc = send(hostSock, 
                    (char *)((BYTE *)cpBuf + dwHaveSent), 
                    (int)(dwBufLen - dwHaveSent), 
                    MSG_NOSIGNAL);
        if (iRc > 0)
        {
            dwHaveSent += (DWORD)iRc;
        }

        iSendTryTimes++;
    }

    if (pdwSent) *pdwSent = dwHaveSent;

    if (dwHaveSent >= dwBufLen)
    {
        return SUCCESS;
    }

    DWORD dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = ERRCODE_SOCK_SEND_ERROR;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSockBase::Recv
  ��    ��: TCP����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Recv(void *pBuf, 
                    DWORD dwBufLen, 
                    DWORD& rdwRecvLen, 
                    bool bTimeControl, 
                    DWORD dwWaitTime)
{
    struct timeval wait;
    fd_set fdread;
    fd_set fdexcept;
    int iRc;
    SOCKET hostSock = sGetSock();

    if (!pBuf || !dwBufLen)
    {
        return ERRCODE_SOCK_PARA_ERR_VALUE;
    }

    if (INVALID_SOCKET == hostSock)
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    if (bTimeControl)
    {
        FD_ZERO(&fdread);
        FD_ZERO(&fdexcept);
        FD_SET(hostSock, &fdread);
        FD_SET(hostSock, &fdexcept);

        if (dwWaitTime != OSWAIT_FOREVER)
        {
            wait.tv_sec = (dwWaitTime != 0)? (long)(dwWaitTime / THOUSAND) : 0;
            wait.tv_usec = (dwWaitTime != 0)? (long)((long)(dwWaitTime % THOUSAND) * THOUSAND) : 0;
            iRc = select(hostSock + 1, &fdread, NULL, &fdexcept, &wait);
            ERROR_CHECK(iRc >= 0);
        }
        else
        {
            iRc = select(hostSock + 1, &fdread, NULL, &fdexcept, NULL);
            ERROR_CHECK(iRc >= 0);
        }

        if (iRc == 0)
        {
            return ERRCODE_SOCK_SELECT_TIMEOUT;
        }

        /// select����ֵ���յ����¼���socket����
        if (FD_ISSET(hostSock, &fdread))
        {
            bTimeControl = false; // ��������Ż��������Ĳ���
        }
        else
        {
            if (FD_ISSET(hostSock, &fdexcept))
            {
                return ERRCODE_SOCK_CONN_CLSOED;
            }

            ERROR_CHECK(false); // һ���ߵ�ERROR_LABEL���
        }
    }

    if (!bTimeControl)
    {
        if ((dwGetSockFlag() != OSSOCK_TCPACCEPT) &&
            (dwGetSockFlag() != OSSOCK_TCPCLIENT))
        {
            return ERRCODE_SOCK_FLAG_ERR_VALUE;
        }

        iRc = recv(hostSock, (char *)pBuf, (int)dwBufLen, 0);
        if (iRc > 0)
        {
            rdwRecvLen = (DWORD)iRc;
            return SUCCESS;
        }

        if (iRc == 0) // �������ʾ "graceful closed!", Ҳһ�����ش���
        {
            return ERRCODE_SOCK_CONN_CLSOED;
        }

        ERROR_CHECK(false); // һ���ߵ�ERROR_LABEL���
    
    }

ERROR_LABEL :

    DWORD dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = ERRCODE_SOCK_RECV_ERROR;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSockBase::Sendto
  ��    ��: UDP����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Sendto(const void *cpBuf, 
                    DWORD dwBufLen, 
                    const char *pDstIP, 
                    WORD wDstPort
                    )
{
    int iRc = 0;
    struct sockaddr_in dstAddr;
    DWORD dwDstAddr;
    DWORD dwHaveSent = 0;
    int iSendTryTimes = 0;
    SOCKET hostSock = sGetSock();

    if (!cpBuf || !dwBufLen)
    {
        return ERRCODE_SOCK_PARA_ERR_VALUE;
    }

    if ((dwGetSockFlag() != OSSOCK_UDP) ||
        (INVALID_SOCKET == hostSock))
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    dwDstAddr = inet_addr((char *)pDstIP);
    dstAddr.sin_family          = AF_INET;
    dstAddr.sin_port            = htons(wDstPort);
    dstAddr.sin_addr.s_addr     = dwDstAddr;

    while ((dwHaveSent < dwBufLen) &&
        (iSendTryTimes <= iGetSendTryTimes()))
    {
        iRc = sendto(hostSock, 
                    (char *)((BYTE *)cpBuf + dwHaveSent),
                    (int)(dwBufLen - dwHaveSent),
                    MSG_NOSIGNAL, 
                    (struct sockaddr *)&dstAddr, 
                    sizeof(struct sockaddr));
        if (iRc > 0)
        {
            dwHaveSent += (DWORD)iRc;
        }

        iSendTryTimes++;
    }

    if (dwHaveSent >= dwBufLen)
    {
        return SUCCESS;
    }
    else
    {
        DWORD dwRc = errno;
        if (dwRc == 0)
        {
            dwRc = ERRCODE_SOCK_SEND_ERROR;
        }

        return dwRc;
    }
}

/*******************************************************
  �� �� ��: CSockBase::Recvfrom
  ��    ��: UDP����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Recvfrom(void *pBuf, 
                    DWORD dwBufLen, 
                    DWORD& rdwRecvLen, 
                    char szSrcIP[OSSOCK_IPSIZE], 
                    WORD &rwSrcPort
                    )
{
    int iRc = 0;
    struct sockaddr_in dstAddr;
    int len;
    SOCKET hostSock = sGetSock();

    if (!pBuf || !dwBufLen)
    {
        return ERRCODE_SOCK_PARA_ERR_VALUE;
    }

    if ((dwGetSockFlag() != OSSOCK_UDP) ||
        (INVALID_SOCKET == hostSock))
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    len = sizeof(dstAddr);

    iRc = recvfrom(hostSock, 
                    (char *)pBuf, 
                    (int)dwBufLen, 
                    0, 
                    (struct sockaddr *)&dstAddr, 
                    (socklen_t *)&len);
    if (iRc > 0)
    {
        if (szSrcIP)
        {
            (void)strncpy(szSrcIP, (char *)inet_ntoa(dstAddr.sin_addr), OSSOCK_IPSIZE);
            szSrcIP[OSSOCK_IPSIZE - 1] = '\0';
        }
        rwSrcPort = (WORD)ntohs(dstAddr.sin_port);

        rdwRecvLen = (DWORD)iRc;
        return SUCCESS;
    }

    DWORD dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = ERRCODE_SOCK_RECV_ERROR;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSockBase::Close
  ��    ��: ֱ�ӹر�
            [�Ͽ����Ӽ�ֱ���ͷž��]
            [���ڽ����߳�ǿ�ƹر�]
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Close()
{
    SOCKET hostSock = sGetSock();
    if (hostSock == INVALID_SOCKET)
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    int iRc = close(hostSock);
    if (iRc >= 0)
    {
        vSetSock(INVALID_SOCKET);
        return SUCCESS;
    }

    DWORD dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSockBase::Shut
  ��    ��: ���Źر�
            [�жϷ���ͨ���������ͷž��]
            [���ڷ����̴߳����������Ž����߳���Recv�ӿڷ��ش���ʱ�ٵ���Close�ر�]
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSockBase::Shut()
{
    SOCKET hostSock = sGetSock();
    if (hostSock == INVALID_SOCKET)
    {
        return ERRCODE_SOCK_INVALID_SOCKET;
    }

    /// ��2������how==1����ָ��ֹ����"����"��ͬʱ�����Զ��׽��ֽ���ر�����
    int iRc = shutdown(hostSock, 1);
    if (iRc >= 0)
    {
        return SUCCESS;
    }

    DWORD dwRc = errno;
    if (dwRc == 0)
    {
        dwRc = FAILURE;
    }

    return dwRc;
}

