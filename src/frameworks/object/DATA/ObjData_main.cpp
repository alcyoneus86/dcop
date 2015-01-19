/// -------------------------------------------------
/// ObjData_main.cpp : ��������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjData_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "list/dlistvector.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CData, "data")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CData)
    DCOP_IMPLEMENT_INTERFACE(IData)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CData)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
    DCOP_IMPLEMENT_CONFIG_STRING("datafiledir", m_szDataFileDir)
DCOP_IMPLEMENT_IOBJECT_END


/*******************************************************
  �� �� ��: CData::CData
  ��    ��: CData���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CData::CData(Instance *piParent, int argc, char **argv)
{
    (void)memset(m_szDataFileDir, 0, sizeof(m_szDataFileDir));
    m_piModel = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CData::~CData
  ��    ��: CData��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CData::~CData()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CData::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IModel,       DCOP_OBJECT_MODEL,      m_piModel)
    DCOP_QUERY_OBJECT_END

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CData::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CData::Fini()
{
    /// �ͷ������е����ݾ��
    Enter();
    for (IT_DATA it_data = m_datas.begin(); 
        it_data != m_datas.end(); 
        ++it_data)
    {
        DCOP_RELEASE_INSTANCE((*it_data).second);
    }
    Leave();

    DCOP_RELEASE_INSTANCE(m_piModel);
}

/*******************************************************
  �� �� ��: CData::Dump
  ��    ��: Dump���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CData::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    AutoObjLock(this);

    if (!argc)
    {
        for (IT_DATA it_data = m_datas.begin();
            it_data != m_datas.end(); ++it_data)
        {
            ((*it_data).second)->Dump(logPrint, logPara);
        }

        return;
    }

    for (int i = 0; i < argc; ++i)
    {
        if (!(argv[i])) continue;

        IT_DATA it_data = m_datas.find(*(DWORD *)(argv[i]));
        if (it_data == m_datas.end()) continue;

        ((*it_data).second)->Dump(logPrint, logPara);
    }
}

/*******************************************************
  �� �� ��: CData::Create
  ��    ��: ����һ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::Create(DWORD dwType, DWORD dwAttrID, IObject *pOwner, Handle &hData)
{
    AutoObjLock(this);

    /// �����Ƿ����ظ�������
    IT_DATA it_data = m_datas.find(dwAttrID);
    if (it_data != m_datas.end())
    {
        return FAILURE;
    }

    /// �����µľ��
    IDataHandle *pDataHandle = 0;
    switch (dwType)
    {
        case TYPE_MEM:
            DCOP_CREATE_INSTANCE(IDataHandle, "DataMem", this, 0, 0, pDataHandle);
            break;
        case TYPE_FILE:
        {
            char *argDataFile[] = {m_szDataFileDir};
            DCOP_CREATE_INSTANCE(IDataHandle, "DataFile", this, ARRAY_SIZE(argDataFile), argDataFile, pDataHandle);
        }
            break;
        case TYPE_MYSQL:
            DCOP_CREATE_INSTANCE(IDataHandle, "DataMySQL", this, 0, 0, pDataHandle);
            break;
        default:
            break;
    }
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ��ʼ�����
    DWORD dwRc = pDataHandle->Init(dwAttrID, pOwner, m_piModel);
    if (dwRc)
    {
        DCOP_RELEASE_INSTANCE(pDataHandle);
        return dwRc;
    }

    /// ��ӵ�������
    it_data = m_datas.insert(m_datas.end(), MAP_DATA::value_type(dwAttrID, pDataHandle));
    if (it_data == m_datas.end())
    {
        DCOP_RELEASE_INSTANCE(pDataHandle);
        return FAILURE;
    }

    /// ������ݾ��
    hData = (Handle)((*it_data).second);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CData::Destroy
  ��    ��: ����һ�����ݾ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::Destroy(DWORD dwAttrID)
{
    AutoObjLock(this);

    IT_DATA it_data = m_datas.find(dwAttrID);
    if (it_data == m_datas.end())
    {
        return FAILURE;
    }

    IDataHandle *pHandle = ((*it_data).second);
    if (pHandle)
    {
        const char *pcszTableName = pHandle->GetTableName();
        CStrFormat strLog("-> [DATA] Release '%s' Handle Ref Count", ((pcszTableName)? pcszTableName : "none"));
        DWORD dwRefCount = pHandle->Release();
        TRACE_LOG(STR_FORMAT("%s:%d", (const char *)strLog, dwRefCount));
    }

    (void)m_datas.erase(it_data);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CData::AddRecord
  ��    ��: ���һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::AddRecord(Handle hData, 
                        DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, 
                        DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    IDataHandle *pDataHandle = dynamic_cast<IDataHandle *>((Instance *)hData);
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ���ﲻʹ��data���������������Ϊ�����ʹ��owner�����������

    return pDataHandle->AddRecord(pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, 
                        ppEvtPara, pdwEvtParaCount, 
                        pEvtData);
}

/*******************************************************
  �� �� ��: CData::DelRecord
  ��    ��: ɾ��һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::DelRecord(Handle hData, BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, 
                        DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    IDataHandle *pDataHandle = dynamic_cast<IDataHandle *>((Instance *)hData);
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ���ﲻʹ��data���������������Ϊ�����ʹ��owner�����������

    return pDataHandle->DelRecord(byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        ppEvtPara, pdwEvtParaCount, 
                        pEvtData);
}

/*******************************************************
  �� �� ��: CData::EditRecord
  ��    ��: �༭һ����¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::EditRecord(Handle hData, BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        void *pReqData, DWORD dwReqDataLen, 
                        DCOP_PARA_NODE **ppEvtPara, 
                        DWORD *pdwEvtParaCount, 
                        CDArray *pEvtData)
{
    IDataHandle *pDataHandle = dynamic_cast<IDataHandle *>((Instance *)hData);
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ���ﲻʹ��data���������������Ϊ�����ʹ��owner�����������

    return pDataHandle->EditRecord(byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        pReqPara, dwReqParaCount, 
                        pReqData, dwReqDataLen, 
                        ppEvtPara, pdwEvtParaCount, 
                        pEvtData);
}

/*******************************************************
  �� �� ��: CData::QueryRecord
  ��    ��: ��ѯ������¼
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::QueryRecord(Handle hData, BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DCOP_PARA_NODE *pReqPara, DWORD dwReqParaCount, 
                        DCOP_PARA_NODE *&rpRspPara, DWORD &rdwRspParaCount, 
                        CDArray &aRspData)
{
    IDataHandle *pDataHandle = dynamic_cast<IDataHandle *>((Instance *)hData);
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ���ﲻʹ��data���������������Ϊ�����ʹ��owner�����������

    return pDataHandle->QueryRecord(byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        pReqPara, dwReqParaCount, 
                        rpRspPara, rdwRspParaCount, 
                        aRspData);
}

/*******************************************************
  �� �� ��: CData::CountRecord
  ��    ��: ͳ�Ƽ�¼����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::CountRecord(Handle hData, BYTE byCond, 
                        DCOP_PARA_NODE *pCondPara, DWORD dwCondParaCount, 
                        void *pCondData, DWORD dwCondDataLen, 
                        DWORD &rdwCount)
{
    IDataHandle *pDataHandle = dynamic_cast<IDataHandle *>((Instance *)hData);
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ���ﲻʹ��data���������������Ϊ�����ʹ��owner�����������

    return pDataHandle->CountRecord(byCond, 
                        pCondPara, dwCondParaCount, 
                        pCondData, dwCondDataLen, 
                        rdwCount);
}

/*******************************************************
  �� �� ��: CData::AddKeyIdx
  ��    ��: ��ӹؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::AddKeyIdx(Handle hData, DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    IDataHandle *pDataHandle = dynamic_cast<IDataHandle *>((Instance *)hData);
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ���ﲻʹ��data���������������Ϊ�����ʹ��owner�����������

    return pDataHandle->AddKeyIdx(pIdxPara, dwIdxParaCount);
}

/*******************************************************
  �� �� ��: CData::DelKeyIdx
  ��    ��: ɾ���ؼ�������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CData::DelKeyIdx(Handle hData, DCOP_PARA_NODE *pIdxPara, DWORD dwIdxParaCount)
{
    IDataHandle *pDataHandle = dynamic_cast<IDataHandle *>((Instance *)hData);
    if (!pDataHandle)
    {
        return FAILURE;
    }

    /// ���ﲻʹ��data���������������Ϊ�����ʹ��owner�����������

    return pDataHandle->DelKeyIdx(pIdxPara, dwIdxParaCount);
}

/*******************************************************
  �� �� ��: CData::GetHandle
  ��    ��: ��ȡ����ֵ��Ӧ�����ݾ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IData::Handle CData::GetHandle(DWORD dwAttrID)
{
    AutoObjLock(this);

    IT_DATA it_data = m_datas.find(dwAttrID);
    if (it_data == m_datas.end())
    {
        return NULL;
    }

    return (Handle)((*it_data).second);
}

