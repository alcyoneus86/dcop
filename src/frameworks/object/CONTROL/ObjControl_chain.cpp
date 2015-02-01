/// -------------------------------------------------
/// ObjControl_chain.cpp : ����������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjControl_chain.h"
#include "string/tablestring.h"


/*******************************************************
  �� �� ��: CControlChain::CControlChain
  ��    ��: CControlChain���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CControlChain::CControlChain() : m_aNodeList(sizeof(Node))
{
    m_piCtrlee = 0;
}

/*******************************************************
  �� �� ��: CControlChain::~CControlChain
  ��    ��: CControlChain��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CControlChain::~CControlChain()
{
    m_piCtrlee = 0;
}

/*******************************************************
  �� �� ��: CControlChain::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControlChain::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(m_piCtrlee);

    if (m_piCtrlee)
    {
        logPrint(STR_FORMAT("'%s'(%d) Ctrl Chain Dump: (Count: %d) \r\n", 
                        m_piCtrlee->Name(), 
                        m_piCtrlee->ID(), 
                        m_aNodeList.Count()), 
                        logPara);
    }
    else
    {
        logPrint(STR_FORMAT("'Null'(0) Ctrl Chain Dump: (Count: %d) \r\n", 
                        m_aNodeList.Count()), 
                        logPara);
    }
    CTableString tableStr(7, m_aNodeList.Count() + 1, "  ");
    tableStr << "attribute";
    tableStr << "ctrl";
    tableStr << "ack";
    tableStr << "usergroup";
    tableStr << "tty";
    tableStr << "function";
    tableStr << "ctrler";

    for (DWORD i = 0; i < m_aNodeList.Count(); ++i)
    {
        Node *pNode = (Node *)m_aNodeList.Pos(i);
        if (!pNode)
        {
            continue;
        }

        tableStr << STR_FORMAT("%d", pNode->m_attribute);
        tableStr << STR_FORMAT("%d", pNode->m_ctrl);
        tableStr << STR_FORMAT("%d", pNode->m_ack);
        tableStr << STR_FORMAT("%d", pNode->m_usergroup);
        tableStr << STR_FORMAT("%d", pNode->m_tty);
        tableStr << STR_FORMAT("%p", pNode->m_function);
        tableStr << STR_FORMAT("%s(%d)", 
                        (pNode->m_ctrler)? pNode->m_ctrler->Name() : "Null",
                        (pNode->m_ctrler)? pNode->m_ctrler->ID() : 0);
    }

    tableStr.Show(logPrint, logPara, "=", "-");
}

/*******************************************************
  �� �� ��: CControlChain::OnCtrl
  ��    ��: ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CControlChain::OnCtrl(objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue)
{
    if (!pInput)
    {
        return FAILURE;
    }

    CDArray aNodeList(sizeof(Node));
    GetCtrlNode(pInput, aNodeList);
    if (!aNodeList.Count())
    {
        return FAILURE;
    }

    DWORD dwRc = FAILURE;

    for (DWORD i = 0; i < aNodeList.Count(); ++i)
    {
        Node *pNode = (Node *)aNodeList.Pos(i);
        if (!pNode)
        {
            continue;
        }

        if (!pNode->m_function)
        {
            continue;
        }

        dwRc = pNode->m_function(pInput, pOutput, bContinue, pNode->m_ctrler);
        if (!bContinue)
        {
            break;
        }
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CControlChain::RegCtrlNode
  ��    ��: ע����Ƶ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CControlChain::RegCtrlNode(IObject *ctrler,
                        IControl::Node *ctrls,
                        DWORD count)
{
    if (!ctrls || !count) return FAILURE;

    AutoObjLock(m_piCtrlee);

    DWORD dwRc = SUCCESS;

    for (DWORD i = 0; i < count; ++i)
    {
        Node node;
        node.m_attribute = ctrls->m_attribute;
        node.m_ctrl = ctrls->m_ctrl;
        node.m_ack = ctrls->m_ack;
        node.m_usergroup = ctrls->m_usergroup;
        node.m_tty = ctrls->m_tty;
        node.m_function = ctrls->m_function;
        node.m_ctrler = ctrler;

        dwRc = m_aNodeList.Append(&node);
        if (dwRc != SUCCESS)
        {
            break;
        }
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CControlChain::GetCtrlNode
  ��    ��: ��ȡ���Ƶ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CControlChain::GetCtrlNode(objMsg *pMsg, CDArray &aNodeList)
{
    if (!pMsg) return;

    /// ��ȡ��Ϣͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(pMsg->GetDataBuf(), pMsg->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    if (!aSessHeads.Count())
    {
        return;
    }

    /// ��ȡ�Ựͷ(ֻ��ȡ��һ���Ựͷ)
    DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(0);
    if (!pSessionHead)
    {
        return;
    }

    AutoObjLock(m_piCtrlee);

    for (DWORD i = 0; i < m_aNodeList.Count(); ++i)
    {
        Node *pNode = (Node *)m_aNodeList.Pos(i);
        if (!pNode)
        {
            continue;
        }

        if (!IsCtrlNode(pNode, pSessionHead))
        {
            continue;
        }

        (void)aNodeList.Append(pNode);
    }
}

/*******************************************************
  �� �� ��: CControlChain::IsCtrlNode
  ��    ��: �Ƿ���Ƶ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CControlChain::IsCtrlNode(Node *pNode, DCOP_SESSION_HEAD *pSessionHead)
{
    if (!pNode || !pSessionHead) return false;

    if (pNode->m_attribute != pSessionHead->m_attribute)
    {
        return false;
    }

    if (pNode->m_ctrl && (pNode->m_ctrl != pSessionHead->m_ctrl))
    {
        return false;
    }

    if (pNode->m_ack != pSessionHead->m_ack)
    {
        return false;
    }

    if (pNode->m_usergroup && (pNode->m_usergroup != pSessionHead->m_user))
    {
        return false;
    }

    if (pNode->m_tty && (pNode->m_tty != pSessionHead->m_tty))
    {
        return false;
    }

    return true;
}

