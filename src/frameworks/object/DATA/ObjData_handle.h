/// -------------------------------------------------
/// ObjData_handle.h : ���ݾ��˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDATA_HANDLE_H_
#define _OBJDATA_HANDLE_H_

#include "ObjModel_if.h"
#include "ObjData_if.h"


INTF_VER(IDataHandle, 1, 0, 0);


interface IDataHandle : public Instance
{
public:
    /// �ֶζ���
    struct Field : public IModel::Field
    {
        DWORD m_fieldOffset;                // �ֶ�ƫ��
    };

public:
    IDataHandle();
    virtual ~IDataHandle();

    /// ��ʼ��
    virtual DWORD Init(
                DWORD dwAttrID, 
                IObject *pOwner, 
                IModel *piModel
                ) = 0;

    /// Dump
    virtual void Dump(
                LOG_PRINT logPrint, 
                LOG_PARA logPara
                ) = 0;

    /// ��Ӽ�¼
    virtual DWORD AddRecord(
                DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                void *pReqData, 
                DWORD dwReqDataLen, 
                DCOP_PARA_NODE **ppEvtPara, 
                DWORD *pdwEvtParaCount, 
                CDArray *pEvtData
                ) = 0;

    /// ɾ����¼
    virtual DWORD DelRecord(
                BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DCOP_PARA_NODE **ppEvtPara, 
                DWORD *pdwEvtParaCount, 
                CDArray *pEvtData
                ) = 0;

    /// �༭��¼
    virtual DWORD EditRecord(
                BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen,
                DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                void *pReqData, 
                DWORD dwReqDataLen, 
                DCOP_PARA_NODE **ppEvtPara, 
                DWORD *pdwEvtParaCount, 
                CDArray *pEvtData
                ) = 0;

    /// ��ѯ��¼
    virtual DWORD QueryRecord(
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

    /// ͳ�Ƽ�¼
    virtual DWORD CountRecord(
                BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DWORD &rdwCount
                ) = 0;

    /// �������
    virtual DWORD AddKeyIdx(
                DCOP_PARA_NODE *pIdxPara, 
                DWORD dwIdxParaCount
                ) = 0;

    /// ɾ������
    virtual DWORD DelKeyIdx(
                DCOP_PARA_NODE *pIdxPara, 
                DWORD dwIdxParaCount
                ) = 0;

    /// ��ȡ��¼����
    virtual DWORD GetRecCount(
                ) = 0;

    /// ���浱ǰID
    virtual void SaveCurIdentify(DWORD curIdentify) {}

    /// ��ȡģ�Ͷ���
    IModel *GetModel() {return m_piModel;}

    /// ��ȡ��������
    IObject *GetOwner() {return m_pOwner;}

    /// ��ȡ����ID
    DWORD GetAttrID() {return m_dwAttrID;}

    /// ��ȡ�������
    DWORD GetType() {return m_dwType;}

    /// ��ȡ���ݱ���
    const char *GetTableName() {return m_szTableName;}

    /// ��ȡ�ֶ��б�
    Field *GetFields() {return m_pFields;}

    /// ��ȡ�ֶθ���
    DWORD GetFieldCount() {return m_dwFieldCount;}

    /// ��ȡ��¼��С
    DWORD GetRecSize() {return m_dwRecSize;}

    /// ��ʼ��(����������ǰ���Init�ӿ��У������ȵ��ø��������ӿ�)
    DWORD Init(DWORD dwType, 
                DWORD dwAttrID, 
                IObject *pOwner, 
                IModel *piModel, 
                IData *piData);

    /// ����¼
    DWORD FillRecord(Field *pFields, 
                DWORD dwFieldCount, 
                BYTE *pbyRec, 
                DWORD dwRecSize, 
                DCOP_PARA_NODE *pPara, 
                DWORD dwParaCount, 
                void *pData, 
                DWORD dwDataLen);

    /// �����ֶ�
    Field *CopyFields(IModel *piModel, 
                DWORD dwAttrID, 
                DWORD &rdwFieldCount, 
                DWORD &rdwRecSize);

    /// ��������
    IModel::Relation *CopyShips(IModel *piModel, 
                DWORD dwAttrID, 
                DWORD &rdwShipCount);

    /// ��ȡ�������
    DCOP_PARA_NODE *GetOutPara(Field *pFields, 
                DWORD dwFieldCount, 
                DWORD &rdwOutParaCount, 
                DCOP_PARA_NODE *pReqPara = 0, 
                DWORD dwReqParaCount = 0, 
                DWORD *pdwOutDataLen = 0, 
                bool bSpecPara = false);

    /// ������¼
    DWORD CopyRecord(Field *pFields, 
                DWORD dwFieldCount, 
                BYTE *pbyRec, 
                DWORD dwRecSize, 
                DCOP_PARA_NODE *pPara, 
                DWORD dwParaCount, 
                void *pData, 
                DWORD dwDataLen);

    /// ���¼�¼
    DWORD UpdateRecord(Field *pFields, 
                DWORD dwFieldCount, 
                BYTE *pbyRec, 
                DWORD dwRecSize, 
                DCOP_PARA_NODE *pPara, 
                DWORD dwParaCount, 
                void *pData, 
                DWORD dwDataLen);

    /// �ж��ֶ��Ƿ�������
    bool IsDigital(DWORD dwFieldType);

    /// �ж��ֶ��Ƿ��Ǳ�ʶ
    bool IsIdentify(DWORD dwFieldType);

    /// ����ֵ
    DWORD SetRecValue(DWORD dwFieldType, 
                void *pDst, 
                DWORD dwDstSize, 
                void *pSrc, 
                DWORD dwSrcSize);

    /// ID������(ѭ��+1��ֱ�����ɲ����ڵ�ID)
    DWORD IdentifyGenerator(DWORD fieldID, DWORD fieldSize, DWORD minValue, DWORD maxValue);

    /// ���õ�ǰID
    void SetCurIdentify(DWORD curIdentify) {m_curIdentify = curIdentify;}

    /// ��ȡ��¼ƫ�ƺ�����
    void GetOffsetAndLimit(DWORD &rdwOffset, 
                DWORD &rdwLimit, 
                DCOP_PARA_NODE *pPara, 
                DWORD dwParaCount, 
                void *pData, 
                DWORD dwDataLen);

    /// �Ƿ�ƥ���¼
    bool bMatchRecord(Field *pFields, 
                DWORD dwFieldCount, 
                BYTE *pbyRec, 
                BYTE byCond, 
                DCOP_PARA_NODE *pPara, 
                DWORD dwParaCount, 
                void *pData, 
                DWORD dwDataLen);

    /// �Ƿ������ַ�ʽƥ��
    bool bDigitalMatch(DWORD dwOpCode, 
                DWORD dwParaValue, 
                DWORD dwRecValue);

    /// �Ƿ����ַ�����ʽƥ��
    bool bStringMatch(DWORD dwOpCode, 
                const char *pParaStr, 
                const char *pRecStr);

    /// �Ƿ���Buf��ʽƥ��
    bool bBufferMatch(DWORD dwOpCode, 
                void *pParaBuf, 
                void *pRecBuf, 
                DWORD dwBufLen);

    /// ��ȡ���������Ժ��ֶ�
    IModel::Relation *GetRelation(DWORD dwRelID);

    /// ��ȡ�����Ĳ���
    DWORD GetRelPara(DWORD dwRelID,
                DCOP_PARA_NODE &rPara);

    /// ��ȡ�����ļ�¼
    DWORD GetRelRecord(DCOP_PARA_NODE &rPara,
                BYTE *pbyRec, 
                void *pData);

private:
    IData *m_piData;                        // ���ݶ���
    IModel *m_piModel;                      // ģ�Ͷ���
    IObject *m_pOwner;                      // ��������
    DWORD m_dwAttrID;                       // ����ID
    DWORD m_dwType;                         // �������
    char m_szTableName[DCOP_TABLE_NAME_LEN]; // ���ݱ���
    Field *m_pFields;                       // �ֶ��б�
    DWORD m_dwFieldCount;                   // �ֶθ���
    IModel::Relation *m_pShips;             // �����б�
    DWORD m_dwShipCount;                    // ��������
    DWORD m_dwRecSize;                      // ��¼��С
    DWORD m_curIdentify;                    // ��ǰ��ʶ
};


#endif // #ifndef _OBJDATA_HANDLE_H_

