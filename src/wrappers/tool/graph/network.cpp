/// -------------------------------------------------
/// network.cpp : �����ݽṹʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "graph/network.h"
#include <stdio.h>
#include <stdlib.h>


CNetwork::CNetwork()
{
    m_pNode                     = 0;
    m_dwNodeMemLen              = 0;
    m_dwNodeCount               = 0;
    m_dwNodeMemLenStep          = 0;

    m_pConnection               = 0;
    m_dwConnectionMemLen        = 0;
    m_dwConnectionCount         = 0;
    m_dwConnectionMemLenStep    = 0;

    m_pRelationShip             = 0;
    m_dwRelationShipMemLen      = 0;
    m_dwRelationShipCount       = 0;
    m_dwRelationShipMemLenStep  = 0;
    m_posRelationShipFirst      = IGraph::Null;
}

CNetwork::~CNetwork()
{
    Clear();
}

bool CNetwork::StartAdd(DWORD dwNodeMemLen, DWORD dwNodeMemLenStep, DWORD dwConnectionMemLen, DWORD dwConnectionMemLenStep, DWORD dwRelationShipMemLen, DWORD dwRelationShipMemLenStep, bool bConnectionIndex)
{
    if (!dwNodeMemLenStep || !dwConnectionMemLenStep || !dwRelationShipMemLenStep)
    {
        return false;
    }

    Clear();

    /// ʧ�ܷ����������м��ڴ��ڶ�������ʱͳһ���

    if (!CGraphSearchTool::Dalloc((void *&)m_pNode, m_dwNodeMemLen, 0, dwNodeMemLen, dwNodeMemLenStep, (DWORD)sizeof(Node)))
    {
        return false;
    }

    if (!CGraphSearchTool::Dalloc((void *&)m_pConnection, m_dwConnectionMemLen, 0, dwConnectionMemLen, dwConnectionMemLenStep, (DWORD)sizeof(Connection)))
    {
        return false;
    }

    if (!CGraphSearchTool::Dalloc((void *&)m_pRelationShip, m_dwRelationShipMemLen, 0, dwRelationShipMemLen, dwRelationShipMemLenStep, (DWORD)sizeof(RelationShip)))
    {
        return false;
    }

    m_dwNodeMemLenStep          = dwNodeMemLenStep;
    m_dwConnectionMemLenStep    = dwConnectionMemLenStep;
    m_dwRelationShipMemLenStep  = dwRelationShipMemLenStep;
    m_bConnectionIndex          = bConnectionIndex;

    return true;
}

bool CNetwork::Add(int startNodeValue, int endNodeValue, int forwardWeight, int reverseWeight)
{
    /// ʧ�ܷ����������м��ڴ��ڶ�������ʱͳһ���

    IGraph::POS startNodePos            = AddNode(startNodeValue);
    if (startNodePos == IGraph::Null)   return false;

    IGraph::POS endNodePos              = AddNode(endNodeValue);
    if (endNodePos == IGraph::Null)     return false;

    IGraph::POS connectIdx              = AddConnection(startNodePos, endNodePos, forwardWeight, reverseWeight);
    if (connectIdx == IGraph::Null)     return false;

    if (forwardWeight)
        m_pNode[startNodePos].firstShip = AddRelationShip(startNodePos, endNodePos, connectIdx);
    if (reverseWeight)
        m_pNode[endNodePos].firstShip   = AddRelationShip(endNodePos, startNodePos, connectIdx);

    return true;
}

void CNetwork::EndAdd()
{
    if (!m_bConnectionIndex)
    {
        IGraph::Init(m_dwNodeCount);
    }
    else
    {
        IGraph::Init(m_dwConnectionCount);
    }
}

IGraph::POS CNetwork::AddNode(int nodeValue)
{
    /// �Ƿ�ֵ
    if (nodeValue < 1)  return IGraph::Null;

    /// �ع��ڴ��С
    IGraph::POS nodePos = nodeValue - 1;
    if (!CGraphSearchTool::Dalloc((void *&)m_pNode, m_dwNodeMemLen, nodePos, 1, m_dwNodeMemLenStep, (DWORD)sizeof(Node)))
    {
        return IGraph::Null;
    }

    /// �ж��Ƿ��Ѿ����
    if (m_pNode[nodePos].value > 0)
    {
        if (m_pNode[nodePos].value != nodeValue)
        {
            return IGraph::Null;
        }

        return nodePos;
    }

    /// ��ӵ����
    m_pNode[nodePos].value = nodeValue;
    m_dwNodeCount++;

    return nodePos;
}

IGraph::POS CNetwork::AddConnection(IGraph::POS startNode, IGraph::POS endNode, int forwardWeight, int reverseWeight)
{
    /// �ع��ڴ��С
    if (!CGraphSearchTool::Dalloc((void *&)m_pConnection, m_dwConnectionMemLen, m_dwConnectionCount, 1, m_dwConnectionMemLenStep, (DWORD)sizeof(Connection)))
    {
        return IGraph::Null;
    }

    /// ��ӵ����
    m_pConnection[m_dwConnectionCount].startNode    = startNode;
    m_pConnection[m_dwConnectionCount].endNode      = endNode;
    m_pConnection[m_dwConnectionCount].value        = forwardWeight;
    m_pConnection[m_dwConnectionCount].reverseValue = reverseWeight;
    m_dwConnectionCount++;

    return (IGraph::POS)(m_dwConnectionCount - 1);
}

IGraph::POS CNetwork::AddRelationShip(IGraph::POS curNode, IGraph::POS adjNode, IGraph::POS connectIdx)
{
    /// �ع��ڴ��С
    if (!CGraphSearchTool::Dalloc((void *&)m_pRelationShip, m_dwRelationShipMemLen, m_dwRelationShipCount, 1, m_dwRelationShipMemLenStep, (DWORD)sizeof(Connection)))
    {
        return IGraph::Null;
    }

    /// ��ӵ����
    m_pRelationShip[m_dwRelationShipCount].value        = IGraph::Null;
    m_pRelationShip[m_dwRelationShipCount].curNode      = curNode;
    m_pRelationShip[m_dwRelationShipCount].adjNode      = adjNode;
    m_pRelationShip[m_dwRelationShipCount].connectIdx   = connectIdx;

    /// ��������������½�������(value�ǰ�˳�������ֵ)
    IGraph::POS pos     = m_posRelationShipFirst;
    IGraph::POS posRc   = m_dwRelationShipCount;
    IGraph::POS posTmp  = IGraph::Null;
    while (pos != IGraph::Null)
    {
        if ((curNode < m_pRelationShip[pos].curNode) ||
            ((curNode == m_pRelationShip[pos].curNode) && 
                (adjNode < m_pRelationShip[pos].adjNode)))
        {
            /// ���ڵ�ָ����һ��
            m_pRelationShip[m_dwRelationShipCount].value = pos;

            /// ǰһ������ͷָ��ָ�򱾽ڵ�
            if (posTmp != IGraph::Null) m_pRelationShip[posTmp].value = m_dwRelationShipCount;
            else m_posRelationShipFirst = m_dwRelationShipCount;

            break;
        }

        /// ���·��ص���ͬ�ڵ㲢������С�ڽӵ�Ĺ���Ϊ��һ������λ��
        if ((curNode == m_pRelationShip[pos].curNode) && 
            (m_pRelationShip[pos].adjNode < m_pRelationShip[posRc].adjNode))
        {
            posRc = pos;
        }

        posTmp = pos;
        pos = GetNextRelationShip(pos);
    } 

    /// ����û����ӳɹ�����ʾ�����ģ���������
    if (IGraph::Null == pos)
    {
        if (posTmp != IGraph::Null) m_pRelationShip[posTmp].value = m_dwRelationShipCount;
        else m_posRelationShipFirst = m_dwRelationShipCount;
    }

    m_dwRelationShipCount++;
    return posRc;
}

IGraph::POS CNetwork::GetSameNodeOfConnect(IGraph::POS parentNode, IGraph::POS curConnection)
{
    if (NullConnectionPos(curConnection))
    {
        return IGraph::Null;
    }

    if (-1 == parentNode)
    {
        /// ������С�Ľڵ�
        return GRAPH_GET_MIN(m_pConnection[curConnection].startNode, m_pConnection[curConnection].endNode);
    }
    else if (NullConnectionPos(parentNode))
    {
        /// �������Ľڵ�
        return GRAPH_GET_MAX(m_pConnection[curConnection].startNode, m_pConnection[curConnection].endNode);
    }
    else
    {
        /// ��������Ĵ���
    }

    if ((m_pConnection[curConnection].startNode == m_pConnection[parentNode].startNode) ||
        (m_pConnection[curConnection].startNode == m_pConnection[parentNode].endNode))
    {
        return m_pConnection[curConnection].startNode;
    }

    if ((m_pConnection[curConnection].endNode == m_pConnection[parentNode].startNode) ||
        (m_pConnection[curConnection].endNode == m_pConnection[parentNode].endNode))
    {
        return m_pConnection[curConnection].endNode;
    }

    return IGraph::Null;
}

IGraph::POS CNetwork::GetThatNodeOfConnect(IGraph::POS curConnection, IGraph::POS thisNode)
{
    if (NullConnectionPos(curConnection))
    {
        return IGraph::Null;
    }

    if (NullNodePos(thisNode))
    {
        return IGraph::Null;
    }

    if (m_pConnection[curConnection].startNode != thisNode)
    {
        return m_pConnection[curConnection].startNode;
    }

    return m_pConnection[curConnection].endNode;
}

IGraph::POS CNetwork::FirstAdjVexOfNode(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &dstVex)
{
    if (NullNodePos(curVex))
    {
        return IGraph::Null;
    }

    IGraph::POS firstShip = m_pNode[curVex].firstShip;
    if (NullRelationShipPos(firstShip))
    {
        return IGraph::Null;
    }

    /// �����ǰ�ڵ㲻�ǵ�ǰ�����е��׽ڵ㣬���ʾ��ǰ�ڵ��Ӧ�Ĺ����Ѿ�����
    if (curVex != m_pRelationShip[firstShip].curNode)
    {
        return IGraph::Null;
    }

    return m_pRelationShip[firstShip].adjNode;
}

IGraph::POS CNetwork::NextAdjVexOfNode(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &adjVex, const IGraph::POS &dstVex)
{
    if (NullNodePos(curVex))
    {
        return IGraph::Null;
    }

    IGraph::POS ship = m_pNode[curVex].firstShip;
    while (ship != IGraph::Null)
    {
        /// �����ǰ�ڵ㲻�ǵ�ǰ�����е��׽ڵ㣬���ʾ��ǰ�ڵ��Ӧ�Ĺ����Ѿ�����
        if (curVex != m_pRelationShip[ship].curNode)
        {
            break;
        }

        /// �����ǰ�ڽӵ���ǵ�ǰ�����е��ڽӵ㣬������¸�������ϵ
        if (adjVex == m_pRelationShip[ship].adjNode)
        {
            ship = GetNextRelationShip(ship);
            break;
        }

        ship = GetNextRelationShip(ship);
    }

    if (NullRelationShipPos(ship) || (curVex != m_pRelationShip[ship].curNode))
    {
        return IGraph::Null;
    }

    return m_pRelationShip[ship].adjNode;
}

IGraph::POS CNetwork::FirstAdjVexOfConnection(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &dstVex)
{
    if (NullConnectionPos(curVex))
    {
        return IGraph::Null;
    }

    IGraph::POS thisNode = GetSameNodeOfConnect(parentVex, curVex);
    if (NullNodePos(thisNode))
    {
        return IGraph::Null;
    }

    IGraph::POS thatNode = GetThatNodeOfConnect(curVex, thisNode);
    if (NullNodePos(thatNode))
    {
        return IGraph::Null;
    }

    IGraph::POS firstShip = m_pNode[thatNode].firstShip;
    if (NullRelationShipPos(firstShip))
    {
        return IGraph::Null;
    }

    /// �����ǰ�ڵ㲻�ǵ�ǰ�����е��׽ڵ㣬���ʾ��ǰ�ڵ��Ӧ�Ĺ����Ѿ�����
    if (thatNode != m_pRelationShip[firstShip].curNode)
    {
        return IGraph::Null;
    }

    /// ���������֮ǰͨ���ĸ��ڽӵ㣬����Ѱ����һ��
    if (curVex == m_pRelationShip[firstShip].connectIdx)
    {
        firstShip = GetNextRelationShip(firstShip);
    }

    if (NullRelationShipPos(firstShip) || (thatNode != m_pRelationShip[firstShip].curNode))
    {
        return IGraph::Null;
    }

    return m_pRelationShip[firstShip].connectIdx;
}

IGraph::POS CNetwork::NextAdjVexOfConnection(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &adjVex, const IGraph::POS &dstVex)
{
    if (NullConnectionPos(curVex))
    {
        return IGraph::Null;
    }

    IGraph::POS thisNode = GetSameNodeOfConnect(parentVex, curVex);
    if (NullNodePos(thisNode))
    {
        return IGraph::Null;
    }

    IGraph::POS thatNode = GetThatNodeOfConnect(curVex, thisNode);
    if (NullNodePos(thatNode))
    {
        return IGraph::Null;
    }

    IGraph::POS ship = m_pNode[thatNode].firstShip;
    while (ship != IGraph::Null)
    {
        /// �����ǰ�ڵ㲻�ǵ�ǰ�����е��׽ڵ㣬���ʾ��ǰ�ڵ��Ӧ�Ĺ����Ѿ�����
        if (thatNode != m_pRelationShip[ship].curNode)
        {
            break;
        }

        /// �ҵ���ǰ�ڽӵ㣬��������
        if (adjVex >= m_pRelationShip[ship].connectIdx)
        {
            ship = GetNextRelationShip(ship);
            continue;
        }

        /// ���������֮ǰͨ���ģ�����Ѱ����һ��
        if (curVex == m_pRelationShip[ship].connectIdx)
        {
            ship = GetNextRelationShip(ship);
            continue;
        }

        break;
    }

    if (NullRelationShipPos(ship) || (thatNode != m_pRelationShip[ship].curNode))
    {
        return IGraph::Null;
    }

    return m_pRelationShip[ship].connectIdx;
}

IGraph::POS CNetwork::FirstAdjVex(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &dstVex)
{
    return (!m_bConnectionIndex)? FirstAdjVexOfNode(parentVex, curVex, dstVex) : FirstAdjVexOfConnection(parentVex, curVex, dstVex);
}

IGraph::POS CNetwork::NextAdjVex(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &adjVex, const IGraph::POS &dstVex)
{
    return (!m_bConnectionIndex)? NextAdjVexOfNode(parentVex, curVex, adjVex, dstVex) : NextAdjVexOfConnection(parentVex, curVex, adjVex, dstVex);
}

void CNetwork::Dump()
{
    printf("################################# Node ###############################\n <No.> [value] [firstShip] \n");
    for (DWORD i = 0; i < m_dwNodeCount; ++i)
    {
        printf("  %-3u   %-5d   %-9d  \n", i, m_pNode[i].value, m_pNode[i].firstShip);
    }
    printf("######################################################################\n");

    printf("############################# Connection #############################\n <No.> [value] [startNode] [endNode] [reverse] \n");
    for (DWORD i = 0; i < m_dwConnectionCount; ++i)
    {
        printf("  %-3u   %-5d   %-9d   %-7d   %-7d  \n", i, m_pConnection[i].value, m_pConnection[i].startNode, m_pConnection[i].endNode, m_pConnection[i].reverseValue);
    }
    printf("######################################################################\n");

    printf("############################ RelationShip ############################\n <No.> [value] [curNode] [connectIdx] [adjNode] \n");
    for (DWORD i = 0; i < m_dwRelationShipCount; ++i)
    {
        printf("  %-3u   %-5d   %-7d   %-10d   %-7d  \n", i, m_pRelationShip[i].value, m_pRelationShip[i].curNode, m_pRelationShip[i].connectIdx, m_pRelationShip[i].adjNode);
    }
    printf("######################################################################\n");
}

void CNetwork::Clear()
{
    CGraphSearchTool::Dfree((void *&)m_pNode, m_dwNodeMemLen);
    CGraphSearchTool::Dfree((void *&)m_pConnection, m_dwConnectionMemLen);
    CGraphSearchTool::Dfree((void *&)m_pRelationShip, m_dwRelationShipMemLen);

    m_dwNodeCount               = 0;
    m_dwNodeMemLenStep          = 0;
    m_dwConnectionCount         = 0;
    m_dwConnectionMemLenStep    = 0;
    m_dwRelationShipCount       = 0;
    m_dwRelationShipMemLenStep  = 0;
}

