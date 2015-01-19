/// -------------------------------------------------
/// graph.cpp : ͼ���ݽṹʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "graph/graph.h"
#include <stdlib.h>
#include <memory.h>


/*******************************************************
  �� �� ��: IGraph::IGraph
  ��    ��: IGraph����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IGraph::IGraph()
{
    m_vexs = 0;
    m_size = sizeof(Vertex);
    m_count = 0;
}

/*******************************************************
  �� �� ��: IGraph::~IGraph
  ��    ��: IGraph����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IGraph::~IGraph()
{
    if (m_vexs)
    {
        free(m_vexs);
    }
}

/*******************************************************
  �� �� ��: IGraph::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void IGraph::Init(DWORD dwCount, int *ipValue, DWORD dwSize)
{
    m_vexs = (Vertex *)malloc(dwCount * dwSize);
    if (!m_vexs)
    {
        return;
    }

    m_count = dwCount;

    if (!ipValue)
    {
        (void)memset(m_vexs, 0, dwCount * dwSize);
        return;
    }

    for (DWORD i = 0; i < dwCount; ++i)
    {
        Vertex *pVex = (Vertex *)((char *)m_vexs + i * dwSize);
        if (pVex) pVex->value = ipValue[i];
    }

}

/*******************************************************
  �� �� ��: IGraph::NullPos
  ��    ��: �Ƿ���Чλ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool IGraph::NullPos(const POS &pos)
{
    if ((pos <= Null) || (pos >= (int)m_count))
    {
        return true;
    }

    return false;
}

/*******************************************************
  �� �� ��: IGraph::VexNum
  ��    ��: �ڵ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD IGraph::VexNum()
{
    return m_count;
}

/*******************************************************
  �� �� ��: IGraph::Set
  ��    ��: ���ýڵ��Ӧ��ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void IGraph::Set(const POS &pos, int value)
{
    if (NullPos(pos))
    {
        return;
    }

    m_vexs[pos].value = value;
}

/*******************************************************
  �� �� ��: IGraph::Get
  ��    ��: ��ȡ�ڵ��Ӧ��ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int IGraph::Get(const POS &pos)
{
    if (NullPos(pos))
    {
        return -1;
    }

    return m_vexs[pos].value;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::CGraphSearchTool
  ��    ��: CGraphSearchTool����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CGraphSearchTool::CGraphSearchTool()
{
    m_graph = 0;
    m_count = 0;

    m_visited = 0;
    m_visitedCount = 0;

    m_queue = 0;
    m_length = 0;
    m_front = 0;
    m_rear = 0;

    m_pIdxBuffer = 0;
    m_dwIdxBufferMemLen = GRAPH_PATH_COUNT_STEP;
    m_dwIdxBufferMemLenStep = GRAPH_PATH_COUNT_STEP;
    m_dwIdxBufferCount = 0;

    m_fnIsDest = 0;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::~CGraphSearchTool
  ��    ��: CGraphSearchTool����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CGraphSearchTool::~CGraphSearchTool()
{
    Clear();

    m_graph = 0;
    m_count = 0;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::SetGraph
  ��    ��: ����ͼ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CGraphSearchTool::SetGraph(IGraph *pGraph)
{
    m_graph = pGraph;
    if (m_graph)
    {
        m_count = m_graph->VexNum();
    }
}

/*******************************************************
  �� �� ��: CGraphSearchTool::InitVisited
  ��    ��: ��ʼ���ι��б�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CGraphSearchTool::InitVisited()
{
    if (!m_count)
    {
        return false;
    }

    if (m_visited)
    {
        free(m_visited);
    }

    m_visited = (bool *)malloc(m_count * sizeof(bool));
    if (!m_visited)
    {
        return false;
    }

    for (DWORD i = 0; i < m_count; ++i)
    {
        m_visited[i] = false;
    }

    m_visitedCount = 0;
    return true;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::InitQueue
  ��    ��: ��ʼ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CGraphSearchTool::InitQueue()
{
    if (!m_count)
    {
        return false;
    }

    if (m_queue)
    {
        free(m_queue);
    }

    m_queue = (QueNode *)malloc(m_count * sizeof(QueNode));
    if (!m_queue)
    {
        return false;
    }

    m_length = m_count;
    m_front = 0;
    m_rear = 0;

    return true;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::InitStack
  ��    ��: ��ʼ��ջ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CGraphSearchTool::InitStack()
{
    return InitQueue();
}

/*******************************************************
  �� �� ��: CGraphSearchTool::InitIdxBuffer
  ��    ��: ��ʼ����������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CGraphSearchTool::InitIdxBuffer(DWORD MemLen, DWORD dwMemLenStep)
{
    m_dwIdxBufferMemLen = (MemLen)? MemLen : GRAPH_PATH_COUNT_STEP;
    m_dwIdxBufferMemLenStep = (dwMemLenStep)? dwMemLenStep : GRAPH_PATH_COUNT_STEP;

    if (m_pIdxBuffer)
    {
        free(m_pIdxBuffer);
    }

    m_pIdxBuffer = (int *)malloc(m_dwIdxBufferMemLen * sizeof(int));
    if (!m_pIdxBuffer)
    {
        return false;
    }

    m_dwIdxBufferCount = 0;

    return true;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::EnQueue
  ��    ��: �ѽڵ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CGraphSearchTool::EnQueue(IGraph::POS adjVex, int curIdx)
{
    /// ���в���Ϊ��
    if (!m_queue)
    {
        return;
    }

    /// rear�쵽�ڴ��ڴ�߽�ʱ��Ҫô���������ڴ棬Ҫô�����ƽ�
    if (m_rear >= m_length)
    {
        if ((m_rear > m_front) && ((m_rear - m_front) >= m_length))
        {
            m_queue = (QueNode *)realloc(m_queue, (m_length + m_count) * sizeof(QueNode));
            if (!m_queue)
            {
                return;
            }

            m_length += m_count;
        }
        else
        {
            m_rear = 0;
        }
    }

    m_queue[m_rear].parent = curIdx;
    m_queue[m_rear].child = m_rear;
    m_queue[m_rear].childPos = adjVex;
    m_rear++;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::DeQueue
  ��    ��: �Ӷ�����ȡ���ڵ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CGraphSearchTool::DeQueue(IGraph::POS &adjVex, int &bufIdx)
{
    /// ���в���Ϊ��
    if (!m_queue)
    {
        return;
    }

    /// ����ȡ�վͷ���
    if (m_front == m_rear)
    {
        return;
    }

    /// front�쵽�ڴ��ڴ�߽�ʱ�������ƽ�
    if (m_front >= m_length)
    {
        m_front = 0;
    }

    adjVex = m_queue[m_front].childPos;
    bufIdx = m_queue[m_front].child;
    m_front++;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::PushStack
  ��    ��: ѹջ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CGraphSearchTool::PushStack(IGraph::POS adjVex, int curIdx)
{
    EnQueue(adjVex, curIdx);
}

/*******************************************************
  �� �� ��: CGraphSearchTool::PopStack
  ��    ��: ��ջ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CGraphSearchTool::PopStack(IGraph::POS &adjVex, int &bufIdx)
{
    /// ���в���Ϊ��
    if (!m_queue)
    {
        return;
    }

    /// ����ȡ�վͷ���
    if (!m_rear)
    {
        return;
    }

    adjVex = m_queue[m_rear - 1].childPos;
    bufIdx = m_queue[m_rear - 1].child;
    m_rear--;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::IsDest
  ��    ��: �Ƿ�Ŀ��ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CGraphSearchTool::IsDest(IGraph::POS curVex, int curIdx, IGraph::POS dstVex, bool &bContinue, bool bBufIdx)
{
    /// ����ҵ�Ŀ�ĵ㣬�ͽ��м�¼
    bool bRc = (m_fnIsDest)? (m_fnIsDest(curVex, dstVex, bContinue, this)) : (curVex == dstVex);
    if ( !bRc )
    {
        return false;
    }

    /// �������Ҫ��¼������ֱ�ӷ���(ʹ����ջ��ջ��ʽ���������ܻ��棬��Ϊ��ջ��ʧЧ��)
    if ( !bBufIdx )
    {
        EnQueue(curVex, curIdx);
        return true;
    }

    /// ���׼����¼���ڴ治�㣬����Ҫ��������
    if ( !Dalloc((void *&)m_pIdxBuffer, m_dwIdxBufferMemLen, m_dwIdxBufferCount, 1, m_dwIdxBufferMemLenStep, (DWORD)(sizeof(int))) )
    {
        return false;
    }

    EnQueue(curVex, curIdx);
    m_pIdxBuffer[m_dwIdxBufferCount++] = GetQueRear();

    return true;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::GetDestPath
  ��    ��: ��ȡĿ��·��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CGraphSearchTool::GetDestPath(IGraph::POS *&rpPath, DWORD &rdwPathLen, int iBufIdx)
{
    if (iBufIdx < 0)
    {
        iBufIdx = GetQueRear() - 1;
    }

    if (iBufIdx < 0)
    {
        return false;
    }

    /// ��ʼ�����
    rpPath = 0;
    rdwPathLen = 0;

    /// ��ȡ·������
    DWORD tmp = 0;
    QueNode *pQueNode = 0;
    for (tmp = iBufIdx; (tmp) && (pQueNode = GetQueNode(tmp)); tmp = pQueNode->parent)
    {
        rdwPathLen++;
    }

    /// ��������ڴ�
    DWORD pathIdx = rdwPathLen - 1;
    rpPath = (IGraph::POS *)malloc(rdwPathLen * sizeof(IGraph::POS));
    if (!rpPath)
    {
        rdwPathLen = 0;
        return false;
    }

    /// �����ȡ·��
    for (tmp = iBufIdx; (tmp) && (pQueNode = GetQueNode(tmp)); tmp = pQueNode->parent)
    {
        rpPath[pathIdx--] = pQueNode->childPos;
    }

    return true;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::Clear
  ��    ��: ����ռ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CGraphSearchTool::Clear()
{
    if (m_visited)
    {
        free(m_visited);
        m_visited = 0;
    }

    m_visitedCount = 0;

    if (m_queue)
    {
        free(m_queue);
        m_queue = 0;
    }

    m_length = 0;
    m_front = 0;
    m_rear = 0;

    if (m_pIdxBuffer)
    {
        free(m_pIdxBuffer);
        m_pIdxBuffer = 0;
    }

    m_dwIdxBufferMemLen = GRAPH_PATH_COUNT_STEP;
    m_dwIdxBufferMemLenStep = GRAPH_PATH_COUNT_STEP;
    m_dwIdxBufferCount = 0;

}

/*******************************************************
  �� �� ��: CGraphSearchTool::Dalloc
  ��    ��: ��̬�����ڴ�ӿ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CGraphSearchTool::Dalloc(void *&rpBuf, DWORD &rdwBufMemLen, DWORD dwUsedMemLen, DWORD dwNeedMemLen, DWORD dwBufMemLenStep, DWORD dwBufMemSize)
{
    /// �����ڴ��ҳ���������Ҫ��ֱ�ӷ��سɹ�
    if ( rpBuf && ((dwUsedMemLen + dwNeedMemLen) <= rdwBufMemLen) )
    {
        return true;
    }

    /// ���볤��ȡ������������Ҫ�����ֵ
    DWORD dwTmpCount = rdwBufMemLen + dwBufMemLenStep;
    if (dwTmpCount < (dwUsedMemLen + dwNeedMemLen))
    {
        dwTmpCount = dwUsedMemLen + dwNeedMemLen;
    }

    /// �޷�����ĳ��ȣ�����ʧ��
    if (!dwTmpCount)
    {
        return false;
    }

    /// �����ڴ棬ʧ�ܷ���
    int *pTmp = (int *)malloc(dwTmpCount * dwBufMemSize);
    if (!pTmp)
    {
        return false;
    }

    /// �ڴ�����
    (void)memset(pTmp, 0, dwTmpCount * dwBufMemSize);

    /// �����Ѿ����ڵ��ڴ浽ǰ��
    if (rpBuf)
    {
        (void)memcpy(pTmp, rpBuf, dwUsedMemLen * dwBufMemSize);
        free(rpBuf);
    }

    /// ����µ��ڴ�ͳ���
    rpBuf = pTmp;
    rdwBufMemLen = dwTmpCount;

    return true;
}

/*******************************************************
  �� �� ��: CGraphSearchTool::Dfree
  ��    ��: ��̬�ͷ��ڴ�ӿ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CGraphSearchTool::Dfree(void *&rpBuf, DWORD &rdwBufMemLen)
{
    if (rpBuf)
    {
        free(rpBuf);
        rpBuf = 0;
    }

    rdwBufMemLen = 0;
}

