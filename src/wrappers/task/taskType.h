/// -------------------------------------------------
/// taskType.h : �����װ�๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TASKTYPE_H_
#define _TASKTYPE_H_

#include "../osBase.h"
#include "task.h"


class CTaskBase : public objTask, private osBase
{
public:
    CTaskBase();
    ~CTaskBase();

    DWORD Create(OSTASK_ENTRY pEntry,
            DWORD dwStackSize,
            DWORD dwPriority,
            IPara *pPara);

    DWORD Destroy();

    const char *GetName() {return osBase::cszGetName();}
    DWORD GetID() {return osBase::dwGetID();}

    void SetName(const char *cszName) {osBase::vSetName(cszName);}
    void SetID(DWORD dwID) {osBase::vSetID(dwID);}

    static DWORD Current();

private:
    static void vAllTaskEntry(void *pPara);
    OSTASK_ENTRY pGetEntry() const {return m_pEntry;}
    IPara *pGetPara() const {return m_pPara;}

private:
    OSTASK_ENTRY    m_pEntry;
    DWORD           m_dwStackSize;
    DWORD           m_dwPriority;
    IPara *         m_pPara;
};


#endif // #ifndef _TASKTYPE_H_