/// -------------------------------------------------
/// ObjDispatch_main.cpp : ��Ϣ�ַ�������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjDispatch_main.h"
#include "Factory_if.h"
#include "Manager_if.h"
#include "BaseID.h"
#include "sock.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CDispatch, "dispatch")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CDispatch)
    DCOP_IMPLEMENT_INTERFACE(IDispatch)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CDispatch)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ��������
/// -------------------------------------------------
IMPLEMENT_CONFIG_ITEM(CDispatch, hookFlag, "k", "hookflag", "Msg Hook Flag", CArgCfgType::TYPE_VALUE, false)
IMPLEMENT_CONFIG_ITEM(CDispatch, lenMax, "l", "lenmax", "Output Len Max", CArgCfgType::TYPE_VALUE, false)
IMPLEMENT_CONFIG_ITEM(CDispatch, srcID, "s", "srcid", "Msg Src ID", CArgCfgType::TYPE_VALUE, false)
IMPLEMENT_CONFIG_ITEM(CDispatch, dstID, "d", "dstid", "Msg Dst ID", CArgCfgType::TYPE_VALUE, false)
IMPLEMENT_CONFIG_ITEM(CDispatch, logParaLen, "lpl", "logparalen", "Log Para Len", CArgCfgType::TYPE_VALUE, false)


/*******************************************************
  �� �� ��: CDispatch::CDispatch
  ��    ��: CDispatch���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDispatch::CDispatch(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);

    INIT_CONFIG_START(m_cfgTable)
        INIT_CONFIG_ITEM_VAL(hookFlag, DCOP_CLOSE)
        INIT_CONFIG_ITEM_VAL(lenMax, 0)
        INIT_CONFIG_ITEM_VAL(srcID, 0)
        INIT_CONFIG_ITEM_VAL(dstID, 0)
        INIT_CONFIG_ITEM_VAL(logParaLen, 0)
    INIT_CONFIG_END

    m_logPrint = 0;
    m_logPara = 0;
}

/*******************************************************
  �� �� ��: CDispatch::~CDispatch
  ��    ��: CDispatch��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CDispatch::~CDispatch()
{
    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();

    if (m_logPara)
    {
        DCOP_Free(m_logPara);
        m_logPara = 0;
    }

    m_logParaLen = 0;
}

/*******************************************************
  �� �� ��: CDispatch::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    ISchedule *piSchedule = 0;
    DCOP_QUERY_OBJECT(ISchedule, DCOP_OBJECT_SCHEDULE, root, piSchedule);
    if (!piSchedule)
    {
        return FAILURE;
    }

    m_pInfLayer = piSchedule;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CDispatch::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDispatch::Fini()
{
}

/*******************************************************
  �� �� ��: CDispatch::Dump
  ��    ��: DUMP���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDispatch::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    AutoObjLock(this);

    if (m_logPara)
    {
        DCOP_Free(m_logPara);
        m_logPara = 0;
    }

    m_logParaLen = 0;

    m_cfgTable.Cfg(argc, (char **)argv);
    m_logPrint = logPrint;
    if (!m_logPrint)
    {
        return;
    }

    if (!m_logParaLen)
    {
        return;
    }

    m_logPara = (LOG_PARA)DCOP_Malloc(m_logParaLen);
    if (!m_logPara)
    {
        return;
    }

    (void)memcpy(m_logPara, logPara, m_logParaLen);

    m_logPrint(STR_FORMAT("  Hook Flag: %d;\r\n  Len Max: %d;\r\n  Src ID: %d;\r\n  Dst ID: %d!\r\n", 
                        (DWORD)m_hookFlag, (DWORD)m_lenMax, (DWORD)m_srcID, (DWORD)m_dstID), logPara);
}

/*******************************************************
  �� �� ��: CDispatch::GetMTU
  ��    ��: ��ȡMTU
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::GetMTU()
{
    /// ͨ����MTU��������������Ϣͷ��������Ӧ��ȥͷ����С
    return OSSOCK_DEFAULT_MTU - OSMsgHeader::GetHeaderSize();
}

/*******************************************************
  �� �� ��: CDispatch::Send
  ��    ��: ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::Send(objMsg *message)
{
    AutoObjLock(this);

    ISchedule *pSchedule = dynamic_cast<ISchedule *>((IObject *)m_pInfLayer);
    if (!pSchedule || !message)
    {
        return FAILURE;
    }

    if (m_hookFlag && m_logPrint)
    {
        Hook(message);
    }

    return pSchedule->Join(message);
}

/*******************************************************
  �� �� ��: CDispatch::SendAndWait
  ��    ��: ���ͺ�ȴ���Ӧ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::SendAndWait(objMsg *request, objMsg **response, DWORD waittime)
{
    AutoObjLock(this);

    return 0;
}

/*******************************************************
  �� �� ��: CDispatch::Hook
  ��    ��: ��Ϣ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CDispatch::Hook(objMsg *message)
{
    if (!message || !m_hookFlag || !m_logPrint)
    {
        return;
    }

    DWORD dwSrcID = message->GetSrcID();
    DWORD dwDstID = message->GetDstID();

    if (m_srcID && (dwSrcID != m_srcID))
    {
        return;
    }

    if (m_dstID && (dwDstID != m_dstID))
    {
        return;
    }

    const char *pcszSrcName = "Null";
    IObject *piSrcObj = 0;
    DCOP_QUERY_OBJECT(IObject, dwSrcID, Parent(), piSrcObj);
    if (piSrcObj) pcszSrcName = piSrcObj->Name();

    const char *pcszDstName = "Null";
    IObject *piDstObj = 0;
    DCOP_QUERY_OBJECT(IObject, dwDstID, Parent(), piDstObj);
    if (piDstObj) pcszDstName = piDstObj->Name();

    DWORD dwDispLen = message->GetDataLen();
    if (m_lenMax && (dwDispLen > m_lenMax)) dwDispLen = m_lenMax;

    PrintBuffer(STR_FORMAT("[MSG HOOK] '%s'(%d) -> '%s'(%d) Len:%d", 
                    pcszSrcName, dwSrcID, pcszDstName, dwDstID, message->GetDataLen()), 
                    message->GetDataBuf(), dwDispLen, m_logPrint, m_logPara);
}

/*******************************************************
  �� �� ��: CDispatch::StackRecvMsg
  ��    ��: �ֲ�ʽЭ��ջ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CDispatch::StackRecvMsg(
                    DWORD dwSrcNodeID, 
                    void *pMsgBuf, 
                    DWORD dwBufLength, 
                    void *pUserArg
                    )
{
    return 0;
}

