/// -------------------------------------------------
/// ObjData_if.h : �������ݹ���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDATA_IF_H_
#define _OBJDATA_IF_H_

#include "Object_if.h"
#include "BaseMessage.h"
#include "array/darray.h"


/// -------------------------------------------------
/// �������ݵĶ���˵��
/// -------------------------------------------------
/// ��Ҫ�����йܶ��������е�"����"
/// �����йܵ�����:
///     ���԰Ѷ��������ͳһ����������ֱ��������ݱ��
///     �ڴ����ݲ�������Ӧ�����ݿ��⡢�ָ�����ز���
///     (��ṹ��������ֵ��Ӧ��ģ��)
/// -------------------------------------------------


/// -------------------------------------------------
/// ����IData�汾��
/// -------------------------------------------------
INTF_VER(IData, 1, 0, 0);


/// -------------------------------------------------
/// ���ݽӿڶ���
/// -------------------------------------------------
interface IData : public IObject
{
    /// ������Ͷ���
    enum TYPE
    {
        TYPE_MEM = 0,                       // [0]�ڴ�
        TYPE_FILE,                          // [1]�ļ�
        TYPE_MYSQL                          // [2]MySQL���ݿ�
    };

    /// ���ݾ��
    typedef void * Handle;

    /// ����һ������
    virtual DWORD Create(
                DWORD dwType, 
                DWORD attrID, 
                IObject *pOwner, 
                Handle &hData
                ) = 0;

    /// ����һ������
    virtual DWORD Destroy(
                DWORD attrID
                ) = 0;

    /// ���һ����¼
    virtual DWORD AddRecord(
                Handle hData, 
                DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                void *pReqData, 
                DWORD dwReqDataLen, 
                DCOP_PARA_NODE **ppEvtPara = 0, 
                DWORD *pdwEvtParaCount = 0, 
                CDArray *pEvtData = 0
                ) = 0;

    /// ɾ��һ����¼
    virtual DWORD DelRecord(
                Handle hData, 
                BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DCOP_PARA_NODE **ppEvtPara = 0, 
                DWORD *pdwEvtParaCount = 0, 
                CDArray *pEvtData = 0
                ) = 0;

    /// �༭һ����¼
    virtual DWORD EditRecord(
                Handle hData, 
                BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                void *pReqData, 
                DWORD dwReqDataLen, 
                DCOP_PARA_NODE **ppEvtPara = 0, 
                DWORD *pdwEvtParaCount = 0, 
                CDArray *pEvtData = 0
                ) = 0;

    /// ��ѯ������¼
    virtual DWORD QueryRecord(
                Handle hData, 
                BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                DCOP_PARA_NODE *&rpRspPara, 
                DWORD &rdwRspParaCount, 
                CDArray &aRspData
                ) = 0;

    /// ͳ�Ƽ�¼����
    virtual DWORD CountRecord(
                Handle hData, 
                BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DWORD &rdwCount
                ) = 0;

    /// ��ӹؼ�������
    virtual DWORD AddKeyIdx(
                Handle hData, 
                DCOP_PARA_NODE *pIdxPara, 
                DWORD dwIdxParaCount
                ) = 0;

    /// ɾ���ؼ�������
    virtual DWORD DelKeyIdx(
                Handle hData, 
                DCOP_PARA_NODE *pIdxPara, 
                DWORD dwIdxParaCount
                ) = 0;

};


#endif // #ifndef _OBJDATA_IF_H_

