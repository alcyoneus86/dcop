/// -------------------------------------------------
/// msg.h : ��Ϣ���ж��幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _MSG_H_
#define _MSG_H_

#include "dcop.h"


/// -------------------------------------------------
/// ��Ϣ���ĵĸ�ʽ����:
/// +----------------------+------------------------+
/// |  OSMsgHeader  (Ctrl) |          Data          |
/// +----------------------+------------------------+
/// 
///  <OSMsgHeader> ��ʽ����:
/// +-----------+-----------+-----------+-----------+
/// |     'D'   |    'c'    |    'o'    |   Offset  |
/// +-----------+-----------+-----------+-----------+
/// |                    DataLen                    |
/// +-----------+-----------+-----------+-----------+
/// |                    MsgType                    |
/// +-----------+-----------+-----------+-----------+
/// |                     SrcID                     |
/// +-----------+-----------+-----------+-----------+
/// |                     DstID                     |
/// +-----------+-----------+-----------+-----------+



/// ��Ϣ����ͷ����ʽ
class OSMsgHeader
{
public:
    OSMsgHeader();
    ~OSMsgHeader();

    /// ��ȡ��Ϣͷ����С
    static BYTE GetHeaderSize() {return sizeof(OSMsgHeader);}

    /// �ж�BUF��ͷ�Ƿ�����ȷ��֡��ʽ
    ////////////////////////////////////////////////////
    /// �ж����ݰ��ı߽�, ��: 
    ///     [1] ����<0: �յ��ĳ��Ȳ����ж�����֡(<8Bytes)
    ///     [2] ����=0: ����һ����ȷ������֡�Ŀ�ͷ
    ///     [3] ����>0: ����ȷ������֡���ҷ�����������֡�ĳ���
    /// [˵��]pBufʹ�õ��������ֽ���(�ᰴ�ձ����ֽ����ȡ)
    ////////////////////////////////////////////////////
    static int bFrame(void *pBuf, DWORD dwLen);

    /// ��������䵽BUF��
    static OSMsgHeader *Format(void *pBuf,
                        DWORD dwBufLen,
                        DWORD dwMsgType = 0,
                        DWORD dwLocalID = 0,
                        const void *cpData = 0,
                        DWORD dwDataLen = 0
                        );

    /// ��ͷ����ʽת��һ��pBuf���ֽ���
    /// ���뱣֤pBuf�Ĵ�С�ǵ�����Ϣ���ĵ�֡ͷ
    /// һ�����ڸ��յ�Զ�˵����ݺ��������Ҫ���͵�Զ��ǰ
    static void ChangeBytesOrder(void *pBuf, DWORD dwLen);

public:
    BYTE    m_byMagicWord0;                                     // ͷ��ħ����(�̶�Ϊ'D')
    BYTE    m_byMagicWord1;                                     // ͷ��ħ����(�̶�Ϊ'c')
    BYTE    m_byMagicWord2;                                     // ͷ��ħ����(�̶�Ϊ'o')
    BYTE    m_byOffset;                                         // ͷ������(HeaderSize==sizeof(OSMsgHeader))
    DWORD   m_dwDataLen;                                        // ���ݳ���(��ͷ��������ݳ���)
    DWORD   m_dwMsgType;                                        // ��Ϣ����(���ڷ�����Ϣ�Ĵ���)
    DWORD   m_dwSrcID;                                          // Դ(������ID)
    DWORD   m_dwDstID;                                          // ��(������ID)
};


/// ������Ϣ
#define DCOP_CreateMsg(dwDataLen, dwMsgType, dwLocalID) \
    objMsg::CreateInstance(dwDataLen, dwMsgType, dwLocalID, __FILE__, __LINE__)


/// ������������Ϣ
#define DCOP_LoadMsg(dwDataLen, dwMsgType, dwLocalID, cpData, dwCopyLen) \
    objMsg::CreateInstance(dwDataLen, dwMsgType, dwLocalID, __FILE__, __LINE__, cpData, dwCopyLen)


/// ������Ϣ
#define DCOP_ParseMsg(pBuf, dwLen) \
    objMsg::Parse(pBuf, dwLen, __FILE__, __LINE__);


/// ������Ϣ
#define DCOP_CloneMsg(pMsg) \
    ((!pMsg) ? NULL : (pMsg)->Clone(__FILE__, __LINE__))


/// ������Ϣ
#define DCOP_PackMsg(pMsg, rdwLen) \
    ((!pMsg) ? NULL : (pMsg)->Pack(rdwLen, __FILE__, __LINE__))


/// ��Ϣ����(����ϵͳ�д�����Ϣ���ĵ�����)
class objMsg
{
public:
    static objMsg *CreateInstance(DWORD dwDataLen,
                        DWORD dwMsgType,
                        DWORD dwLocalID,
                        const char *file,
                        int line,
                        const void *cpData = 0,
                        DWORD dwCopyLen = 0);

    static objMsg *Parse(void *pFrameBuf,
                        DWORD dwFrameLen,
                        const char *file,
                        int line);

    virtual ~objMsg() = 0;

    /// ��ȡͷ������
    virtual DWORD GetOffset() = 0;

    /// ��ȡ���ݳ���
    virtual DWORD GetDataLen() = 0;

    /// ��ȡ��Ϣ����
    virtual DWORD GetMsgType() = 0;

    /// ��ȡ��ϢԴ��ַ
    virtual DWORD GetSrcID() = 0;

    /// ��ȡ��ϢĿ�ĵ�ַ
    virtual DWORD GetDstID() = 0;

    /// ����һ���µ���Ϣ
    virtual objMsg *Clone(const char *file, int line) = 0;

    /// ������Ϣ��һ��������
    virtual void *Pack(DWORD &rdwLen, const char *file, int line) = 0;

    /// ��ȡ��Ϣͷ
    virtual OSMsgHeader &MsgHead() = 0;

    /// ��ȡ��Ϣ��ַ
    virtual void *GetDataBuf() = 0;

    /// ���ÿ�����Ϣ
    virtual void *SetCtrl(void *pBuf, DWORD dwLen) = 0;

    /// ��ȡ������Ϣ
    virtual void *GetCtrl(DWORD &rdwLen) = 0;

    /// ���������Ϣ
    virtual void *AddData(void *pBuf, DWORD dwLen) = 0;
};


/// ������Ϣ���еĺ�
#define DCOP_CreateMsgQue(dwMaxSize) \
    objMsgQue::CreateInstance(dwMaxSize, __FILE__, __LINE__)


/// ��Ϣ���ж���(����ϵͳ�л�����Ϣ���ĵ�����)
class objMsgQue
{
public:
    typedef enum tagOSMSGQUE_SENDPRIO                           // �������ȼ�
    {
        OSMSGQUE_SENDPRIO_NORMAL,                               // ��ͨ����
        OSMSGQUE_SENDPRIO_URGENT                                // ��������
    }OSMSGQUE_SENDPRIO;

public:
    static objMsgQue *CreateInstance(DWORD dwMaxSize,
                            const char *file,
                            int line);
    virtual ~objMsgQue() = 0;

    virtual DWORD Send(
                    objMsg *message, 
                    DWORD sendPrio = OSMSGQUE_SENDPRIO_NORMAL
                    ) = 0;

    virtual DWORD Recv(
                    objMsg *&message
                    ) = 0;

    virtual DWORD Wait(
                    DWORD waitMilliseconds
                    ) = 0;

    virtual DWORD Size() = 0;

};


#endif // #ifndef _MSG_H_

