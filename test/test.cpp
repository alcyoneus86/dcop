/// -------------------------------------------------
/// test.cpp : �������ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "test.h"
#include <assert.h>

#if DCOP_OS == DCOP_OS_WINDOWS
#include "console/color/ConsoleColor.h"
#pragma comment(lib, "gtestd")
#endif


/// -------------------------------------------------
/// ǿ������.a���е�һЩԭ���Զ�ִ�е�ȫ�ֹ�������
/// -------------------------------------------------
CPPBUILDUNIT_FORCELINK(vRegOsTaskStubFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegOsSemStubFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCObjectManagerToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDispatchToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCNotifyToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCControlToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCTimerToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCScheduleToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCStatusToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCResponseToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCModelToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataMemToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataFileToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataMySQLToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCTlvToSQLToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCConnectToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCProxyToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCSessionToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCUserToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCSecureToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCAccessToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCCommandToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpServerToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpRequestToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpProcessToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpJsonToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpResponseToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCMonitorToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCAppBaseToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCPythonXToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCLuaXToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCTestForIObjectAToFactoryFunc, 0);


/// -------------------------------------------------
/// �����׹�������ʼ��
/// -------------------------------------------------
TAILQ_HEAD(ITestSuite) g_testSuites;            // ȫ�ֲ����׶���
int g_testCasesCount;                           // ȫ�ֲ�����������
int g_testSuccessCount;                         // ȫ�ֲ��Գɹ�����������
int g_testFailureCount;                         // ȫ�ֲ���ʧ�ܵ���������
void vInitTestSuitesFunc()
{
    /// ��ʼ�������׹�����
    TAILQ_INIT(&g_testSuites);
    g_testCasesCount = 0;
    g_testSuccessCount = 0;
    g_testFailureCount = 0;

    /// ��������µĲ�����
#if 0
    CALL_REGTESTSUITE_FUNC(TASK);
#endif
    CALL_REGTESTSUITE_FUNC(GRAPH);
    CALL_REGTESTSUITE_FUNC(MEM);
    CALL_REGTESTSUITE_FUNC(XML);
    CALL_REGTESTSUITE_FUNC(AVL);
    CALL_REGTESTSUITE_FUNC(IBASE);
    CALL_REGTESTSUITE_FUNC(IOBJECT);
    CALL_REGTESTSUITE_FUNC(OBJATTR);
    CALL_REGTESTSUITE_FUNC(TIMER);
    CALL_REGTESTSUITE_FUNC(DATA);
    CALL_REGTESTSUITE_FUNC(STREAM);
    CALL_REGTESTSUITE_FUNC(MD5);
    CALL_REGTESTSUITE_FUNC(ALGO);
    CALL_REGTESTSUITE_FUNC(RANDOM);

}
/// �����Զ���ӵ�������
/// CPPBUILDUNIT_AUTO(vInitTestSuitesFunc, 0);


/// -------------------------------------------------
/// �����׹��������캯��
/// -------------------------------------------------
ITestSuite::ITestSuite()
{
    TAILQ_NEXT(this, m_field) = 0;
    TAILQ_INIT(&m_testCases);

    m_testTitle = "";
    m_testSuccessCount = 0;
    m_testFailureCount = 0;
}

/// -------------------------------------------------
/// �����׹�������������
/// -------------------------------------------------
ITestSuite::~ITestSuite()
{
    TAILQ_NEXT(this, m_field) = 0;
    CTestCase *pTestCase = TAILQ_FIRST(&m_testCases);
    while (pTestCase)
    {
        CTestCase *pTmp = pTestCase;
        pTestCase = TAILQ_NEXT(pTestCase, m_field);

        free((void *)pTmp);
        pTmp = 0;
    }
}

/// -------------------------------------------------
/// �����׹�����Ĭ��ʵ�ֲ���ǰ
/// -------------------------------------------------
void ITestSuite::BeforeTest()
{
}

/// -------------------------------------------------
/// �����׹�����Ĭ��ʵ�ֲ������
/// -------------------------------------------------
int ITestSuite::TestEntry(int argc, char* argv[])
{
    return -1;
}

/// -------------------------------------------------
/// �����׹�����Ĭ��ʵ�ֲ��Ժ�
/// -------------------------------------------------
void ITestSuite::AfterTest()
{
}

/// -------------------------------------------------
/// �����׹�����ע��һ��������
/// -------------------------------------------------
void ITestSuite::RegTestSuite(ITestSuite *pTestSuite, const char *title)
{
    assert(pTestSuite);
    TAILQ_PUSH_BACK(&g_testSuites, pTestSuite, m_field);
    pTestSuite->SetTestTitle(title);
}

/// -------------------------------------------------
/// ȫ��������в�����
/// -------------------------------------------------
void ClrTestSuites()
{
    ITestSuite *pTestSuite = TAILQ_FIRST(&g_testSuites);
    while (pTestSuite)
    {
        ITestSuite *pTmp = pTestSuite;
        pTestSuite = TAILQ_NEXT(pTestSuite, m_field);

        delete pTmp;
        pTmp = 0;
    }
}

/// -------------------------------------------------
/// ȫ��ע��һ��������
/// -------------------------------------------------
void RegTestSuites(TestSuiteRegNode *pRegTable, int iRegCount)
{
    for (int i = 0; i < iRegCount; ++i)
    {
        ITestSuite::RegTestSuite(pRegTable[i].testSuite, pRegTable[i].testTitle);
        int j = 0;
        while (pRegTable[i].testCases[j].argc)
        {
            CTestCase *pTestCaseObj = (CTestCase *)malloc(sizeof(CTestCase) + 
                            pRegTable[i].testCases[j].argc * sizeof(char *));
            pTestCaseObj->argc = pRegTable[i].testCases[j].argc;
            pTestCaseObj->argv = (char **)(pTestCaseObj + 1);
            TAILQ_NEXT(pTestCaseObj, m_field) = 0;
            (void)memcpy((void *)(pTestCaseObj->argv), 
                         (void *)(pRegTable[i].testCases[j].argv), 
                         pTestCaseObj->argc * sizeof(char *));
            TAILQ_PUSH_BACK(&(pRegTable[i].testSuite->m_testCases), pTestCaseObj, m_field);
            j++;
            g_testCasesCount++;
        }
    }
}

/// -------------------------------------------------
/// ��ӡ�������в���
/// -------------------------------------------------
void PrintTestArgs(int TestCaseNo, int argc, char* argv[], int bStartEnd, int rc = 0, DWORD cost = 0)
{
    printf("  [TC %d] %s", TestCaseNo++, (bStartEnd)? "Start(" : "End");
    if (bStartEnd) for (int i = 0; i < argc; ++i) { printf( (((i + 1) != argc)? "%s," : "%s) ... \n"), argv[i]); }
    else printf("(rc:%d)<costtime:%dms>!\n", rc, cost);
}

/// -------------------------------------------------
/// �������в�������
/// -------------------------------------------------
void CallTestCases(ITestSuite *pTestSuite)
{
    int rc = 0;
    int i = 0;

    if (!pTestSuite)
    {
        return;
    }

    CTestCase *pTestCase = TAILQ_FIRST(&(pTestSuite->m_testCases));
    if (!pTestCase)
    {
        /// ��������Ϊ��, ֱ�ӵ��ò������
        PrintTestArgs(0, 0, 0, true);
        rc = pTestSuite->TestEntry(0, 0);
        PrintTestArgs(0, 0, 0, false, rc);

        EXPECT_EQ(0, rc);

        /// ¼����Խ��
        pTestSuite->SetTestResult(rc);
        (void)((!rc)? g_testSuccessCount++ : g_testFailureCount++);

        return;
    }

    /// ѭ���������в�������
    while (pTestCase)
    {
        PrintTestArgs(++i, pTestCase->argc, pTestCase->argv, true);
        DCOP_START_TIME();
        rc = pTestSuite->TestEntry(pTestCase->argc, pTestCase->argv);
        DCOP_END_TIME();
        PrintTestArgs(i, pTestCase->argc, pTestCase->argv, false, rc, DCOP_COST_TIME());

        EXPECT_EQ(0, rc);

        pTestSuite->SetTestResult(rc);
        (void)((!rc)? g_testSuccessCount++ : g_testFailureCount++);

        pTestCase = TAILQ_NEXT(pTestCase, m_field);
    }

}

/// -------------------------------------------------
/// �ⲿ���õĲ��������
/// -------------------------------------------------
#include <iostream>
int main(int argc, char* argv[])
{
    std::cout << "Test Start ... " << std::endl;
    testing::InitGoogleTest(&argc, argv);
    int rc = RUN_ALL_TESTS();
    std::cout << "Test End! Press <Enter> To Quit ... " << std::endl;
    std::cin.get();
    return rc;
}

/// -------------------------------------------------
/// gtest���õĲ������
/// -------------------------------------------------
TEST(test_main, TestBaseAssert)
{
    /// ����־��¼
    DebugLogStatus(1);

    /// ���ڴ����
    DebugMemStatus(1);
    RecordAllocCallstack(1);
    OutputMemLog(1);

    /// ��ʼ��������
    vInitTestSuitesFunc();

    /// ��ʼ����
    int i = 0;
    ITestSuite *pTestSuite = TAILQ_FIRST(&g_testSuites);
    while (pTestSuite)
    {
        printf("\n");
        printf("----------------------------------------------------------------------\n");
        printf("[TS %d '%s'] Start ... ... \n", ++i, pTestSuite->GetTestTitle());
        printf("----------------------------------------------------------------------\n");
        pTestSuite->BeforeTest();

        CallTestCases(pTestSuite);

        pTestSuite->AfterTest();
        printf("----------------------------------------------------------------------\n");
        printf("[TS %d '%s'] End(testcase:%d, success:%d, failure:%d)! \n", i, 
            pTestSuite->GetTestTitle(), TAILQ_COUNT(&(pTestSuite->m_testCases)), 
            pTestSuite->GetSuccessCount(), pTestSuite->GetFailureCount());
        printf("----------------------------------------------------------------------\n");
        printf("\n");

        pTestSuite = TAILQ_NEXT(pTestSuite, m_field);
    }

    printf("######################################################################\n");
    if (!g_testFailureCount)
        std::cout 
                #if DCOP_OS == DCOP_OS_WINDOWS
                    << green 
                #endif
                            << "    [TEST CASE REPORT]    TOTAL: " << g_testCasesCount
                            << "    SUCCESS: " << g_testSuccessCount
                            << "    FAILURE: " << g_testFailureCount
                #if DCOP_OS == DCOP_OS_WINDOWS
                    << white 
                #endif
                            << std::endl;
    else
        std::cout 
                #if DCOP_OS == DCOP_OS_WINDOWS
                    << red 
                #endif
                            << "    [TEST CASE REPORT]    TOTAL: " << g_testCasesCount
                            << "    SUCCESS: " << g_testSuccessCount
                            << "    FAILURE: " << g_testFailureCount
                #if DCOP_OS == DCOP_OS_WINDOWS
                    << white 
                #endif
                            << std::endl;
    printf("######################################################################\n");

    /// ���Խ���
    ClrTestSuites();

    /// �ر��ڴ����
    /// DebugMemStatus(0);

    /// �ر���־��¼
    /// DebugLogStatus(0);
}

