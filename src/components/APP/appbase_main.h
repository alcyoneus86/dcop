/// -------------------------------------------------
/// appbase_main.h : Ӧ�û���˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _APPBASE_MAIN_H_
#define _APPBASE_MAIN_H_

#include "appbase_if.h"
#include "ObjAttribute_if.h"
#include "fs/file.h"


/// -------------------------------------------------
/// Ӧ�û���ʵ����
/// -------------------------------------------------
class CAppBase : public IAppBase
{
public:
    CAppBase(Instance *piParent, int argc, char **argv);
    ~CAppBase();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();
    void Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv);

    void OnStart(objMsg *msg);
    void OnFinish(objMsg *msg);
    void OnRequest(objMsg *msg);
    void OnDefault(objMsg *msg);

private:
    DWORD InitModelData();

private:
    char m_szModelCfg[DCOP_FILE_NAME_LEN];          // ģ�������ļ�

    CObjectMemberIndex m_dataIndex;                 // ��������
    CObjectMember<IData *> *m_pDatas;               // ���ݳ�Ա�б�
    CAttribute *m_pDataAttrs;                       // ���������б�
    DWORD m_dwDataCount;                            // ���ݳ�Ա����

    IModel *m_piModel;                              // ģ�͹���
    IData *m_piData;                                // ��������

    IDispatch *m_piDispatch;                        // ��Ϣ�ַ���
    INotify *m_piNotify;                            // �¼�֪ͨ��
    INotify::IPool *m_pNotifyPool;                  // �¼������
};


#endif // #ifndef _APPBASE_MAIN_H_

