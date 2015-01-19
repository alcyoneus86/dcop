/// -------------------------------------------------
/// ObjData_file.h : �ļ�����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDATA_FILE_H_
#define _OBJDATA_FILE_H_

#include "ObjData_index.h"
#include "ObjData_handle.h"
#include "cpu/bytes.h"
#include "string/dstring.h"


class CDataFile : public IDataHandle
{
public:
    /// ��¼��Ч��ʶ
    enum REC_FLAG
    {
        REC_FLAG_NULL = 0,
        REC_FLAG_VALID
    };

    /// �ļ�ͷ
    struct FileHead
    {
        char m_tableName[DCOP_TABLE_NAME_LEN];  // ����
        BYTE m_ver;                             // �汾��
        BYTE m_level;                           // ����
        WORD m_headerSize;                      // ͷ����С
        DWORD m_objectID;                       // ��������ID
        DWORD m_attrID;                         // ����ID
        DWORD m_fieldCount;                     // �ֶθ���
        DWORD m_recordCount;                    // ��¼����
        DWORD m_totalCount;                     // �ܸ���(��ɾ���ļ�¼)
        DWORD m_curIdentify;                    // ��ǰ��ʶ
    };

    /// �ļ���� : �ļ�ͷ��Ϣ(FileHead) + �ֶ���Ϣ(Field*N) + ��¼��Ϣ(M*N)

    /// �ļ�ͷ���ֶ���Ϣ����Ҫʹ�������ֽ��򱣴棬�ֽ���ת������
    static void BytesChangeFileHead(FileHead *pFileHead);
    static void BytesChangeFieldInfo(IModel::Field *pFileInfo);

public:
    CDataFile(Instance *piParent, int argc, char **argv);
    ~CDataFile();

    DCOP_DECLARE_INSTANCE;

    DWORD Init(DWORD dwAttrID, 
                IObject *pOwner, 
                IModel *piModel);

    void Dump(LOG_PRINT logPrint, LOG_PARA logPara = 0);

    DWORD AddRecord(DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                void *pReqData, 
                DWORD dwReqDataLen, 
                DCOP_PARA_NODE **ppEvtPara, 
                DWORD *pdwEvtParaCount, 
                CDArray *pEvtData);

    DWORD DelRecord(BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DCOP_PARA_NODE **ppEvtPara, 
                DWORD *pdwEvtParaCount, 
                CDArray *pEvtData);

    DWORD EditRecord(BYTE byCond, 
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
                CDArray *pEvtData);

    DWORD QueryRecord(BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DCOP_PARA_NODE *pReqPara, 
                DWORD dwReqParaCount, 
                DCOP_PARA_NODE *&rpRspPara, 
                DWORD &rdwRspParaCount, 
                CDArray &aRspData);

    DWORD CountRecord(BYTE byCond, 
                DCOP_PARA_NODE *pCondPara, 
                DWORD dwCondParaCount, 
                void *pCondData, 
                DWORD dwCondDataLen, 
                DWORD &rdwCount);

    DWORD AddKeyIdx(DCOP_PARA_NODE *pIdxPara, 
                DWORD dwIdxParaCount);

    DWORD DelKeyIdx(DCOP_PARA_NODE *pIdxPara, 
                DWORD dwIdxParaCount);

    DWORD GetRecCount() {return m_head.m_recordCount;}

    void SaveCurIdentify(DWORD curIdentify);

private:
    DWORD AddRecord(BYTE *pbyRec);

    DWORD DelRecord(DWORD dwRecNo);

    DWORD SetRecord(DWORD dwRecNo, BYTE *pbyRec);

    DWORD GetRecord(DWORD dwRecNo, BYTE *pbyRec);

    DWORD GetIdleRec();

    DWORD BldAllIdx();

private:
    FileHead m_head;                        // ͷ������
    CDString m_fileName;                    // �ļ�������
    CRecIdx m_recIdx;                       // ��¼����
};


#endif // #ifndef _OBJDATA_FILE_H_

