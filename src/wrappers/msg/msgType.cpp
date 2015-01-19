/// -------------------------------------------------
/// msgType.cpp : msgQ��װ��ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "msgType.h"
#include "cpu/bytes.h"


/*******************************************************
  �� �� ��: objMsg::CreateInstance
  ��    ��: ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsg *objMsg::CreateInstance(DWORD dwDataLen, DWORD dwMsgType, DWORD dwLocalID, 
                        const char *file, int line, 
                        const void *cpData, DWORD dwCopyLen)
{
    void *pBuffer = DCOP_MallocEx(sizeof(CMsgBase) + dwDataLen, file, line);
    if (!pBuffer)
    {
        return 0;
    }

    #undef new
    CMsgBase *pMsg = new (pBuffer) CMsgBase(dwDataLen, dwMsgType, dwLocalID, cpData, dwCopyLen);
    #define new new(__FILE__, __LINE__)

    return pMsg;
}

/*******************************************************
  �� �� ��: objMsg::Parse
  ��    ��: ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsg *objMsg::Parse(void *pFrameBuf, DWORD dwFrameLen, 
                        const char *file, int line)
{
    if (!pFrameBuf || (dwFrameLen < sizeof(OSMsgHeader)))
    {
        return 0;
    }

    void *pBuffer = DCOP_MallocEx(sizeof(CMsgBase) + dwFrameLen - sizeof(OSMsgHeader), file, line);
    if (!pBuffer)
    {
        return 0;
    }

    #undef new
    CMsgBase *pMsg = new (pBuffer) CMsgBase(pFrameBuf, dwFrameLen);
    #define new new(__FILE__, __LINE__)

    return pMsg;
}

/*******************************************************
  �� �� ��: objMsg::~objMsg
  ��    ��: ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsg::~objMsg()
{
}

/*******************************************************
  �� �� ��: CMsgBase::CMsgBase
  ��    ��: CMsgBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMsgBase::CMsgBase(DWORD dwDataLen, DWORD dwMsgType, DWORD dwLocalID, const void *cpData, DWORD dwCopyLen)
{
    m_setCtrlBuf = NULL;
    m_setCtrlLen = 0;
    TAILQ_INIT(&m_addDataLst);
    m_addDataLen = 0;
    (void)OSMsgHeader::Format(&m_header,
                        dwDataLen + sizeof(OSMsgHeader),
                        dwMsgType,
                        dwLocalID,
                        cpData,
                        dwCopyLen);
}

/*******************************************************
  �� �� ��: CMsgBase::CMsgBase
  ��    ��: CMsgBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMsgBase::CMsgBase(void *pFrameBuf, DWORD dwFrameLen)
{
    m_setCtrlBuf = NULL;
    m_setCtrlLen = 0;
    TAILQ_INIT(&m_addDataLst);
    m_addDataLen = 0;
    if (!pFrameBuf || (dwFrameLen < sizeof(OSMsgHeader)))
    {
        (void)OSMsgHeader::Format(&m_header,
                        sizeof(OSMsgHeader),
                        0,
                        0);
    }
    else
    {
        (void)memcpy(&m_header, pFrameBuf, dwFrameLen);
        if ((DWORD)m_header.m_byOffset > dwFrameLen)
        {
            /// ֡������ͷ��ƫ�ƣ�ֻ�а�ͷ������Ϊ����֡����
            m_header.m_byOffset = (BYTE)dwFrameLen;
        }
        /// ����ȷ��: ����+ͷ��=֡��
        m_header.m_dwDataLen = dwFrameLen - m_header.m_byOffset;
    }
}

/*******************************************************
  �� �� ��: CMsgBase::~CMsgBase
  ��    ��: CMsgBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMsgBase::~CMsgBase()
{
    if (m_setCtrlBuf)
    {
        DCOP_Free(m_setCtrlBuf);
        m_setCtrlBuf = NULL;
    }
    m_setCtrlLen = 0;
    TAILQ_CLEAR(&m_addDataLst, AddDataNode, m_field, DCOP_Free);
}

/*******************************************************
  �� �� ��: CMsgBase::GetOffset
  ��    ��: ��ȡͷ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgBase::GetOffset()
{
    return m_header.m_byOffset;
}

/*******************************************************
  �� �� ��: CMsgBase::GetDataLen
  ��    ��: ��ȡ���ݳ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgBase::GetDataLen()
{
    return m_header.m_dwDataLen;
}

/*******************************************************
  �� �� ��: CMsgBase::GetMsgType
  ��    ��: ��ȡ��Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgBase::GetMsgType()
{
    return m_header.m_dwMsgType;
}

/*******************************************************
  �� �� ��: CMsgBase::GetSrcID
  ��    ��: ��ȡ��ϢԴID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgBase::GetSrcID()
{
    return m_header.m_dwSrcID;
}

/*******************************************************
  �� �� ��: CMsgBase::GetDstID
  ��    ��: ��ȡ��ϢĿ��ID
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgBase::GetDstID()
{
    return m_header.m_dwDstID;
}

/*******************************************************
  �� �� ��: CMsgBase::Clone
  ��    ��: ��¡��Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsg *CMsgBase::Clone(const char *file, int line)
{
    DWORD dwMsgLen = 0;
    OSMsgHeader *pMsgBuf = (OSMsgHeader *)Pack(dwMsgLen, file, line);
    if (!pMsgBuf)
    {
        return NULL;
    }

    objMsg *pMsg = DCOP_ParseMsg(pMsgBuf, dwMsgLen);
    DCOP_Free(pMsgBuf);

    return pMsg;
}

/*******************************************************
  �� �� ��: CMsgBase::Pack
  ��    ��: ������Ϣ��һ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CMsgBase::Pack(DWORD &rdwLen, const char *file, int line)
{
    BYTE byOffset = (BYTE)((m_setCtrlLen)? (sizeof(OSMsgHeader) + m_setCtrlLen) : m_header.m_byOffset);
    OSMsgHeader *pMsgBuf = (OSMsgHeader *)DCOP_MallocEx(byOffset + 
                        m_header.m_dwDataLen + m_addDataLen,
                        file, line);
    if (!pMsgBuf)
    {
        rdwLen = 0;
        return 0;
    }

    /// ������Ϣͷ
    if (m_setCtrlLen)
    {
        (void)memcpy(pMsgBuf, &m_header, sizeof(OSMsgHeader));
        (void)memcpy(pMsgBuf + sizeof(OSMsgHeader), &m_setCtrlBuf, m_setCtrlLen);
    }
    else
    {
        (void)memcpy(pMsgBuf, &m_header, m_header.m_byOffset);
    }

    pMsgBuf->m_byOffset = byOffset;

    /// ����������
    if (m_header.m_dwDataLen)
    {
        (void)memcpy((BYTE *)pMsgBuf + pMsgBuf->m_byOffset, 
                        (BYTE *)&m_header + m_header.m_byOffset, m_header.m_dwDataLen);
        pMsgBuf->m_dwDataLen = m_header.m_dwDataLen;
    }
    else
    {
        pMsgBuf->m_dwDataLen = 0;
    }

    AddDataNode *pTmpData = TAILQ_FIRST(&m_addDataLst);
    DWORD dwTmpDataLen = 0;
    while (pTmpData)
    {
        if ((dwTmpDataLen + pTmpData->m_dwDataLen) >= m_addDataLen)
        {
            break;
        }

        (void)memcpy((BYTE *)pMsgBuf + pMsgBuf->m_byOffset + pMsgBuf->m_dwDataLen, 
                        pTmpData + 1, pTmpData->m_dwDataLen);
        pMsgBuf->m_dwDataLen += pTmpData->m_dwDataLen;
        dwTmpDataLen += pTmpData->m_dwDataLen;

        pTmpData = TAILQ_NEXT(pTmpData, m_field);
    }

    rdwLen = pMsgBuf->m_byOffset + pMsgBuf->m_dwDataLen;
    return pMsgBuf;
}

/*******************************************************
  �� �� ��: CMsgBase::MsgHead
  ��    ��: ��ȡ��Ϣͷ(����)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
OSMsgHeader &CMsgBase::MsgHead()
{
    return m_header;
}

/*******************************************************
  �� �� ��: CMsgBase::GetDataBuf
  ��    ��: ��ȡ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CMsgBase::GetDataBuf()
{
    if (!m_header.m_dwDataLen)
    {
        return NULL;
    }

    return (BYTE *)&m_header + m_header.m_byOffset;
}

/*******************************************************
  �� �� ��: CMsgBase::SetCtrl
  ��    ��: ���ÿ�����Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CMsgBase::SetCtrl(void *pBuf, DWORD dwLen)
{
    if (!pBuf || !dwLen)
    {
        return NULL;
    }

    if ((dwLen + sizeof(OSMsgHeader)) > 255)
    {
        return NULL;
    }

    void *pNewCtrl = DCOP_Malloc(dwLen);
    if (!pNewCtrl)
    {
        return NULL;
    }

    (void)memcpy(pNewCtrl, pBuf, dwLen);

    if (m_setCtrlBuf)
    {
        DCOP_Free(m_setCtrlBuf);
    }

    m_setCtrlBuf = pNewCtrl;
    m_setCtrlLen = dwLen;

    return m_setCtrlBuf;
}

/*******************************************************
  �� �� ��: CMsgBase::GetCtrl
  ��    ��: ��ȡ������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CMsgBase::GetCtrl(DWORD &rdwLen)
{
    /// �ȳ��Դ����õĿ�����Ϣ�л�ȡ
    if (m_setCtrlBuf && m_setCtrlLen)
    {
        rdwLen = m_setCtrlLen;
        return m_setCtrlBuf;
    }

    /// �ٳ��Դ���Ϣͷ���л�ȡ
    if (m_header.m_byOffset > OSMsgHeader::GetHeaderSize())
    {
        rdwLen = m_header.m_byOffset - OSMsgHeader::GetHeaderSize();
        return &m_header + 1;
    }

    rdwLen = 0;
    return NULL;
}

/*******************************************************
  �� �� ��: CMsgBase::AddData
  ��    ��: ���������Ϣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void *CMsgBase::AddData(void *pBuf, DWORD dwLen)
{
    if (!pBuf || !dwLen)
    {
        return NULL;
    }

    AddDataNode *pNewData = (AddDataNode *)DCOP_Malloc(sizeof(AddDataNode) + dwLen);
    if (!pNewData)
    {
        return NULL;
    }

    (void)memcpy(pNewData + 1, pBuf, dwLen);
    pNewData->m_dwDataLen = dwLen;
    TAILQ_NEXT(pNewData, m_field) = NULL;

    TAILQ_PUSH_BACK(&m_addDataLst, pNewData, m_field);
    m_addDataLen += dwLen;

    return pNewData + 1;
}

/*******************************************************
  �� �� ��: OSMsgHeader::OSMsgHeader
  ��    ��: ��Ϣͷ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
OSMsgHeader::OSMsgHeader()
{
    (void)Format(this, sizeof(OSMsgHeader));
}

/*******************************************************
  �� �� ��: OSMsgHeader::~OSMsgHeader
  ��    ��: ��Ϣͷ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
OSMsgHeader::~OSMsgHeader()
{
}

/*******************************************************
  �� �� ��: OSMsgHeader::bFrame
  ��    ��: �ж��Ƿ�������֡
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int OSMsgHeader::bFrame(void *pBuf, DWORD dwLen)
{
    if (!pBuf || !dwLen)
    {
        return -1;
    }

    if (dwLen < 8)
    {
        return -1;
    }

    BYTE byMagicWord0 = *((BYTE *)pBuf + 0);
    BYTE byMagicWord1 = *((BYTE *)pBuf + 1);
    BYTE byMagicWord2 = *((BYTE *)pBuf + 2);
    BYTE byOffset = *((BYTE *)pBuf + 3);

    if ((byMagicWord0 != OSBUF_MAGIC0) ||
        (byMagicWord1 != OSBUF_MAGIC1) ||
        (byMagicWord2 != OSBUF_MAGIC2) ||
        (byOffset < sizeof(OSMsgHeader)))
    {
        return 0;
    }

    DWORD dwDataLen = Bytes_GetDword((BYTE *)pBuf + 4);
    return (byOffset + dwDataLen);
}

/*******************************************************
  �� �� ��: OSMsgHeader::Format
  ��    ��: ��ʽ������֡
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
OSMsgHeader *OSMsgHeader::Format(void *pBuf,
                        DWORD dwBufLen,
                        DWORD dwMsgType,
                        DWORD dwLocalID,
                        const void *cpData,
                        DWORD dwDataLen)
{
    if (!pBuf || (dwBufLen < sizeof(OSMsgHeader)))
    {
        return 0;
    }

    OSMsgHeader *pMsgHeader = (OSMsgHeader *)pBuf;
    pMsgHeader->m_byMagicWord0  = OSBUF_MAGIC0;
    pMsgHeader->m_byMagicWord1  = OSBUF_MAGIC1;
    pMsgHeader->m_byMagicWord2  = OSBUF_MAGIC2;
    pMsgHeader->m_byOffset      = sizeof(OSMsgHeader);
    pMsgHeader->m_dwDataLen     = dwBufLen - sizeof(OSMsgHeader);
    pMsgHeader->m_dwMsgType     = dwMsgType;
    pMsgHeader->m_dwSrcID       = dwLocalID;
    pMsgHeader->m_dwDstID       = 0;

    DWORD dwMsgDataLen = pMsgHeader->m_dwDataLen;
    if (dwMsgDataLen)
    {
        DWORD dwCopyLen = (cpData)? MIN(dwMsgDataLen, dwDataLen) : 0;
        DWORD dwLeftLen = dwMsgDataLen - dwCopyLen;
        if (dwCopyLen)
        {
            (void)memcpy(pMsgHeader + 1, cpData, dwCopyLen);
        }
        if (dwLeftLen)
        {
            (void)memset((char *)(pMsgHeader + 1) + dwCopyLen, 0, dwLeftLen);
        }
    }

    return pMsgHeader;
}

/*******************************************************
  �� �� ��: OSMsgHeader::ChangeBytesOrder
  ��    ��: ��ͷ����ʽת���ֽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void OSMsgHeader::ChangeBytesOrder(void *pBuf, DWORD dwLen)
{
    if (!pBuf || (dwLen < sizeof(OSMsgHeader))) return;

    const BYTES_CHANGE_RULE aHeaderRule[] = 
    {
        {SIZE_OF(OSMsgHeader, m_dwDataLen), OFFSET_OF(OSMsgHeader, m_dwDataLen)},
        {SIZE_OF(OSMsgHeader, m_dwMsgType), OFFSET_OF(OSMsgHeader, m_dwMsgType)},
        {SIZE_OF(OSMsgHeader, m_dwSrcID), OFFSET_OF(OSMsgHeader, m_dwSrcID)},
        {SIZE_OF(OSMsgHeader, m_dwDstID), OFFSET_OF(OSMsgHeader, m_dwDstID)},
    };

    Bytes_ChangeOrderByRule(aHeaderRule, ARRAY_SIZE(aHeaderRule), pBuf, dwLen);
}

/*******************************************************
  �� �� ��: objMsgQue::CreateInstance
  ��    ��: ������Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsgQue *objMsgQue::CreateInstance(DWORD dwMaxSize, const char *file, int line)
{
    #undef new
    CMsgQueBase *pMsgQueBase = new (file, line) CMsgQueBase(dwMaxSize);
    #define new new(__FILE__, __LINE__)
    if (pMsgQueBase)
    {
        if (pMsgQueBase->Create())
        {
            delete pMsgQueBase;
            pMsgQueBase = 0;
        }
    }

    return pMsgQueBase;
}

/*******************************************************
  �� �� ��: objMsgQue::~objMsgQue
  ��    ��: ������Ϣ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
objMsgQue::~objMsgQue()
{
}

/*******************************************************
  �� �� ��: CMsgQueBase::CMsgQueBase
  ��    ��: CMsgQueBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMsgQueBase::CMsgQueBase(DWORD dwMaxSize) :
    m_dwMaxSize(dwMaxSize), m_pLock(0), m_pEvent(0)
{
}

/*******************************************************
  �� �� ��: CMsgQueBase::~CMsgQueBase
  ��    ��: CMsgQueBase����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CMsgQueBase::~CMsgQueBase()
{
    Destroy();
}

/*******************************************************
  �� �� ��: CMsgQueBase::Create
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgQueBase::Create()
{
    m_pEvent = objEvent::CreateInstance(FALSE, __FILE__, __LINE__);
    if (!m_pEvent)
    {
        return FAILURE;
    }

    m_pLock = DCOP_CreateLock();
    if (!m_pLock)
    {
        delete m_pEvent;
        m_pEvent = 0;
        return FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CMsgQueBase::Destroy
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CMsgQueBase::Destroy()
{
    /////////////////////////////////////////////////
    /// �ȰѶ�����գ��ͷ�������Ϣ
    /////////////////////////////////////////////////

    if (m_pLock)
    {
        m_pLock->Enter();
    }

    IT_MSGQUE it_que = m_msgQues.begin();
    while (it_que != m_msgQues.end())
    {
        objMsg *pMsg = (*it_que);
        if (pMsg)
        {
            delete pMsg;
        }

        ++it_que;
    }

    m_msgQues.clear();

    if (m_pLock)
    {
        m_pLock->Leave();
    }

    /////////////////////////////////////////////////
    /// ���ͷ�����ϵͳ��Դ
    /////////////////////////////////////////////////

    if (m_pLock)
    {
        delete m_pLock;
        m_pLock = 0;
    }

    if (m_pEvent)
    {
        delete m_pEvent;
        m_pEvent = 0;
    }
}

/*******************************************************
  �� �� ��: CMsgQueBase::Send
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgQueBase::Send(objMsg *message, DWORD sendPrio)
{
    DWORD dwRc = SUCCESS;

    AutoLock(m_pLock);

    if (m_msgQues.size() >= m_dwMaxSize)
    {
        return ERRCODE_MSG_QUE_FULL;
    }

    switch (sendPrio)
    {
        case OSMSGQUE_SENDPRIO_NORMAL :
            m_msgQues.push_back(message);
            break;
        case OSMSGQUE_SENDPRIO_URGENT :
            m_msgQues.push_front(message);
            break;
        default :
            dwRc = ERRCODE_MSG_SENDPRIO_ERROR;
            break;
    }

    if (SUCCESS == dwRc)
    {
        (void)m_pEvent->Send();
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CMsgQueBase::Recv
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgQueBase::Recv(objMsg *&message)
{
    DWORD dwRc = SUCCESS;

    AutoLock(m_pLock);

    IT_MSGQUE it_que = m_msgQues.begin();
    if (it_que == m_msgQues.end())
    {
        dwRc = ERRCODE_MSG_QUE_EMPTY;
    }
    else
    {
        message = (*it_que);
        m_msgQues.pop_front();
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CMsgQueBase::Wait
  ��    ��: ���¼�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgQueBase::Wait(DWORD waitMilliseconds)
{
    return m_pEvent->Recv(waitMilliseconds);
}

/*******************************************************
  �� �� ��: CMsgQueBase::Size
  ��    ��: ��С
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CMsgQueBase::Size()
{
    AutoLock(m_pLock);

    return (DWORD)m_msgQues.size();
}

