/// -------------------------------------------------
/// dfs.h : ������������㷨����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_GRAPH_DFS_H_
#define _TOOL_GRAPH_DFS_H_

/////////////////////////////////////////////////////
///     DFS:������������㷨(DepthFirstSearch)
/// -------------------------------------------------
/// ������ȱ�����������ȱ������������ڣ�������ȱ���
/// ���Բ�Ϊ˳�򣬽�ĳһ���ϵ����нڵ㶼��������֮���
/// ����һ����������������ȱ����ǽ�ĳһ��֦���ϵ�����
/// �ڵ㶼��������֮�󣬲�ת��������һ��֦���ϵ����н�
/// �㡣
/// -------------------------------------------------
/// ������ȱ�����ĳ��������������ȷ���������㣬Ȼ��
/// �ҳ��շ���������ĵ�һ��δ�����ʵ��ڽ�㣬Ȼ����
/// �Դ��ڽ��Ϊ���㣬������������һ���µĶ�����з��ʣ�
/// �ظ��˲��裬ֱ�����н�㶼��������Ϊֹ��
/// -------------------------------------------------
/// ������ȱ�����ĳ��������������ȷ���������㣬Ȼ��
/// �ҳ������������δ�����ʵ��ڽӵ㣬��������ٷ���
/// ��Щ����е�һ���ڽӵ�����н�㣬�ظ��˷�����ֱ��
/// ���н�㶼��������Ϊֹ��
/// -------------------------------------------------
/// ���Կ������ַ���������������ǰ�ߴӶ���ĵ�һ����
/// �ӵ�һֱ������ȥ�ٷ��ʶ���ĵڶ����ڽӵ㣻���ߴӶ�
/// �㿪ʼ���ʸö���������ڽӵ����������£�һ��һ���
/// ���ʡ�
/// -------------------------------------------------
/////////////////////////////////////////////////////

#include "graph/graph.h"



#define DFS_PATH_COUNT_STEP 10

/// ���������������·��
class CDFS : private CGraphSearchTool
{
public:

    /// ·���б�ڵ�
    typedef struct tagPathNode
    {
        IGraph::POS *path;
        DWORD pathLen;
    }PathNode;

public:
    CDFS();
    ~CDFS();

    /// ����ͼ����
    void SetGraph(IGraph *pGraph) { CGraphSearchTool::SetGraph(pGraph); }

    /// ����Ŀ���жϻص�����
    void SetIsDestFunc(IS_DEST_FUNC fnIsDest) { CGraphSearchTool::SetIsDestFunc(fnIsDest); }

    /// ��������·��
    bool FindAllPath(
                    IGraph::POS throughVex, 
                    IGraph::POS srcVex, 
                    IGraph::POS dstVex, 
                    PathNode *&rpPaths, 
                    DWORD &rdwPathCount, 
                    bool bRecur = false);       // �Ƿ�ݹ�

    /// �ͷ�·��
    void FreePaths(PathNode *pPaths, 
                    DWORD dwPathCount);

private:
    /// �ݹ�
    void DFS(IGraph::POS throughVex, 
                    IGraph::POS curVex, 
                    IGraph::POS dstVex, 
                    bool &bContinue, 
                    int curQueIdx);

    /// �ǵݹ�
    bool DFS(IGraph::POS throughVex, 
                    IGraph::POS curVex, 
                    IGraph::POS dstVex, 
                    PathNode *&rpPaths, 
                    DWORD &rdwPathCount);

};


#endif // #ifndef _TOOL_GRAPH_DFS_H_

