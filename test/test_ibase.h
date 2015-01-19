/// -------------------------------------------------
/// test_ibase.h : ��Ҫ����frameworks ibase����
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_INSTANCE_H_
#define _TEST_INSTANCE_H_

#include "test.h"
#include "BaseClass.h"


/// ����ITestForInstance�汾��
INTF_VER(ITestForInstance, 1, 0, 0);


/// ����ITestForInstance�ӿ���
interface ITestForInstance : public Instance
{
    /// ���ýӿ�
    virtual void SetTestNo(DWORD dwTestNo) = 0;

    /// ��ѯ�ӿ�
    virtual DWORD GetTestNo() = 0;
};


/// ����CTestForInstanceʵ����
class CTestForInstance : public ITestForInstance
{
public:
    CTestForInstance(Instance *piParent, int argc, char **argv);
    ~CTestForInstance();

    DCOP_DECLARE_INSTANCE;

    void SetTestNo(DWORD dwTestNo);

    DWORD GetTestNo();

private:
    DWORD m_dwTestNo;
};


/// ����frameworks Instance Base
class CTestSuiteIBase : public ITestSuite
{
public:
    CTestSuiteIBase();
    ~CTestSuiteIBase();

    void BeforeTest();
    void AfterTest();

    int TestEntry(int argc, char* argv[]);

private:
    Instance *m_piBase;
    ITestForInstance *m_piTestForInstance;
};


#endif // #ifndef _TEST_INSTANCE_H_

