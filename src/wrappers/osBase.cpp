/// -------------------------------------------------
/// osBase.cpp : ����ϵͳ����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "osBase.h"
#include "task.h"
#include "string/tablestring.h"


/// -------------------------------------------------
/// OS����Ԫ�ؼ���
/// -------------------------------------------------
static LIST_HEAD(osBase) g_osBaseHead[OSTYPE_NUM] =
{
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}
};

/// -------------------------------------------------
/// OS����Ԫ�ؼ��ϲ���������
/// -------------------------------------------------
objSpinLock g_osBaseLock;
objSpinLock *g_pOsBaseLock = &g_osBaseLock;


/*******************************************************
  �� �� ��: osBase::osBase
  ��    ��: osBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
osBase::osBase()
{
    m_hHandle = 0;
    m_osType = OSTYPE_NULL;
    m_objPtr = NULL;
    (void)memset(m_szName, 0, sizeof(m_szName));
    m_dwID = 0;
    LIST_NODE_INIT(this, m_field);
}

/*******************************************************
  �� �� ��: osBase::~osBase
  ��    ��: osBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
osBase::~osBase()
{
    vDelFromList();
}

/*******************************************************
  �� �� ��: osBase::vSetName
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void osBase::vSetName(const char *cszName)
{
    if (!cszName || !(*cszName))
    {
        return;
    }

    (void)snprintf(m_szName, sizeof(m_szName), "%s", cszName);
    m_szName[sizeof(m_szName) - 1] = '\0';
}

/*******************************************************
  �� �� ��: osBase::vAddToList
  ��    ��: ��ӵ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void osBase::vAddToList(DWORD osType, void *objPtr)
{
    if (osType >= OSTYPE_NUM)
    {
        return;
    }

    m_osType = osType;
    m_objPtr = objPtr;

    AutoSpinLock(g_pOsBaseLock);
    LIST_INSERT_HEAD(&g_osBaseHead[osType], this, m_field);
}

/*******************************************************
  �� �� ��: osBase::vAddToList
  ��    ��: �Ӽ�����ɾ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void osBase::vDelFromList()
{
    if (m_osType >= OSTYPE_NUM)
    {
        return;
    }

    AutoSpinLock(g_pOsBaseLock);
    LIST_REMOVE(&g_osBaseHead[m_osType], this, m_field);
}

/*******************************************************
  �� �� ��: osBase::First
  ��    ��: ��ʼԪ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
osBase *osBase::First(DWORD osType)
{
    if (osType >= OSTYPE_NUM)
    {
        return NULL;
    }

    AutoSpinLock(g_pOsBaseLock);
    return LIST_FIRST(&g_osBaseHead[osType]);
}

/*******************************************************
  �� �� ��: osBase::Next
  ��    ��: ��һ��Ԫ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
osBase *osBase::Next()
{
    AutoSpinLock(g_pOsBaseLock);
    return LIST_NEXT(this, m_field);
}

/*******************************************************
  �� �� ��: osBase::Find
  ��    ��: ͨ�����ֲ���Ԫ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
osBase *osBase::Find(DWORD osType, const char *cszName)
{
    if (osType >= OSTYPE_NUM)
    {
        return NULL;
    }

    AutoSpinLock(g_pOsBaseLock);
    osBase *pBase = LIST_FIRST(&g_osBaseHead[osType]);
    while (pBase)
    {
        if (!strcmp(pBase->cszGetName(), cszName))
        {
            break;
        }

        pBase = LIST_NEXT(pBase, m_field);
    }

    return pBase;
}

/*******************************************************
  �� �� ��: osBase::Find
  ��    ��: ͨ��ID����Ԫ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
osBase *osBase::Find(DWORD osType, DWORD dwID)
{
    if (osType >= OSTYPE_NUM)
    {
        return NULL;
    }

    AutoSpinLock(g_pOsBaseLock);
    osBase *pBase = LIST_FIRST(&g_osBaseHead[osType]);
    while (pBase)
    {
        if (pBase->dwGetID() == dwID)
        {
            break;
        }

        pBase = LIST_NEXT(pBase, m_field);
    }

    return pBase;
}

/*******************************************************
  �� �� ��: osBase::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void osBase::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    DWORD osType = (argc)? *(DWORD *)(argv[0]) : OSTYPE_NULL;

    objTask *pTask = objTask::Current();
    PrintLog(STR_FORMAT("Current Task: '%s'(%d)", 
                        (pTask)? pTask->Name() : "Null", 
                        (pTask)? pTask->ID() : 0), 
                        PrintToConsole, 0);
    ShowCallStack(0, 0, 0);

    AutoSpinLock(g_pOsBaseLock);

    for (DWORD i = 0; i < OSTYPE_NUM; ++i)
    {
        if ((osType < OSTYPE_NUM) && (osType != i))
        {
            continue;
        }

        osBase *pBase = LIST_FIRST(&g_osBaseHead[i]);
        if (!pBase)
        {
            continue;
        }

        DWORD dwCount = (DWORD)LIST_COUNT(&g_osBaseHead[i]);
        CTableString tableStr(5, dwCount + 1, "  ");
        tableStr << "Type";
        tableStr << "Name";
        tableStr << "ID";
        tableStr << "Handle";
        tableStr << "Ptr";

        DCOP_START_TIME();

        while (pBase)
        {
            DWORD dwType = pBase->osGetType();
            tableStr << ((dwType >= OSTYPE_NUM)? "OSTYPE_NULL" : OSTYPE_INFO[dwType]);
            tableStr << pBase->cszGetName();
            tableStr << STR_FORMAT("%d", pBase->dwGetID());
            tableStr << STR_FORMAT("%p", pBase->hGetHandle());
            tableStr << STR_FORMAT("%p", pBase->objGetPtr());
            pBase = LIST_NEXT(pBase, m_field);
        }

        DCOP_END_TIME();

        logPrint(STR_FORMAT("%s Dump: (Count: %d) \r\n", OSTYPE_INFO[i], dwCount), logPara);
        tableStr.Show(logPrint, logPara, "=", "-");
        logPrint(STR_FORMAT("[cost time: %d ms] \r\n", DCOP_COST_TIME()), logPara);
    }
}

