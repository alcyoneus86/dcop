/// -------------------------------------------------
/// tablestring.h : ����ַ�������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_STREAM_TABLESTREAM_H_
#define _TOOL_STREAM_TABLESTREAM_H_

#include "dcop.h"
#include "string/dstring.h"

/////////////////////////////////////////////////////
///                ����ַ���˵��
/// -------------------------------------------------
/// ��Ҫ�������Ա����ʽ��ʾ���ַ���
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// ����ַ���
/// -------------------------------------------------
class CTableString
{
public:
    CTableString();
    CTableString(DWORD dwColCount, DWORD dwRowCount, const char *cszAlign = 0);
    ~CTableString();

    /// �����к�����Ϣ
    DWORD Init(DWORD dwColCount, DWORD dwRowCount);

    /// ���ö�����Ϣ
    void Indent(const char *cpszAlign) {m_cpszAlign = cpszAlign;}

    /// �����ַ���
    CTableString &operator <<(const char *cpszStr);

    /// ��ȡָ����
    CDString *Line(DWORD dwIdx) const;

    /// ��ȡ������
    DWORD Count() const {return m_rowCount;}

    /// ��ʾ���
    void Show(LOG_PRINT logPrint, LOG_PARA logPara, const char *cpszMargin = 0, const char *cpszTitle = 0) const;

private:
    void Clear();
    void ExpandCol(DWORD dwStrLen);
    DWORD GetColOffset();

private:
    DWORD *m_colNode;
    DWORD m_colCount;

    CDString *m_rowNode;
    DWORD m_rowCount;

    DWORD m_dwColPos;
    DWORD m_dwRowPos;

    const char *m_cpszAlign;
};


#endif // #ifndef _TOOL_STREAM_TABLESTREAM_H_

