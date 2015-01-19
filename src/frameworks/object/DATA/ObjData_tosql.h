/// -------------------------------------------------
/// ObjData_tosql.h : ��TLVת��ΪSQL���˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJDATA_TOSQL_H_
#define _OBJDATA_TOSQL_H_

#include "ObjData_handle.h"
#include "string/dstring.h"
#include "mysql.h"


INTF_VER(ITlvToSQL, 1, 0, 0);


/// TLV��SQLת���ӿ�
interface ITlvToSQL : public Instance
{
    /// ��ȡ���������б�
    virtual DWORD GetCreateFieldList(
                        CDString &strArgList
                        ) = 0;

    /// ��ȡ��������б�
    virtual DWORD GetInsertFieldList(
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen, 
                        CDString &strArgList
                        ) = 0;

    /// ��ȡ���������б�
    virtual DWORD GetCondFieldList(
                        BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, 
                        DWORD dwCondParaCount, 
                        void *pCondData, 
                        DWORD dwCondDataLen, 
                        CDString &strArgList
                        ) = 0;

    /// ��ȡ��������б�
    virtual DWORD GetReqFieldList(
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        void *pReqData, 
                        DWORD dwReqDataLen, 
                        CDString &strArgList
                        ) = 0;

    /// ��ȡ���������б�
    virtual DWORD GetFieldNameList(
                        DCOP_PARA_NODE *pPara, 
                        DWORD dwParaCount, 
                        CDString &strArgList, 
                        const char *cszSplit = ", ", 
                        CDString *pStrJoinTable = NULL
                        ) = 0;

    /// ��ȡ��Ӧ�����б�
    virtual DWORD GetRspFieldList(
                        DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        MYSQL_RES *res, 
                        DCOP_PARA_NODE *&rpRspPara, 
                        DWORD &rdwRspParaCount, 
                        CDArray &aRspData
                        ) = 0;

    /// ��ȡIdentify�ֶ�����
    virtual DWORD GetIdentifyName(
                        CDString &strIdName
                        ) = 0;
};


/// TLV��SQLת��ʵ��
class CTlvToSQL : public ITlvToSQL
{
public:
    /// �������������Ͷ���
    static const char *ArgOpCode[];

public:
    CTlvToSQL(Instance *piParent, int argc, char **argv);
    ~CTlvToSQL();

    DCOP_DECLARE_INSTANCE;

    DWORD GetCreateFieldList(CDString &strArgList);

    DWORD GetInsertFieldList(DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        CDString &strArgList);

    DWORD GetCondFieldList(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        CDString &strArgList);

    DWORD GetReqFieldList(DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        CDString &strArgList);

    DWORD GetFieldNameList(DCOP_PARA_NODE *pPara, 
                        DWORD dwParaCount, 
                        CDString &strArgList, 
                        const char *cszSplit, 
                        CDString *pStrJoinTable);

    DWORD GetRspFieldList(DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount, 
                        MYSQL_RES *res, 
                        DCOP_PARA_NODE *&rpRspPara, 
                        DWORD &rdwRspParaCount, 
                        CDArray &aRspData);

    DWORD GetIdentifyName(CDString &strIdName);

private:
    DWORD GetCondArgOpCode(BYTE cond, 
                        BYTE opCode, 
                        BYTE paraType, 
                        const char *fieldName, 
                        CDString &strArgList);

    DWORD GetReqArgOpCode(BYTE opCode, 
                        BYTE paraType, 
                        const char *fieldName, 
                        CDString &strArgList);

    DWORD GetRspArgValue(DCOP_PARA_NODE *pRspPara, 
                        DWORD dwRspParaCount, 
                        MYSQL_ROW row, 
                        unsigned long *lens, 
                        void *pRspData, 
                        DWORD dwRspDataLen);

    DWORD GetJoinTable(DWORD dwRelID, 
                        CDString &strFieldName,
                        CDString &strJoinTable);

private:
    IDataHandle *m_piDataHandle;
};


#endif // #ifndef _OBJDATA_TOSQL_H_

