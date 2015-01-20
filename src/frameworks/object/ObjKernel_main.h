/// -------------------------------------------------
/// ObjKernel_main.h : object��ܺ�����ʵ��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJKERNEL_MAIN_H_
#define _OBJKERNEL_MAIN_H_

#include "../../wrappers/osBase.h"
#include "BaseClass.h"
#include "Manager_if.h"
#include "xml/xml.h"
#include "array/darray.h"
#include "sem.h"


/// �����ַ�����󳤶�
#define DCOP_STRING_ARG_LEM_MAX             64


/// ϵͳ����������
#define DCOP_SYSTEM_ARG_MAX_COUNT           24

/// ��������������
#define DCOP_OBJECT_ARG_MAX_COUNT           32


/// Object��ܺ���ʵ����
class CFrameKernel : public objBase, private osBase
{
public:
    static CFrameKernel sm_instance;

    CFrameKernel();
    ~CFrameKernel();

    void Enter();
    void Leave();

    objBase *Start(const char *cfgDeploy);
    void End(objBase *pBase);

private:
    IManager *Load(const char *xmlFile);

private:
    DWORD GetXmlAttribute(const XMLElement *pXMLElement, CDArray &rArgs);
    DWORD GetXmlChildValue(const XMLElement *pXMLElement, CDArray &rArgs);
    void  GetArgList(DWORD argc, char **argv, const CDArray &crArgs);
    DWORD CreateAllObjects(IManager *piManager, const XMLElement *pXMLElement);

private:
    objLock *m_pLock;
};


#endif // #ifndef _OBJKERNEL_MAIN_H_

