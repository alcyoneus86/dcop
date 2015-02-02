/// -------------------------------------------------
/// mem.h : �ڴ��װ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_MEM_MEM_H_
#define _TOOL_MEM_MEM_H_


/// ��ͷ�ļ�ֻ����mem.cpp����


/// ˵��: ��ʵ��������placement new���﷨��ͨ��һ���򵥵ĺ꣬�Ϳ��԰���ͨ��new������Ӧ����Ӧ��������ȥ:
/// new Class; => new (__FILE__, __LINE__) Class;
/// ����delete��Ϊû��������placement new���﷨�����Զ����ߵ����ȫ�ֺ���:
/// void operator delete(void *p);
/// ����ȫ��ʵ�ֺ�c����ͻ����˶���Ϊ��������(���ⲿʹ�õ�ͷ�ļ�os.h��)


#include <map>
#include "sem.h"
#include "fs/file.h"
#include "string/dstring.h"


#undef new              /// newֻ��ʹ��ԭ��ʵ��
#undef malloc           /// mallocֻ��ʹ��ԭ��ʵ��
#undef free             /// freeֻ��ʹ��ԭ��ʵ��
#undef realloc          /// reallocֻ��ʹ��ԭ��ʵ��


/// β��У�鳤��
#define MEM_TAIL_CHECK_LEN 16


/// �ڴ������
class CMemTrack
{
public:

    /// �ڴ���Ϣ
    typedef struct
    {
        void *address;
        size_t size;
        const char *file;
        int line;
        CDString callstack;
    }MEM_INFO;

    /// �ڴ���Ϣ��¼����
    typedef std::map<void *, MEM_INFO> MAP_ALLOC;
    typedef MAP_ALLOC::iterator IT_ALLOC;

    /// �ڴ��¼״̬
    enum record_status
    {
        record_status_none = 0,         // δ��ʼ��
        record_status_init  ,           // �ѳ�ʼ��
        record_status_start,            // ��¼��ʼ
        record_status_end               // ��¼����
    };

public:
    CMemTrack();
    ~CMemTrack();

    /// ��ʼ(�����³�ʼ�����м�¼����)
    void Init();

    /// ����(����ֹͣ��¼״̬������֮����ӡ������������ڴ��ͷ�)
    void Fini();

    /// ��ȡ�ļ�·�����е�����
    static const char *GetFileName(const char *file);

    /// �ڴ�����ʱ�Ĺ켣
    void AddTrack(void *address, size_t size, const char *file, int line);

    /// �ڴ��ͷ�ʱ�Ĺ켣
    void RemoveTrack(void *address, const char *file, int line);

    /// ���úͻ�ȡ��ϸ��¼����
    void SetRecordDetail(bool enable, bool only_cur_task, const char *only_file_name);
    bool GetRecordDetail(const char *file);

    /// �����Ƿ��¼�������ջ
    void SetRecordAllocCallstack(bool enable) {m_record_alloc_callstack = enable;}

    /// ��ӡ�ڴ���Ϣ
    void DumpMemInfo();

public:
    static int m_record_status;

private:
    MAP_ALLOC m_track_alloc_info;
    MAP_ALLOC m_track_free_info;
    int m_alloc_count;
    int m_free_count;
    int m_repeat_free_count;
    int m_over_write_count;
    objLock *m_pLock;
    int m_track_inside;
    bool m_record_detail;
    DWORD m_cur_task_id;
    char m_file_name[DCOP_FILE_NAME_LEN];
    bool m_record_alloc_callstack;
};


#endif // #ifndef _TOOL_MEM_MEM_H_

