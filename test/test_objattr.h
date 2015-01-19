/// -------------------------------------------------
/// test_objattr.h : ��Ҫ����frameworks object attribute����
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_OBJATTR_H_
#define _TEST_OBJATTR_H_

#include "test.h"
#include "ObjAttribute_if.h"
#include "Manager_if.h"
#include "BaseID.h"


enum TEST_OBJ_ID
{
    TEST_OBJ_ID_A = DCOP_OBJECT_CUSTOM,
    TEST_OBJ_ID_B,
};


/// ����ITestForIObjectA�汾��
INTF_VER(ITestAttrForIObjectA, 1, 0, 0);


/// ����ITestForIObjectA�ӿ���
interface ITestAttrForIObjectA : public IObject
{
    /// ���ýӿ�
    virtual void SetTestNo(DWORD dwTestNo) = 0;

    /// ��ѯ�ӿ�
    virtual DWORD GetTestNo() = 0;
};


/// ����CTestAttrForIObjectAʵ����
class CTestAttrForIObjectA : public ITestAttrForIObjectA
{
public:
    CTestAttrForIObjectA(Instance *piParent, int argc, char **argv);
    ~CTestAttrForIObjectA();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);

    void SetTestNo(DWORD dwTestNo);
    DWORD GetTestNo();

    DECLARE_ATTRIBUTE_INDEX(testIndex);
    DECLARE_ATTRIBUTE(DWORD, dwTestNo);

private:
    IManager *m_piManager;
};

/// ����CTestForIObjectBʵ����
class CTestAttrForIObjectB : public IObject
{
public:
    CTestAttrForIObjectB(Instance *piParent, int argc, char **argv);
    ~CTestAttrForIObjectB();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    void OnTestNoSet(IObject *sender, DWORD event, void *para, DWORD paralen);
    void OnTestNoGet(IObject *sender, DWORD event, void *para, DWORD paralen);

private:
    DWORD m_dwTestNo;
};

/// ����frameworks object attribute
class CTestSuite_OBJATTR : public ITestSuite
{
public:
    CTestSuite_OBJATTR();
    ~CTestSuite_OBJATTR();

    void BeforeTest();
    void AfterTest();

    int TestEntry(int argc, char* argv[]);

private:
    ITestAttrForIObjectA *m_piTA;
    IObject *m_piTB;
    IManager *m_piManager;
};


#endif // #ifndef _TEST_OBJATTR_H_

