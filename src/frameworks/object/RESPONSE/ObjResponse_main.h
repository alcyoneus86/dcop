/// -------------------------------------------------
/// ObjResponse_main.h : ��Ӧ������˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJRESPONSE_MAIN_H_
#define _OBJRESPONSE_MAIN_H_

#define INC_SET

#include "ObjResponse_if.h"
#include "Manager_if.h"
#include "ObjResponse_pool.h"
#include "ObjTimer_if.h"


/////////////////////////////////////////////////////
/// [˵��] ���ﶨʱ�ֵ�ʹ�÷����Ͷ�ʱ��������һ���ģ�
/// ʹ��4���������ֱ��ʾ�롢�֡�ʱ���죻
/// Ҳ����ʹ��һ�����ӣ�ͬʱ�ڶ�ʱ��ֵ��������һ������
/// ���ڵı�������: ��������������32�룬�����г�ʱΪ
/// 100��Ľڵ㣬��ô���ڶ�ʱ��������һ������100/32,
/// ��ȡ����100%32���ó���ʱ���ֵĲ�λ��
/// Ϊʲôȡ32�룬��Ϊ:
///     1. ԭֵ '>>5' �պþͻ���˱���
///     2. ԭֵ���ϵ�ǰ��λ�̶ȣ��� '& 0x1F' ����������
///        Ҳ������Ҫ����Ķ�ʱ����λ
/////////////////////////////////////////////////////


/// ������Ӧ����
class CResponse : public IResponse
{
public:
    /// ���󻺳��SET
    typedef std::set<CResponsePool *> SET_RESPONSE;
    typedef SET_RESPONSE::iterator IT_RESPONSE;

public:
    CResponse(Instance *piParent, int argc, char **argv);
    ~CResponse();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    void OnStart(objMsg *msg);
    void OnFinish(objMsg *msg);
    void OnTimer1s(objMsg *msg);

    IPool *CreatePool(IObject *owner, DWORD count);

    void DestroyPool(IPool *pool);

private:
    SET_RESPONSE m_requests;
    IManager *m_piManager;
    ITimer *m_piTimer;
    ITimer::Handle m_hTimer1s;
};


#endif // #ifndef _OBJRESPONSE_MAIN_H_

