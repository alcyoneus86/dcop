/// -------------------------------------------------
/// taskStub_linux.cpp : linux����ϵͳ����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "dcop.h"

#if DCOP_OS == DCOP_OS_LINUX

/// -------------------------------------------------
/// ʵ������ӿ�׮
/// -------------------------------------------------
#include "taskApi.h"
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

/*******************************************************
  �� �� ��: STUB_TaskCreate
  ��    ��: ��������׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_TaskCreate(OSHANDLE *pHandle,
            const char *cszName,
            DWORD *pdwID,
            void (*pEntry)(void *pPara),
            DWORD dwStackSize,
            DWORD dwPriority,
            void *pPara)
{
    (void)dwPriority;
    /// linuxϵͳĬ����SCHED_OTHER���Ȳ��ԣ��ǲ�֧���������ȼ��ģ��Ժ����ʹ����������֧��

    if (!pHandle)
    {
        return ERRCODE_TASK_WRONG_HANDLE;
    }

    pthread_t a_thread;
    pthread_attr_t thread_attr;

    DWORD dwRc = pthread_attr_init(&thread_attr);
    if (dwRc != 0)
    {
        return (errno)? (DWORD)(errno) : ERRCODE_TASK_CREATE_FAIL;
    }

    if (dwStackSize)
    {
        (void)pthread_attr_setstacksize(&thread_attr, dwStackSize);
    }

    dwRc = (DWORD)pthread_create(&a_thread, &thread_attr, (void* (*)(void*))pEntry, pPara);
    (void)pthread_attr_destroy(&thread_attr);
    if (dwRc != 0)
    {
        return (errno)? (DWORD)(errno) : ERRCODE_TASK_CREATE_FAIL;
    }

    *pHandle = (OSHANDLE)a_thread;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: STUB_TaskDestroy
  ��    ��: ɾ������׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_TaskDestroy(OSHANDLE Handle, 
        const char *cszName,
        DWORD dwID)
{
    pthread_t a_thread = (pthread_t)Handle;

    if (pthread_self() == a_thread)
    {
        /// �μ�taskStub_Win32�е�ע�ͣ��˳�������Ҫ���������Լ��˳�
        /// pthread_exit(NULL);
    }
    else
    {
        (void)pthread_cancel(a_thread);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: STUB_TaskDelay
  ��    ��: ������ʱ׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void STUB_TaskDelay(DWORD dwMilliseconds)
{
    /// ��linux�µ���sleep����ʱ�ӵ�,һ�����̵�ʱ��ϵͳ�������Ƶ�
    /// ���ÿ���߳�ʹ��sleep,������������,���ս��̻����.�������selectȡ��
    /// usleep��nanosleep��̫��ȷ.sleep�����յ�ϵͳ�ź���ǰ����
    /// select��ʹ��tick��ѯ��ʽ��(����Ч�ʲ���,���ȵ�λ��tick�ĵ�λ:һ��10ms)

    struct timeval timeout;

    timeout.tv_sec = dwMilliseconds / THOUSAND;
    timeout.tv_usec = (dwMilliseconds % THOUSAND) * THOUSAND;

    (void)select(0, NULL, NULL, NULL, &timeout);
}

/*******************************************************
  �� �� ��: STUB_TaskCurrent
  ��    ��: ��ȡ��ǰ����׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD STUB_TaskCurrent()
{
    return (DWORD)syscall(SYS_gettid);
}

/*******************************************************
  �� �� ��: vRegOsTaskStubFunc
  ��    ��: ע������׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void vRegOsTaskStubFunc()
{
    TaskFuncs funcs = 
    {
        STUB_TaskCreate,
        STUB_TaskDestroy,
        STUB_TaskDelay,
        STUB_TaskCurrent
    };

    vSetTaskFuncs(&funcs);
}

/*******************************************************
  �� �� ��: CPPBUILDUNIT_AUTO
  ��    ��: �Զ���װ����׮
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CPPBUILDUNIT_AUTO(vRegOsTaskStubFunc, 0);


/// -------------------------------------------------
/// ʵ��ԭ�Ӳ���
/// -------------------------------------------------
#include "task.h"

/*******************************************************
  �� �� ��: objAtomic::Inc
  ��    ��: ԭ�ӵ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objAtomic::T objAtomic::Inc(T &val)
{
    return (T)__sync_add_and_fetch(&val, 1);
}

/*******************************************************
  �� �� ��: objAtomic::Dec
  ��    ��: ԭ�ӵݼ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objAtomic::T objAtomic::Dec(T &val)
{
    return (T)__sync_sub_and_fetch(&val, 1);
}

/*******************************************************
  �� �� ��: objAtomic::Add
  ��    ��: ԭ�Ӽ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objAtomic::T objAtomic::Add(T &val, T add)
{
    return (T)__sync_add_and_fetch(&val, add);
}

/*******************************************************
  �� �� ��: objAtomic::Sub
  ��    ��: ԭ�Ӽ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objAtomic::T objAtomic::Sub(T &val, T sub)
{
    return (T)__sync_sub_and_fetch(&val, sub);
}

/*******************************************************
  �� �� ��: objAtomic::Set
  ��    ��: ԭ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objAtomic::T objAtomic::Set(T &val, T set)
{
    return (T)__sync_lock_test_and_set(&val, set);
}

/*******************************************************
  �� �� ��: objAtomic::CAS
  ��    ��: ԭ�ӱȽϽ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool objAtomic::CAS(T &val, T cmp, T swap)
{
    return __sync_bool_compare_and_swap(&val, cmp, swap);
}


/// -------------------------------------------------
/// ʵ�����������
/// -------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

/// -------------------------------------------------
/// linux dev�������
/// -------------------------------------------------
class CDevRandom : public objRandom
{
public:
    static const DWORD SRAND_COUNT = 32;
    static const DWORD TRY_COUNT = 16;

public:
    CDevRandom();
    ~CDevRandom();

    void Gen(void *pBuf, DWORD dwLen);

private:
    void Rand(void *pBuf, DWORD dwLen);
};

/*******************************************************
  �� �� ��: CDevRandom::CDevRandom
  ��    ��: CDevRandom����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDevRandom::CDevRandom()
{
    unsigned int ticks;
    struct timeval tv;
    int fd;
    gettimeofday(&tv, NULL);
    ticks = tv.tv_sec + tv.tv_usec;
    fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) // linux����п�����0��ʧ��ʱ����(-1)
    {
        unsigned int r;
        for (DWORD i = 0; i < SRAND_COUNT; i++)
        {
            (void)read(fd, &r, sizeof(r));
            ticks += r;
        }

        close(fd);
    }

    srand(ticks);
}

/*******************************************************
  �� �� ��: CDevRandom::~CDevRandom
  ��    ��: CDevRandom����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDevRandom::~CDevRandom()
{
}

/*******************************************************
  �� �� ��: CDevRandom::Gen
  ��    ��: ��ȡ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDevRandom::Gen(void *pBuf, DWORD dwLen)
{
    if (!pBuf || !dwLen)
    {
        return;
    }

    DWORD dwReadLen = 0;

    /// "/dev/urandom"�豸�������������ǿ��ܳ��ֻ�ȡʧ��
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0)
    {
        DWORD dwTryTimes = 0;
        while ((dwTryTimes < TRY_COUNT) && (dwReadLen < dwLen))
        {
            int r = read(fd, (BYTE *)pBuf + dwReadLen, dwLen - dwReadLen);
            if (r > 0) dwReadLen += (DWORD)r;
            dwTryTimes++;
        }

        close(fd);
    }

    /// ��ȡʧ�ܺ��ֻ�����÷����������ȡ
    if (dwReadLen < dwLen)
    {
        Rand((BYTE *)pBuf + dwReadLen, dwLen - dwReadLen);
    }
}

/*******************************************************
  �� �� ��: CDevRandom::Rand
  ��    ��: ģ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDevRandom::Rand(void *pBuf, DWORD dwLen)
{
    DWORD dwReadLen = 0;

    while (dwReadLen < dwLen)
    {
        *((BYTE *)pBuf + dwReadLen) = (BYTE)(rand() % 256);
        dwReadLen++;
    }
}

/*******************************************************
  �� �� ��: objRandom::CreateInstance
  ��    ��: ���������ʵ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objRandom *objRandom::CreateInstance(const char *file, int line)
{
    #undef new
    return new (file, line) CDevRandom();
    #define new new(__FILE__, __LINE__)
}

/*******************************************************
  �� �� ��: objRandom::~objRandom
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objRandom::~objRandom()
{
}


/// -------------------------------------------------
/// ʵ�ֻ�ȡ����ջ����
/// -------------------------------------------------
#include <execinfo.h>

#define CALLSTACK_DEPTH_MAX 10

/*******************************************************
  �� �� ��: ShowCallStack
  ��    ��: ��ʾ����ջ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void ShowCallStack(LOG_PRINT print, LOG_PARA para, int depth)
{
    void *stack_addr[CALLSTACK_DEPTH_MAX];
    int layer = 0;
    char **ppstack_funcs = 0;

    if (!print)
    {
        print = PrintToConsole;
        para = 0;
    }

#undef free

    layer = backtrace(stack_addr, ARRAY_SIZE(stack_addr));
    ppstack_funcs = backtrace_symbols(stack_addr, layer);
    if (!ppstack_funcs)
    {
        print("Obtained CallStack Failed! \r\n", para);
        free(ppstack_funcs);
        return;
    }

    if (depth && (layer > (depth + 1)))
    {
        layer = depth + 1;
    }

    print(STR_FORMAT("Obtained CallStack %d Frames! \r\n", (layer - 1)), para);
    for(int i = 1; i < layer; i++)
    {
        print(STR_FORMAT("%s \r\n", ppstack_funcs[i]), para);
    }

    free(ppstack_funcs);

#define free(ptr) DCOP_FreeEx(ptr, __FILE__, __LINE__)
}


/// -------------------------------------------------
/// ��̬����ز���
/// -------------------------------------------------
#include <dlfcn.h>

/// -------------------------------------------------
/// linux ��̬��
/// -------------------------------------------------
class CDynamicLibrary : public objDynamicLoader
{
public:
    CDynamicLibrary();
    ~CDynamicLibrary();

    DWORD Load(const char *dllFile);
    DWORD Unload();

    void *FindSymbol(const char *symName);

    void SetErrLog(LOG_PRINT logPrint, LOG_PARA logPara);

private:
    void *m_handle;
    LOG_PRINT m_logPrint;
    LOG_PARA m_logPara;
};

/*******************************************************
  �� �� ��: CDynamicLibrary::CDynamicLibrary
  ��    ��: CDynamicLibrary����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDynamicLibrary::CDynamicLibrary()
{
    m_handle = NULL;
    m_logPrint = PrintToConsole;
    m_logPara = 0;
}

/*******************************************************
  �� �� ��: CDynamicLibrary::~CDynamicLibrary
  ��    ��: CDynamicLibrary����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDynamicLibrary::~CDynamicLibrary()
{
    (void)Unload();
}

/*******************************************************
  �� �� ��: CDynamicLibrary::Load
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDynamicLibrary::Load(const char *dllFile)
{
    if (!dllFile || !(*dllFile))
    {
        return FAILURE;
    }

    if (m_handle)
    {
        return FAILURE;
    }

    void *pf = dlopen(dllFile, RTLD_NOW | RTLD_GLOBAL);
    if (!pf)
    {
        PrintLog(dlerror(), m_logPrint, m_logPara);
        return FAILURE;
    }

    m_handle = pf;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDynamicLibrary::Unload
  ��    ��: ж��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDynamicLibrary::Unload()
{
    if (!m_handle)
    {
        return FAILURE;
    }

    int rc = dlclose(m_handle);
    if (rc)
    {
        PrintLog(dlerror(), m_logPrint, m_logPara);
        return FAILURE;
    }

    m_handle = NULL;
    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDynamicLibrary::~CDynamicLibrary
  ��    ��: CDynamicLibrary����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CDynamicLibrary::FindSymbol(const char *symName)
{
    if (!symName || !(*symName))
    {
        return NULL;
    }

    if (!m_handle)
    {
        return NULL;
    }

    void *pSymAddr = dlsym(m_handle, symName);
    if (!pSymAddr)
    {
        PrintLog(dlerror(), m_logPrint, m_logPara);
        return NULL;
    }

    return pSymAddr;
}

/*******************************************************
  �� �� ��: CDynamicLibrary::~CDynamicLibrary
  ��    ��: CDynamicLibrary����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDynamicLibrary::SetErrLog(LOG_PRINT logPrint, LOG_PARA logPara)
{
    m_logPrint = (logPrint)? logPrint : PrintToConsole;
    m_logPara = (logPrint)? logPara : 0;
}

/*******************************************************
  �� �� ��: objDynamicLoader::CreateInstance
  ��    ��: ������̬����ʵ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objDynamicLoader *objDynamicLoader::CreateInstance(const char *file, int line)
{
    #undef new
    return new (file, line) CDynamicLibrary();
    #define new new(__FILE__, __LINE__)
}

/*******************************************************
  �� �� ��: objDynamicLoader::~objDynamicLoader
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objDynamicLoader::~objDynamicLoader()
{
}


#endif // #if DCOP_OS == DCOP_OS_LINUX

