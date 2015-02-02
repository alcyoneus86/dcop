/// -------------------------------------------------
/// os.h : OS���幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _DCOP_OS_H_
#define _DCOP_OS_H_

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#if DCOP_OS == DCOP_OS_LINUX
#include <stdint.h>
#endif
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <time.h>
#ifdef __cplusplus
#include <new>
#endif


//////////////////////////////////////////////////////
/// OS���
//////////////////////////////////////////////////////
typedef void               *OSHANDLE;


//////////////////////////////////////////////////////
/// ��Զ�ȴ�
//////////////////////////////////////////////////////
#define OSWAIT_FOREVER      0xFFFFFFFFL


//////////////////////////////////////////////////////
/// NAME����
//////////////////////////////////////////////////////
#define OSNAME_LENGTH       16


//////////////////////////////////////////////////////
/// BUFͷ��ħ����
//////////////////////////////////////////////////////
#define OSBUF_MAGIC         "Dcop"
#define OSBUF_MAGIC0        'D'
#define OSBUF_MAGIC1        'c'
#define OSBUF_MAGIC2        'o'
#define OSBUF_MAGIC3        'p'


//////////////////////////////////////////////////////
/// ʹ������ļ������ں�����β������"ERROR_LABEL"
///     ��ǩ, �����д�����
//////////////////////////////////////////////////////
#define ERROR_CHECK(exp)        \
    do                          \
    {                           \
        if (!(exp))             \
        {                       \
            goto ERROR_LABEL;   \
        }                       \
    }while(0)


//////////////////////////////////////////////////////
/// �ص��������ú�(�з���ֵ��һ�����)
/// �÷�ʾ��: dwRc = BACKFUNC_CALL(callback)(...);
///     �൱�ڵ��� dwRc = callback(...);
//////////////////////////////////////////////////////
#define BACKFUNC_CALL(pFunc) \
    (!(pFunc))? (FAILURE) : (pFunc)

//////////////////////////////////////////////////////
/// �ص��������ú�(�з���ֵ��һ�����)
/// �÷�ʾ��: b = BBACKFUNC_CALL(callback)(...);
///     �൱�ڵ��� b = callback(...);
//////////////////////////////////////////////////////
#define BBACKFUNC_CALL(pFunc) \
    (!(pFunc))? (false) : (pFunc)

//////////////////////////////////////////////////////
/// �ص��������ú�(�޷���ֵ��һ�����)
/// �÷�ʾ��: VDBACKFUNC_CALL(callback)(...);
///     �൱�ڵ��� callback(...);
//////////////////////////////////////////////////////
#define VDBACKFUNC_VDCALL(pFunc) \
    if (pFunc) (pFunc)

//////////////////////////////////////////////////////
/// �ص��������ú�(�з���ֵ��void����)
/// �÷�ʾ��: BACKFUNC_VDCALL(callback)(...);
///     �൱�ڵ��� (void)callback(...);
//////////////////////////////////////////////////////
#define BACKFUNC_VDCALL(pFunc) \
    if (pFunc) (void)(pFunc)


/////////////////////////////////////////////////////
/// �����ڴ�ӿڣ����㶨λ�ڴ�����
/////////////////////////////////////////////////////
/// �����ε���ĵط��ĺ궨��
#ifdef __cplusplus
    #ifdef new
        #undef new
    #endif
    #ifdef delete
        #undef delete
    #endif
#endif

#ifdef malloc
    #undef malloc
#endif

#ifdef free
    #undef free
#endif


/////////////////////////////////////////////////////
///                 �ڴ�������ͷ�
/// -------------------------------------------------
/// �ڴ�������ͷ���ʹ�ú�:
///     DCOP_Malloc
///     DCOP_Free
/// �򿪺͹ر��ڴ����״̬:
///     DebugMemStatus(1);  // ������ļ���
///     DebugMemStatus(2);  // ��printf��ʽ���
///     DebugMemStatus(3);  // ͬʱ���ļ�+printf���
///     DebugMemStatus(0);  // �ر����
/// ��ӡ�ڴ������Ϣ:
///     DumpMemInfo();
/////////////////////////////////////////////////////
#ifdef __cplusplus
    extern "C" {
#endif
extern void *               DCOP_MallocEx(size_t size, const char *file, int line);
extern void                 DCOP_FreeEx(void *p, const char *file, int line);
extern void *               DCOP_ReallocEx(void *p, size_t size, const char *file, int line);
extern void                 DebugMemStatus(int status);
extern void                 OutputMemLog(int console);
extern void                 RecordMemDetail(int enable, int only_cur_task, const char *only_file_name);
extern void                 RecordAllocCallstack(int enable);
extern void                 DumpMemInfo();
#ifdef __cplusplus
    }
#endif
#ifdef __cplusplus
    #ifdef _MSC_VER
        #pragma warning(disable:4290)
    #endif
    inline void *operator   new(size_t size) throw (std::bad_alloc) {return DCOP_MallocEx(size, 0, 0);}
    inline void *operator   new[](size_t size) throw (std::bad_alloc) {return DCOP_MallocEx(size, 0, 0);}
    inline void *operator   new(size_t size, const char *file, int line) throw (std::bad_alloc) {return DCOP_MallocEx(size, file, line);}
    inline void *operator   new[](size_t size, const char *file, int line) throw (std::bad_alloc) {return DCOP_MallocEx(size, file, line);}
    inline void operator    delete(void *p) throw() {DCOP_FreeEx(p, 0, 0);}
    inline void operator    delete[](void *p) throw() {DCOP_FreeEx(p, 0, 0);}
    inline void operator    delete(void *p, const char *file, int line) throw() {DCOP_FreeEx(p, file, line);}
    inline void operator    delete[](void *p, const char *file, int line) throw() {DCOP_FreeEx(p, file, line);}
#endif

/////////////////////////////////////////////////////
/// ����Ҫ�����ļ���ض���new
/////////////////////////////////////////////////////
#ifdef __cplusplus
    #define new             new(__FILE__, __LINE__)
#endif

/////////////////////////////////////////////////////
/// �����ڴ�������ͷŽӿ�
/////////////////////////////////////////////////////
#define DCOP_Malloc(size)   DCOP_MallocEx((size_t)size, __FILE__, __LINE__)
#define DCOP_Free(ptr)      DCOP_FreeEx(ptr, __FILE__, __LINE__)
#define DCOP_Realloc(ptr,size) DCOP_ReallocEx(ptr, (size_t)size, __FILE__, __LINE__)
#define malloc(size)        DCOP_MallocEx((size_t)size, __FILE__, __LINE__)
#define free(ptr)           DCOP_FreeEx(ptr, __FILE__, __LINE__)
#define realloc(ptr,size)   DCOP_ReallocEx(ptr, (size_t)size, __FILE__, __LINE__)


/////////////////////////////////////////////////////
///              ��־�ӿ�ʹ��˵��
/// -------------------------------------------------
/// ��¼������Ϣ:
///     TRACE_LOG("������Ϣ");
///     TRACE_LOG(STR_FORMAT("������Ϣ:%s", str));
/// ��������Ϣ:
///     CHECK_ERRCODE(RC, "������Ϣ");
///     CHECK_ERRCODE(RC, STR_FORMAT("������Ϣ:%s", str));
/// �򿪺͹رյ�����־����:
///     DebugLogStatus(1);
///     DebugLogStatus(0);
/////////////////////////////////////////////////////

/// [ע��] �ַ�����ʽ������󳤶Ȳ�����256(����'\0')
#define STR_FORMAT_LEN_MAX          256

/// ��¼������Ϣ(�п��ؿ��Ƶĵ�����Ϣ)
#define TRACE_LOG(Info)             TraceLogEx(Info, __FILE__, __LINE__)
#define TRACE_BUF(Info, Buf, Len)   TraceBufEx(Info, Buf, Len, __FILE__, __LINE__)

/// ��鷵����(������SUCESS��Ч;��¼������־�����Ḵλ)
#define CHECK_RETCODE(Rc, Info)     CheckRetCodeEx((int)(Rc), (int)SUCCESS, (const char *)(Info), __FILE__, __LINE__)

/// ��������(������SUCESS��Ч;��¼������־���һḴλ)
#define CHECK_ERRCODE(Rc, Info)     CheckErrCodeEx((int)(Rc), (int)SUCCESS, (const char *)(Info), __FILE__, __LINE__)

/// ��־��ӡ�ص�
typedef void (*LOG_PRINT)(const char *info, void *para);
typedef void * LOG_PARA;

/// ��ӡ��־
#define PrintLog(Info, Print, Para) PrintLogEx((const char *)(Info), (LOG_PRINT)(Print), (LOG_PARA)(Para), __FILE__, __LINE__)

/// ��ӡ�ڴ�
#define PrintBuffer(Info, Buf, Len, Print, Para) PrintBufferEx((const char *)(Info), (const void *)(Buf), (size_t)(Len), (LOG_PRINT)(Print), (LOG_PARA)(Para), __FILE__, __LINE__)

/// ������Ϣ�ʹ�����
#ifdef __cplusplus
    extern "C" {
#endif
extern void ShowCallStack(LOG_PRINT print, LOG_PARA para, int depth);
extern void TraceLogEx(const char *info, const char *file, int line);
extern void TraceBufEx(const char *info, const void *buf, size_t len, const char *file, int line);
extern void CheckRetCodeEx(int rc, int expect, const char *info, const char *file, int line);
extern void CheckErrCodeEx(int rc, int expect, const char *info, const char *file, int line);
extern void DebugLogStatus(int status);
extern int  GetLogTime(char *szStr, int strLen);
extern int  GetLogFileNameAndLine(char *szStr, int strLen, const char *file, int line);
extern void PrintLogEx(const char *info, LOG_PRINT print, LOG_PARA para, const char *file, int line);
extern void PrintBufferEx(const char *info, const void *buf, size_t len, LOG_PRINT print, LOG_PARA para, const char *file, int line);
extern void PrintToConsole(const char *info, void *para);
#ifdef __cplusplus
    }
#endif


/////////////////////////////////////////////////////
/// ��ʽ������ַ���[�÷�:TRACE_LOG(STR_FORMAT("xxx:%s", yyy));]
/// -------------------------------------------------
#ifdef __cplusplus
#define STR_FORMAT (const char *)CStrFormat
class CStrFormat
{
public:
    CStrFormat(const char *format, ...)
    {
        va_list args;

        va_start(args, format);
        (void)vsnprintf(m_szStr, sizeof(m_szStr), format, args);
        va_end(args);

        m_szStr[sizeof(m_szStr) - 1] = '\0';
    }
    ~CStrFormat() {}

    operator const char *() { return m_szStr;}

private:
    CStrFormat() {}
    char m_szStr[STR_FORMAT_LEN_MAX];
};
#endif
/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
/// cpp���뵥Ԫ����tmp���Զ�ִ��
/// ����ȫ��������Զ������乹�캯����ԭ��
/// -------------------------------------------------
#ifdef __cplusplus
#define CPPBUILDUNIT_AUTO(constructFunc, destroyFunc) \
class CBuildUnitAutoTmp##constructFunc##destroyFunc \
{                                                   \
public:                                             \
    CBuildUnitAutoTmp##constructFunc##destroyFunc() \
    {                                               \
        void (*__fn__)() = constructFunc;           \
        if (__fn__) __fn__();                       \
    }                                               \
    ~CBuildUnitAutoTmp##constructFunc##destroyFunc() \
    {                                               \
        void (*__fn__)() = destroyFunc;             \
        if (__fn__) __fn__();                       \
    }                                               \
} __tmp##constructFunc##destroyFunc
#endif // #ifdef __cplusplus
/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
/// ����.a���ߺ���ȫ�ֶ����.o�е�ǿ������
/// �޷�����ȫ�ֶ�����޷��Զ�ִ������ĺ���
/// -------------------------------------------------
#ifdef __cplusplus
#define CPPBUILDUNIT_FORCELINK(constructFunc, destroyFunc) \
class CBuildUnitAutoTmp##constructFunc##destroyFunc; \
extern CBuildUnitAutoTmp##constructFunc##destroyFunc __tmp##constructFunc##destroyFunc; \
void *__forcelink##constructFunc##destroyFunc = (void *)&__tmp##constructFunc##destroyFunc
#endif // #ifdef __cplusplus
/////////////////////////////////////////////////////


//////////////////////////////////////////////////////
/// ASSERT�����
//////////////////////////////////////////////////////
#ifdef __cplusplus
    extern "C" {
#endif
extern void DCOP_Assert(int x, const char *file, int line);
#ifdef __cplusplus
    }
#endif
#define OSASSERT(x) DCOP_Assert((int)(x), __FILE__, __LINE__)


/////////////////////////////////////////////////////
/// ��λ����
/////////////////////////////////////////////////////
#ifdef __cplusplus
    extern "C" {
#endif
extern void DCOP_Reset(int type, const char *file, int line);
#ifdef __cplusplus
    }
#endif
#define OSREBOOT(type) DCOP_Reset((int)(type), __FILE__, __LINE__)


/////////////////////////////////////////////////////
/// ��ͬos�µ�C�����
/////////////////////////////////////////////////////
#if DCOP_OS == DCOP_OS_WINDOWS
    #define snprintf    _snprintf
    #define stricmp     _stricmp
    #define strnicmp    _strnicmp
#endif
#if DCOP_OS == DCOP_OS_LINUX
    #include <strings.h>
    #define stricmp     strcasecmp
    #define strnicmp    strncasecmp
#endif


/////////////////////////////////////////////////////
/// ��ȡ����ʱ��
/////////////////////////////////////////////////////
#define DCOP_START_TIME()   clock_t __start_time=clock()
#define DCOP_END_TIME()     clock_t __end_time=clock()
#define DCOP_COST_TIME()    int((__end_time-__start_time)/(CLOCKS_PER_SEC/1000))


#endif // #ifndef _DCOP_OS_H_

