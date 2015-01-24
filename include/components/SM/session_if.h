/// -------------------------------------------------
/// session_if.h : �Ự������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SESSION_IF_H_
#define _SESSION_IF_H_

#include "Object_if.h"
#include "ObjTimer_if.h"


/////////////////////////////////////////////////////
///                 '�Ự����'˵��
/// -------------------------------------------------
/// '�Ự'�������û���¼ϵͳ����ϵͳ�з��ʸ�����Դ��
/// һ����ʱƾ֤��
/// 1) '�Ựƾ֤'�ڽ���ʱ��һ��DWORD���͵�'�ỰID'���
/// 2) '�ỰID'�ڷ�������ǰ�Ự�в����ظ�
/// 3) '�ỰID'�ڷ����ڲ���Դʱ�ᱻ������ȥ�������ٴ�
///    ����'�Ự����'�о��Ѿ����ڣ�����û�й�ϵ������
///    �Ự��Ȼ��ֻͨ��ID������������֤�Ự�Ƿ���ȷ��
///    ��Ҫͨ����Ϣ��Я����'�ỰID'+'�û�ID'+'�ն�ID'
///    һ����Ϊ��֤��׼�������ƥ�䣬�ûỰ�ͻᱻ����
/// 4) '�ỰID'Ҳ�ᱻ���г�ʱ���˳�������Ự�н�����
///    ������ͨ��'���»Ự'�ӿ�����ֹ��ʱ���������ø�
///    �ӿ�ʱ���벻Ҫ��ʱ���и��£�������Ự�������
///    ���ظ���
/// 5) '�û�ID'��'�û�����'ά���������ô��ڵģ����Ǳ�
///    �ֹ������ݿ���ɾ��
/// 6) '�ն�ID'��ʶ���û���¼���նˣ��Ǹ����Ӧ�ն˵�
///    ����ģ��Ķ���ID
/// 7) '�Ự����'���ṩ�����������Ӧ������Ϊ�����
///    ���ù��ܵĻ����᲻�Ͻ���'�Ự����'����Һ�����
///    ���ӵ������ӦӦ���ɸ�������ģ���Լ���������
///    ���������(�Ѿ���װ�ã���ʵ���úܼ�)
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// ����ISession�汾��
/// -------------------------------------------------
INTF_VER(ISession, 1, 0, 0);


/// -------------------------------------------------
/// �û��ӿڶ���
/// -------------------------------------------------
interface ISession : public IObject
{
    /// ��Ϣ����
    static const WORD INFO_SIZE = 22;

    /// �ڵ�
    struct NODE
    {
        DWORD SessID;
        DWORD UserID;
        DWORD UserGroup;
        DWORD TTY;
        ITimer::Node Timer;
        DWORD IP;
        WORD Port;
    };

    /// �����Ự
    virtual DWORD CreateSession(
                        DWORD dwUserID,             // �û�ID
                        DWORD dwUserGroup,          // �û���
                        DWORD dwTTY,                // �ն�ID
                        DWORD dwRemoteIP,           // Զ��IP
                        WORD wRemotePort,           // Զ�̶˿�
                        DWORD &rdwSessionID         // �ỰID
                        ) = 0;

    /// ɾ���Ự
    virtual DWORD DeleteSession(
                        DWORD dwSessionID           // �ỰID
                        ) = 0;

    /// ���»Ự
    /// [ʹ֮��Ҫ��ʱ]
    virtual DWORD UpdateSession(
                        DWORD dwSessionID           // �ỰID
                        ) = 0;

    /// �����û���Ϣ
    virtual DWORD UpdateUserID(
                        DWORD dwSessionID,          // �ỰID
                        DWORD dwUserID,             // �û�ID
                        DWORD dwUserGroup           // �û���
                        ) = 0;

    /// ���һỰ
    /// [ͨ��Զ��IP��Զ�̶˿�]
    virtual DWORD FindSession(
                        DWORD dwRemoteIP,           // Զ��IP
                        WORD wRemotePort,           // Զ�̶˿�
                        NODE &rNode                 // �Ự�ڵ�
                        ) = 0;

    /// ��ȡ�Ự
    virtual DWORD GetSession(
                        DWORD dwSessionID,          // �ỰID
                        NODE &rNode                 // �Ự�ڵ�
                        ) = 0;

    /// ���ûỰ��Ϣ
    virtual DWORD SetSessionInfo(
                        DWORD dwSessionID,          // �ỰID
                        char szInfo[INFO_SIZE]      // �Ự��Ϣ
                        ) = 0;

    /// ��ȡ�Ự��Ϣ
    virtual DWORD GetSessionInfo(
                        DWORD dwSessionID,          // �ỰID
                        char szInfo[INFO_SIZE]      // �Ự��Ϣ
                        ) = 0;
};


#endif // #ifndef _SESSION_IF_H_


