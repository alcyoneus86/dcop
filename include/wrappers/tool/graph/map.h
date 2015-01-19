/// -------------------------------------------------
/// map.h : ͼת��Ϊ��ͼ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_GRAPH_MAP_H_
#define _TOOL_GRAPH_MAP_H_

#include "graph.h"


/// XY����ĵ�ͼ
class CXYMap : public IGraph
{
public:
    /// ��Ԫ����, ���Ͻǵ�������{0,0}, ���½ǵ�������{weight-1,hight-1}
    /// ����߽�����Ϊ�������Ͷ������ת������Ϊ:
    /// �ϱ�: {-1,-1} ~ {weight+1,-1}               ��Ӧ����: -1 ~ -(weight+2)
    /// �ұ�: {weight+1,-1} ~ {weight+1,hight+1}    ��Ӧ����: -(weight+2) ~ -(weight+hight+3)
    /// �±�: {-1,hight+1} ~ {weight+1,hight+1}     ��Ӧ����: -(2weight+hight+4) ~ -(weight+hight+3)
    /// ���: {-1,-1} ~ {weight+1,hight+1}          ��Ӧ����: -(2weight+2hight+4) ~ -(2weight+hight+4)
    typedef struct tagPOS
    {
        int x;
        int y;

        tagPOS() {x=-1;y=-1;}
        tagPOS(int tmpx, int tmpy) {x=tmpx;y=tmpy;}
    }POS;

    /// ������
    typedef enum tagDIRECT
    {
        DIRECT_NONE = -1,

        DIRECT_UP = 0,
        DIRECT_RIGHTUP,
        DIRECT_RIGHT,
        DIRECT_RIGHTDOWN,
        DIRECT_DOWN,
        DIRECT_LEFTDOWN,
        DIRECT_LEFT,
        DIRECT_LEFTUP,

        DIRECT_COUNT
    }DIRECT;

    /// ������
    #define XYMAP_GET_REVERSE_DIRECT(direct)    ((DIRECT)((direct + DIRECT_COUNT / 2) % DIRECT_COUNT))

    /// ���õķ���
    #define XYMAP_AVAILABLE_DIRECT(direct)      ((direct > DIRECT_NONE) && (direct < DIRECT_COUNT))

    /// ����
    #define XYMAP_GET_DISTANCE(a, b)            (((a)>(b))? ((a)-(b)) : ((b)-(a)))

    /// ��ѡ��������, Ҳ����дGetNextDirect��ò�ͬ����
    typedef enum tagDIRECT_TYPE
    {
        DIRECT_TYPE_4 = 0,                      // �ĸ�����(��ֱ����)
        DIRECT_TYPE_8                           // �˸�����(��ֱ+б����)
    }DIRECT_TYPE;


public:
    CXYMap();
    virtual ~CXYMap();

    /// ��ʼ����ͼ�Ŀ���Լ���Ԫ��ֵ
    void Init(DWORD dwWeight, DWORD dwHight, int *ipValue = 0);

    /// ���õ�Ԫֵ
    virtual void Set(const POS &pos, int value);

    /// ��ȡ��Ԫֵ
    virtual int Get(const POS &pos);

    /// �Ƿ�����Ч����
    virtual bool NullPos(const POS &pos);

    /// ����ͼ����ת��Ϊͼ�Ķ�������
    virtual IGraph::POS Vex(const POS &pos, DIRECT comeDirect = DIRECT_NONE);

    /// ��ͼ�Ķ�������ת��Ϊ��ͼ����
    virtual void Pos(const IGraph::POS &vex, POS &pos);

    /// ���÷�������(����4������/8������)
    void SetDirectType(DIRECT_TYPE directType) { m_directType = directType; }

    /// ���ÿ��
    void SetWeight(DWORD dwWeight) { m_weight = dwWeight; }

    /// ���ø߶�
    void SetHight(DWORD dwHight) { m_hight = dwHight; }

    /// ��ȡ���
    DWORD GetWeight() { return m_weight; }

    /// ��ȡ�߶�
    DWORD GetHight() { return m_hight; }

    /// �Ƿ���Է���
    virtual bool CanVisit(const POS &throughPos, const POS &parentPos, const POS &curPos);

    /// ��ȡ��һ������
    virtual DIRECT GetFirstDirect(const POS &curPos, const POS &dstPos);

    /// ��ȡ��һ������
    virtual DIRECT GetNextDirect(DIRECT firstDirect, DIRECT curDirect);

    /// ��ȡ��һ�����������
    virtual DIRECT GetNextDirectPos(const POS &parentPos, const POS &curPos, POS &nextPos, DIRECT firstDirect);

    /// ���������˵��: ���ǰ�����������Ŀ�ĵ�ķ�����Ϊ��һ�������ķ���

    /// =============== ��д���ຯ�� ================
    virtual IGraph::POS FirstAdjVex(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &dstVex);
    virtual IGraph::POS NextAdjVex(const IGraph::POS &parentVex, const IGraph::POS &curVex, const IGraph::POS &adjVex, const IGraph::POS &dstVex);
    /// =============================================

public:
    /// ��ȡ��ǰ���������һ����ķ���
    DIRECT GetDirect(const POS &parentPos, const POS &curPos);

    /// ��ȡ��һ�����ϵ�ָ�����������
    void GetDirectPos(const POS &parentPos, DIRECT curDirect, POS &curPos);

private:
    DIRECT_TYPE m_directType;                   // ��������
    DWORD m_weight;                             // ���(X��)
    DWORD m_hight;                              // �߶�(Y��)
};


#endif // #ifndef _TOOL_GRAPH_MAP_H_

