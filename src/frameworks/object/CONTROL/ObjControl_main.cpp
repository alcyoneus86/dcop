/// -------------------------------------------------
/// ObjControl_main.cpp : ����������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjControl_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "string/tablestring.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CControl, "control")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CControl)
    DCOP_IMPLEMENT_INTERFACE(IControl)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CControl)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CControl::CControl
  ��    ��: CControl����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CControl::CControl(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CControl::~CControl
  ��    ��: CControl����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CControl::~CControl()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CControl::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CControl::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CControl::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControl::Fini()
{
}

/*******************************************************
  �� �� ��: CControl::Proc
  ��    ��: ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControl::Proc(objMsg *msg)
{
    if (!msg)
    {
        return;
    }
}

/*******************************************************
  �� �� ��: CControl::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControl::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);

    for (IT_CHAIN it = m_chains.begin(); it != m_chains.end(); ++it)
    {
        ((*it).second).Dump(logPrint, logPara, argc, argv);
    }
}

/*******************************************************
  �� �� ��: CControl::CreateChain
  ��    ��: ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IControl::IChain *CControl::CreateChain(IObject *ctrlee)
{
    if (!ctrlee) return NULL;

    AutoObjLock(this);

    IT_CHAIN it = m_chains.find(ctrlee->ID());
    if (it != m_chains.end())
    {
        return &((*it).second);
    }

    CControlChain chain;
    it = m_chains.insert(m_chains.end(), MAP_CHAIN::value_type(ctrlee->ID(), chain));
    if (it == m_chains.end())
    {
        return NULL;
    }

    CControlChain *pChain = &((*it).second);
    pChain->SetCtrlee(ctrlee);

    return pChain;
}

/*******************************************************
  �� �� ��: CControl::DestroyChain
  ��    ��: ɾ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControl::DestroyChain(IChain *chain)
{
    if (!chain) return;

    AutoObjLock(this);

    CControlChain *pChain = (CControlChain *)chain;
    IObject *piCtrlee = pChain->GetCtrlee();
    if (!piCtrlee)
    {
        return;
    }

    (void)m_chains.erase(piCtrlee->ID());
}

/*******************************************************
  �� �� ��: CControl::RegCtrlNode
  ��    ��: ע����Ƶ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CControl::RegCtrlNode(IObject *ctrler,
                    DWORD ctrlee,
                    Node *ctrls,
                    DWORD count)
{
    AutoObjLock(this);

    IT_CHAIN it = m_chains.find(ctrlee);
    if (it == m_chains.end())
    {
        return FAILURE;
    }

    CControlChain *pChain = &((*it).second);
    return pChain->RegCtrlNode(ctrler, ctrls, count);
}

