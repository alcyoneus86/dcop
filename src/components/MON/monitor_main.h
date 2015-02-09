/// -------------------------------------------------
/// monitor_main.h : ϵͳ���˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _MONITOR_MAIN_H_
#define _MONITOR_MAIN_H_

#include "monitor_if.h"


/// -------------------------------------------------
/// ϵͳ���ʵ����
/// -------------------------------------------------
class CMonitor : public IMonitor
{
public:
    CMonitor(Instance *piParent, int argc, char **argv);
    ~CMonitor();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    void OnStart(objMsg *msg);
    void OnFinish(objMsg *msg);

private:
    
};


#endif // #ifndef _MONITOR_MAIN_H_


