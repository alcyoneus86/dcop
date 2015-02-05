/// -------------------------------------------------
/// err.h : �����붨�幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _DCOP_ERR_H_
#define _DCOP_ERR_H_

/// �������ַ(��Ҫ����16��bit)
enum DCOP_ERRCODE
{
    SUCCESS = 0,

    ERRCODE_OS,                                 // 0x0001 - ������ش�����
    ERRCODE_TASK,                               // 0x0002 - ������ش�����
    ERRCODE_SEM,                                // 0x0003 - �ź�����ش�����
    ERRCODE_MSG,                                // 0x0004 - ��Ϣ��ش�����
    ERRCODE_SOCK,                               // 0x0005 - �׽�����ش�����
    ERRCODE_IO,                                 // 0x0006 - ���������ش�����

    ERRCODE_END,

    ERRCODE_USER = 0x000F                       // 0x000F - �û��Զ��������
};

#define FAILURE (__LINE__)
#define TIMEOUT 1

/// ����ϵͳ������
enum DCOP_ERRCODE_OS
{
    ERRCODE_OS_BASE         = (ERRCODE_OS) << 16,

    ERRCODE_OS_END
};

/// ���������
enum DCOP_ERRCODE_TASK
{
    ERRCODE_TASK_BASE       = (ERRCODE_TASK) << 16,

    ERRCODE_TASK_WRONG_HANDLE,                  // 0x00020001 - ������
    ERRCODE_TASK_CREATE_FAIL,                   // 0x00020002 - ��������ʧ��
    ERRCODE_TASK_DESTROY_FAIL,                  // 0x00020003 - ��������ʧ��
    

    ERRCODE_TASK_END
};

/// �ź���������
enum DCOP_ERRCODE_SEM
{
    ERRCODE_SEM_BASE        = (ERRCODE_SEM) << 16,
    ERRCODE_SEM_WRONG_HANDLE,                   // 0x00030001 - ������
    ERRCODE_SEM_WAIT_TIMEOUT,                   // 0x00030002 - �ȴ���ʱ
    ERRCODE_SEM_OVER_MAXCOUNT,                  // 0x00030003 - �������ֵ

    ERRCODE_SEM_END
};

/// ��Ϣ���д�����
enum DCOP_ERRCODE_MSG
{
    ERRCODE_MSG_BASE        = (ERRCODE_MSG) << 16,

    ERRCODE_MSG_QUE_FULL,                       // ��Ϣ������
    ERRCODE_MSG_QUE_EMPTY,                      // ��Ϣ���п�
    ERRCODE_MSG_SENDPRIO_ERROR,                 // �������ȼ�����
    ERRCODE_MSG_NOT_FRAME_HEADER,               // ������Ϣ֡ͷ��(BUF��ʽ����)
    ERRCODE_MSG_PACKET_NOT_HEADER,              // ���������ͷ��(CMD��ʽ����)
    ERRCODE_MSG_PACKET_FULL,                    // ���������
    ERRCODE_MSG_PACKET_NO_SPACE,                // ������޿��пռ�

    ERRCODE_MSG_END
};

/// �׽��ִ�����
enum DCOP_ERRCODE_SOCK
{
    ERRCODE_SOCK_BASE       = (ERRCODE_SOCK) << 16,

    ERRCODE_SOCK_FLAG_NON_NONE,                 // ��ʶ��Ϊ��
    ERRCODE_SOCK_FLAG_ERR_VALUE,                // ��ʶ����
    ERRCODE_SOCK_PARA_ERR_VALUE,                // ��������
    ERRCODE_SOCK_INIT_FAILURE,                  // ��ʼ��ʧ��
    ERRCODE_SOCK_CREATE_FAILURE,                // ����ʧ��
    ERRCODE_SOCK_INVALID_SOCKET,                // �������õ��׽���
    ERRCODE_SOCK_SELECT_ERROR,                  // �׽���select���ô���
    ERRCODE_SOCK_SELECT_TIMEOUT,                // �׽���select���ô���
    ERRCODE_SOCK_SEND_ERROR,                    // �׽���send(sendto)���ô���
    ERRCODE_SOCK_RECV_ERROR,                    // �׽���recv(recvfrom)���ô���
    ERRCODE_SOCK_INPUT_ERROR,                   // �����������
    ERRCODE_SOCK_CONN_CLSOED,                   // �����Ѿ����ر�
    ERRCODE_SOCK_ACCEPT_ERROR,                  // �������Ӵ���
    ERRCODE_SOCK_CONN_ERROR,                    // ���Ӵ���
    ERRCODE_SOCK_CLTSOCK_ERROR,                 // �ͻ��׽��ִ���

    ERRCODE_SOCK_END
};

/// IO������
enum DCOP_ERRCODE_IO
{
    ERRCODE_IO_BASE         = (ERRCODE_IO) << 16,

    ERRCODE_IO_NO_RIGHT_TO_OPERATE,             // ����Ȩ�޲���

    ERRCODE_IO_END
};


#endif // #ifndef _DCOP_ERR_H_

