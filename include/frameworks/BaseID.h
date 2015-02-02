/// -------------------------------------------------
/// BaseID.h : ����ID���幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _BASEID_H_
#define _BASEID_H_


/// -------------------------------------------------
/// ��������ֵ�����
/// -------------------------------------------------
///     ��16λ : ϵͳID
///     ��16λ : ����ID
/// -------------------------------------------------
/// ����IDС��0x00010000�Ķ�����ֻ���صĶ���
/// -------------------------------------------------

#define DCOP_OBJECT_NULL                    0               // ��Ч����ID
#define DCOP_OBJECT_KERNEL                  1               // �ں�(Ҳ��ʾ�������)
#define DCOP_OBJECT_FACTORY                 2               // �๤��
#define DCOP_OBJECT_MANAGER                 3               // ���������
#define DCOP_OBJECT_DISPATCH                4               // ��Ϣ�ַ���
#define DCOP_OBJECT_NOTIFY                  5               // ���ķ�����
#define DCOP_OBJECT_CONTROL                 6               // ������
#define DCOP_OBJECT_TIMER                   7               // ��ʱ��
#define DCOP_OBJECT_SCHEDULE                8               // ������
#define DCOP_OBJECT_STATUS                  9               // ״̬��
#define DCOP_OBJECT_RESPONSE                10              // ��Ӧ��
#define DCOP_OBJECT_MODEL                   11              // ģ��
#define DCOP_OBJECT_DATA                    12              // ����

#define DCOP_OBJECT_SESSION                 20              // �Ự����
#define DCOP_OBJECT_USER                    21              // �û�����
#define DCOP_OBJECT_SECURE                  22              // ��ȫ����
#define DCOP_OBJECT_ACCESS                  23              // �ֲ�ʽ����
#define DCOP_OBJECT_COMMAND                 24              // �����н���
#define DCOP_OBJECT_HTTPD                   25              // ���ı�����
#define DCOP_OBJECT_RESTFUL                 26              // Rest API

#define DCOP_OBJECT_MONITOR                 50              // ϵͳ���

#define DCOP_OBJECT_CUSTOM                  256             // �û��Զ���


/// -------------------------------------------------
/// �ڲ����ⲿϵͳ����(ֻ�Ǳ�ʶ���ⲿϵͳ�������ģ�������ָ�����ĸ�ϵͳ)
/// -------------------------------------------------

#define DCOP_SYSTEM_INTERNAL                0x0000FFFF
#define DCOP_SYSTEM_EXTERNAL                0xFFFF0000


/// -------------------------------------------------
/// ��������ֵ�����
/// -------------------------------------------------
///     ��16λ : ����ID
///     ��16λ : ƫ��ֵ
/// -------------------------------------------------

#define DCOP_OBJATTR_USER_TABLE             ((DCOP_OBJECT_USER      << (16)) | 1)   // �û���
#define DCOP_OBJATTR_SESSION_TABLE          ((DCOP_OBJECT_SESSION   << (16)) | 1)   // �Ự��
#define DCOP_OBJATTR_ACCESS_LOGIN           ((DCOP_OBJECT_ACCESS    << (16)) | 1)   // �����½
#define DCOP_OBJATTR_ACCESS_LOGOUT          ((DCOP_OBJECT_ACCESS    << (16)) | 2)   // �˳���½


/// -------------------------------------------------
/// �û���(�û�����)����
/// -------------------------------------------------

#define DCOP_GROUP_ADMINISTRATOR            0               // ������   : ϵͳ����Ȩ�ޣ�ӵ������Ȩ��
#define DCOP_GROUP_MAINTAINER               1               // ά����   : ϵͳά������Ȩ�ޣ����·�ά������
#define DCOP_GROUP_MANAGER                  2               // ������   : ҵ���������û������о���ҵ��Ĺ���Ȩ��
#define DCOP_GROUP_USER                     3               // �û�     : ҵ�������ͨ�û������о���ҵ���Լ��Ȩ��
#define DCOP_GROUP_VISITOR                  4               // �ι���   : ҵ�����ȱʡ�û���δ��¼ʱĬ���û���Ȩ��Լ������
#define DCOP_GROUP_MONITOR                  5               // �����   : ��Ϊϵͳ�ļ���ߣ�ֻ�ܲ鿴�ռ���־


/// -------------------------------------------------
/// ��ʱ�����û�
/// -------------------------------------------------

#define DCOP_USER_UNLOGIN                   0               // δ��¼�û�


#endif // #ifndef _BASEID_H_

