/// -------------------------------------------------
/// dlistvector.h : ˫���������鹤���๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_LIST_DLISTVECTOR_H_
#define _TOOL_LIST_DLISTVECTOR_H_

#include "type.h"


#define INVALID_NODEID      0xFFFFFFFFL         // �������õĽڵ�ID
#define INVALID_NODEVALUE   0                   // �������õĽڵ�ֵ
#define SINGLE_INC_LENGTH   256                 // ���ε����ĳ���
#define MAX_ARRAY_LENGTH    0xFFFFFFFFL         // ������鳤��(ȫ8FΪ������)


/// =================================================
///     ��  ��  ˵  ��
/// -------------------------------------------------
/// 1. ����������Ľṹ:
/// |value|point|
/// +-----+-----+
/// |     |     |
/// | Val | Ptr |--+
/// |     |     |  |
/// +-----+-----+  |
/// |     |     |  |
/// | Val | Ptr |--+
/// |     |     |  |
/// +-----+-----+  |
/// |     |     |  |
/// | Val | Ptr |  |
/// |     |     |  |
/// +-----+-----+  |
/// |     |     |  |
/// | Val | Ptr |--+
/// |     |     |
/// +-----+-----+
/// -------------------------------------------------
/// 2. �����Ǹ�˫����������0Ԫ��ָ���������β
/// -------------------------------------------------
/// 3. ����Ľڵ�ʵ�����Ǹ�ָ�룬�����Ҫɾ������Ļ���
///    ��������ɾ��������������һ��ɾ�����̻ص�����
/// -------------------------------------------------
/// 4. ����û����Լ̳У�ʹ�ڵ㳤�ȳ���ԭʼ��С������
///    ��Ĭ�ϵ�CopyNode�������ǿ������ݵ�ָ��λ�ã���
///    ָ��m_pNodeValue�Ͳ������ˣ������������ݳ��ȿ�
///    ʼ��λ��(�û�Ҳ���Լ̳���дCopyNode���ı�֮)
/// =================================================


/// =================================================
///     �Զ�Ϊ����ϻص������ĺ�
/// -------------------------------------------------
///     ��  ��: DECLARE_DLVT_DELNODE_PROC_AUTO(c);
/// -------------------------------------------------
#define DECLARE_DLVT_DELNODE_PROC_AUTO(UserClass) \
    static void ___##UserClass##___DelNodeProc( \
        const void *cpNodeValue, \
        DWORD dwNodeID, \
        void *pProcPara)
/// -------------------------------------------------
///     ʵ  ��: IMPLEMENT_DLVT_DELNODE_PROC_AUTO(t,c)
/// -------------------------------------------------
#define IMPLEMENT_DLVT_DELNODE_PROC_AUTO(TypeClass, UserClass) \
    void TypeClass::___##UserClass##___DelNodeProc( \
        const void *cpNodeValue, \
        DWORD dwNodeID, \
        void *pProcPara) \
    { \
        (*(TypeClass *)cpNodeValue).~UserClass(); \
    }
/// -------------------------------------------------
///     ע  ��: REG_DLVT_DELNODE_PROC_AUTO(t,c,o);
/// -------------------------------------------------
#define REG_DLVT_DELNODE_PROC_AUTO(TypeClass, UserClass, UserObjPtr) \
    UserObjPtr->vSetDelNodeProc( \
        TypeClass::___##UserClass##___DelNodeProc, \
        this)
/// =================================================


class CdListVectorTool
{
public:
    /// ID(������)
    struct UserNode
    {
        DWORD m_dwNodeID;
    };

    /// �ڵ�ɾ������(�ص��������Ͷ���)
    typedef void (*DEL_NODE_PROC)(
        const void *cpNodeValue,
        DWORD dwNodeID,
        void *pProcPara
        );

    /// ��������ڵ�����
    struct ListNode : UserNode
    {
        ListNode *m_pPrevNode;
        ListNode *m_pNextNode;
        void *m_pNodeValue;                 // �ڵ�ֵ(����ռ�ø�λ�ò���չ�����ڴ�)
    };

protected:
    ListNode *m_pNodesBuf;                  // �ڵ��б�ָ�뻺����
    DWORD m_dwNodeSize;                     // �ڵ㻺�����ĸ���ֵ
    DWORD m_dwNodeNum;                      // �ڵ���Ŀ
    DEL_NODE_PROC m_pDelNodeProc;           // �ڵ�ɾ������
    void *m_pDelNodeProcPara;               // �ڵ�ɾ�������û�����

    DWORD m_dwInvalidID;                    // [�̳пɸ�]�������õĽڵ�ID
    void* m_pInvalidValue;                  // [�̳пɸ�]�������õĽڵ�ֵ
    DWORD m_dwSingleIncLength;              // [�̳пɸ�]���ε����ĳ���
    DWORD m_dwMaxArrayLength;               // [�̳пɸ�]������鳤��(ȫ8FΪ������)
    DWORD m_dwExtendNodeLength;             // [�̳пɸ�]��չ�ڵ㳤��(����ڵ㲻��ָ��,��Ϊ�����ռ�Ļ�,�����ֳ���4���ֽ�ʱ)

    ListNode *m_pLoopTmp;                   // ����������
    BOOL m_bDirect;                         // ��������(TRUEΪ����,FALSEΪ����)

    DWORD m_dwQueFront;                     // ����ͷ
    DWORD m_dwQueBack;                      // ����β

public:
    CdListVectorTool();
    virtual ~CdListVectorTool();

    /// ɾ���ڵ��¼�(���õ��ǻص�;�ɼ̳���д)
    virtual void onDelNode(const void *cpNodeValue, DWORD dwNodeID);

    /// ��ȡ�ڵ㳤��(�ɼ̳���д)
    virtual DWORD dwGetNodeLength();

    /// �����ڵ�(�ɼ̳���д)
    virtual void vCopyNode(ListNode *pNode, void *pValue);

    /// ��ȡ�ڵ�(�ɼ̳���д)
    virtual void *pGetNode(ListNode *pNode);

    /// ��ʼ��Value(�ɼ̳���д)
    virtual void vInitValue(ListNode *pNode);

    /// �Ƿ���õ�Value(�ɼ̳���д)
    virtual bool bValidValue(void *pValue);

    /// =================================================
    ///     ��ͨ�������
    /// -------------------------------------------------
    DWORD Add(DWORD dwIdx, void *pValue);
    DWORD Del(DWORD dwIdx);
    void *Get(DWORD dwIdx);
    DWORD Set(DWORD dwIdx, void *pValue);
    DWORD Append(void *pValue);
    /// =================================================

    /// =================================================
    ///     �����������(����һ������tmp, ��ֹ�ظ�����)
    /// -------------------------------------------------
    ListNode *First();
    ListNode *Prev();
    ListNode *Next();
    ListNode *Cur();
    /// =================================================

    /// =================================================
    ///     ���з�ʽ����(���ж�����λԪ�ر�ʶtmp)
    /// -------------------------------------------------
    void Push_back(void *pValue);           // ���������һ��Ԫ�غ����
    void Push_front(void *pValue);          // ������ĵ�һ��Ԫ��ǰ���
    void Pop_back();                        // ɾ����������һ���ڵ�
    void Pop_front();                       // ɾ������ĵ�һ���ڵ�
    void Push(void *pValue);                // ��Push_backһ��
    void Pop();                             // ��Pop_frontһ��
    /// =================================================

    /// ȫ�����
    void Clear();

    void    SetLoopDirect(BOOL bDirect) {m_bDirect = bDirect;}
    BOOL    GetLoopDirect() {return m_bDirect;}

    ListNode *pGetRootNode() const {return m_pNodesBuf;}
    DWORD   dwGetNodeSize() const {return m_dwNodeSize;}
    DWORD   dwGetNodeNum() const {return m_dwNodeNum;}

    void vSetDelNodeProc(DEL_NODE_PROC pProc, void *pProcPara)
    {
        m_pDelNodeProc = pProc; m_pDelNodeProcPara = pProcPara;
    }
    void vGetDelNodeProc(DEL_NODE_PROC& rpProc, void *& rpProcPara)
    {
        rpProc = m_pDelNodeProc; rpProcPara = m_pDelNodeProcPara;
    }

    DWORD   dwGetInvalidID() const {return m_dwInvalidID;}
    void*   pGetInvalidValue() const {return m_pInvalidValue;}
    DWORD   dwGetSingleIncLength() const {return m_dwSingleIncLength;}
    DWORD   dwGetMaxArrayLength() const {return m_dwMaxArrayLength;}
    DWORD   dwGetExtendNodeLength() const {return m_dwExtendNodeLength;}

    void    vSetInvalidID(DWORD dwInvalidID) {m_dwInvalidID = dwInvalidID;}
    void    vSetInvalidValue(void *pInvalidValue) {m_pInvalidValue = pInvalidValue;}
    void    vSetSingleIncLength(DWORD dwSingleIncLength) {m_dwSingleIncLength = dwSingleIncLength;}
    void    vSetMaxArrayLength(DWORD dwMaxArrayLength) {m_dwMaxArrayLength = dwMaxArrayLength;}
    void    vSetExtendNodeLength(DWORD dwExtendNodeLength) {m_dwExtendNodeLength = dwExtendNodeLength;}

};


#endif // #ifndef _TOOL_LIST_DLISTVECTOR_H_
