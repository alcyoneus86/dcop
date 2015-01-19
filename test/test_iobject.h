/// -------------------------------------------------
/// test_iobject.h : ��Ҫ����frameworks iobject����
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_IOBJECT_H_
#define _TEST_IOBJECT_H_

#include "test.h"
#include "Object_if.h"
#include "Manager_if.h"
#include "BaseMessage.h"
#include "BaseID.h"


enum TEST_IOBJECT_EVENT{
    TEST_IOBJECT_EVENT_TESTNO_SET = DCOP_MSG_CUSTOM,
    TEST_IOBJECT_EVENT_TESTNO_GET
};


enum TEST_OBJ_ID
{
    TEST_OBJ_ID_A = DCOP_OBJECT_CUSTOM,
    TEST_OBJ_ID_B,
};


/// ����ITestForIObjectA�汾��
INTF_VER(ITestForIObjectA, 1, 0, 0);


/// ����ITestForIObjectA�ӿ���
interface ITestForIObjectA : public IObject
{
    /// ���ýӿ�
    virtual void SetTestNo(DWORD dwTestNo) = 0;

    /// ��ѯ�ӿ�
    virtual DWORD GetTestNo() = 0;
};

/// ����CTestForIObjectAʵ����
class CTestForIObjectA : public ITestForIObjectA
{
public:
    CTestForIObjectA(Instance *piParent, int argc, char **argv);
    ~CTestForIObjectA();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);

    void SetTestNo(DWORD dwTestNo);
    DWORD GetTestNo();

private:
    IManager *m_piManager;
};

/// ����CTestForIObjectBʵ����
class CTestForIObjectB : public IObject
{
public:
    CTestForIObjectB(Instance *piParent, int argc, char **argv);
    ~CTestForIObjectB();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DCOP_DECLARE_IOBJECT_MSG_HANDLE;

    void OnTestNoSet(objMsg *pMsg);
    void OnTestNoGet(objMsg *pMsg);

private:
    DWORD m_dwTestNo;
};

/// ����frameworks IObject
class CTestSuite_IOBJECT : public ITestSuite
{
public:
    CTestSuite_IOBJECT();
    ~CTestSuite_IOBJECT();

    void BeforeTest();
    void AfterTest();

    int TestEntry(int argc, char* argv[]);

private:
    ITestForIObjectA *m_piTA;
    IObject *m_piTB;
    IManager *m_piManager;
};


#endif // #ifndef _TEST_IOBJECT_H_

