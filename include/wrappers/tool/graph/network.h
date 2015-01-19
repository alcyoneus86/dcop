/// -------------------------------------------------
/// network.h : �����ݽṹ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_GRAPH_NETWORK_H_
#define _TOOL_GRAPH_NETWORK_H_

#include "graph.h"


/// ��
class CNetwork : public IGraph
{
public:

    /////////////////////////////////////////////////////////////////
    /// [�洢˵��]Ϊ���ڴ��������Ҫ�����һЩ�洢��Ϣ������������(��
    /// ��̬�䳤)����Ϊ�����򷽱㣬ͬʱ������ڵ�������һ����һ���ڵ�
    /// ��ָ�룬���ָ��ָ�����һ���Ǿ�������ģ����߸�����Ҫָ��ա�
    /////////////////////////////////////////////////////////////////

    /// �ڵ�ֵ��1��ʼ��ţ�λ��������0��ʼ

    /// ������ϵ(Vertex::valueΪ��һ����ϵ)
    class RelationShip : public Vertex
    {
    public:
        IGraph::POS curNode;                    // ���ڵ�
        IGraph::POS connectIdx;                 // ��������
        IGraph::POS adjNode;                    // �ڽӵ�
    };

    /// ����(Vertex::valueΪȨ��)
    class Connection :  public Vertex
    {
    public:
        IGraph::POS startNode;                  // ��ʼ�ڵ�
        IGraph::POS endNode;                    // �����ڵ�
        int reverseValue;                       // ����Ȩ��(2->1��ֵ;Vertex::valueΪ����Ȩ��(1->2��ֵ))
    };

    /// �ڵ�(Vertex::valueΪpos+1�Ľڵ�ֵ)
    class Node : public Vertex
    {
    public:
        IGraph::POS firstShip;                  // ��һ��������ϵ
    };

public:
    CNetwork();
    virtual ~CNetwork();

    /// ��ʼ��ӹ�����ϵ
    bool StartAdd(
                DWORD dwNodeMemLen,             // �ڵ�/����/��ϵ�ĳ�ʼ�ڴ泤��
                DWORD dwNodeMemLenStep,         // �ڵ�/����/��ϵ�ĳ�ʼ�ڴ���������
                DWORD dwConnectionMemLen,       // �ڵ�/����/��ϵ�ĳ�ʼ�ڴ泤��
                DWORD dwConnectionMemLenStep,   // �ڵ�/����/��ϵ�ĳ�ʼ�ڴ���������
                DWORD dwRelationShipMemLen,     // �ڵ�/����/��ϵ�ĳ�ʼ�ڴ泤��
                DWORD dwRelationShipMemLenStep, // �ڵ�/����/��ϵ�ĳ�ʼ�ڴ���������
                bool  bConnectionIndex = false  //��������Ϊ����
                );

    /// ��ӹ�����ϵ
    bool Add(
                int startNodeValue,             // ��ʼ�ڵ�
                int endNodeValue,               // �����ڵ�
                int forwardWeight,              // ����Ȩ��
                int reverseWeight               // ����Ȩ��
                );

    /// ������ӹ�����ϵ
    void EndAdd();

    /// ��ȡ�ڵ�ĵ�һ���ڽӵ�
    IGraph::POS FirstAdjVexOfNode(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &dstVex);

    /// ��ȡ�ڵ����һ���ڽӵ�
    IGraph::POS NextAdjVexOfNode(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &adjVex, const IGraph::POS &dstVex);

    /// ��ȡ���ӵĵ�һ���ڽӵ�����
    IGraph::POS FirstAdjVexOfConnection(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &dstVex);

    /// ��ȡ���ӵ���һ���ڽӵ�����
    IGraph::POS NextAdjVexOfConnection(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &adjVex, const IGraph::POS &dstVex);

    /// =============== ��д���ຯ�� ================
    virtual IGraph::POS FirstAdjVex(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &dstVex);
    virtual IGraph::POS NextAdjVex(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &adjVex, const IGraph::POS &dstVex);
    /// =============================================

    /// ��ȡ������ϵ
    IGraph::POS GetFirstRelationShip() {return m_posRelationShipFirst;}
    IGraph::POS GetNextRelationShip(IGraph::POS pos) {return m_pRelationShip[pos].value;}
    RelationShip *GetRelationShipNode(IGraph::POS pos) {return &(m_pRelationShip[pos]);}

    /// ��ȡ���������е���ͬ�ڵ�
    IGraph::POS GetSameNodeOfConnect(
                IGraph::POS parentNode, 
                IGraph::POS curConnection
                );

    /// ��ȡ�����е���һ���ڵ�
    IGraph::POS GetThatNodeOfConnect(
                IGraph::POS curConnection,
                IGraph::POS thisNode
                );

    /// ��λ���ж�
    bool NullNodePos(IGraph::POS pos) {return ((pos < 0) || (pos >= (int)m_dwNodeCount))? true : false;}
    bool NullConnectionPos(IGraph::POS pos) {return ((pos < 0) || (pos >= (int)m_dwConnectionCount))? true : false;}
    bool NullRelationShipPos(IGraph::POS pos) {return ((pos < 0) || (pos >= (int)m_dwRelationShipCount))? true : false;}

    /// ������Ϣ
    void Dump();

private:
    /// ��ӽڵ�(����λ��)
    IGraph::POS AddNode(
                int nodeValue                   // �ڵ�ֵ
                );

    /// �������(����λ��)
    IGraph::POS AddConnection(
                IGraph::POS startNode,          // ��ʼ�ڵ�
                IGraph::POS endNode,            // �����ڵ�
                int forwardWeight,              // ����Ȩ��
                int reverseWeight               // ����Ȩ��
                );

    /// ��ӹ���(���ر��ڵ�ĵ�һ������λ��)
    IGraph::POS AddRelationShip(
                IGraph::POS curNode,            // ���ڵ�
                IGraph::POS adjNode,            // �ڽӵ�
                IGraph::POS connectIdx          // ��������
                );

    /// ����ڴ��
    void Clear();

private:
    Node *m_pNode;                              // �ڵ��б�
    DWORD m_dwNodeMemLen;                       // �ڵ��б��ڴ泤��
    DWORD m_dwNodeCount;                        // �ڵ����
    DWORD m_dwNodeMemLenStep;                   // �ڵ��б��ڴ���������

    Connection *m_pConnection;                  // �����б�
    DWORD m_dwConnectionMemLen;                 // �����б��ڴ泤��
    DWORD m_dwConnectionCount;                  // ���Ӹ���
    DWORD m_dwConnectionMemLenStep;             // �����б��ڴ���������

    RelationShip *m_pRelationShip;              // �����б�
    DWORD m_dwRelationShipMemLen;               // �����б��ڴ泤��
    DWORD m_dwRelationShipCount;                // ��������
    DWORD m_dwRelationShipMemLenStep;           // �����б��ڴ���������

    IGraph::POS m_posRelationShipFirst;         // ��һ������λ��(��С)
    bool m_bConnectionIndex;                    // �Ƿ�������Ϊ����

};


#endif // _TOOL_GRAPH_NETWORK_H_

