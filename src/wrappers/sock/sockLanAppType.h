/// -------------------------------------------------
/// sockLanAppType.h : sockӦ�÷�װ�๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _LANAPPTYPE_H_
#define _LANAPPTYPE_H_

#define INC_SET
#define INC_MAP

#include "msg.h"
#include "sem.h"
#include "task.h"
#include "sock.h"
#include "sockType.h"

/// ���û��ר������EPOLL�Ƿ�����(1)���ǲ���(0)��linux��Ĭ�ϴ�EPOLL
#if !defined(_USE_EPOLL_) && (DCOP_OS == DCOP_OS_LINUX)
#define _USE_EPOLL_ 1
#endif

#if _USE_EPOLL_
#include <sys/epoll.h>
#endif


/// ����Ӧ��
class CLanAppBase;
class CLanAppEventBus;


/// ����Ӧ���������
struct LanAppTaskPara : public objTask::IPara
{
    CLanAppBase *m_pLanApp;
    CLanAppEventBus *m_pLanAppEventBus;
    void *m_pRecvBuf;
    DWORD m_dwTaskNo;

    LanAppTaskPara(CLanAppBase *pLanApp, CLanAppEventBus *pLanAppEventBus, DWORD dwTaskNo);
    ~LanAppTaskPara();
};


/// ����Ӧ��ͨ��
struct LanAppChannel
{
    DWORD m_dwChannelID;
    objSock *m_pSock;
    CLanAppBase *m_pThis;
    objAtomic::T m_process;
    char m_szRemoteIP[OSSOCK_IPSIZE];
    WORD m_wRemotePort;
    BYTE m_connected;
    BYTE m_channelType;
    void *m_pSavedBuf;
    DWORD m_dwSavedBufLen;
    CLanAppEventBus *m_pEventBus;

    enum {NONE, LOCAL, REMOTE};

    LanAppChannel();
    LanAppChannel(const LanAppChannel &rChannel);
    LanAppChannel(DWORD dwChannelID,
                        objSock *pSock,
                        CLanAppBase *pThis,
                        const char *szRemoteIP,
                        WORD wRemotePort,
                        BYTE connected = FALSE);
    ~LanAppChannel();
    bool operator <(const LanAppChannel &rThis) const;
    DWORD StartProcess() {return (DWORD)objAtomic::Inc(m_process);}
    DWORD EndProcess() {return (DWORD)objAtomic::Dec(m_process);}
    void ClearProcess() {(void)objAtomic::Set(m_process, 0);}
};


/// ����Ӧ���¼�����
class CLanAppEventBus
{
public:
    typedef std::set<LanAppChannel> SET_EVENTSOCKS;
    typedef SET_EVENTSOCKS::iterator IT_EVENTSOCKS;

#if _USE_EPOLL_
    /// epoll��Ȼ���԰󶨺ܴ������ľ��������һ�β��ܴ���̫��(��úͶ��������)
    static const DWORD EVENT_BUS_SIZE = 1;
#else
    /// select���ֻ֧��1024�����(��Ҫ��չ)����������Ҫ����
    static const DWORD EVENT_BUS_SIZE = 1024;
#endif

    /// �Զ��˳�ʱ�ļ�������
    static const DWORD AUTO_EXIT_COUNT = 16;

public:
    CLanAppEventBus();
    ~CLanAppEventBus();

    DWORD Init(CLanAppBase *pLanApp);

    void Add(LanAppChannel *pChannel);
    void Del(LanAppChannel *pChannel);

    DWORD Count() {AutoLock(m_pLock); return (DWORD)m_eventSocks.size();}

    void vDisableAutoExit() {m_bAutoExit = false;}
    bool bAutoExit() {return m_bAutoExit;}

private:
    static void TaskRun(objTask::IPara *para);
    void AddEvent(LanAppChannel *pChannel);
    void SetEvents(CLanAppBase *pLanApp);
    int  WaitEvents(CLanAppBase *pLanApp);
    void ProcEvents(LanAppTaskPara *pPara, CLanAppBase *pLanApp, DWORD dwEventCount);

private:
    objLock *m_pLock;

#if _USE_EPOLL_
    int m_epollFd;
    epoll_event *m_chanEvts;
#else
    fd_set m_fdread;
    fd_set m_fdwrite;
    LanAppChannel **m_chanEvts;
#endif

    SET_EVENTSOCKS m_eventSocks;
    bool m_bAutoExit;
};


/// ����Ӧ���¼�����
struct LanAppEventGroup
{
    CLanAppEventBus *m_pEventBus;

    bool operator <(const LanAppEventGroup &rThis) const
    {
        if (!m_pEventBus || !rThis.m_pEventBus)
        {
            return false;
        }

        if (m_pEventBus == rThis.m_pEventBus)
        {
            return false;
        }

        if (m_pEventBus->Count() > rThis.m_pEventBus->Count())
        {
            return false;
        }

        return true;
    }
};


/// ����Ӧ�ö���
class CLanAppBase : public objLanApp, private osBase
{
public:
    CLanAppBase();
    ~CLanAppBase();

    typedef std::map<DWORD, LanAppChannel> MAP_LOCALSOCKS;
    typedef MAP_LOCALSOCKS::iterator IT_LOCALSOCKS;
    typedef std::set<LanAppEventGroup> SET_EVENTGROUPS;
    typedef SET_EVENTGROUPS::iterator IT_EVENTGROUPS;

    /// ��־��ӡ
#define logPrint(szInfo) \
    logPrintEx(szInfo, __FILE__, __LINE__)
    void logPrintEx(const char *cszLogInfo, 
                        const char *pFile,
                        DWORD dwLine)
    {
        VDBACKFUNC_VDCALL(m_fnLogPrint)(
                        cszLogInfo,
                        pFile,
                        dwLine,
                        m_pLogPrintPara);
    }

    DWORD Init(const char *cszAppName, DWORD dwLocalID, DWORD dwTaskCount,
                        LanEventProc *pEventProc,
                        LanFrameProc *pFrameProc,
                        LanLogProc *pLogProc);

    void AddChannel(DWORD dwChannelID,
                        objSock *pSock,
                        const char *szRemoteIP = 0,
                        WORD wRemotePort = 0);

    const char *GetAppName() {return m_szAppName;}

    DWORD GetTaskCount() {return m_dwTaskCount;}

    void Process(LanAppTaskPara *pPara, LanAppChannel *pChannel);

    void Accept(LanAppTaskPara *pPara, LanAppChannel *pChannel);

    void Connect(LanAppTaskPara *pPara, LanAppChannel *pChannel);

    void Disconnect(LanAppTaskPara *pPara, LanAppChannel *pChannel);

    void TCPRecv(LanAppTaskPara *pPara, LanAppChannel *pChannel);

    void UDPRecv(LanAppTaskPara *pPara, LanAppChannel *pChannel);

    DWORD OnAccept(DWORD dwChannelID, objSock *pServerSock, objSock *pAcceptSock)
    {
        return BACKFUNC_CALL(m_fnAcceptProc)(dwChannelID, pServerSock, pAcceptSock, m_pAcceptProcPara);
    }
    DWORD OnConnect(DWORD dwChannelID, objSock *pClientSock, const char *cszRemoteIP, WORD wRemotePort)
    {
        return BACKFUNC_CALL(m_fnConnectProc)(dwChannelID, pClientSock, cszRemoteIP, wRemotePort, m_pConnectProcPara);
    }
    void OnDisconnect(DWORD dwChannelID, objSock *pSock, const char *cszRemoteIP, WORD wRemotePort)
    {
        VDBACKFUNC_VDCALL(m_fnDisconnectProc)(dwChannelID, pSock, cszRemoteIP, wRemotePort, m_pDisconnectProcPara);
    }
    void OnRecv(DWORD dwChannelID, objSock *pSock, void *pFrameBuf, DWORD dwFrameLen, const char *cszRemoteIP, WORD wRemotePort)
    {
        VDBACKFUNC_VDCALL(m_fnRecvProc)(dwChannelID, pSock, pFrameBuf, dwFrameLen, cszRemoteIP, wRemotePort, m_pRecvProcPara);
    }

    int bFrame(void *pBuf, DWORD dwLen)
    {
        if (m_fnBFrameProc)
        {
            return (m_fnBFrameProc)(pBuf, dwLen, m_pBFrameProcPara);
        }

        return OSMsgHeader::bFrame(pBuf, dwLen);
    }
    void ChangeBytesOrder(void *pBuf, DWORD dwLen)
    {
        if (m_fnChangeBytesOrderProc)
        {
            return (m_fnChangeBytesOrderProc)(pBuf, dwLen, m_pChangeBytesOrderProcPara);
        }

        return OSMsgHeader::ChangeBytesOrder(pBuf, dwLen);
    }
    

    ////////////////////////////////////////////////////
    /// ʵ�ּ̳е���ӿ� - begin
    ////////////////////////////////////////////////////

    /// ��ȡ����ID
    DWORD GetLocalID() {return m_dwLocalID;}

    /// ��ȡͨ���׽���
    objSock *GetChannel(DWORD dwChannelID)
    {
        AutoLock(m_pLock);

        IT_LOCALSOCKS it_local = m_localSocks.find(dwChannelID);
        if (it_local == m_localSocks.end())
        {
            return NULL;
        }

        return ((*it_local).second).m_pSock;
    }

    /// ���TCPServerͨ��
    DWORD AddTCPServer(DWORD dwChannelID,
                        WORD wLocalPort);

    /// ���TCPClientͨ��
    DWORD AddTCPClient(DWORD dwChannelID,
                        const char *szRemoteIP,
                        WORD wRemotePort);

    /// ���UDPͨ��
    DWORD AddUDP(DWORD dwChannelID,
                        bool bBind,
                        WORD wLocalPort,
                        bool bBoardcast);

    /// ɾ��ͨ��
    void DelChannel(DWORD dwChannelID);

    /// ����Ӧ��
    DWORD Start(const char *cszAppName,
                        DWORD dwLocalID,
                        DWORD dwTaskCount,
                        LanEventProc *pEventProc,
                        LanFrameProc *pFrameProc = 0,
                        LanLogProc *pLogProc = 0);

    /// ֹͣӦ��
    void Stop()
    {
        (void)objAtomic::Set(m_startFlag, FALSE);
    }

    ////////////////////////////////////////////////////
    /// ʵ�ּ̳е���ӿ� - end
    ////////////////////////////////////////////////////


public:
    ////////////////////////////////////////////////////
    /// Name  : dwGetFrameFromStream
    /// Desp  : ����ʽ�����в�����ݰ��ı߽�
    /// Input : pChannel    Զ�˷���ID
    ///       : pRecvBuf    �������׵�ַ
    ///       : dwRecvLen   ��������С
    /// Output: rdwLeftLen  ʣ�µĻ���������
    /// Return: 
    ///         ���ν��պ�, �����һ��������Frame,����ֵָ��
    ///         ��Ϣ����ʣ�µĻ�����(ֻ��ָ��pRecvBuf��ĳ��λ��)
    ////////////////////////////////////////////////////
    void *pGetFrameFromStream(LanAppChannel *pChannel, 
                        void *pRecvBuf, 
                        DWORD dwRecvLen, 
                        DWORD& rdwLeftLen);

    /// �ж��Ƿ�ϣ������(�����ȡ��״̬�������˳�)
    bool bExit() {return (!m_startFlag)? true : false;}

    /// ������������
    void vIncTaskRef()
    {
        AutoLock(m_pLock);

        if (!m_pTaskRefCountSem)
        {
            m_pTaskRefCountSem = DCOP_CreateCounter(0, OSSOCK_LANAPP_TASK_MAX_COUNT);
            if (!m_pTaskRefCountSem) return;
        }

        m_dwTaskRefCount++;
    }

    /// �ݼ���������
    void vDecTaskRef()
    {
        AutoLock(m_pLock);

        if (m_pTaskRefCountSem) m_pTaskRefCountSem->Give();
    }

    /// �����¼������е�ͨ��
    void AddToEventBus(LanAppChannel *pChannel);
    void DelFromEventBus(LanAppChannel *pChannel);

private:
    /// ���ò���
    char m_szAppName[OSNAME_LENGTH];
    DWORD m_dwLocalID;
    DWORD m_dwTaskCount;

    /// �¼����߷���
    objLock *m_pLock;
    MAP_LOCALSOCKS m_localSocks;
    CLanAppEventBus m_firstEvents;
    SET_EVENTGROUPS m_eventGroups;

    /// �¼�����ص�
    FUNC_ON_ACCEPT m_fnAcceptProc;
    void *m_pAcceptProcPara;
    FUNC_ON_CONNECT m_fnConnectProc;
    void *m_pConnectProcPara;
    FUNC_ON_DISCONNECT m_fnDisconnectProc;
    void *m_pDisconnectProcPara;
    FUNC_ON_RECV m_fnRecvProc;
    void *m_pRecvProcPara;

    /// ����֡����ص�
    FUNC_B_FRAME m_fnBFrameProc;
    void *m_pBFrameProcPara;
    FUNC_CHANGE_BYTES_ORDER m_fnChangeBytesOrderProc;
    void *m_pChangeBytesOrderProcPara;

    /// ��־�ص�
    FUNC_LOG_PRINT m_fnLogPrint;
    void *m_pLogPrintPara;

    /// ��ʼ��ʶ-FALSE:δ��ʼ;TRUE:��ʼ(��ʼ��������ΪFALSE��ͣ��)
    objAtomic::T m_startFlag;
    DWORD m_dwTaskRefCount;
    objCounter *m_pTaskRefCountSem;
};


#endif // #ifndef _LANAPPTYPE_H_

