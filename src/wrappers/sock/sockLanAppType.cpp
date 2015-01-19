/// -------------------------------------------------
/// sockLanAppType.cpp : sockӦ�÷�װ��ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "sockLanAppType.h"
#include <stdio.h>


/*******************************************************
  �� �� ��: objLanApp::CreateInstance
  ��    ��: ��������Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objLanApp *objLanApp::CreateInstance()
{
    return new CLanAppBase();
}

/*******************************************************
  �� �� ��: objLanApp::~objLanApp
  ��    ��: ��������Ӧ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objLanApp::~objLanApp()
{
}

/*******************************************************
  �� �� ��: LanAppTaskPara::LanAppTaskPara
  ��    ��: ��������Ӧ���������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
LanAppTaskPara::LanAppTaskPara(CLanAppBase *pLanApp, CLanAppEventBus *pLanAppEventBus, DWORD dwTaskNo)
{
    if (pLanApp) pLanApp->vIncTaskRef();
    m_pRecvBuf = DCOP_Malloc(OSSOCK_DEFAULT_MTU);
    m_pLanApp = pLanApp;
    m_pLanAppEventBus = pLanAppEventBus;
    m_dwTaskNo = dwTaskNo;
}

/*******************************************************
  �� �� ��: LanAppTaskPara::~LanAppTaskPara
  ��    ��: ��������Ӧ���������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
LanAppTaskPara::~LanAppTaskPara()
{
    if (m_pLanApp) m_pLanApp->vDecTaskRef();
    if (m_pRecvBuf) DCOP_Free(m_pRecvBuf);
    m_pLanApp = 0;
    m_pLanAppEventBus = 0;
    m_pRecvBuf = 0;
    m_dwTaskNo = 0;
}

/*******************************************************
  �� �� ��: LanAppChannel::LanAppChannel
  ��    ��: ����Ӧ��ͨ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
LanAppChannel::LanAppChannel()
{
    m_dwChannelID = 0;
    m_pSock = 0;
    m_pThis = 0;
    m_process = 0;
    (void)memset(m_szRemoteIP, 0, sizeof(m_szRemoteIP));
    m_wRemotePort = 0;

    m_connected = FALSE;
    m_channelType = NONE;

    m_pSavedBuf = 0;
    m_dwSavedBufLen = 0;

    m_pEventBus = 0;
}

/*******************************************************
  �� �� ��: LanAppChannel::LanAppChannel
  ��    ��: ����Ӧ��ͨ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
LanAppChannel::LanAppChannel(const LanAppChannel &rChannel)
{
    m_dwChannelID = rChannel.m_dwChannelID;
    m_pSock = rChannel.m_pSock;
    m_pThis = rChannel.m_pThis;
    m_process = rChannel.m_process;
    if (rChannel.m_szRemoteIP)
    {
        (void)strncpy(m_szRemoteIP, rChannel.m_szRemoteIP, sizeof(m_szRemoteIP));
        m_szRemoteIP[sizeof(m_szRemoteIP) - 1] = '\0';
    }
    else
    {
        (void)memset(m_szRemoteIP, 0, sizeof(m_szRemoteIP));
    }
    m_wRemotePort = rChannel.m_wRemotePort;

    m_connected = rChannel.m_connected;
    m_channelType = NONE;

    m_pSavedBuf = 0;
    m_dwSavedBufLen = 0;

    m_pEventBus = 0;
}

/*******************************************************
  �� �� ��: LanAppChannel::LanAppChannel
  ��    ��: ����Ӧ��ͨ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
LanAppChannel::LanAppChannel(DWORD dwChannelID,
                        objSock *pSock,
                        CLanAppBase *pThis,
                        const char *szRemoteIP,
                        WORD wRemotePort,
                        BYTE connected)
{
    m_dwChannelID = dwChannelID;
    m_pSock = pSock;
    m_pThis = pThis;
    m_process = 0;
    if (szRemoteIP)
    {
        (void)strncpy(m_szRemoteIP, szRemoteIP, sizeof(m_szRemoteIP));
        m_szRemoteIP[sizeof(m_szRemoteIP) - 1] = '\0';
    }
    else
    {
        (void)memset(m_szRemoteIP, 0, sizeof(m_szRemoteIP));
    }
    m_wRemotePort = wRemotePort;

    m_connected = connected;
    m_channelType = NONE;

    m_pSavedBuf = 0;
    m_dwSavedBufLen = 0;

    m_pEventBus = 0;
}

/*******************************************************
  �� �� ��: LanAppChannel::~LanAppChannel
  ��    ��: ����Ӧ��ͨ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
LanAppChannel::~LanAppChannel()
{
    if (!m_pSock)
    {
        return;
    }

    DWORD dwSockFlag = m_pSock->dwGetSockFlag();

    if ((m_channelType == LOCAL) && (dwSockFlag != objSock::OSSOCK_TCPACCEPT))
    {
        /// ��Ҫ�������˶������Լ��������׽���
        delete m_pSock;
        m_pSock = 0;
    }

    if ((m_channelType == REMOTE) && (dwSockFlag == objSock::OSSOCK_TCPACCEPT))
    {
        /// ��Ҫ����Զ�˶����н��յ��Ŀͻ��׽���
        delete m_pSock;
        m_pSock = 0;
    }

    if (m_pSavedBuf)
    {
        DCOP_Free(m_pSavedBuf);
        m_pSavedBuf = 0;
    }
}

/*******************************************************
  �� �� ��: LanAppChannel::operator <
  ��    ��: <������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool LanAppChannel::operator <(const LanAppChannel &rThis) const
{
    if (m_dwChannelID < rThis.m_dwChannelID)
    {
        return true;
    }

    if (m_pSock && rThis.m_pSock &&
        (m_pSock->sGetSock() < rThis.m_pSock->sGetSock()))
    {
        return true;
    }

    return false;
}

/*******************************************************
  �� �� ��: CLanAppEventBus::CLanAppEventBus
  ��    ��: CLanAppEventBus����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLanAppEventBus::CLanAppEventBus()
{
    m_pLock = 0;

#if _USE_EPOLL_
    m_epollFd = -1;
#else
    FD_ZERO(&m_fdread);
    FD_ZERO(&m_fdwrite);
#endif
    m_chanEvts = 0;
    m_bAutoExit = true;
}

/*******************************************************
  �� �� ��: CLanAppEventBus::~CLanAppEventBus
  ��    ��: CLanAppEventBus����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLanAppEventBus::~CLanAppEventBus()
{
#if _USE_EPOLL_
    if (m_epollFd > -1)
    {
        close(m_epollFd);
        m_epollFd = -1;
    }
#endif

    if (m_chanEvts)
    {
        DCOP_Free(m_chanEvts);
        m_chanEvts = 0;
    }

    if (m_pLock)
    {
        m_pLock->Enter();
        delete m_pLock;
        m_pLock = 0;
    }
}

/*******************************************************
  �� �� ��: CLanAppEventBus::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CLanAppEventBus::Init(CLanAppBase *pLanApp)
{
    if (!pLanApp) return FAILURE;

    /// ������
    m_pLock = DCOP_CreateLock();
    if (!m_pLock)
    {
        return FAILURE;
    }

#if _USE_EPOLL_
    /// �����¼�����(EPOLLCLOEXEC��ʾִ�к�رգ���ֹ�ӽ��̸��ƾ��)
    m_epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (m_epollFd < 0)
    {
        return FAILURE;
    }

    m_chanEvts = (epoll_event *)DCOP_Malloc(sizeof(epoll_event) * EVENT_BUS_SIZE);
    if (!m_chanEvts)
    {
        return FAILURE;
    }

    /// �ѳ�ʼ��֮ǰ����������ӵľ���������¼�(��Ϊ֮ǰ��δ��ʼ���¼�����)
    for (IT_EVENTSOCKS it_event = m_eventSocks.begin(); it_event != m_eventSocks.end(); ++it_event)
    {
        AddEvent((LanAppChannel *)(&(*it_event)));
    }
#else
    m_chanEvts = (LanAppChannel **)DCOP_Malloc(sizeof(LanAppChannel *) * EVENT_BUS_SIZE);
    if (!m_chanEvts)
    {
        return FAILURE;
    }
#endif

    DWORD dwTaskCount = 
#if _USE_EPOLL_
    /// epollģ�Ϳ���ʹ�ö�������Ϊ���
    pLanApp->GetTaskCount();
#else
    /// selectģ��ֻ��ʹ�õ�������Ϊ���
    1;
#endif

    for (DWORD i = 0; i < dwTaskCount; ++i)
    {
        /// �����������
        LanAppTaskPara *pProcessTaskPara = new LanAppTaskPara(pLanApp, this, i);
        if (!pProcessTaskPara)
        {
            return FAILURE;
        }

        /// ��������
        char szTaskName[OSNAME_LENGTH];
        (void)snprintf(szTaskName, sizeof(szTaskName), "t%s%u", pLanApp->GetAppName(), (i+1));
        szTaskName[sizeof(szTaskName) - 1] = '\0';
        objTask *pProcessTask = DCOP_CreateTask(szTaskName, 
                            TaskRun, 
                            OSSOCK_DEFAULT_TASK_STACKSIZE, 
                            objTask::OSTASK_PRIORITY_NORMAL, 
                            pProcessTaskPara);
        if (!pProcessTask)
        {
            delete pProcessTaskPara;
            pProcessTaskPara = 0;
            return FAILURE;
        }
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CLanAppEventBus::Add
  ��    ��: ���ͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppEventBus::Add(LanAppChannel *pChannel)
{
    if (!pChannel) return;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    /// ��ӵ��¼�������
    {
        AutoLock(m_pLock);

        IT_EVENTSOCKS it_event = m_eventSocks.insert(m_eventSocks.end(), *pChannel);
        if (it_event == m_eventSocks.end())
        {
            return;
        }

        LanAppChannel *pChannelWrite = (LanAppChannel *)(&(*it_event));

        /// c++�±�׼�У�std:set��iterator����const���͵ģ�
        /// ��Ϊset��ֵ���ǲ����޸ĵģ����ƻ�std:set��������
        /// ���ȷ��û���ƻ���ֵ������ǿ��ת��һ��

        /// ����ֻ�ܰѽ��յ���Զ�����ӵ��׽�������ΪREMOTE (�����Ķ��Ǳ��صģ�����ɾ��)
        if (pSock->dwGetSockFlag() == objSock::OSSOCK_TCPACCEPT)
        {
            pChannelWrite->m_channelType = LanAppChannel::REMOTE;
        }

        pChannelWrite->m_pEventBus = this;

    #if _USE_EPOLL_
        AddEvent(pChannelWrite);
    #endif
    }
}

/*******************************************************
  �� �� ��: CLanAppEventBus::Del
  ��    ��: ɾ��ͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppEventBus::Del(LanAppChannel *pChannel)
{
    AutoLock(m_pLock);

    (void)m_eventSocks.erase(*pChannel);
}

/*******************************************************
  �� �� ��: CLanAppEventBus::TaskRun
  ��    ��: �������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppEventBus::TaskRun(objTask::IPara *para)
{
    LanAppTaskPara *pPara = (LanAppTaskPara *)para;
    OSASSERT(pPara != 0);

    CLanAppEventBus *pThis = pPara->m_pLanAppEventBus;
    OSASSERT(pThis != 0);

    CLanAppBase *pLanApp = pPara->m_pLanApp;
    OSASSERT(pLanApp != 0);

    DWORD dwNullEventCount = 0;

    /// ѭ�������¼�
    while (!pLanApp->bExit())
    {
    #if !_USE_EPOLL_
        pThis->SetEvents(pLanApp);
    #endif

        /// ����յ��¼�����Ϊ������������������
        int iEventCount = pThis->WaitEvents(pLanApp);
        if (iEventCount > 0)
        {
            dwNullEventCount = 0;
            pThis->ProcEvents(pPara, pLanApp, (DWORD)iEventCount);
            continue;
        }

        /// ����ֵΪ�������������쳣������ʱһ��
        if (iEventCount < 0)
        {
            objTask::Delay(0);
        }

        /// �ߵ������������¼������쳣���أ������һ��ʱ�䣬��Ҫ�����˳�����
        if (pThis->bAutoExit())
        {
            if (!pThis->Count()) dwNullEventCount++;
            if (dwNullEventCount > AUTO_EXIT_COUNT)
            {
                delete pThis;
                break;
            }
        }
    }
}

/*******************************************************
  �� �� ��: CLanAppEventBus::AddEvent
  ��    ��: ����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppEventBus::AddEvent(LanAppChannel *pChannel)
{
#if !_USE_EPOLL_

    return;

#else

    if (!pChannel) return;
    if (m_epollFd <= -1) return;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    epoll_event epollEv;
    epollEv.data.ptr = pChannel;

    /// ֻ��TCPClient��δ����ǰ��д�¼����������ͺ�������Ƕ��¼�
    if ((pSock->dwGetSockFlag() == objSock::OSSOCK_TCPCLIENT) && (pChannel->m_connected != TRUE))
    {
        /// ʹ�ô��첽��ʽ��connect
        (void)pSock->Connect(pChannel->m_szRemoteIP, pChannel->m_wRemotePort, true);
        epollEv.events = EPOLLOUT | EPOLLET;
    }
    else
    {
        epollEv.events = EPOLLIN | EPOLLET;
    }

    (void)epoll_ctl(m_epollFd, EPOLL_CTL_ADD, pSock->sGetSock(), &epollEv);

#endif
}

/*******************************************************
  �� �� ��: CLanAppEventBus::SetEvents
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppEventBus::SetEvents(CLanAppBase *pLanApp)
{
#if _USE_EPOLL_

    /// epollֻ��Ҫ���һ���¼�
    return;

#else

    if (!pLanApp) return;

    FD_ZERO(&m_fdread);
    FD_ZERO(&m_fdwrite);
    AutoLock(m_pLock);

    for (IT_EVENTSOCKS it_event = m_eventSocks.begin(); it_event != m_eventSocks.end(); ++it_event)
    {
        LanAppChannel *pChannel = (LanAppChannel *)(&(*it_event));
        if (!pChannel)
        {
            continue;
        }

        objSock *pSock = pChannel->m_pSock;
        if (!pSock)
        {
            continue;
        }

        /// ֻ��TCPClient��δ����ǰ��д�¼����������ͺ�������Ƕ��¼�
        if ((pSock->dwGetSockFlag() == objSock::OSSOCK_TCPCLIENT) && (pChannel->m_connected != TRUE))
        {
            /// ʹ�ô��첽��ʽ��connect
            (void)pSock->Connect(pChannel->m_szRemoteIP, pChannel->m_wRemotePort, true);
            FD_SET(pSock->sGetSock(), &m_fdwrite);
        }
        else
        {
            FD_SET(pSock->sGetSock(), &m_fdread);
        }
    }

#endif
}

/*******************************************************
  �� �� ��: CLanAppEventBus::WaitEvents
  ��    ��: �ȴ��¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int CLanAppEventBus::WaitEvents(CLanAppBase *pLanApp)
{
    if (!pLanApp) return (-1);

    int iRc;
    DWORD dwBlockTime = OSSOCK_DEFAULT_BLOCKTIME;

#if _USE_EPOLL_

    /// epollģ��ֱ�ӷ��صľ����յ��¼��ľ���б�
    iRc = epoll_wait(m_epollFd, m_chanEvts, EVENT_BUS_SIZE, dwBlockTime);

#else

    struct timeval wait;
    struct timeval *pwait = NULL;

    if (dwBlockTime != OSWAIT_FOREVER)
    {
        wait.tv_sec = (dwBlockTime != 0)? (long)(dwBlockTime / THOUSAND) : 0;
        wait.tv_usec = (dwBlockTime != 0)? (long)((long)(dwBlockTime % THOUSAND) * THOUSAND) : 0;
        pwait = &wait;
    }

    iRc = select(FD_SETSIZE, &m_fdread, &m_fdwrite, NULL, pwait);
    if (iRc <= 0)
    {
        return iRc;
    }

    /// selectģ����Ҫ�������о��������֪������Щ���������¼�
    {
        AutoLock(m_pLock);
        DWORD dwEventCount = (DWORD)iRc;
        DWORD dwProcCount = 0;

        /// �����׽��ֶ���
        for (IT_EVENTSOCKS it_event = m_eventSocks.begin(); it_event != m_eventSocks.end(); ++it_event)
        {
            /// Ϊ�˲��˷�Ч�ʣ��������Ѿ����յ��¼��������˳�
            if (dwProcCount >= dwEventCount)
            {
                break;
            }

            LanAppChannel *pChannel = (LanAppChannel *)(&(*it_event));
            if (!pChannel)
            {
                continue;
            }

            objSock *pSock = pChannel->m_pSock;
            if (!pSock)
            {
                continue;
            }

            if ((pSock->dwGetSockFlag() == objSock::OSSOCK_TCPCLIENT) && (pChannel->m_connected != TRUE))
            {
                /// TCPClientδ����ǰ��д�¼�
                if (FD_ISSET(pSock->sGetSock(), &m_fdwrite))
                {
                    m_chanEvts[dwProcCount++] = pChannel;
                }
            }
            else
            {
                /// ����������Ƕ��¼�
                if (FD_ISSET(pSock->sGetSock(), &m_fdread))
                {
                    m_chanEvts[dwProcCount++] = pChannel;
                }
            }
        }

        iRc = (int)dwProcCount;
    }

#endif

    return iRc;
}

/*******************************************************
  �� �� ��: CLanAppEventBus::ProcEvents
  ��    ��: �����¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppEventBus::ProcEvents(LanAppTaskPara *pPara, CLanAppBase *pLanApp, DWORD dwEventCount)
{
    if (!pLanApp) return;

    for (DWORD i = 0; i < dwEventCount; ++i)
    {
    #if _USE_EPOLL_
        LanAppChannel *pChannel = (LanAppChannel *)m_chanEvts[i].data.ptr;
    #else
        LanAppChannel *pChannel = m_chanEvts[i];
    #endif

        if (!pChannel)
        {
            continue;
        }

        /// Ϊ1��Ϊ�˱�֤��һ�ν���ʱ���ܽ����¼�����
        DWORD dwRefCount = pChannel->StartProcess();
        if (dwRefCount == 1)
        {
            pLanApp->Process(pPara, pChannel);
        }
    }
}

/*******************************************************
  �� �� ��: CLanAppBase::CLanAppBase
  ��    ��: CLanAppBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLanAppBase::CLanAppBase()
{
    (void)snprintf(m_szAppName, sizeof(m_szAppName), "LanApp");
    m_szAppName[sizeof(m_szAppName) - 1] = '\0';
    m_pLock = 0;
    m_dwLocalID = 0;
    m_dwTaskCount = 1;

    m_fnAcceptProc = 0;
    m_pAcceptProcPara = 0;
    m_fnConnectProc = 0;
    m_pConnectProcPara = 0;
    m_fnDisconnectProc = 0;
    m_pDisconnectProcPara = 0;
    m_fnRecvProc = 0;
    m_pRecvProcPara = 0;

    m_fnBFrameProc = 0;
    m_pBFrameProcPara = 0;
    m_fnChangeBytesOrderProc = 0;
    m_pChangeBytesOrderProcPara = 0;

    m_fnLogPrint = 0;
    m_pLogPrintPara = 0;

    m_startFlag = FALSE;
    m_dwTaskRefCount = 0;
    m_pTaskRefCountSem = 0;
}

/*******************************************************
  �� �� ��: CLanAppBase::~CLanAppBase
  ��    ��: CLanAppBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLanAppBase::~CLanAppBase()
{
    /// ���ڽ�������
    Stop();

    m_pLock->Enter();
    DWORD dwTaskCount = m_dwTaskRefCount;
    objCounter *pTaskRefCountSem = m_pTaskRefCountSem;
    m_pLock->Leave();

    /// ����ֱ�����е������˳�
    if (dwTaskCount && pTaskRefCountSem)
    {
        for (DWORD i = 0; i < dwTaskCount; ++i)
        {
            pTaskRefCountSem->Take();
        }
    }

    /// �ͷ���
    if (m_pLock)
    {
        m_pLock->Enter();
        delete m_pLock;
        m_pLock = 0;
    }

    /// �ͷż�����
    if (m_pTaskRefCountSem)
    {
        delete m_pTaskRefCountSem;
        m_pTaskRefCountSem = 0;
    }
}

/*******************************************************
  �� �� ��: CLanAppBase::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CLanAppBase::Init(const char *cszAppName, DWORD dwLocalID, DWORD dwTaskCount,
                        LanEventProc *pEventProc,
                        LanFrameProc *pFrameProc,
                        LanLogProc *pLogProc)
{
    /// �Ѿ���ʼ�Ͳ��ܳ�ʼ����
    if (m_startFlag)
    {
        return FAILURE;
    }

    /// ������
    m_pLock = DCOP_CreateLock();
    if (!m_pLock)
    {
        return FAILURE;
    }

    /// ��������Ȼ����г�ʼ��
    {
        AutoLock(m_pLock);

        if (cszAppName && *cszAppName)
        {
            (void)snprintf(m_szAppName, sizeof(m_szAppName), "%s", cszAppName);
            m_szAppName[sizeof(m_szAppName) - 1] = '\0';
        }

        m_dwLocalID = dwLocalID;
        m_dwTaskCount = dwTaskCount;

        if (pEventProc)
        {
            m_fnAcceptProc = pEventProc->fnAcceptProc;
            m_pAcceptProcPara = pEventProc->pAcceptProcPara;
            m_fnConnectProc = pEventProc->fnConnectProc;
            m_pConnectProcPara = pEventProc->pConnectProcPara;
            m_fnDisconnectProc = pEventProc->fnDisconnectProc;
            m_pDisconnectProcPara = pEventProc->pDisconnectProcPara;
            m_fnRecvProc = pEventProc->fnRecvProc;
            m_pRecvProcPara = pEventProc->pRecvProcPara;
        }

        if (pFrameProc)
        {
            m_fnBFrameProc = pFrameProc->fnBFrameProc;
            m_pBFrameProcPara = pFrameProc->pBFrameProcPara;
            m_fnChangeBytesOrderProc = pFrameProc->fnChangeBytesOrderProc;
            m_pChangeBytesOrderProcPara = pFrameProc->pChangeBytesOrderProcPara;
        }

        if (pLogProc)
        {
            m_fnLogPrint = pLogProc->fnLogProc;
            m_pLogPrintPara = pLogProc->pLogProcPara;
        }

        /// ����������־
        (void)objAtomic::Set(m_startFlag, TRUE);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CLanAppBase::AddChannel
  ��    ��: ���ͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::AddChannel(DWORD dwChannelID,
                        objSock *pSock,
                        const char *szRemoteIP,
                        WORD wRemotePort)
{
    AutoLock(m_pLock);

    LanAppChannel channel(dwChannelID, pSock, this, szRemoteIP, wRemotePort);

    IT_LOCALSOCKS it_local = m_localSocks.insert(m_localSocks.end(), 
                        MAP_LOCALSOCKS::value_type(dwChannelID, channel));
    if (it_local == m_localSocks.end())
    {
        /// ������û�з���ֵ��
        /// ���洴���ʹ������ˣ�
        /// ֻ���ڱ������ͷ�
        delete pSock;
        pSock = 0;

        return;
    }

    /// ���뵽�¼�������
    AddToEventBus(&((*it_local).second));

    ((*it_local).second).m_channelType = LanAppChannel::LOCAL;
}

/*******************************************************
  �� �� ��: CLanAppBase::AddTCPServer
  ��    ��: ���TCPServerͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CLanAppBase::AddTCPServer(DWORD dwChannelID,
                        WORD wLocalPort)
{
    objSock *pSock = DCOP_CreateSock(objSock::OSSOCK_TCPSERVER, 0, wLocalPort);
    if (!pSock)
    {
        return FAILURE;
    }

    if (pSock->Open() != SUCCESS)
    {
        delete pSock;
        return FAILURE;
    }

    AddChannel(dwChannelID, pSock);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CLanAppBase::AddTCPClient
  ��    ��: ���TCPClientͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CLanAppBase::AddTCPClient(DWORD dwChannelID,
                        const char *szRemoteIP,
                        WORD wRemotePort)
{
    objSock *pSock = DCOP_CreateSock(objSock::OSSOCK_TCPCLIENT, 0, 0);
    if (!pSock)
    {
        return FAILURE;
    }

    if (pSock->Open() != SUCCESS)
    {
        delete pSock;
        return FAILURE;
    }

    AddChannel(dwChannelID, pSock, szRemoteIP, wRemotePort);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CLanAppBase::AddUDP
  ��    ��: ���UDPͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CLanAppBase::AddUDP(DWORD dwChannelID,
                        bool bBind,
                        WORD wLocalPort,
                        bool bBoardcast)
{
    objSock *pSock = DCOP_CreateSock(objSock::OSSOCK_UDP, 0, ((bBind)? wLocalPort : 0));
    if (!pSock)
    {
        return FAILURE;
    }

    pSock->vSetUdpBind(bBind);
    pSock->vSetUdpBoardcast(bBoardcast);

    if (pSock->Open() != SUCCESS)
    {
        delete pSock;
        return FAILURE;
    }

    AddChannel(dwChannelID, pSock);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CLanAppBase::DelChannel
  ��    ��: ɾ��ͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::DelChannel(DWORD dwChannelID)
{
    AutoLock(m_pLock);

    IT_LOCALSOCKS it_local = m_localSocks.find(dwChannelID);
    if (it_local == m_localSocks.end())
    {
        return;
    }

    DelFromEventBus(&((*it_local).second));

    (void)m_localSocks.erase(dwChannelID);
}

/*******************************************************
  �� �� ��: CLanAppBase::Start
  ��    ��: ��ʼӦ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CLanAppBase::Start(const char *cszAppName,
                        DWORD dwLocalID,
                        DWORD dwTaskCount,
                        LanEventProc *pEventProc,
                        LanFrameProc *pFrameProc,
                        LanLogProc *pLogProc)
{
    /// ��ʼ��������
    DWORD dwRc = Init(cszAppName, dwLocalID, dwTaskCount, 
                        pEventProc, 
                        pFrameProc, 
                        pLogProc);
    if (dwRc != SUCCESS)
    {
        logPrint("LanApp Init Fail!");
        return dwRc;
    }

    /// ��ʼ����һ���¼�����
    dwRc = m_firstEvents.Init(this);
    if (dwRc != SUCCESS)
    {
        logPrint("LanApp Frist Event Bus Init Fail!");
        return dwRc;
    }

    m_firstEvents.vDisableAutoExit();
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CLanAppBase::Process
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::Process(LanAppTaskPara *pPara, LanAppChannel *pChannel)
{
    if (!pChannel) return;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    DWORD dwSockFlag = pSock->dwGetSockFlag();
    if (dwSockFlag == objSock::OSSOCK_TCPSERVER)
    {
        /// TCPServer�����׽������յ������¼�
        Accept(pPara, pChannel);
    }
    else if (dwSockFlag == objSock::OSSOCK_TCPACCEPT)
    {
        /// TCPServer�ͻ��׽������յ������¼�
        TCPRecv(pPara, pChannel);
    }
    else if (dwSockFlag == objSock::OSSOCK_TCPCLIENT)
    {
        /// TCPClientδ����ǰ��ȥ�����¼������Ӻ����������¼�
        if (pChannel->m_connected != TRUE)
        {
            Connect(pPara, pChannel);
        }
        else
        {
            TCPRecv(pPara, pChannel);
        }
    }
    else
    {
        /// �����������UDP�׽��ֵ��������¼�
        UDPRecv(pPara, pChannel);
    }
}

/*******************************************************
  �� �� ��: CLanAppBase::Accept
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::Accept(LanAppTaskPara *pPara, LanAppChannel *pChannel)
{
    if (!pPara || !pChannel) return;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    for (;;)
    {
        objSock *pCltSock = 0;
        DWORD dwRc = pSock->Accept(pCltSock);
        if (dwRc != SUCCESS)
        {
            DWORD dwRefCount = pChannel->EndProcess();
            if (dwRefCount)
            {
                continue;
            }

            break;
        }

        dwRc = OnAccept(pChannel->m_dwChannelID, pSock, pCltSock);
        if (dwRc != SUCCESS)
        {
            /// �ܾ�����
            if (pCltSock) delete pCltSock;
            continue;
        }

        /// ���뵽�¼�������
        LanAppChannel channel(pChannel->m_dwChannelID, pCltSock, this, 
                            pCltSock->cszGetHostIP(), pCltSock->wGetHostPort(), TRUE);
        AddToEventBus(&channel);
    }
}

/*******************************************************
  �� �� ��: CLanAppBase::Connect
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::Connect(LanAppTaskPara *pPara, LanAppChannel *pChannel)
{
    if (!pPara || !pChannel) return;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    DWORD dwRc = pSock->FinishConnect();
    if (dwRc != SUCCESS)
    {
        return;
    }

    dwRc = OnConnect(pChannel->m_dwChannelID, pSock, 
                        pChannel->m_szRemoteIP, 
                        pChannel->m_wRemotePort);
    pChannel->ClearProcess();
    if (dwRc != SUCCESS)
    {
        /// �ܾ�������Ҫ���´�
        (void)pSock->Shut();
        (void)pSock->Close();
        (void)pSock->Open();
        return;
    }

    /// ���±��˶����е�״̬
    pChannel->m_connected = TRUE;
}

/*******************************************************
  �� �� ��: CLanAppBase::Disconnect
  ��    ��: �Ͽ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::Disconnect(LanAppTaskPara *pPara, LanAppChannel *pChannel)
{
    if (!pPara || !pChannel) return;

    const char *cszRemoteIP;
    WORD wRemotePort;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    DWORD dwSockFlag = pSock->dwGetSockFlag();
    if (dwSockFlag == objSock::OSSOCK_TCPCLIENT)
    {
        cszRemoteIP = pChannel->m_szRemoteIP;
        wRemotePort = pChannel->m_wRemotePort;
    }
    else
    {
        cszRemoteIP = pSock->cszGetHostIP();
        wRemotePort = pSock->wGetHostPort();
    }

    OnDisconnect(pChannel->m_dwChannelID, pSock, cszRemoteIP, wRemotePort);

    /// '����״̬'��ΪFALSE
    pChannel->m_connected = FALSE;

    /// TCPClient��Ҫ����ˢ��һ���׽���
    if (dwSockFlag == objSock::OSSOCK_TCPCLIENT)
    {
        (void)pSock->Shut();
        (void)pSock->Close();
        (void)pSock->Open();
        return;
    }

    /// �������͵��׽�����Ҫ���¼�������ɾ����ɾ�����ͨ�������ٱ�ʹ��
    DelFromEventBus(pChannel);
}

/*******************************************************
  �� �� ��: CLanAppBase::TCPRecv
  ��    ��: TCP��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::TCPRecv(LanAppTaskPara *pPara, LanAppChannel *pChannel)
{
    if (!pPara || !pChannel) return;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    void *pRecvBuf = pPara->m_pRecvBuf;
    if (!pRecvBuf)
    {
        return;
    }

    for (;;)
    {
        DWORD dwRecvLen = 0;
        DWORD dwRc = pSock->Recv(pRecvBuf, OSSOCK_DEFAULT_MTU, dwRecvLen);
        if (dwRc != SUCCESS)
        {
            if (dwRc == ERRCODE_SOCK_CONN_CLSOED)
            {
                /// ���ӱ��ر�
                Disconnect(pPara, pChannel);
                break;
            }

            DWORD dwRefCount = pChannel->EndProcess();
            if (dwRefCount)
            {
                continue;
            }

            break;
        }

        void *pFrameBuf = 0;
        DWORD dwFrameLen = 0;

        /// ֧�ַ�֡�����Ƚ������ݻ���
        void *pSavedBuf = pChannel->m_pSavedBuf;
        DWORD dwSavedLen = pChannel->m_dwSavedBufLen;
        if (pSavedBuf)
        {
            if (!(pFrameBuf = DCOP_Malloc(dwSavedLen + dwRecvLen)))
            {
                continue;
            }

            /// ���ϴ�δ����������ݺ����Ҫ���յ����ݺϲ�
            (void)memcpy(pFrameBuf, pSavedBuf, dwSavedLen);
            dwFrameLen = dwSavedLen;
            (void)memcpy((BYTE *)pFrameBuf + dwFrameLen, pRecvBuf, dwRecvLen);
            dwFrameLen += dwRecvLen;

            pChannel->m_pSavedBuf = 0;
            pChannel->m_dwSavedBufLen = 0;
            DCOP_Free(pSavedBuf);
        }
        else
        {
            /// Ҫ���յ����ݰ����ǵ�ǰ�յ���
            pFrameBuf = pRecvBuf;
            dwFrameLen = dwRecvLen;
        }

        /// ��������ʽ��������֡
        pSavedBuf = pGetFrameFromStream(pChannel, pRecvBuf, dwRecvLen, dwSavedLen);
        if (pSavedBuf)
        {
            /// �������ڴ�û������
            void *dwBufTmp = 0;
            if ((dwBufTmp = DCOP_Malloc(dwSavedLen)) != NULL)
            {
                (void)memcpy(dwBufTmp, pSavedBuf, dwSavedLen);
                pSavedBuf = dwBufTmp;
                pChannel->m_pSavedBuf = pSavedBuf;
                pChannel->m_dwSavedBufLen = dwSavedLen;
            }
        }
    }

    ////////////////////////////////////////////////////
    /// ����pRecvBuf, ÿ�ν���ʱ�̶��ӿ�ʼ���գ����ô���
    ////////////////////////////////////////////////////
}

/*******************************************************
  �� �� ��: CLanAppBase::UDPRecv
  ��    ��: UDP��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::UDPRecv(LanAppTaskPara *pPara, LanAppChannel *pChannel)
{
    if (!pPara || !pChannel) return;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return;
    }

    void *pRecvBuf = pPara->m_pRecvBuf;
    if (!pRecvBuf)
    {
        return;
    }

    DWORD dwRecvLen = 0;
    DWORD dwRc = pSock->Recvfrom(pRecvBuf, OSSOCK_DEFAULT_MTU, dwRecvLen, 
                        pChannel->m_szRemoteIP, 
                        pChannel->m_wRemotePort);
    pChannel->ClearProcess();
    if (dwRc != SUCCESS)
    {
        return;
    }

    int frameLen = bFrame(pRecvBuf, dwRecvLen);
    if (frameLen <= 0)
    {
        return;
    }

    ChangeBytesOrder(pRecvBuf, (DWORD)frameLen);
    OnRecv(pChannel->m_dwChannelID, pSock, pRecvBuf, (DWORD)frameLen, 
                        pChannel->m_szRemoteIP, 
                        pChannel->m_wRemotePort);
}

/*******************************************************
  �� �� ��: CLanAppBase::pGetFrameFromStream
  ��    ��: ���������н���һ������֡
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CLanAppBase::pGetFrameFromStream(LanAppChannel *pChannel, 
                        void *pRecvBuf, 
                        DWORD dwRecvLen, 
                        DWORD& rdwLeftLen)
{
    if (!pRecvBuf || !dwRecvLen)
    {
        return 0;
    }

    const char *cszRemoteIP;
    WORD wRemotePort;

    objSock *pSock = pChannel->m_pSock;
    if (!pSock)
    {
        return 0;
    }

    DWORD dwSockFlag = pSock->dwGetSockFlag();
    if (dwSockFlag == objSock::OSSOCK_TCPCLIENT)
    {
        cszRemoteIP = pChannel->m_szRemoteIP;
        wRemotePort = pChannel->m_wRemotePort;
    }
    else
    {
        cszRemoteIP = pSock->cszGetHostIP();
        wRemotePort = pSock->wGetHostPort();
    }

    DWORD dwProcLen = 0;
    while (dwProcLen < dwRecvLen)
    {
        void *pBuf = (BYTE *)pRecvBuf + dwProcLen;
        DWORD dwLen = dwRecvLen - dwProcLen;

        ////////////////////////////////////////////////////
        /// �ж��ǲ���һ֡�Ŀ�ͷ
        ////////////////////////////////////////////////////
        int frameLen = bFrame(pBuf, dwLen);
        if (frameLen < 0)
        {
            /// ���Ȳ����ж�
            break;
        }

        if (frameLen == 0)
        {
            /// ����֡ͷ, ֻ�ܵ��ֽ�ƫ�ƣ����¶�λ֡ͷ
            dwProcLen++;
            continue;
        }

        if ((DWORD)frameLen > dwLen)
        {
            /// ���Ȳ���һ֡
            break;
        }

        ////////////////////////////////////////////////////
        /// ��֡�ַ���Ϣ
        ////////////////////////////////////////////////////
        ChangeBytesOrder(pBuf, (DWORD)frameLen);
        OnRecv(pChannel->m_dwChannelID, pSock, pBuf, (DWORD)frameLen, cszRemoteIP, wRemotePort);
        dwProcLen += (DWORD)frameLen;
    }

    if (dwProcLen >= dwRecvLen)
    {
        return 0;
    }

    rdwLeftLen = dwRecvLen - dwProcLen;
    return (BYTE *)pRecvBuf + dwProcLen;
}

/*******************************************************
  �� �� ��: CLanAppBase::AddToEventBus
  ��    ��: ���ͨ�����¼�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::AddToEventBus(LanAppChannel *pChannel)
{
    if (!pChannel) return;

    /// ����ӵ���һ���¼�������(epollģ��ֻʹ��һ���¼�����)
#if !_USE_EPOLL_
    if (m_firstEvents.Count() < CLanAppEventBus::EVENT_BUS_SIZE)
#endif
    {
        m_firstEvents.Add(pChannel);
        return;
    }

    LanAppEventGroup eventGroup;

    /// ��һ�����ˣ�����ֻ����ӵ���չ���������У��������ǰ���������ģ���һ������������С��
    IT_EVENTGROUPS it_group = m_eventGroups.begin();
    if ((it_group != m_eventGroups.end()) && 
        ((*it_group).m_pEventBus) && 
        ((*it_group).m_pEventBus->Count() < CLanAppEventBus::EVENT_BUS_SIZE))
    {
        eventGroup.m_pEventBus = (*it_group).m_pEventBus;
        (void)m_eventGroups.erase(it_group);
    }
    else
    {
        eventGroup.m_pEventBus = new CLanAppEventBus();
        DWORD dwRc = eventGroup.m_pEventBus->Init(this);
        if (dwRc != SUCCESS)
        {
            CHECK_RETCODE(dwRc, "CLanAppEventBus::Init");
            return;
        }
    }

    eventGroup.m_pEventBus->Add(pChannel);
    (void)m_eventGroups.insert(eventGroup);
}

/*******************************************************
  �� �� ��: CLanAppBase::DelFromEventBus
  ��    ��: ���¼�������ɾ��ͨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLanAppBase::DelFromEventBus(LanAppChannel *pChannel)
{
    if (!pChannel) return;

    CLanAppEventBus *pEventBus = pChannel->m_pEventBus;
    if (!pEventBus)
    {
        return;
    }

    /// ����ǵ�һ�飬���ڵ�һ����ɾ����
    if (pEventBus == &m_firstEvents)
    {
        m_firstEvents.Del(pChannel);
        return;
    }

    LanAppEventGroup eventGroup = {pEventBus};

    /// ����չ�����в����¼����ߣ��ҵ���������չ���������룬ɾ������Ҫ���¼���
    IT_EVENTGROUPS it_group = m_eventGroups.find(eventGroup);
    if (it_group != m_eventGroups.end())
    {
        (void)m_eventGroups.erase(it_group);
    }

    eventGroup.m_pEventBus->Del(pChannel);
    (void)m_eventGroups.insert(eventGroup);
}

