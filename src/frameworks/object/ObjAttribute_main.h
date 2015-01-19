/// -------------------------------------------------
/// ObjAttribute_main.h : ��������˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJATTRIBUTE_MAIN_H_
#define _OBJATTRIBUTE_MAIN_H_

#include "ObjAttribute_if.h"
#include "BaseMessage.h"


/// -------------------------------------------------
/// Ĭ���ط�������Ĭ�ϳ�ʱʱ��
/// -------------------------------------------------
const DWORD SEND_TRY_TIMES_DEFAULT = 3;
const DWORD SEND_TIMEOUT_DEFAULT = 5000;

/// -------------------------------------------------
/// ������Ϣ
/// -------------------------------------------------
DCOP_MSG_HEAD *ParseMsg(void *pBuf, DWORD dwLen, DWORD &rdwOffset);

/// -------------------------------------------------
/// �����Ϣͷ
/// -------------------------------------------------
bool CheckMsgHead(void *pBuf, DWORD dwLen);

/// -------------------------------------------------
/// �ı���Ϣͷ�ֽ���
/// -------------------------------------------------
void ChangeMsgHeadOrder(DCOP_MSG_HEAD *pHead);

/// -------------------------------------------------
/// ������Ϣ����������
/// -------------------------------------------------
void AddMsgParaData(DCOP_MSG_HEAD *pHead, 
                        DWORD dwParaCount, 
                        DWORD dwDataLen, 
                        DCOP_PARA_NODE *pPara, 
                        void *pData);

/// -------------------------------------------------
/// �Ƿ���������
/// -------------------------------------------------
bool IsCountReq(DCOP_PARA_NODE *pReqPara, 
                        DWORD dwReqParaCount);

/// -------------------------------------------------
/// ��ʼ����Ϣͷ
/// -------------------------------------------------
void InitMsgHead(void *pBuf, DWORD dwLen, BYTE byHeadType, const DCOP_MSG_HEAD *pHeadCopy = NULL);

/// -------------------------------------------------
/// �ַ���Ϣ
/// -------------------------------------------------
DWORD DispatchMsg(IDispatch *pDispatch, DWORD dwDstID, objMsg *pMsg, DWORD dwSendTryTimes, 
                        IResponse::IPool *pReqPool = 0, 
                        DWORD dwRspMsgType = 0, 
                        DWORD dwTimeout  = 0);


#endif // #ifndef _OBJATTRIBUTE_MAIN_H_

