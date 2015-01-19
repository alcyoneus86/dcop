/// -------------------------------------------------
/// test_ibase.cpp : ��Ҫ����frameworks ibase����
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "test_ibase.h"
#include "Factory_if.h"


/// -------------------------------------------------
/// INSTANCE ��������
/// -------------------------------------------------
TEST_SUITE_TABLE(IBASE)
    TEST_SUITE_ITEM(CTestSuiteIBase)
        TEST_CASE_ITEM(1)
            "1"
        TEST_CASE_ITEM_END
        TEST_CASE_ITEM(1)
            "10"
        TEST_CASE_ITEM_END
        TEST_CASE_ITEM(1)
            "100"
        TEST_CASE_ITEM_END
        TEST_CASE_ITEM(1)
            "1000"
        TEST_CASE_ITEM_END
        TEST_CASE_ITEM(1)
            "4294967295"
        TEST_CASE_ITEM_END
    TEST_SUITE_ITEM_END
TEST_SUITE_TABLE_END

/// -------------------------------------------------
/// INSTANCE ������
/// -------------------------------------------------
IMPLEMENT_REGTESTSUITE_FUNC(IBASE)

/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CTestForInstance, "TestForInstance")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CTestForInstance)
    DCOP_IMPLEMENT_INTERFACE(ITestForInstance)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END


CTestForInstance::CTestForInstance(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);

    m_dwTestNo = 0;
}

CTestForInstance::~CTestForInstance()
{
    m_dwTestNo = 0;

    DCOP_DESTRUCT_INSTANCE();
}

void CTestForInstance::SetTestNo(DWORD dwTestNo)
{
    m_dwTestNo = dwTestNo;
}

DWORD CTestForInstance::GetTestNo()
{
    return m_dwTestNo;
}


/// ================ ����Instanceʵ���� ================

CTestSuiteIBase::CTestSuiteIBase()
{
    m_piBase = 0;
    m_piTestForInstance = 0;
}

CTestSuiteIBase::~CTestSuiteIBase()
{
    m_piBase = 0;
    m_piTestForInstance = 0;
}

void CTestSuiteIBase::BeforeTest()
{
    DCOP_CREATE_INSTANCE(Instance, "TestForInstance", 0, 0, 0, m_piBase);
}

void CTestSuiteIBase::AfterTest()
{
    DCOP_RELEASE_INSTANCE_REFER(0, m_piBase);
}

int CTestSuiteIBase::TestEntry(int argc, char* argv[])
{
    if ((argc < 1) || (!argv))
    {
        return -1;
    }

    int iTestNo = atoi(argv[0]);

    if (!m_piBase)
    {
        return -2;
    }

    if (m_piBase->QueryInterface(ID_INTF(ITestForInstance), REF_PTR(m_piTestForInstance)) != SUCCESS)
    {
        return -3;
    }

    m_piTestForInstance->SetTestNo((DWORD)iTestNo);
    if (m_piTestForInstance->GetTestNo() != (DWORD)iTestNo)
    {
        (void)m_piTestForInstance->Release();
        return -4;
    }

    (void)m_piTestForInstance->Release();
    return 0;
}

