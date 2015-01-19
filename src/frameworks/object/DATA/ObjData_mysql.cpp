/// -------------------------------------------------
/// ObjData_mysql.cpp : MySQL���ݿ�����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_mysql.h"
#include "Factory_if.h"
#include "string/tablestring.h"
#include "sock.h"
#include "ObjTimer_if.h"

#if DCOP_OS == DCOP_OS_WINDOWS
#pragma comment(lib, "mysqlclient")
#endif


/// -------------------------------------------------
/// ʵ���೧(����)
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CDataMySQL, "DataMySQL")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CDataMySQL)
    DCOP_IMPLEMENT_INTERFACE(IDataHandle)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ������Ϣ
/// -------------------------------------------------
const char *CDataMySQL::AddrInfo = "127.0.0.1";
const char *CDataMySQL::UserInfo = "root";
const char *CDataMySQL::ConnInfo = "Dcoplatform@31418";
const char *CDataMySQL::BaseInfo = "DcopDataBase";


/*******************************************************
  �� �� ��: CDataMySQL::CDataMySQL
  ��    ��: CDataMySQL���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDataMySQL::CDataMySQL(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);

    m_conn_ptr = NULL;
    m_piTlvToSQL = NULL;
}

/*******************************************************
  �� �� ��: CDataMySQL::~CDataMySQL
  ��    ��: CDataMySQL��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDataMySQL::~CDataMySQL()
{
    if (m_conn_ptr)
    {
        mysql_close(m_conn_ptr);
        m_conn_ptr = NULL;
    }

    DCOP_RELEASE_INSTANCE(m_piTlvToSQL);
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CDataMySQL::Init
  ��    ��: ��ʼ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::Init(DWORD dwAttrID, IObject *pOwner, IModel *piModel)
{
    /// ��Ϊ��ʵ���ǵ���������ֻ�ܳ�ʼ��һ�Σ��Ѿ���ʼ���ˣ���ֱ�ӷ��سɹ�
    if (m_conn_ptr)
    {
        return SUCCESS;
    }

    /// �ȵ��ø���ĳ�ʼ��
    DWORD dwRc = IDataHandle::Init(IData::TYPE_MYSQL, dwAttrID, pOwner, piModel, (IData *)m_piParent);
    if (dwRc)
    {
        return dwRc;
    }

    /// ����SQL���ת��ʵ��
    DCOP_CREATE_INSTANCE(ITlvToSQL, "TLV2SQL", this, 0, 0, m_piTlvToSQL);
    if(!m_piTlvToSQL)
    {
        return FAILURE;
    }

    /// ��ʼ��mysql : mysql_init()����NULLָ�룬���潫�Զ������µĶ��󣻵�����mysql_close()ʱ���ͷŸö���
    m_conn_ptr = mysql_init(NULL);
    if(!m_conn_ptr)
    {
        return FAILURE;
    }

    /// �������ݿ� : mysql_connect�Ѿ���ʱ��ʹ��mysql_real_connect()ȡ����֮��dbΪNULL������Ĭ�ϵ����ݿ�
    MYSQL *conn_ptr = mysql_real_connect(m_conn_ptr, AddrInfo, UserInfo, ConnInfo, 
                        BaseInfo, 0, NULL, 0);
    if(!conn_ptr)
    {
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    /// ��ȡ���������б�
    CDString strCreateArgList;
    dwRc = m_piTlvToSQL->GetCreateFieldList(strCreateArgList);
    if (dwRc)
    {
        return dwRc;
    }

    /// �������ݱ� : mysql_create_db�Ѿ���ʱ��ʹ��mysql_query()����SQL CREATE DATABASE��������д���
    /// CREATE TABLE IF NOT EXISTS table_name (i INT NOT NULL)
    CDString strSQL("CREATE TABLE IF NOT EXISTS ");
    strSQL << GetTableName() << " (" << strCreateArgList << ")";
    int iRc = mysql_query(m_conn_ptr, strSQL);
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMySQL::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDataMySQL::Dump(LOG_PRINT logPrint, LOG_PARA logPara)
{
    if (!logPrint)
    {
        return;
    }

    Field *pFields = GetFields();
    DWORD dwFieldCount = GetFieldCount();
    if (!pFields || !dwFieldCount)
    {
        return;
    }

    /// SELECT * FROM table_name
    CDString strSQL("SELECT * FROM ");
    strSQL << GetTableName();
    int iRc = mysql_real_query(m_conn_ptr, strSQL, strSQL.Length());
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
        return;
    }

    MYSQL_RES *res = mysql_store_result(m_conn_ptr);
    if (!res)
    {
        TRACE_LOG(mysql_error(m_conn_ptr));
        return;
    }

    DWORD dwRecCount = (DWORD)mysql_num_rows(res);
    logPrint(STR_FORMAT("MySQL Data Table: ['%s'], Records Count: %d \r\n", GetTableName(), dwRecCount), logPara);
    CTableString tableStr(dwFieldCount, dwRecCount + 1, "  ");

    for (DWORD i = 0; i < dwFieldCount; ++i)
    {
        tableStr << pFields[i].m_fieldName;
    }

    DCOP_START_TIME();

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        for (DWORD i = 0; i < dwFieldCount; ++i)
        {
            if (pFields[i].m_fieldType == IModel::FIELD_PASS)
            {
                tableStr << "********";
                continue;
            }

            if (!(row[i]))
            {
                tableStr << "";
                continue;
            }

            if (pFields[i].m_fieldType == IModel::FIELD_BUFFER)
            {
                tableStr << CBufferString((void *)(row[i]), pFields[i].m_fieldSize);
                continue;
            }

            if (pFields[i].m_fieldType == IModel::FIELD_IP)
            {
                char szIP[OSSOCK_IPSIZE];
                (void)memset(szIP, 0, sizeof(szIP));
                objSock::GetIPStringByValue((DWORD)atoi(row[i]), szIP);
                tableStr << szIP;
                continue;
            }

            if (pFields[i].m_fieldType == IModel::FIELD_PTR)
            {
                tableStr << STR_FORMAT("%p", *(void **)(row[i]));
                continue;
            }

            if (pFields[i].m_fieldType == IModel::FIELD_TIMER)
            {
                CDString strTimer;
                ITimer::IWheel::GetString((ITimer::Handle)(row[i]), strTimer);
                tableStr << (const char *)strTimer;
                continue;
            }

            tableStr << row[i];
        }
    }

    DCOP_END_TIME();

    tableStr.Show(logPrint, logPara, "=", "-");

    logPrint(STR_FORMAT("[cost time: %d ms] \r\n", DCOP_COST_TIME()), logPara);

    mysql_free_result(res);
}

/*******************************************************
  �� �� ��: CDataMySQL::AddRecord
  ��    ��: ���һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::AddRecord(DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    if (!m_conn_ptr || !m_piTlvToSQL)
    {
        return FAILURE;
    }

    CDString strArgList;
    DWORD dwRc = m_piTlvToSQL->GetInsertFieldList(pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, 
                        strArgList);
    if (dwRc)
    {
        return dwRc;
    }

    /// INSERT INTO table_name (a,b,c) VALUES (1,2,3)
    CDString strSQL("INSERT INTO ");
    strSQL << GetTableName() << " " << strArgList;

    int iRc = mysql_query(m_conn_ptr, strSQL);
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    /// �����¼�
    if (ppEvtPara && pdwEvtParaCount && pEvtData)
    {
        CDString strIdName;
        dwRc = m_piTlvToSQL->GetIdentifyName(strIdName);
        if (dwRc != SUCCESS)
        {
            return SUCCESS;
        }

        CDString strEvtSQL = "SELECT * FROM ";
        strEvtSQL << GetTableName() << " WHERE " << strIdName << "=LAST_INSERT_ID()";
        int iRc = mysql_real_query(m_conn_ptr, strEvtSQL, strEvtSQL.Length());
        if (iRc != 0)
        {
            return SUCCESS;
        }

        MYSQL_RES *res = mysql_store_result(m_conn_ptr);
        if (!res)
        {
            return SUCCESS;
        }

        /// ��ȡ�¼�����
        (void)m_piTlvToSQL->GetRspFieldList(0, 0, res, 
                        *ppEvtPara, *pdwEvtParaCount, *pEvtData);
        mysql_free_result(res);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMySQL::DelRecord
  ��    ��: ɾ��һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::DelRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    if (!m_conn_ptr || !m_piTlvToSQL)
    {
        return FAILURE;
    }

    CDString strCondArgList;
    DWORD dwRc = m_piTlvToSQL->GetCondFieldList(byCond, 
                        pCondPara, dwCondParaCount, pCondData, dwCondDataLen, 
                        strCondArgList);
    if (dwRc)
    {
        return dwRc;
    }

    /// �����¼�
    if (ppEvtPara && pdwEvtParaCount && pEvtData)
    {
        CDString strEvtSQL = "SELECT * FROM ";
        strEvtSQL << GetTableName() << " WHERE " << strCondArgList;
        int iRc = mysql_real_query(m_conn_ptr, strEvtSQL, strEvtSQL.Length());
        if (iRc != 0)
        {
            return SUCCESS;
        }

        MYSQL_RES *res = mysql_store_result(m_conn_ptr);
        if (!res)
        {
            return SUCCESS;
        }

        /// ��ȡ�¼�����
        (void)m_piTlvToSQL->GetRspFieldList(0, 0, res, 
                        *ppEvtPara, *pdwEvtParaCount, *pEvtData);
        mysql_free_result(res);
    }

    /// DELETE FROM table_name WHERE user='jcole'
    CDString strSQL("DELETE FROM ");
    strSQL << GetTableName();
    if (strCondArgList.Length()) strSQL << " WHERE " << strCondArgList;

    int iRc = mysql_query(m_conn_ptr, strSQL);
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));

        if (ppEvtPara && pdwEvtParaCount && pEvtData)
        {
            *ppEvtPara = 0;
            *pdwEvtParaCount = 0;
            pEvtData->Clear();
        }

        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMySQL::EditRecord
  ��    ��: �༭һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::EditRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    if (!m_conn_ptr || !m_piTlvToSQL)
    {
        return FAILURE;
    }

    CDString strCondArgList;
    DWORD dwRc = m_piTlvToSQL->GetCondFieldList(byCond, 
                        pCondPara, dwCondParaCount, pCondData, dwCondDataLen, 
                        strCondArgList);
    if (dwRc)
    {
        return dwRc;
    }

    CDString strReqArgList;
    dwRc = m_piTlvToSQL->GetReqFieldList(pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, strReqArgList);
    if (dwRc)
    {
        return dwRc;
    }

    /// UPDATE table_name SET age=age*2 WHERE id=1
    CDString strSQL("UPDATE ");
    strSQL << GetTableName() << " SET " << strReqArgList;
    if (strCondArgList.Length()) strSQL << " WHERE " << strCondArgList;

    int iRc = mysql_query(m_conn_ptr, strSQL);
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    /// �����¼�
    if (ppEvtPara && pdwEvtParaCount && pEvtData)
    {
        DWORD dwEvtParaCount = 0;
        DWORD dwEvtDataLen = 0;
        DCOP_PARA_NODE *pEvtPara = GetOutPara(GetFields(), GetFieldCount(), dwEvtParaCount, 
                        pReqPara, dwReqParaCount, &dwEvtDataLen, true);
        if (!pEvtPara)
        {
            return SUCCESS;
        }

        CDString strEvtArgNameList;
        dwRc = m_piTlvToSQL->GetFieldNameList(pEvtPara, dwEvtParaCount, strEvtArgNameList);
        if (dwRc)
        {
            DCOP_Free(pEvtPara);
            return SUCCESS;
        }

        CDString strEvtSQL("SELECT ");
        strEvtSQL << strEvtArgNameList << " FROM " << GetTableName();
        if (strCondArgList.Length()) strEvtSQL << " WHERE " << strCondArgList;
        int iRc = mysql_real_query(m_conn_ptr, strEvtSQL, strEvtSQL.Length());
        if (iRc != 0)
        {
            DCOP_Free(pEvtPara);
            return SUCCESS;
        }

        MYSQL_RES *res = mysql_store_result(m_conn_ptr);
        if (!res)
        {
            DCOP_Free(pEvtPara);
            return SUCCESS;
        }

        /// ��ȡ�¼�����
        (void)m_piTlvToSQL->GetRspFieldList(pEvtPara, dwEvtParaCount, res, 
                        *ppEvtPara, *pdwEvtParaCount, *pEvtData);
        DCOP_Free(pEvtPara);
        mysql_free_result(res);
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMySQL::QueryRecord
  ��    ��: ��ѯ������¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::QueryRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        DCOP_PARA_NODE *&rpRspPara, DWORD &rdwRspParaCount, 
                        CDArray &aRspData)
{
    if (!m_conn_ptr || !m_piTlvToSQL)
    {
        return FAILURE;
    }

    /// ��ȡ����(������Ҳ�᷵�سɹ�,����ʧ�ܾ���Ҫ�˳�)
    CDString strCondArgList;
    DWORD dwRc = m_piTlvToSQL->GetCondFieldList(byCond, 
                        pCondPara, dwCondParaCount, pCondData, dwCondDataLen, 
                        strCondArgList);
    if (dwRc)
    {
        return dwRc;
    }

    /// ��ȡ�ֶ������б�
    CDString strArgNameList;
    CDString strJoinTable;
    dwRc = m_piTlvToSQL->GetFieldNameList(pReqPara, dwReqParaCount, strArgNameList, ", ", &strJoinTable);
    if (dwRc)
    {
        return dwRc;
    }

    /// SELECT columna, columnb FROM table_name WHERE a='test'
    CDString strSQL("SELECT ");
    strSQL << strArgNameList << " FROM " << GetTableName();
    if (strJoinTable.Length()) strSQL << strJoinTable;
    if (strCondArgList.Length()) strSQL << " WHERE " << strCondArgList;

    /// ��ȡ��¼��
    DWORD dwReqOffset = 0;
    DWORD dwReqLimit = 0;
    GetOffsetAndLimit(dwReqOffset, dwReqLimit, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen);
    if (dwReqOffset || dwReqLimit)
    {
        if (!dwReqLimit) dwReqLimit = 0xffffffff;
        strSQL << STR_FORMAT(" LIMIT %lu,%lu", dwReqOffset, dwReqLimit);
    }

    /// ִ�в�ѯ(��Ϊ���ܰ��������ƣ�����ʹ��'mysql_real_query')
    int iRc = mysql_real_query(m_conn_ptr, strSQL, strSQL.Length());
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    /// ����mysql_store_result��������Mysql��������ѯ���������ݶ��洢���ͻ��ˣ�Ȼ���ȡ;
    /// ����mysql_use_result��ʼ���������Ա��ں���һ��һ�еĶ�ȡ���������������û�д�
    /// ��������ȡ�κ����ݣ����ַ�ʽ��֮��һ���ٶȸ����������ڴ���٣�������󶨷�������
    /// ��ֹ�����̸߳����κα����ұ����ظ�ִ��mysql_fetch_row��ȡ���ݣ�ֱ������NULL����
    /// ��δ��ȡ���л�����һ�β�ѯʱ��Ϊ�����һ���ַ��أ��ʾ�������ʹ��mysql_store_result.
    MYSQL_RES *res = mysql_store_result(m_conn_ptr);
    if (!res)
    {
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    /// ��ȡ��Ӧ����
    dwRc = m_piTlvToSQL->GetRspFieldList(pReqPara, dwReqParaCount, 
                        res, rpRspPara, rdwRspParaCount, aRspData);
    mysql_free_result(res);

    return dwRc;
}

/*******************************************************
  �� �� ��: CDataMySQL::CountRecord
  ��    ��: ͳ�Ƽ�¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::CountRecord(BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DWORD &rdwCount)
{
    if (!m_conn_ptr || !m_piTlvToSQL)
    {
        return FAILURE;
    }

    CDString strCondArgList;
    DWORD dwRc = m_piTlvToSQL->GetCondFieldList(byCond, 
                        pCondPara, dwCondParaCount, pCondData, dwCondDataLen, 
                        strCondArgList);
    if (dwRc)
    {
        return dwRc;
    }

    /// SELECT count(*) FROM table_name WHERE a='test'
    CDString strSQL("SELECT count(*) FROM ");
    strSQL << GetTableName();
    if (strCondArgList.Length()) strSQL << " WHERE " << strCondArgList;

    int iRc = mysql_real_query(m_conn_ptr, strSQL, strSQL.Length());
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    /// ��ȡ���
    MYSQL_RES *res = mysql_store_result(m_conn_ptr);
    if (!res)
    {
        TRACE_LOG(mysql_error(m_conn_ptr));
        return FAILURE;
    }

    /// ��ȡ����
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row)
    {
        TRACE_LOG(mysql_error(m_conn_ptr));
        mysql_free_result(res);
        return FAILURE;
    }

    rdwCount = (DWORD)atoi(row[0]);

    mysql_free_result(res);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMySQL::AddKeyIdx
  ��    ��: ��ӹؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::AddKeyIdx(DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    if (!m_conn_ptr || !m_piTlvToSQL)
    {
        return FAILURE;
    }

    /// ��ȡ������(�ɸ����ֶ������)
    CDString strIdxName("idx_");
    DWORD dwRc = m_piTlvToSQL->GetFieldNameList(pIdxPara, dwIdxParaCount, strIdxName, "_");
    if (dwRc)
    {
        return dwRc;
    }

    /// ���������ܳ�����󳤶ȣ������Ƴ���������
    if (strIdxName.Length() > ColNameMaxLen)
    {
        strIdxName.Remove(ColNameMaxLen);
    }

    /// ��ȡ�ֶ������б�
    CDString strArgNameList;
    dwRc = m_piTlvToSQL->GetFieldNameList(pIdxPara, dwIdxParaCount, strArgNameList);
    if (dwRc)
    {
        return dwRc;
    }

    /// CREATE UNIQUE INDEX index_name ON table_name (column_list)
    CDString strSQL("CREATE UNIQUE INDEX ");
    strSQL << strIdxName << " ON " << GetTableName() << "(" << strArgNameList << ")";

    int iRc = mysql_query(m_conn_ptr, strSQL);
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMySQL::DelKeyIdx
  ��    ��: ɾ���ؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::DelKeyIdx(DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    if (!m_conn_ptr || !m_piTlvToSQL)
    {
        return FAILURE;
    }

    /// ��ȡ������(�ɸ����ֶ������)
    CDString strIdxName("idx_");
    DWORD dwRc = m_piTlvToSQL->GetFieldNameList(pIdxPara, dwIdxParaCount, strIdxName, "_");
    if (dwRc)
    {
        return dwRc;
    }

    /// ���������ܳ�����󳤶ȣ������Ƴ���������
    if (strIdxName.Length() > ColNameMaxLen)
    {
        strIdxName.Remove(ColNameMaxLen);
    }

    /// DROP INDEX index_name ON table_name
    CDString strSQL("DROP INDEX ");
    strSQL << strIdxName << " ON " << GetTableName();

    int iRc = mysql_query(m_conn_ptr, strSQL);
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDataMySQL::GetRecCount
  ��    ��: ��ȡ��¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDataMySQL::GetRecCount()
{
    if (!m_conn_ptr)
    {
        return 0;
    }

    /// SELECT count(*) FROM table_name
    CDString strSQL("SELECT count(*) FROM ");
    strSQL << GetTableName();
    int iRc = mysql_real_query(m_conn_ptr, strSQL, strSQL.Length());
    if (iRc != 0)
    {
        TRACE_LOG(strSQL);
        TRACE_LOG(mysql_error(m_conn_ptr));
        return 0;
    }

    /// ��ȡ���
    MYSQL_RES *res = mysql_store_result(m_conn_ptr);
    if (!res)
    {
        TRACE_LOG(mysql_error(m_conn_ptr));
        return 0;
    }

    /// ��ȡ����
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row)
    {
        TRACE_LOG(mysql_error(m_conn_ptr));
        mysql_free_result(res);
        return 0;
    }

    DWORD dwCount = (DWORD)atoi(row[0]);

    mysql_free_result(res);
    return dwCount;
}

