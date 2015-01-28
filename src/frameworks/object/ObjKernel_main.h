/// -------------------------------------------------
/// ObjKernel_main.h : object��ܺ�����ʵ��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJKERNEL_MAIN_H_
#define _OBJKERNEL_MAIN_H_

#define INC_MAP

#include "../../wrappers/osBase.h"
#include "BaseClass.h"
#include "Manager_if.h"
#include "xml/xml.h"
#include "array/darray.h"
#include "sem.h"


/// -------------------------------------------------
/// �����ַ�����󳤶�
/// -------------------------------------------------
#define DCOP_STRING_ARG_LEM_MAX             64

/// -------------------------------------------------
/// ϵͳ����������
/// -------------------------------------------------
#define DCOP_SYSTEM_ARG_MAX_COUNT           24

/// -------------------------------------------------
/// ��������������
/// -------------------------------------------------
#define DCOP_OBJECT_ARG_MAX_COUNT           32


/// -------------------------------------------------
/// ��ܺ���ʵ����
/// -------------------------------------------------
class CFrameKernel : public objBase, private osBase
{
public:
    /// ���ýڵ�
    class CReferNode
    {
    public:
        /// ����MAP
        typedef std::map<Instance *, DWORD> MAP_COUNT;
        typedef MAP_COUNT::iterator IT_COUNT;

    public:
        CReferNode() : m_refer(sizeof(Instance *)), m_refercnt(sizeof(DWORD)) {m_loader = 0;}
        ~CReferNode()
        {
            if (m_loader)
            {
                delete m_loader;
                m_loader = 0;
            }
        }

        void SetLoader(objDynamicLoader *loader) {m_loader = loader;}

        void OnReferto(Instance *refer);
        void OnRelease(Instance *refer);
        void OnGetRefer(Instance ***refers, DWORD *count);
        void OnGetReferCount(DWORD **refercount, DWORD *count);

    private:
        objDynamicLoader *m_loader;
        CDArray m_refer;
        CDArray m_refercnt;
        MAP_COUNT m_count;
    };

    /// ����MAP
    typedef std::map<Instance *, CReferNode> MAP_REFERS;
    typedef MAP_REFERS::iterator IT_REFERS;

public:
    static CFrameKernel sm_instance;

    CFrameKernel();
    ~CFrameKernel();

    void Enter();
    void Leave();

    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

    objBase *Start(const char *cfgDeploy);
    void End(objBase *pBase);

public:
    IManager *LoadAllObjects(const char *xmlFile);
    IObject *Load(const char *cszName, IManager *piManager, int argc, char **argv);
    objDynamicLoader *DynamicLoad(const char *dllFile);
    DWORD AddRefer(Instance *piThis, Instance *piRefer, objDynamicLoader *pLoader = 0);
    DWORD DelRefer(Instance *piThis, Instance *piRefer);
    DWORD GetRefer(Instance *piThis, Instance ***pppiRefers);
    static void OnInstanceQueryInterface(
                        Instance *piThis, 
                        Instance *piRefer, 
                        void *pPara);
    static void OnInstanceRelease(
                        Instance *piThis, 
                        Instance *piRefer, 
                        void *pPara);
    static void OnInstanceGetRef(
                        Instance *piThis, 
                        Instance ***pppiRefers, 
                        DWORD *pdwReferCount, 
                        void *pPara);

private:
    DWORD GetXmlAttribute(const XMLElement *pXMLElement, CDArray &rArgs);
    DWORD GetXmlChildValue(const XMLElement *pXMLElement, CDArray &rArgs);
    void  GetArgList(DWORD argc, char **argv, const CDArray &crArgs);
    DWORD CreateAllObjects(IManager *piManager, const XMLElement *pXMLElement);

private:
    objLock *m_pLock;
    objTask *m_pTask;
    MAP_REFERS m_refers;
};


#endif // #ifndef _OBJKERNEL_MAIN_H_

