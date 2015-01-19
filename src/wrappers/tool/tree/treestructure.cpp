/// -------------------------------------------------
/// treestructure.cpp : ���Խṹ���鹤����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "tree/treestructure.h"


CTreeStructureTool::TreeNode::TreeNode(DWORD dwID, void *pValue)
{
    m_dwXID = 0;
    m_dwYID = 0;
    m_pFatherNode = 0;
    m_pChildLists = 0;

    m_fnDelNodeProc = 0;
    m_pDelNodePara = 0;

    m_pNodeValue = pValue;
}

CTreeStructureTool::TreeNode::~TreeNode()
{
    if (m_fnDelNodeProc)
    {
        m_fnDelNodeProc(this, m_pDelNodePara);
    }

    LeftFather();
    DelAllChildren();
}

/// ʵ���ӽڵ����������(ʵ���Ϻ���Ϊ���ڵ���Լ���ͬһ����)
IMPLEMENT_DLVT_DELNODE_PROC_AUTO(CTreeStructureTool::TreeNode, TreeNode)

/// ���뵽ָ�����ڵ���
void CTreeStructureTool::TreeNode::JoinFather(CTreeStructureTool::TreeNode *pFatherNode)
{
    if (!pFatherNode)
    {
        return;
    }

    m_pFatherNode = pFatherNode;
    m_dwYID = pFatherNode->m_dwYID + 1;
    pFatherNode->AddChild(this);
}

/// ��ָ�����ڵ����뿪
void CTreeStructureTool::TreeNode::LeftFather()
{
    CTreeStructureTool::TreeNode *pFatherNode = m_pFatherNode;

    if (!pFatherNode)
    {
        return;
    }

    pFatherNode->DelChild(this);
}

/// ���뱾�ڵ���ӽڵ�
void CTreeStructureTool::TreeNode::AddChild(CTreeStructureTool::TreeNode *pChildNode)
{
    if (!m_pChildLists)
    {
        /// û���ֽڵ��б����ӽڵ�����
        m_pChildLists = new CdListVectorTool;
        OSASSERT(m_pChildLists != 0);

        /// ע���ӽڵ����������(ʵ���Ϻ���Ϊ���ڵ���Լ���ͬһ����)
        REG_DLVT_DELNODE_PROC_AUTO(CTreeStructureTool::TreeNode, 
            TreeNode, m_pChildLists);
    }

    pChildNode->m_dwNodeID = m_pChildLists->Append(pChildNode);
}

/// ɾ�����ڵ���ӽڵ�
void CTreeStructureTool::TreeNode::DelChild(CTreeStructureTool::TreeNode *pChildNode)
{
    if (!m_pChildLists)
    {
        return;
    }

    m_pChildLists->Del(pChildNode->m_dwNodeID);

    /// =================================================
    /// ����ӽڵ�������û�нڵ�����δ���, ÿ���ͷ�(?)
    /// =================================================
    if (!(m_pChildLists->dwGetNodeNum()))
    {
        /// �ӽڵ���ĿΪ��
        delete m_pChildLists;
        m_pChildLists = 0;
    }
}

/// ɾ�����ڵ�������ӽڵ�
void CTreeStructureTool::TreeNode::DelAllChildren()
{
    if (!m_pChildLists)
    {
        return;
    }

    delete m_pChildLists;
    m_pChildLists = 0;
}

CTreeStructureTool::DepthListNode::DepthListNode()
{
}

CTreeStructureTool::DepthListNode::~DepthListNode()
{
}

void CTreeStructureTool::DepthListNode::AddNode(TreeNode *pNode)
{
    DWORD dwIdx = Append(pNode);

    pNode->m_dwXID = dwIdx;
}

void CTreeStructureTool::DepthListNode::DelNode(TreeNode *pNode)
{
    (void)Del(pNode->m_dwXID);
}

CTreeStructureTool::CTreeStructureTool()
{
    m_pTreeRootNode = 0;
}

CTreeStructureTool::~CTreeStructureTool()
{
    if (m_pTreeRootNode)
    {
        delete m_pTreeRootNode;
        m_pTreeRootNode = 0;
    }
}

void CTreeStructureTool::onDelNode(
                const void *cpNodeValue, 
                DWORD dwNodeID)
{
    /// ����ڵ�ʵ������һ����������ָ��
    DepthListNode *pDepthNode = (DepthListNode *)cpNodeValue;
    if (pDepthNode)
    {
        delete pDepthNode;
        pDepthNode = 0;
    }
}

void CTreeStructureTool::DelNodeFromDepthList(
                CTreeStructureTool::TreeNode *pNode, 
                void *pPara)
{
    CTreeStructureTool *pThis = (CTreeStructureTool *)pPara;
    OSASSERT(pThis != 0);

    DepthListNode *pDepthNode = (DepthListNode *)(pThis->Get)(pNode->m_dwYID);
    if (pDepthNode)
    {
        pDepthNode->DelNode(pNode);
    }

    /// �������Ȳ��������ӽڵ㶼��ɾ������δ���(?)
}

/// ��ĳ�ڵ���������µ��ӽڵ�
CTreeStructureTool::TreeNode *CTreeStructureTool::AddNode(
                CTreeStructureTool::TreeNode *pNode, 
                DWORD dwID, 
                void *pValue)
{
    CTreeStructureTool::TreeNode *pFatherNode = pNode;

    /// -------------------------------------------------
    /// ���pFatherNodeΪ�գ��¼ӽڵ���Ϊ���ڵ�
    /// -------------------------------------------------

    if (!pFatherNode && m_pTreeRootNode)
    {
        /// �Ѿ��и��ڵ���
        return 0;
    }

    /// -------------------------------------------------
    /// ���pFatherNode�ǿգ��ڷǸ��ڵ���������½ڵ�
    /// -------------------------------------------------

    if (pFatherNode && !m_pTreeRootNode)
    {
        /// ���ǻ��޸��ڵ�, �������
        return 0;
    }

    /// ��ʼ���½ڵ�
    CTreeStructureTool::TreeNode *pChildNode = 
                new CTreeStructureTool::TreeNode(dwID, pValue);

    if (!pFatherNode)
    {
        /// ������ڵ�Ϊ�գ���Ϊ���ڵ�
        m_pTreeRootNode = pChildNode;
    }
    else
    {
        /// ���½ڵ���뵽���ڵ�����
        pChildNode->JoinFather(pFatherNode);
    }

    /// ��ӵ�����б���
    DepthListNode *pDepthNode = pGetDepthListNode(pChildNode->m_dwYID);
    if (!pDepthNode)
    {
        /// ��һ�㻹δ�нڵ����
        pDepthNode = new DepthListNode;

        /// ���뵽����б���
        (void)Add(pChildNode->m_dwYID, pDepthNode);
    }

    /// ���ýڵ�����ɾ���ص�
    pChildNode->SetDelTreeNodeProc(DelNodeFromDepthList, this);

    /// �Ѹýڵ���뵽����б���
    pDepthNode->AddNode(pChildNode);
    

    return pChildNode;

}

/// ɾ��ĳ�ڵ�
void CTreeStructureTool::DelNode(CTreeStructureTool::TreeNode *pNode)
{
    if (!m_pTreeRootNode)
    {
        /// �����޽ڵ�
        return;
    }

    /// -------------------------------------------------
    /// ���pFatherNodeΪ�գ��¼ӽڵ���Ϊ���ڵ�
    /// -------------------------------------------------

    /// -------------------------------------------------
    /// ��Ϊ�ڵ��Զ�ע�����ӽڵ������������������������
    /// �Ľڵ㶼�����������
    /// -------------------------------------------------

    if (!pNode)
    {
        /// �������б�
        Clear();

        /// ɾ�����ڵ�
        delete m_pTreeRootNode;
        m_pTreeRootNode = 0;

        return;
    }

    /// ����б��ɾ���Ѿ��ڽڵ���������ɾ���ص��������ڵ�ʱ������ɾ����

    delete pNode;
    pNode = 0;

}

CTreeStructureTool::DepthListNode *CTreeStructureTool::pGetDepthListNode(DWORD dwDepthValue)
{
    return (DepthListNode *)Get(dwDepthValue);
}

