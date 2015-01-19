/// -------------------------------------------------
/// ObjData_index.h : ��¼����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDATA_INDEX_H_
#define _OBJDATA_INDEX_H_

#define INC_MAP

#include "ObjData_handle.h"
#include "BaseMessage.h"


/// ��¼������
class CRecIdx
{
public:

    /// �ؼ���
    class CKey
    {
    public:
        CKey();
        CKey(DWORD dwLen);
        CKey(const void *cpKey, DWORD dwLen);
        CKey(const CKey &rKey);
        ~CKey();

        void *GetKey() const {return m_pKey;}
        DWORD GetLen() const {return m_dwLen;}

        CKey& operator=(const void *cpKey);
        CKey& operator=(const CKey &rKey);
        bool operator<(const CKey &rKey) const;
        bool operator==(const CKey &rKey) const;

    private:
        void *m_pKey;
        DWORD m_dwLen;
    };

    class CIdx
    {
    public:
        CIdx();
        CIdx(void *ptr);
        CIdx(DWORD pos);
        ~CIdx();

        operator void *()
        {
            return m_idx.m_ptr;
        }

        operator DWORD()
        {
            return m_idx.m_pos;
        }

    private:
        union
        {
            void *m_ptr;
            DWORD m_pos;
        }m_idx;
    };

    /// ��¼����MAP
    typedef std::map<CKey, CIdx> MAP_RECIDX;
    typedef MAP_RECIDX::iterator IT_RECIDX;

    /// �ֶ�����MAP
    typedef std::map<CKey, MAP_RECIDX> MAP_FLDIDX;
    typedef MAP_FLDIDX::iterator IT_FLDIDX;

public:
    CRecIdx();
    ~CRecIdx();

    /// ��ӹؼ���(����Ҫ������¼������BldKeyIdx)
    DWORD AddKey(DCOP_PARA_NODE *pPara, DWORD dwParaCount);

    /// ɾ���ؼ���
    DWORD DelKey(DCOP_PARA_NODE *pPara, DWORD dwParaCount);

    /// ���������¼�����Ĳ���
    DWORD OnAddRec(IDataHandle::Field *pFields, BYTE *pbyRec, CIdx recordIdx);

    /// ɾ��������¼�����Ĳ���
    void OnDelRec(IDataHandle::Field *pFields, BYTE *pbyRec);

    /// �����ؼ�������(����ȫ�������������¼������OnAddRec)
    DWORD BldKeyIdx(DCOP_PARA_NODE *pPara, DWORD dwParaCount, 
                        IDataHandle::Field *pFields, BYTE *pbyRec, 
                        CIdx recordIdx);

    /// ���Ҽ�¼
    CIdx FindRec(DCOP_PARA_NODE *pPara, DWORD dwParaCount, 
                        void *pData, DWORD dwDataLen);

    /// ����ؼ������Ĳ���/����(ֻ���ֶ�ID�ʹ�С��������)
    void ClearType(CKey &fieldKey);

private:
    MAP_FLDIDX m_idx;
};



#endif // #ifndef _OBJDATA_INDEX_H_

