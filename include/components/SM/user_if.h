/// -------------------------------------------------
/// user_if.h : �û�������ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _USER_IF_H_
#define _USER_IF_H_

#include "Object_if.h"


/// -------------------------------------------------
/// ����IUser�汾��
/// -------------------------------------------------
INTF_VER(IUser, 1, 0, 0);


/// -------------------------------------------------
/// �û�����ӿ�
/// -------------------------------------------------
interface IUser : public IObject
{
    /// �û�������
    static const WORD NAME_SIZE = 32;

    /// У���ֳ���
    static const WORD PASS_SIZE = 32;

    /// ��Ϣ����
    static const WORD INFO_SIZE = 64;

    /// �ڵ�
    struct NODE
    {
        char UserName[NAME_SIZE];
        DWORD UserID;
        char PassText[PASS_SIZE];
        DWORD Level;
        DWORD Group;
        char Info[INFO_SIZE];
    };

    /// �����û�
    virtual DWORD CreateUser(
                        const char *cszUserName,
                        const char *cszPassText,
                        DWORD dwLevel,
                        DWORD dwGroup,
                        DWORD &rdwUserID
                        ) = 0;

    /// ɾ���û�
    virtual DWORD DeleteUser(
                        DWORD dwUserID
                        ) = 0;

    /// �����û�
    virtual DWORD FindUser(
                        const char *cszUserName,
                        NODE &rNode
                        ) = 0;

    /// ���У����
    virtual DWORD CheckPass(
                        const char *cszUserName,
                        const char *cszPassText,
                        NODE *pNode = 0
                        ) = 0;

    /// �޸�У����
    virtual DWORD ChangePass(
                        DWORD dwUserID,
                        char szUserName[NAME_SIZE],
                        char szOldPass[PASS_SIZE],
                        char szNewPass[PASS_SIZE]
                        ) = 0;

    /// ��ȡ�û�
    virtual DWORD GetUser(
                        DWORD dwUserID,
                        NODE &rNode
                        ) = 0;
};


#endif // #ifndef _USER_IF_H_

