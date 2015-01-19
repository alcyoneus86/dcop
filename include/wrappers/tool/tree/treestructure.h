/// -------------------------------------------------
/// treestructure.h : ���Խṹ���鹤���๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TREESTRUCTURE_H_
#define _TREESTRUCTURE_H_

#include "dcop.h"
#include "list/dlistvector.h"

/// =================================================
///     ���Խṹ����˵��
/// -------------------------------------------------
/// ����б�        ��״�ṹ
/// +------+
/// |  0   | -------- Root
/// |      |           |
/// |      |        +--+--+
/// |      |        |     |
/// |  1   | ---- Child Child
/// |      |        |     |
/// |      |     +--+  +--+--+
/// |      |     |     |     |
/// |  2   | - Child Child Child
/// |      |
/// |      |
/// |  .   |    ...   ...   ...
/// +------+
/// =================================================

/// -------------------------------------------------
/// �̳�CdListVectorTool�ǽ�������Ϊ����б�
/// -------------------------------------------------

class CTreeStructureTool : private CdListVectorTool
{
public:

    /// ID(������)
    struct UserNode
    {
        DWORD m_dwNodeID;
    };

    /// ���ڵ�(�ڲ���������)
    struct TreeNode : public UserNode
    {
        DWORD m_dwXID;                      // ��ͬһ������е��б�ID
        DWORD m_dwYID;                      // ���ID(DepthValue Idx)
        TreeNode *m_pFatherNode;            // ���ڵ�
        CdListVectorTool *m_pChildLists;    // �ӽڵ��б�
        void *m_pNodeValue;                 // �ڵ�ֵ(����ռ�ø�λ�ò���չ�����ڴ�)

        TreeNode(DWORD dwID = 0, void *pValue = 0);
        ~TreeNode();

        /// ���뵽���ڵ�
        void JoinFather(TreeNode *pFatherNode);

        /// �Ӹ��ڵ��뿪
        void LeftFather();

        /// �����ӽڵ�
        void AddChild(TreeNode *pChildNode);

        /// ɾ���ӽڵ�
        void DelChild(TreeNode *pChildNode);

        /// ɾ�������ӽڵ�
        void DelAllChildren();

        /// �����ӽڵ����������(ʵ���Ϻ���Ϊ���ڵ���Լ���ͬһ����)
        DECLARE_DLVT_DELNODE_PROC_AUTO(TreeNode);

        /// ���ڵ�ɾ���ص�����(Node::~Node�е���)
        typedef void (*DEL_TREENODE_PROC)(
            TreeNode *pNode,
            void *pPara
        );

        DEL_TREENODE_PROC m_fnDelNodeProc;
        void *m_pDelNodePara;

        void SetDelTreeNodeProc(
            DEL_TREENODE_PROC fnDelNodeProc,
            void *pProcPara)
        {m_fnDelNodeProc = fnDelNodeProc; m_pDelNodePara = pProcPara;}

    };

    /// ������б�ڵ�
    struct DepthListNode : public CdListVectorTool
    {
        DepthListNode();
        ~DepthListNode();

        void AddNode(TreeNode *pNode);
        void DelNode(TreeNode *pNode);

    };

private:
    TreeNode *m_pTreeRootNode;

public:
    CTreeStructureTool();
    ~CTreeStructureTool();

    //////////////////////////////////////////////
    /// ��д����б� - begin
    //////////////////////////////////////////////

    /// ɾ������(��Ϊ�ڵ���new�����Ŀռ�)
    void onDelNode(const void *cpNodeValue, DWORD dwNodeID);

    //////////////////////////////////////////////
    /// ��������б� - end
    //////////////////////////////////////////////

    static void DelNodeFromDepthList(TreeNode *pNode, void *pPara);

    /// ��ȡ�������ĸ�(�����ڲ����ƽڵ�����)
    TreeNode *pGetRootNode() const {return m_pTreeRootNode;}

    TreeNode * AddNode(TreeNode *pNode, DWORD dwID, void *pValue);
    void DelNode(TreeNode *pNode);

    DepthListNode *pGetDepthListNode(DWORD dwDepthValue);

};


#endif // #ifndef _TREESTRUCTURE_H_

