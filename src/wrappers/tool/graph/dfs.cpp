/// -------------------------------------------------
/// dfs.cpp : ������������㷨ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "graph/dfs.h"
#include <stdlib.h>
#include <memory.h>


/*******************************************************
  �� �� ��: CDFS::CDFS
  ��    ��: CDFS����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDFS::CDFS()
{
}

/*******************************************************
  �� �� ��: CDFS::~CDFS
  ��    ��: CDFS����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDFS::~CDFS()
{
}

/*******************************************************
  �� �� ��: CDFS::DFS
  ��    ��: DFS�ǵݹ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CDFS::DFS(IGraph::POS throughVex, 
                        IGraph::POS srcVex, 
                        IGraph::POS dstVex, 
                        PathNode *&rpPaths, 
                        DWORD &rdwPathCount)
{
    /// ����������� - ��BFS��ͬ������ֻ�и��ڵ������Ϊ���ʵģ����ҵݹ�����󷵻���һ��ʱҪ���ӽڵ�ȫ�����±�ʾΪδ����
    bool bRc = false;
    bool bContinue = true;
    IGraph::POS startVex = throughVex;
    IGraph::POS curVex = srcVex;
    IGraph::POS tmpVex = IGraph::Null;
    int curIdx = -1;
    while (!GetGraph()->NullPos(curVex) || !QueueEmpty())
    {
        /// ��ǰ�ڵ�����Чֵ����ջ����Чֵ�ͳ�ջһ����Чֵ
        if (!GetGraph()->NullPos(curVex))
        {
            PushStack(curVex, curIdx);
            SetVisited(curVex, true);
            tmpVex = IGraph::Null;
            curIdx = GetQueRear() - 1;
        }
        else
        {
            PopStack(curVex, curIdx);
            SetVisited(curVex, false);
            tmpVex = curVex;
            QueNode *pParentQueNode = GetParentQueNode(curIdx);
            curVex = (pParentQueNode)? pParentQueNode->childPos : IGraph::Null;
            curIdx = (pParentQueNode)? pParentQueNode->child : -1;
            if (GetGraph()->NullPos(curVex))
            {
                continue;
            }
        }

        /// ��ȡ���ϼ��ڵ�
        QueNode *pThroughQueNode = GetParentQueNode(curIdx);
        throughVex = (pThroughQueNode)? pThroughQueNode->childPos : startVex;

        /// ��ȡ��һ���ӽڵ�
        while (!GetGraph()->NullPos(curVex))
        {
            if (GetGraph()->NullPos(tmpVex))
            {
                tmpVex = GetGraph()->FirstAdjVex(throughVex, curVex, dstVex);
            }
            else
            {
                tmpVex = GetGraph()->NextAdjVex(throughVex, curVex, tmpVex, dstVex);
            }

            if (GetGraph()->NullPos(tmpVex))
            {
                break;
            }

            /// ����ҵ�Ŀ�ĵ㣬�ͽ��м�¼
            if (IsDest(tmpVex, curIdx, dstVex, bContinue, false))
            {
                tmpVex = IGraph::Null;

                DWORD dwPathsCount = rdwPathCount;
                if ( !Dalloc( (void *&)rpPaths, dwPathsCount, rdwPathCount, 1, 1, (DWORD)(sizeof(PathNode)) ) )
                {
                    continue;
                }

                bRc = GetDestPath(rpPaths[rdwPathCount].path, rpPaths[rdwPathCount].pathLen);
                if ( !bRc )
                {
                    continue;
                }

                rdwPathCount++;

                /// �ҵ�Ŀ�꣬�������Ҫ����Ѱ�ң��򷵻�
                if (!bContinue) return true;

                break;
            }

            if (GetVisited(tmpVex))
            {
                continue;
            }

            break;
        }

        curVex = tmpVex;

    }

    return bRc;
}

/*******************************************************
  �� �� ��: CDFS::DFS
  ��    ��: DFS�ݹ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDFS::DFS(IGraph::POS throughVex, 
                        IGraph::POS curVex, 
                        IGraph::POS dstVex, 
                        bool &bContinue, 
                        int curQueIdx)
{
    /// ��BFS��ͬ������ֻ�и��ڵ������Ϊ���ʵģ����ҵݹ�����󷵻���һ��ʱҪ���ӽڵ�ȫ�����±�ʾΪδ����
    SetVisited(curVex, true);

    for (IGraph::POS tmpVex = GetGraph()->FirstAdjVex(throughVex, curVex, dstVex);
            tmpVex != IGraph::Null;
            tmpVex = GetGraph()->NextAdjVex(throughVex, curVex, tmpVex, dstVex))
    {
        /// ����ҵ�Ŀ�ĵ㣬�ͽ��м�¼
        if (IsDest(tmpVex, curQueIdx, dstVex, bContinue))
        {
            /// �ҵ�Ŀ�꣬�������Ҫ����Ѱ�ң��򷵻�
            if (!bContinue) return;

            continue;
        }

        if (GetVisited(tmpVex))
        {
            continue;
        }

        EnQueue(tmpVex, curQueIdx);

        /// �ұ�tmp�ڵ���ڽӵ�
        DFS(curVex, tmpVex, dstVex, bContinue, GetQueRear() - 1);

        /// �ٰѱ�tmp�ڵ�����Ϊδ����
        SetVisited(tmpVex, false);
    }
}

/*******************************************************
  �� �� ��: CDFS::FindAllPath
  ��    ��: DFS��������·��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CDFS::FindAllPath(IGraph::POS throughVex, 
                        IGraph::POS srcVex, 
                        IGraph::POS dstVex, 
                        PathNode *&rpPaths, 
                        DWORD &rdwPathCount, 
                        bool bRecur)
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
    if (!bRecur)
    {
        /// ʹ�÷ǵݹ鷽ʽ����
        return DFS(throughVex, srcVex, dstVex, rpPaths, rdwPathCount);
    }

    /// ʹ�õݹ鷽ʽ���ң��ȳ�ʼ����������
    if (!InitIdxBuffer(DFS_PATH_COUNT_STEP, DFS_PATH_COUNT_STEP))
    {
        return false;
    }

    bool bContinue = true;
    EnQueue(srcVex, -1);
    DFS(throughVex, srcVex, dstVex, bContinue, 0);

    int *pLstDstIdx = GetIdxBuffer();
    DWORD dwDstCount = GetIdxBufferCount();

    if (!dwDstCount)
    {
        return false;
    }

    /// �������·����(��ԭ·���м������)
    DWORD dwPathsCount = rdwPathCount;
    if ( !Dalloc( (void *&)rpPaths,  dwPathsCount, rdwPathCount, dwDstCount, dwDstCount, (DWORD)(sizeof(PathNode)) ) )
    {
        return false;
    }

    /// ��ȡ·��
    (void)memset(rpPaths, 0, dwDstCount * sizeof(PathNode));
    for (DWORD i = 0; i < dwDstCount; ++i)
    {
        GetDestPath(rpPaths[rdwPathCount + i].path, rpPaths[rdwPathCount + i].pathLen, pLstDstIdx[i] - 1);
    }

    rdwPathCount += dwDstCount;
    return true;
}

/*******************************************************
  �� �� ��: CDFS::FreePaths
  ��    ��: �ͷ�·��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDFS::FreePaths(PathNode *pPaths, DWORD dwPathCount)
{
    if (!pPaths)
    {
        return;
    }

    for (DWORD i = 0; i < dwPathCount; ++i)
    {
        if (pPaths[i].path)
        {
            free(pPaths[i].path);
        }
    }

    free(pPaths);

    Clear();
}

