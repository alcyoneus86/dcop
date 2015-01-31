/// -------------------------------------------------
/// secure_main.h : ��ȫ����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _SECURE_MAIN_H_
#define _SECURE_MAIN_H_

#include "secure_if.h"


/// -------------------------------------------------
/// ��ȫ����ʵ����
/// -------------------------------------------------
class CSecure : public ISecure
{
public:
    CSecure(Instance *piParent, int argc, char **argv);
    ~CSecure();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

private:
    
};


#endif // #ifndef _SECURE_IF_H_

