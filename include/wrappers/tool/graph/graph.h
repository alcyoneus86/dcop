/// -------------------------------------------------
/// graph.h : ͼ���ݽṹ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_GRAPH_GRAPH_H_
#define _TOOL_GRAPH_GRAPH_H_

#include "dcop.h"


/// ͼ
class IGraph
{
public:

    /// ����λ��
    typedef int POS;

    /// ����λ����Чֵ
    static const POS Null = -1;

    /// ��������
    class Vertex
    {
    public:
        int value;                              // ����ֵ
    };

public:
    IGraph();
    virtual ~IGraph();

    /////////////////////////////////////////////////////////////////
    /// ��ʼ������
    /////////////////////////////////////////////////////////////////
    /// ��ʼ������
    void Init(
                DWORD dwCount,                  // �������
                int *ipValue = 0,               // ����ֵ�ĳ�ʼ���б�
                DWORD dwSize = sizeof(int)      // ����ֵ�ĳ�ʼ���б���
                );
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    /// ͼ�Ͷ���λ�ù�������ز���
    /////////////////////////////////////////////////////////////////
    /// �Ƿ�����Чλ��(����true:��Чλ��;false:��Чλ��)
    bool NullPos(
                const POS &pos                  // ����λ��
                );

    /// ��ȡ��������
    DWORD VexNum();
    /////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////
    /// �����Լ�����ز���
    /////////////////////////////////////////////////////////////////
    /// ���ö���ֵ
    void Set(
                const POS &pos,                 // ����λ��
                int value                       // ����ֵ
                );

    /// ��ȡ����ֵ
    int Get(
                const POS &pos                  // ����λ��
                );

    /////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////
    /// �ڽӵ���ز���
    /////////////////////////////////////////////////////////////////
    /// ��ȡ������ĵ�һ���ڽӵ�(�Ӷ���)(�����ڽӵ�λ��)
    virtual POS FirstAdjVex(
                const POS &parentVex,           // ������(�����߽�����)
                const POS &curVex,              // ������
                const POS &dstVex               // Ŀ�Ķ���
                ) = 0;

    /// ��ȡ��һ���ڽӵ�(�Ӷ���)(�����ڽӵ�λ��)
    virtual POS NextAdjVex(
                const POS &parentVex,           // ������(�����߽�����)
                const POS &curVex,              // ������
                const POS &adjVex,              // ��ǰ�ڽӵ�
                const POS &dstVex               // Ŀ�Ķ���
                ) = 0;
    /////////////////////////////////////////////////////////////////


private:
    Vertex     *m_vexs;                         // �����б�
    DWORD       m_size;                         // ���������С
    DWORD       m_count;                        // ��������
};


/// ����������
class CGraphSearchTool
{
public:

    /// �������ж���
    typedef struct tagQueNode
    {
        int parent;                             // ����������
        int child;                              // ����������(��Ը�������˵���Ӷ���)
        IGraph::POS childPos;                   // ������λ��(��Ը�������˵���Ӷ���)
    }QueNode;

    /// Ŀ�Ķ����жϻص�
    typedef bool (*IS_DEST_FUNC)(
                IGraph::POS curVex,             // ��ǰ����
                IGraph::POS dstVex,             // Ŀ�궥��
                bool &bContinue,                // �ҵ����Ƿ����Ѱ��
                CGraphSearchTool *pThis         // ������
                );

public:
    CGraphSearchTool();
    ~CGraphSearchTool();


    /////////////////////////////////////////////////////////////////
    /// ͼ����Ĺ�����ؽӿ�
    /////////////////////////////////////////////////////////////////
    /// ����ͼ����
    void SetGraph(
                IGraph *pGraph                  // ͼ����
                );

    /// ��ȡͼ����
    IGraph *GetGraph() { return m_graph; }

    /// ��ȡͼ�Ķ�������
    DWORD GetCount() { return m_count; }
    /////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////
    /// ����ǰ�ĳ�ʼ������
    /////////////////////////////////////////////////////////////////
    /// ��ʼ�������б�(����Ҫ���ʻ���ʱ��ʼ��ʹ��)
    bool InitVisited();

    /// ��ʼ������(����Ҫ����ʱ��ʼ��ʹ��)
    bool InitQueue();

    /// ��ʼ��ջ(����Ҫ����ʱ��ʼ��ʹ�ã����к�ջΪѡ������һ��)
    bool InitStack();

    /// ��ʼ����������(����Ҫ��������ʱʹ�ã�ջ��ʽ��������ʹ����������)
    bool InitIdxBuffer(
                DWORD MemLen,                   // �����ڴ泤��
                DWORD dwMemLenStep              // �����ڴ���������
                );
    /////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////
    /// ���в���
    /////////////////////////////////////////////////////////////////
    /// �����
    void EnQueue(
                IGraph::POS adjVex,             // ����
                int curIdx                      // ��ǰ����
                );

    /// ������
    void DeQueue(
                IGraph::POS &adjVex,            // ����(���)
                int &bufIdx);                   // ��ʱ���������(���)
    /////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////
    /// ջ����
    /////////////////////////////////////////////////////////////////
    /// ��ջ
    void PushStack(
                IGraph::POS adjVex,             // ����
                int curIdx                      // ��ǰ����
                );

    /// ��ջ
    void PopStack(
                IGraph::POS &adjVex,            // ����(���)
                int &bufIdx                     // ��ʱ���������(���)
                );
    /////////////////////////////////////////////////////////////////

    /// �����Ƿ�Ϊ��
    bool QueueEmpty() { return (m_front == m_rear)? true : false; }

    /// ��ȡ�����ж���
    QueNode *GetQueNode(DWORD index)
    {
        if (index < m_length)
            return &(m_queue[index]);

        return 0;
    }

    /// ��ȡ�����ж���ĸ�����
    QueNode *GetParentQueNode(DWORD index)
    {
        if (index < m_length)
            if ((m_queue[index].parent >= 0) && (m_queue[index].parent < (int)m_length))
                return &(m_queue[m_queue[index].parent]);

        return 0;
    }

    /// ��ȡ��ǰ����β������
    DWORD GetQueRear() { return m_rear; }

    /// ���÷���״̬
    void SetVisited(DWORD index, bool state)
    {
        if (index >= m_count)
            return;

        if (state == m_visited[index])
            return;

        m_visited[index] = state;
        if (state)
            m_visitedCount++;
        else
            m_visitedCount--;
    }

    /// ��ȡ����״̬
    bool GetVisited(DWORD index)
    {
        if (index < m_count)
            return m_visited[index];

        return false;
    }

    /// �Ƿ���Ч��λ��
    bool NullPos(DWORD index) { return (index < m_count)? false : true; }

    /// ��ȡ�ѷ��ʵĶ�������
    DWORD GetVisitedCount() { return m_visitedCount; }

    /// ��ȡ��������
    int *GetIdxBuffer() { return m_pIdxBuffer; }

    /// ��ȡ������������
    DWORD GetIdxBufferCount() { return m_dwIdxBufferCount; }

    /// ����Ŀ���жϻص�����
    void SetIsDestFunc(IS_DEST_FUNC fnIsDest) { m_fnIsDest = fnIsDest; }

    /// �Ƿ���Ŀ���ж�
    bool IsDest(IGraph::POS curVex, int curIdx, IGraph::POS dstVex, bool &bContinue, bool bBufIdx = true);

    /// ��ȡĿ��·��
    bool GetDestPath(IGraph::POS *&rpPath, DWORD &rdwPathLen, int iBufIdx = -1);

    /// �������������
    void Clear();

    /// ��̬����һ���ڴ�
    static bool Dalloc(
                void *&rpBuf,                   // ����ڴ�(�����ѷ����ڴ�)
                DWORD &rdwBufMemLen,            // �ѷ����ڴ泤��
                DWORD dwUsedMemLen,             // ��ʹ���ڴ泤��
                DWORD dwNeedMemLen,             // �´�д��Ҫ���ڴ泤��
                DWORD dwBufMemLenStep,          // �����ڴ���������
                DWORD dwBufMemSize              // �ڴ�����ĵ�λ��С
                );

    /// �ͷŶ�̬�ڴ�
    static void Dfree(
                void *&rpBuf,                   // ����ͷź��ָ��(�����ѷ����ڴ�)
                DWORD &rdwBufMemLen             // �ڴ泤��
                );

private:
    IGraph *m_graph;                            // ͼ
    DWORD m_count;                              // ��������

    bool *m_visited;                            // �ѷ��ʹ�
    DWORD m_visitedCount;                       // �ѷ��ʹ��Ķ�������

    QueNode *m_queue;                           // ��������
    DWORD m_length;                             // ���г���
    DWORD m_front;                              // ������
    DWORD m_rear;                               // ����β

    int *m_pIdxBuffer;                          // ���������еĶ�����������
    DWORD m_dwIdxBufferCount;                   // ���������еĶ��������������
    DWORD m_dwIdxBufferMemLen;                  // ���������еĶ������������ڴ泤��
    DWORD m_dwIdxBufferMemLenStep;              // ���������еĶ������������ڴ���������

    IS_DEST_FUNC m_fnIsDest;                    // Ŀ���жϺ���

};

/// ͼ��·��������������
#define GRAPH_PATH_COUNT_STEP 10

/// ��ȡ��Сֵ�����ֵ
#define GRAPH_GET_MIN(x, y)     (((x) < (y))? (x) : (y))
#define GRAPH_GET_MAX(x, y)     (((x) > (y))? (x) : (y))


#endif // #ifndef _TOOL_GRAPH_GRAPH_H_

