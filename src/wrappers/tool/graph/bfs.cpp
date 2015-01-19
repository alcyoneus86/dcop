/// -------------------------------------------------
/// bfs.cpp : ������������㷨ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "graph/bfs.h"
#include <stdlib.h>
#include <memory.h>


/*******************************************************
  �� �� ��: CBFS::CBFS
  ��    ��: CBFS����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CBFS::CBFS()
{
}

/*******************************************************
  �� �� ��: CBFS::~CBFS
  ��    ��: CBFS����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CBFS::~CBFS()
{
}

/*******************************************************
  �� �� ��: CBFS::FindShortestPath
  ��    ��: �������·��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CBFS::FindShortestPath(IGraph::POS throughVex, 
                        IGraph::POS srcVex, 
                        IGraph::POS dstVex, 
                        IGraph::POS *&rpPath, 
                        DWORD &rdwPathLen)
{
    if (!GetGraph())
    {
        return false;
    }

    if ((IGraph::Null == srcVex) || (IGraph::Null == dstVex))
    {
        return false;
    }

    if (!InitVisited())
    {
        return false;
    }

    if (!InitQueue())
    {
        return false;
    }

    /// ������Ƚ�������
    IGraph::POS curVex = srcVex;
    SetVisited(curVex, true);
    EnQueue(curVex, -1);
    int curIdx = 0;
    bool bFound = false;
    while (!QueueEmpty() && !bFound)
    {
        DeQueue(curVex, curIdx);
        QueNode *pParentQueNode = GetParentQueNode(curIdx);
        if (pParentQueNode) throughVex = pParentQueNode->childPos;

        for (IGraph::POS tmpVex = GetGraph()->FirstAdjVex(throughVex, curVex, dstVex);
            tmpVex != IGraph::Null;
            tmpVex = GetGraph()->NextAdjVex(throughVex, curVex, tmpVex, dstVex))
        {
            if (tmpVex == dstVex)
            {
                bFound = true;
                EnQueue(tmpVex, curIdx);
                break;
            }

            if (GetVisited(tmpVex))
            {
                continue;
            }

            /// ��DFS��ͬ, ����ֻҪ���ʹ����ӽڵ㶼����Ϊ���ʹ���
            SetVisited(tmpVex, true);
            EnQueue(tmpVex, curIdx);
        }
    }

    if (!bFound)
    {
        return false;
    }

    /// ��ȡ·������
    DWORD tmp = 0;
    QueNode *pQueNode = 0;
    for (tmp = GetQueRear() - 1; (tmp) && (pQueNode = GetQueNode(tmp)); tmp = pQueNode->parent)
    {
        rdwPathLen++;
    }

    DWORD pathIdx = rdwPathLen - 1;
    rpPath = (IGraph::POS *)malloc(rdwPathLen * sizeof(IGraph::POS));
    if(!rpPath)
    {
        rdwPathLen = 0;
        return false;
    }

    /// �����ȡ·��
    for (tmp = GetQueRear() - 1; (tmp) && (pQueNode = GetQueNode(tmp)); tmp = pQueNode->parent)
    {
        rpPath[pathIdx--] = pQueNode->childPos;
    }

    return true;
}

/*******************************************************
  �� �� ��: CBFS::FreePath
  ��    ��: �ͷ�·��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  
  �޸ļ�¼: 
 *******************************************************/
void CBFS::FreePath(IGraph::POS *pPath)
{
    if (pPath)
    {
        free(pPath);
    }

    Clear();
}


