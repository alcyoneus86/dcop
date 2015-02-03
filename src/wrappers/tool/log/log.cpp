/// -------------------------------------------------
/// log.cpp : ��־��װʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "log.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#if DCOP_OS == DCOP_OS_WINDOWS
#elif DCOP_OS == DCOP_OS_LINUX
#include <signal.h>
#endif


/// -------------------------------------------------
/// ������Ϣ����
/// -------------------------------------------------
int g_debug_switch = 
#ifdef DEBUG
    LOG_OUT_FILE;
#else
    LOG_OUT_NULL;
#endif

/// -------------------------------------------------
/// �Զ���
/// -------------------------------------------------
#define AutoLogLock(x) AutoLogLockEx __tmp_##x(x)

/// -------------------------------------------------
/// ��־����
/// -------------------------------------------------
static CLog *sg_pErrLog = 0;                        // ������־
static CLog *sg_pTraceLog = 0;                      // ������Ϣ
static CLog *sg_pCheckLog = 0;                      // �����־


/*******************************************************
  �� �� ��: DebugLogStatus
  ��    ��: ���Կ���״̬
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DebugLogStatus(int status)
{
    if (status)
    {
        SetSysErrHandler();
    }

    g_debug_switch = status;
}

/*******************************************************
  �� �� ��: GetLogTime
  ��    ��: ��ȡ��־ʱ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int GetLogTime(char *szStr, int strLen)
{
    struct tm *newtime;
    time_t ltime;

    time(&ltime);
    newtime = localtime(&ltime);

    return snprintf(szStr, strLen, "[%02d-%02d %02d:%02d:%02d.%03d] ", 
        newtime->tm_mon + 1, newtime->tm_mday, 
        newtime->tm_hour, newtime->tm_min, newtime->tm_sec, 
        int(clock()/(CLOCKS_PER_SEC/1000)) % 1000);
}

/*******************************************************
  �� �� ��: GetLogFileNameAndLine
  ��    ��: ��ȡ�ļ������к�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int GetLogFileNameAndLine(char *szStr, int strLen, const char *file, int line)
{
    if (!file) return 0;

    const char *pFileName = strrchr(file, '\\');
    if (!pFileName) pFileName = strrchr(file, '/');
    if (!pFileName) pFileName = file;
    else ++pFileName;

    return snprintf(szStr, strLen, " <%s:%d>", pFileName, line);
}

/*******************************************************
  �� �� ��: PrintLogEx
  ��    ��: ��ӡ��־
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void PrintLogEx(const char *info, LOG_PRINT print, LOG_PARA para, const char *file, int line)
{
    if (!info || !(*info) || !print) return;

    char szStr[STR_FORMAT_LEN_MAX];
    int pos = GetLogTime(szStr, sizeof(szStr));
    int lprint = snprintf(szStr+pos, sizeof(szStr)-pos, "%s", info);
    szStr[sizeof(szStr)-1] = '\0';
    if (lprint < 0) pos = (int)strlen(szStr);
    else pos += lprint;

    while (pos && ((szStr[pos-1] == '\n') || 
                   (szStr[pos-1] == '\r') || 
                   (szStr[pos-1] == ' ')))
    {
        pos--;
    }

    pos += GetLogFileNameAndLine(szStr+pos, sizeof(szStr)-pos, file, line);

    (void)snprintf(szStr+pos, sizeof(szStr)-pos, "\r\n");
    szStr[sizeof(szStr)-1] = '\0';

    print(szStr, para);
}

/*******************************************************
  �� �� ��: PrintBufferEx
  ��    ��: ��ӡ�ڴ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void PrintBufferEx(const char *info, const void *buf, size_t len, LOG_PRINT print, LOG_PARA para, const char *file, int line)
{
    if (!buf || !len || !print) return;

    char szLine[84]; // 80���ַ��պ�����һ��16���ֽڵĴ�ӡ
    char szByte[20]; // 16���ֽڵ��ַ���ʾ
    size_t offset = 0;

    PrintLogEx(info, print, para, file, line);

    for (size_t i = 0; i < len; ++i)
    {
        /// ��ͷ�Ĵ���
        if ((i % 16) == 0)
        {
            offset = (size_t)snprintf(szLine, sizeof(szLine), "0x%08x: ", (unsigned int)i);
        }

        /// �м�Ĵ���
        BYTE *pbyTmp = (BYTE *)buf;
        offset += (size_t)snprintf(szLine + offset, sizeof(szLine) - offset, "%02x ", pbyTmp[i]);
        if (isprint(pbyTmp[i]))
        {
            szByte[i % 16] = pbyTmp[i];
        }
        else
        {
            szByte[i % 16] = '.';
        }

        /// ���Ĵ���
        if (((i % 16) == 15) || (i == (len - 1)))
        {
            size_t differ = 15 - (i % 16);
            for (size_t j = 0; j < differ; ++j)
            {
                offset += (size_t)snprintf(szLine + offset, sizeof(szLine) - offset, "   ");
                szByte[(i % 16) + j + 1] = ' ';
            }
            szByte[16] = '\0';
            offset += (size_t)snprintf(szLine + offset, sizeof(szLine) - offset, "; %s\r\n", szByte);
            print(szLine, para);
        }

    }
}

/*******************************************************
  �� �� ��: PrintToConsole
  ��    ��: �����̨��ӡ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void PrintToConsole(const char *info, void *)
{
    if (!info) return;

    printf("%s", info);
}

/*******************************************************
  �� �� ��: TraceLogEx
  ��    ��: ��¼��Ϣ����־
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void TraceLogEx(const char *info, const char *file, int line)
{
    /// infoΪ�գ�Ҳ��ӡ�ļ������к�

    if (!g_debug_switch)
    {
        return;
    }

    /// �ϲ�������Ϣ������
    char szStr[STR_FORMAT_LEN_MAX];
    int pos = GetLogTime(szStr, sizeof(szStr));
    pos += snprintf(szStr+pos, sizeof(szStr)-pos, "%s", info);
    pos += GetLogFileNameAndLine(szStr+pos, sizeof(szStr)-pos, file, line);

    /// д��trace.log�ļ�
    if (g_debug_switch & LOG_OUT_FILE)
    {
        if (!sg_pTraceLog)
        {
            sg_pTraceLog = CLog::Init(LOG_ID_TRACELOG, "trace.log");
            if (!sg_pTraceLog) return;

            sg_pTraceLog->Write("\r\n========================= TRACE START =========================");
        }

        sg_pTraceLog->Write(szStr);
    }

    /// ��printf��ʽ���
    if (g_debug_switch & LOG_OUT_PRINT)
    {
        printf("%s\r\n", szStr);
    }
}

/*******************************************************
  �� �� ��: TraceBufEx
  ��    ��: ��¼�ڴ浽��־
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void TraceBufEx(const char *info, const void *buf, size_t len, const char *file, int line)
{
    if (!g_debug_switch)
    {
        return;
    }

    /// д��trace.log�ļ�
    if (g_debug_switch & LOG_OUT_FILE)
    {
        if (!sg_pTraceLog)
        {
            sg_pTraceLog = CLog::Init(LOG_ID_TRACELOG, "trace.log");
            if (!sg_pTraceLog) return;

            sg_pTraceLog->Write("\r\n========================= TRACE START =========================");
        }

        sg_pTraceLog->Write(info, buf, len, file, line);
    }

    /// ��printf��ʽ���
    if (g_debug_switch & LOG_OUT_PRINT)
    {
        PrintBufferEx(info, buf, len, PrintToConsole, 0, file, line);
    }
}

/*******************************************************
  �� �� ��: CheckRetCodeEx
  ��    ��: ��鷵��ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CheckRetCodeEx(int rc, int expect, const char *info, const char *file, int line)
{
    if (rc == expect)
    {
        return;
    }

    char szStr[STR_FORMAT_LEN_MAX];
    int pos = GetLogTime(szStr, sizeof(szStr));
    pos += snprintf(szStr+pos, sizeof(szStr)-pos, "%s('%d'!='%d')", info, rc, expect);
    pos += GetLogFileNameAndLine(szStr+pos, sizeof(szStr)-pos, file, line);

    if (!sg_pCheckLog)
    {
        sg_pCheckLog = CLog::Init(LOG_ID_CHECKLOG, "check.log");
        if (!sg_pCheckLog) return;

        sg_pCheckLog->Write("\r\n========================= CHECK START =========================");
    }

    sg_pCheckLog->Write(szStr);

    /// ��printf��ʽ���
    if (g_debug_switch & LOG_OUT_PRINT)
    {
        printf("%s\r\n", szStr);
    }
}

/*******************************************************
  �� �� ��: CheckErrCodeEx
  ��    ��: ������ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CheckErrCodeEx(int rc, int expect, const char *info, const char *file, int line)
{
    if (rc == expect)
    {
        return;
    }

    char szStr[STR_FORMAT_LEN_MAX];
    int pos = GetLogTime(szStr, sizeof(szStr));
    pos += snprintf(szStr+pos, sizeof(szStr)-pos, "%s('%d'!='%d')", info, rc, expect);
    pos += GetLogFileNameAndLine(szStr+pos, sizeof(szStr)-pos, file, line);

    if (!sg_pErrLog)
    {
        sg_pErrLog = CLog::Init(LOG_ID_ERRLOG, "err.log");
        if (sg_pErrLog)
        {
            sg_pErrLog->Write("\r\n========================= SYSTEM START =========================");
        }
    }

    if (sg_pErrLog)
    {
        sg_pErrLog->Write(szStr);
    }

    /// ��printf��ʽ���
    if (g_debug_switch & LOG_OUT_PRINT)
    {
        printf("%s\r\n", szStr);
    }
    DCOP_Reset(-1, file, line);
}

/*******************************************************
  �� �� ��: HandleSysErrLog
  ��    ��: ����ϵͳ�����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void HandleSysErrLog(int error)
{
#if DCOP_OS == DCOP_OS_WINDOWS
#elif DCOP_OS == DCOP_OS_LINUX

    signal(error, SIG_DFL); // ��ԭĬ�ϵ��źŴ���handler

#endif

    PrintLog(STR_FORMAT("!!! System Error(%d) !!!", error), CLog::PrintCallBack, sg_pErrLog);
    ShowCallStack(CLog::PrintCallBack, sg_pErrLog, 0);
}

/*******************************************************
  �� �� ��: SetSysErrHandler
  ��    ��: ����ϵͳ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void SetSysErrHandler()
{
#if DCOP_OS == DCOP_OS_WINDOWS
#elif DCOP_OS == DCOP_OS_LINUX

    /// �����źŵĴ�����
    signal(SIGSEGV, HandleSysErrLog); // SIGSEGV    11       Core Invalid memory reference
    signal(SIGABRT, HandleSysErrLog); // SIGABRT     6       Core Abort signal from

#endif
}


/// -------------------------------------------------
/// ȫ����־����
/// -------------------------------------------------
CLog::MAP_LOGS CLog::sm_logs;


/*******************************************************
  �� �� ��: CLog::CLog
  ��    ��: CLog����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLog::CLog(const char *szFileName)
{
    m_pLock = 0;
    (void)strncpy(m_szFileName, szFileName, LOG_FILENAME_LEN_MAX);
    m_szFileName[LOG_FILENAME_LEN_MAX - 1] = '\0';
    m_pLock = objLock::CreateInstance(szFileName, __LINE__);
    m_bOutputToConsole = false;
}

/*******************************************************
  �� �� ��: CLog::~CLog
  ��    ��: CLog����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLog::~CLog()
{
    if (m_pLock)
    {
        delete m_pLock;
        m_pLock = 0;
    }
}

/*******************************************************
  �� �� ��: CLog::Init
  ��    ��: ��ʼ����־
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLog *CLog::Init(DWORD dwID, const char *szFileName)
{
    IT_LOGS it = sm_logs.find(dwID);
    if (it != sm_logs.end())
    {
        return 0;
    }

    #undef new
    CLog *pLog = new (szFileName, __LINE__) CLog(szFileName);
    #define new new(__FILE__, __LINE__)
    if (!pLog)
    {
        return 0;
    }

    sm_logs.insert(MAP_LOGS::value_type(dwID, pLog));

    return pLog;
}

/*******************************************************
  �� �� ��: CLog::Get
  ��    ��: ��ȡ��־
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CLog *CLog::Get(DWORD dwID)
{
    IT_LOGS it = sm_logs.find(dwID);
    if (it == sm_logs.end())
    {
        return 0;
    }

    CLog *pLog = (*it).second;

    return pLog;
}

/*******************************************************
  �� �� ��: CLog::Write
  ��    ��: ��¼��Ϣ����־
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLog::Write(const char *info, bool newline)
{
    if (!info || !(*info)) return;

    AutoLogLock(this);
    FILE *fp;

    fp = fopen(m_szFileName , "a+b");
    if (!fp)
    {
        return;
    }

    if (newline && (info[strlen(info) - 1] == '\n'))
    {
        newline = false;
    }

    fprintf(fp, ((newline)? "%s\r\n" : "%s"), info);
    fclose(fp);

    if (m_bOutputToConsole)
    {
        PrintToConsole(info, 0);
    }
}

/*******************************************************
  �� �� ��: CLog::Write
  ��    ��: ��¼�ڴ浽��־
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLog::Write(const char *info, const void *buf, size_t len, const char *file, int line)
{
    AutoLogLock(this);

    PrintBufferEx(info, buf, len, PrintCallBack, this, file, line);
}

/*******************************************************
  �� �� ��: CLog::PrintCallBack
  ��    ��: �ڴ��ӡ�ص�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CLog::PrintCallBack(const char *info, void *para)
{
    if (!info || !(*info)) return;

    CLog *pLog = (CLog *)para;
    if (pLog)
    {
        pLog->Write(info, false);
    }
}

