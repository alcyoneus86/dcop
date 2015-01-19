/// -------------------------------------------------
/// triangle.h : ͼת��Ϊ���ǵ�ͼ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_GRAPH_TRIANGLE_H_
#define _TOOL_GRAPH_TRIANGLE_H_


#include "map.h"


/// ���ǵ�ͼ(ÿ�������м���б�ߣ����ָ�Ϊ����������)
class CTriangleMap : public CXYMap
{
public:

    /// ��ʼ����ͼ�Ŀ���Լ���Ԫ��ֵ
    void Init(DWORD dwWeight, DWORD dwHight, int *ipValue = 0);

    /// =============== ��д���ຯ�� ================
    void Set(const CXYMap::POS &pos, int value);
    int Get(const CXYMap::POS &pos);
    IGraph::POS Vex(const CXYMap::POS &pos, DIRECT comeDirect = DIRECT_NONE);
    void Pos(const IGraph::POS &vex, POS &pos);
    bool CanVisit(const POS &throughPos, const POS &parentPos, const POS &curPos);
    /// =============================================
};


#endif // #ifndef _TOOL_GRAPH_TRIANGLE_H_

