/// -------------------------------------------------
/// mem.cpp : �ڴ��װʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "mem.h"
#include "task.h"
#include "../log/log.h"


/// -------------------------------------------------
/// ȫ���ڴ���Խӿ�
/// -------------------------------------------------
CMemTrack g_mem_track;
int CMemTrack::m_record_status = CMemTrack::record_status_none;
static CLog *sg_pMemLog = 0;


/*******************************************************
  �� �� ��: CMemTrack::CMemTrack
  ��    ��: �ڴ���ٹ��캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemTrack::CMemTrack()
{
    m_record_status = record_status_none;

    m_alloc_count = 0;
    m_free_count = 0;
    m_double_free_count = 0;
    m_over_write_count = 0;
    m_mem_count = 0;
    m_pLock = 0;
    m_track_inside = 0;
    m_record_detail = false;
    m_cur_task_id = 0;
    (void)memset(m_file_name, 0, sizeof(m_file_name));
    m_record_alloc_callstack = false;
}

/*******************************************************
  �� �� ��: CMemTrack::~CMemTrack
  ��    ��: �ڴ������������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMemTrack::~CMemTrack()
{
    Fini();

    m_record_status = record_status_none;
}

/*******************************************************
  �� �� ��: CMemTrack::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemTrack::Init()
{
    if (!m_pLock)
    {
        m_pLock = DCOP_CreateLock();
    }

    if (!sg_pMemLog)
    {
        sg_pMemLog = CLog::Init(LOG_ID_MEMLOG, "mem.log");
    }

    m_record_status = record_status_init;
}

/*******************************************************
  �� �� ��: CMemTrack::Fini
  ��    ��: ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemTrack::Fini()
{
    /// ��ֹ��dump�����ͷ�ʱҲ�������ڴ���ٺ���
    m_record_status = record_status_none;

    DumpMemInfo();

    m_track_alloc_info.clear();
    m_track_free_info.clear();
    if (m_pLock)
    {
        delete m_pLock;
    }

    m_record_status = record_status_end;
}

/*******************************************************
  �� �� ��: CMemTrack::GetFileName
  ��    ��: ��ȡ�ļ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
const char *CMemTrack::GetFileName(const char *file)
{
    if (!file) return 0;

    const char *pFileName = strrchr(file, '\\');
    if (!pFileName) pFileName = strrchr(file, '/');
    if (!pFileName) return file;

    return ++pFileName;
}

/*******************************************************
  �� �� ��: CMemTrack::AddTrack
  ��    ��: ��ӹ켣
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemTrack::AddTrack(void *address, size_t size, const char *file, int line)
{
    AutoLock(m_pLock);

    m_record_status = record_status_start;

    /// ��ֹ�ݹ����
    if (m_track_inside)
    {
        return;
    }

    m_track_inside = 1;

    file = GetFileName(file);

    /// ��ȡ��ǰ������
    DWORD system = 0;
    DWORD handler = 0;
    objTask *pTask = objTask::Current();
    if (pTask)
    {
        DWORD *pdwSystemID = (DWORD *)pTask->GetLocal(TASK_LOCAL_SYSTEM);
        if (pdwSystemID) system = *pdwSystemID;
        DWORD *pdwHandlerID = (DWORD *)pTask->GetLocal(TASK_LOCAL_HANDLER);
        if (pdwHandlerID) handler = *pdwHandlerID;
    }

    /// ��ϸ��¼
    if (sg_pMemLog && GetRecordDetail(file))
    {
        objTask *pTask = objTask::Current();
        sg_pMemLog->Write(STR_FORMAT(" ADDRESS '%p' (len:%d) Alloc in %s:%d (handler:%d/%d), curTask:'%s'(%d). \r\n",
                        address,
                        size,
                        file,
                        line,
                        system,
                        handler,
                        (pTask)? pTask->Name() : "Null",
                        (pTask)? pTask->ID() : 0));
    }

    /// �ڴ���Ϣ�ṹ
    MEM_INFO info;
    info.address = address;
    info.size = size;
    info.file = file;
    info.line = line;
    info.system = system;
    info.handler = handler;
    time(&info.time);
    info.clock = clock();

    /// β��У��
    for(int i = 0; i < (int)(MEM_TAIL_CHECK_LEN/sizeof(DWORD)); ++i)
    {
        memcpy((void *)((char *)address + size + i*sizeof(DWORD)), OSBUF_MAGIC, sizeof(DWORD));
    }

    /// �����¼��map����
    m_alloc_count++;
    IT_ALLOC it = m_track_alloc_info.insert(m_track_alloc_info.end(), MAP_ALLOC::value_type(address, info));
    if ((it != m_track_alloc_info.end()) && (m_record_alloc_callstack || (!system && !handler)))
    {
        ShowCallStack(CDString::Print, &(((*it).second).callstack), 0);
    }
    (void)m_track_free_info.erase(address);

    /// �����ܴ�С
    m_mem_count += size;

    m_track_inside = 0;
};

/*******************************************************
  �� �� ��: CMemTrack::RemoveTrack
  ��    ��: ɾ���켣
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemTrack::RemoveTrack(void *address, const char *file, int line)
{
    AutoLock(m_pLock);

    if (record_status_end == m_record_status)
    {
        if (sg_pMemLog)
        {
            sg_pMemLog->Write(STR_FORMAT(" ADDRESS '%p' is removed after record over! Free in %s:%d. \r\n", 
                        address, 
                        GetFileName(file), 
                        line));
            ShowCallStack(CLog::PrintCallBack, sg_pMemLog, 0);
        }
        /// ��¼�Ѿ�����
        return;
    }

    if (record_status_start != m_record_status)
    {
        /// ��δ��ʼ��¼
        return;
    }

    /// ��ֹ�ݹ����
    if (m_track_inside)
    {
        return;
    }

    m_track_inside = 1;

    file = GetFileName(file);

    /// ��ȡ��ǰ������
    DWORD system = 0;
    DWORD handler = 0;
    objTask *pTask = objTask::Current();
    if (pTask)
    {
        DWORD *pdwSystemID = (DWORD *)pTask->GetLocal(TASK_LOCAL_SYSTEM);
        if (pdwSystemID) system = *pdwSystemID;
        DWORD *pdwHandlerID = (DWORD *)pTask->GetLocal(TASK_LOCAL_HANDLER);
        if (pdwHandlerID) handler = *pdwHandlerID;
    }

    /// ��ϸ��¼
    if (sg_pMemLog && GetRecordDetail(file))
    {
        objTask *pTask = objTask::Current();
        sg_pMemLog->Write(STR_FORMAT(" ADDRESS '%p' Free in %s:%d (handler:%d/%d), curTask:'%s'(%d). \r\n",
                        address,
                        file,
                        line,
                        system,
                        handler,
                        (pTask)? pTask->Name() : "Null",
                        (pTask)? pTask->ID() : 0));
    }

    char timeStr[MEM_TIME_STRING_LEN];

    /// �����������в��Ҽ�¼
    IT_ALLOC it = m_track_alloc_info.find(address);
    if (it == m_track_alloc_info.end())
    {
        /// û�����룬���Ƿ����ͷŹ�
        it = m_track_free_info.find(address);
        if (it != m_track_free_info.end())
        {
            /// �ͷ��м�¼��˵�����ظ��ͷ�
            m_double_free_count++;
            if (sg_pMemLog)
            {
                GetTimeStr(((*it).second).time, ((*it).second).clock, timeStr, sizeof(timeStr));
                sg_pMemLog->Write(STR_FORMAT(" ADDRESS '%p' %d bytes doublefreed! Previously free in %s:%d (handler:%d/%d, time:%s), Currently free in %s:%d. \r\n", 
                        ((*it).second).address, 
                        ((*it).second).size, 
                        ((*it).second).file, 
                        ((*it).second).line, 
                        ((*it).second).system,
                        ((*it).second).handler,
                        timeStr,
                        file, 
                        line));
                ShowCallStack(CLog::PrintCallBack, sg_pMemLog, 0);
            }
        }

        m_track_inside = 0;
        
        return;
    }

    /// β��У��
    for(int i = 0; i < (int)(MEM_TAIL_CHECK_LEN/sizeof(DWORD)); ++i)
    {
        if (memcmp((void *)((char *)address + ((*it).second).size + i*sizeof(DWORD)), 
                        OSBUF_MAGIC, sizeof(DWORD)) != 0)
        {
            m_over_write_count++;
            if (sg_pMemLog)
            {
                GetTimeStr(((*it).second).time, ((*it).second).clock, timeStr, sizeof(timeStr));
                sg_pMemLog->Write(STR_FORMAT(" ADDRESS '%p' %d bytes overwrited! Alloc in %s:%d (handler:%d/%d, time:%s), Free in %s:%d (handler:%d/%d). \r\n", 
                        ((*it).second).address, 
                        ((*it).second).size, 
                        ((*it).second).file, 
                        ((*it).second).line, 
                        ((*it).second).system, 
                        ((*it).second).handler, 
                        timeStr, 
                        file, 
                        line,
                        system,
                        handler), 
                        (char *)address + ((*it).second).size, 
                        MEM_TAIL_CHECK_LEN, 
                        0, 
                        0);
                ShowCallStack(CLog::PrintCallBack, sg_pMemLog, 0);
            }
            break;
        }
    }

    /// ����¼����Ϊ��ǰ�ͷţ����û���ļ����ͱ�������ʱ���ļ����к�
    if (file)
    {
        ((*it).second).file = file;
        ((*it).second).line = line;
    }

    ((*it).second).system = system;
    ((*it).second).handler = handler;
    time(&(((*it).second).time));
    ((*it).second).clock = clock();

    /// �����ܴ�С
    if (m_mem_count > ((*it).second).size)
    {
        m_mem_count -= ((*it).second).size;
    }
    else
    {
        m_mem_count = 0;
    }

    /// ��map������ɾ����¼
    m_free_count++;
    m_track_free_info.insert(MAP_ALLOC::value_type(address, (*it).second));
    m_track_alloc_info.erase(address);

    m_track_inside = 0;
};

/*******************************************************
  �� �� ��: CMemTrack::SetRecordDetail
  ��    ��: ������ϸ��¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemTrack::SetRecordDetail(bool enable, bool only_cur_task, const char *only_file_name)
{
    AutoLock(m_pLock);

    m_record_detail = enable;

    if (only_cur_task)
    {
        objTask *pTask = objTask::Current();
        if (pTask) m_cur_task_id = pTask->ID();
        else m_cur_task_id = 0;
    }
    else
    {
        m_cur_task_id = 0;
    }

    if (only_file_name && (*only_file_name))
    {
        (void)snprintf(m_file_name, sizeof(m_file_name), "%s", only_file_name);
        m_file_name[sizeof(m_file_name) - 1] = '\0';
    }
    else
    {
        (void)memset(m_file_name, 0, sizeof(m_file_name));
    }
}

/*******************************************************
  �� �� ��: CMemTrack::GetRecordDetail
  ��    ��: ��ȡ��ϸ��¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CMemTrack::GetRecordDetail(const char *file)
{
    AutoLock(m_pLock);

    if (!m_record_detail)
    {
        return false;
    }

    objTask *pTask = objTask::Current();
    if (m_cur_task_id && pTask && (pTask->ID() != m_cur_task_id))
    {
        return false;
    }

    if (*m_file_name)
    {
        if (file && (*file) && strstr(file, m_file_name))
        {
            return true;
        }

        return false;
    }

    return true;
}

/*******************************************************
  �� �� ��: CMemTrack::GetTimeStr
  ��    ��: ��ȡʱ���ַ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemTrack::GetTimeStr(const time_t &time, const clock_t &clock, char *szStr, int strLen)
{
    struct tm *newtime = localtime(&time);

    (void)snprintf(szStr, strLen, "'%02d-%02d %02d:%02d:%02d.%03d'", 
        newtime->tm_mon + 1, newtime->tm_mday, 
        newtime->tm_hour, newtime->tm_min, newtime->tm_sec, 
        int(clock/(CLOCKS_PER_SEC/1000)) % 1000);

    szStr[strLen - 1] = '\0';
}

/*******************************************************
  �� �� ��: CMemTrack::DumpMemInfo
  ��    ��: ��ӡ�ڴ���Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMemTrack::DumpMemInfo()
{
    size_t totalSize = 0;
    char szStr[STR_FORMAT_LEN_MAX];

    AutoLock(m_pLock);

    int pos = GetLogTime(szStr, sizeof(szStr)-1);
    pos += snprintf(szStr+pos, sizeof(szStr)-pos-1, "DumpMemInfo ... \r\n");
    if (sg_pMemLog) sg_pMemLog->Write(szStr);

    for(IT_ALLOC it = m_track_alloc_info.begin(); it != m_track_alloc_info.end(); it++)
    {
        if (sg_pMemLog)
        {
            void *address = ((*it).second).address;
            size_t size = ((*it).second).size;
            const char *file = ((*it).second).file;
            int line = ((*it).second).line;
            DWORD system = ((*it).second).system;
            DWORD handler = ((*it).second).handler;
            char timeStr[MEM_TIME_STRING_LEN];
            GetTimeStr(((*it).second).time, ((*it).second).clock, timeStr, sizeof(timeStr));
            sg_pMemLog->Write(STR_FORMAT(" ADDRESS '%p' %d bytes unfreed! Alloc in %s:%d (handler:%d/%d, time:%s). \r\n", 
                        address, 
                        size, 
                        file, 
                        line, 
                        system, 
                        handler, 
                        timeStr), 
                        address, 
                        ((size > MEM_DETAIL_PRINT_LEN)? MEM_DETAIL_PRINT_LEN : size));
            sg_pMemLog->Write(((*it).second).callstack);
        }
        totalSize += ((*it).second).size;
    }

    if (sg_pMemLog)
    {
        sg_pMemLog->Write(STR_FORMAT("------------------------------------------- \r\n"));
        sg_pMemLog->Write(STR_FORMAT("    Total Unfreed: %d bytes \r\n", totalSize));
        sg_pMemLog->Write(STR_FORMAT("            Alloc: %d times \r\n", m_alloc_count));
        sg_pMemLog->Write(STR_FORMAT("             Free: %d times \r\n", m_free_count));
        sg_pMemLog->Write(STR_FORMAT("             Leak: %d times \r\n", m_track_alloc_info.size()));
        sg_pMemLog->Write(STR_FORMAT("      Double Free: %d times \r\n", m_double_free_count));
        sg_pMemLog->Write(STR_FORMAT("       Over Write: %d times \r\n", m_over_write_count));
        sg_pMemLog->Write(STR_FORMAT("\r\n"));
    }
}

/*******************************************************
  �� �� ��: DCOP_MallocEx
  ��    ��: �ڴ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *DCOP_MallocEx(size_t size, const char *file, int line)
{
    if (!size)
    {
        return 0;
    }

    void *ptr = (void *)malloc(size + MEM_TAIL_CHECK_LEN);
    if (!ptr)
    {
        return 0;
    }

    if (CMemTrack::m_record_status >= CMemTrack::record_status_init)
    {
        g_mem_track.AddTrack(ptr, size, file, line);
    }

    return ptr;
}

/*******************************************************
  �� �� ��: DCOP_FreeEx
  ��    ��: �ڴ��ͷ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DCOP_FreeEx(void *p, const char *file, int line)
{
    if (!p)
    {
        return;
    }

    if (CMemTrack::m_record_status >= CMemTrack::record_status_init)
    {
        g_mem_track.RemoveTrack(p, file, line);
    }

    free(p);
}

/*******************************************************
  �� �� ��: DCOP_ReallocEx
  ��    ��: �ڴ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *DCOP_ReallocEx(void *p, size_t size, const char *file, int line)
{
    if (!size)
    {
        return 0;
    }

    if (p && (CMemTrack::m_record_status >= CMemTrack::record_status_init))
    {
        g_mem_track.RemoveTrack(p, file, line);
    }

    void *ptr = (void *)realloc(p, size + MEM_TAIL_CHECK_LEN);
    if (!ptr)
    {
        return 0;
    }

    if (CMemTrack::m_record_status >= CMemTrack::record_status_init)
    {
        g_mem_track.AddTrack(ptr, size, file, line);
    }

    return ptr;
}

/*******************************************************
  �� �� ��: DebugMemStatus
  ��    ��: �ڴ���ٵ���ںͳ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DebugMemStatus(int status)
{
    if (status)
    {
        if (CMemTrack::record_status_none == CMemTrack::m_record_status)
        {
            g_mem_track.Init();
        }
    }
    else
    {
        if (CMemTrack::record_status_none != CMemTrack::m_record_status)
        {
            g_mem_track.Fini();
        }
    }
}

/*******************************************************
  �� �� ��: OutputMemLog
  ��    ��: ����ڴ���־(������̨)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void OutputMemLog(int console)
{
    if (sg_pMemLog)
    {
        sg_pMemLog->OutputToConsole((console)? true : false);
    }
}

/*******************************************************
  �� �� ��: RecordMemDetail
  ��    ��: ��ϸ��¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void RecordMemDetail(int enable, int only_cur_task, const char *only_file_name)
{
    g_mem_track.SetRecordDetail((enable)? true : false, (only_cur_task)? true : false, only_file_name);
}

/*******************************************************
  �� �� ��: RecordAllocCallstack
  ��    ��: �����Ƿ��¼�������ջ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void RecordAllocCallstack(int enable)
{
    g_mem_track.SetRecordAllocCallstack((enable)? true : false);
}

/*******************************************************
  �� �� ��: DumpMemInfo
  ��    ��: �ڴ��ӡ�ӿ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DumpMemInfo()
{
    g_mem_track.DumpMemInfo();
}

/*******************************************************
  �� �� ��: GetMemAllocCount
  ��    ��: ��ȡ�ڴ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
size_t GetMemAllocCount()
{
    return g_mem_track.GetMemAllocCount();
}

/*******************************************************
  �� �� ��: GetMemFreeCount
  ��    ��: ��ȡ�ڴ��ͷŴ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
size_t GetMemFreeCount()
{
    return g_mem_track.GetMemFreeCount();
}

/*******************************************************
  �� �� ��: GetMemDoubleFreeCount
  ��    ��: ��ȡ�ڴ��ظ��ͷŴ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
size_t GetMemDoubleFreeCount()
{
    return g_mem_track.GetMemDoubleFreeCount();
}

/*******************************************************
  �� �� ��: GetMemOverWriteCount
  ��    ��: ��ȡ�ڴ�дԽ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
size_t GetMemOverWriteCount()
{
    return g_mem_track.GetMemOverWriteCount();
}

/*******************************************************
  �� �� ��: GetMemTotalSize
  ��    ��: ��ȡ�ڴ��ܴ�С
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
size_t GetMemTotalSize()
{
    return g_mem_track.GetMemTotalSize();
}

