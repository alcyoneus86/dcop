/// -------------------------------------------------
/// msgType.h : msgQ��װ�๫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _MSGTYPE_H_
#define _MSGTYPE_H_

#define INC_DEQUE

#include "../osBase.h"
#include "msg.h"
#include "sem.h"
#include "list/tailq.h"


/// ��Ϣʵ��
class CMsgBase : public objMsg, private osBase
{
public:
    CMsgBase(DWORD dwDataLen,
                DWORD dwMsgType,
                DWORD dwLocalID,
                const void *cpData = 0,
                DWORD dwCopyLen = 0);
    CMsgBase(void *pFrameBuf,
                DWORD dwFrameLen);
    ~CMsgBase();

    DWORD GetOffset();

    DWORD GetDataLen();

    DWORD GetMsgType();

    DWORD GetSrcID();

    DWORD GetDstID();

    objMsg *Clone(const char *file, int line);

    void *Pack(DWORD &rdwLen, const char *file, int line);

    OSMsgHeader &MsgHead();

    void *GetDataBuf();

    void *SetCtrl(void *pBuf, DWORD dwLen);

    void *GetCtrl(DWORD &rdwLen);

    void *AddData(void *pBuf, DWORD dwLen);

public:
    /// ��Ϣ�����Ӹ�ʽ˵��
    ////////////////////////////////////////////////////
    /// m_header���滹�пռ䣬������������������;
    /// m_headerǰ��ļ����ṹ����ʹ�ù����е���ʱ����:
    /// (�����Ҫʹ��Pack()�������Clone()һ���µ�Msg)
    ///     [1] m_setCtrlBuf - �������õĿ�����Ϣ
    ///     [1] m_setCtrlLen - ������Ϣ����[���ܳ���offset-sizeof(OSMsgHeader)]
    ///     [2] m_addDataLst - ���ӵ������б�
    ///     [2] m_addDataLen - ���ӵ������ܳ���
    ////////////////////////////////////////////////////
    struct AddDataNode
    {
        TAILQ_ENTRY(AddDataNode) m_field;
        DWORD m_dwDataLen;
    };

private:
    void *m_setCtrlBuf;
    DWORD m_setCtrlLen;
    TAILQ_HEAD(AddDataNode) m_addDataLst;
    DWORD m_addDataLen;
    OSMsgHeader m_header;
};


/// ��Ϣ����ʵ��
class CMsgQueBase : public objMsgQue, private osBase
{
public:
    typedef std::deque<objMsg *> MSGQUE;
    typedef MSGQUE::iterator IT_MSGQUE;

public:
    CMsgQueBase(DWORD dwMaxSize);
    ~CMsgQueBase();

    DWORD Create();
    void Destroy();

    DWORD Send(objMsg *message, DWORD sendPrio = OSMSGQUE_SENDPRIO_NORMAL);
    DWORD Recv(objMsg *&message);
    DWORD Wait(DWORD waitMilliseconds);

    DWORD Size();

private:
    DWORD               m_dwMaxSize;
    MSGQUE              m_msgQues;
    objLock *           m_pLock;
    objEvent *          m_pEvent;

};


#endif // #ifndef _MSGTYPE_H_

