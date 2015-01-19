/// -------------------------------------------------
/// test.cpp : �������ͷ���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_TEST_H_
#define _TEST_TEST_H_

/// [gtest��ͷ�ļ�һ��Ҫ����ǰ�棬����ᱨ��]
#include <gtest/gtest.h>
#include "dcop.h"
#include "list/tailq.h"


/// -------------------------------------------------
/// �������������е�����������
/// -------------------------------------------------
#define TEST_CASE_ARG_COUNT_MAX 30

/// -------------------------------------------------
/// �����������е����������
/// -------------------------------------------------
#define TEST_SUITE_CASE_COUNT_MAX 30


/// -------------------------------------------------
/// ����������
/// -------------------------------------------------
class CTestCase
{
public:
    int argc;           // ��������
    char** argv;        // �����б�(��ָ�򱾶������Ŀռ�)

public:
    TAILQ_ENTRY(CTestCase) m_field;         // ָ����һ����������
};


/// -------------------------------------------------
/// �����׻���
/// -------------------------------------------------
class ITestSuite
{
public:
    ITestSuite();
    virtual ~ITestSuite();

    /// �����׿�ʼǰ��׼������
    virtual void BeforeTest();

    /// �������(ÿ���������������һ��;����0Ϊ�ɹ�,��0Ϊʧ��)
    virtual int TestEntry(int argc, char* argv[]);

    /// �����׽������������
    virtual void AfterTest();

    /// ע�������
    static void RegTestSuite(ITestSuite *pTest, const char *title = 0);

    /// ���úͻ�ȡ�����ױ���
    void SetTestTitle(const char *title) { m_testTitle = title; }
    const char *GetTestTitle() { return m_testTitle; }

    /// ���Խ��
    void SetTestResult(int rc) { (!rc)? m_testSuccessCount++ : m_testFailureCount++; }
    int GetSuccessCount() { return m_testSuccessCount; }
    int GetFailureCount() { return m_testFailureCount; }

public:
    TAILQ_ENTRY(ITestSuite) m_field;            // ָ����һ��������
    TAILQ_HEAD(CTestCase) m_testCases;          // �����Ĳ�����������
    const char *m_testTitle;                    // �����ױ���
    int m_testSuccessCount;                     // ���Գɹ�����������
    int m_testFailureCount;                     // ����ʧ�ܵ���������
};


/// -------------------------------------------------
/// ������ע��ṹ
/// -------------------------------------------------
typedef struct tagTestSuiteRegNode
{
    const char *testTitle;
    ITestSuite *testSuite;
    struct
    {
        int argc;
        char *argv[TEST_CASE_ARG_COUNT_MAX];
    } testCases[TEST_SUITE_CASE_COUNT_MAX];
}TestSuiteRegNode;


/// -------------------------------------------------
/// ������ע���ͷ
/// -------------------------------------------------
#define TEST_SUITE_TABLE(TestName)                  \
    TestSuiteRegNode g_aReg##TestName##TestSuiteTable[] = {


/// -------------------------------------------------
/// ������ע���β
/// -------------------------------------------------
#define TEST_SUITE_TABLE_END                        \
    };


/// -------------------------------------------------
/// ������ע�����
/// -------------------------------------------------
#define TEST_SUITE_ITEM(CTestSuite)                 \
    {#CTestSuite, new CTestSuite, {


/// -------------------------------------------------
/// �������еĲ��������б����
/// -------------------------------------------------
#define TEST_CASE_ITEM(num)                         \
    {num, {(char *)


/// -------------------------------------------------
/// �������еĲ��������б�������
/// -------------------------------------------------
#define TEST_CASE_ITEM_END                          \
    }},


/// -------------------------------------------------
/// ������ע��������
/// -------------------------------------------------
#define TEST_SUITE_ITEM_END                         \
    {0}}},


/// -------------------------------------------------
/// �����Խ��
/// -------------------------------------------------
#define TEST_CHECK(expected, actual, rc)            \
do {                                                \
    int expValue = 0;                               \
    int actValue = 0;                               \
    EXPECT_EQ((expValue = (int)(expected)), (actValue = (int)(actual))); \
    if (expValue != actValue) rc |= (-1);           \
} while (0)


/// -------------------------------------------------
/// ���μ��ض���
/// -------------------------------------------------
#define TEST_LOAD_OBJECT(Manager, Intf, Name, ID, Obj) \
    char *Name##_name = #Name;                      \
    char Name##_id[16];                             \
    (void)snprintf(Name##_id, sizeof(Name##_id), "%d", ID); \
    char *Name##_arg[] = {"-name", Name##_name, "-id", Name##_id}; \
    DCOP_CREATE_INSTANCE(Intf, #Name, Manager, ARRAY_SIZE(Name##_arg), Name##_arg, Obj); \
    (void)Manager->InsertObject(Obj)


/// -------------------------------------------------
/// ʵ�ֲ�����ע�ắ��
/// -------------------------------------------------
#define IMPLEMENT_REGTESTSUITE_FUNC(TestName)       \
    void vReg##TestName##TestSuiteFunc()            \
{                                                   \
    RegTestSuites(g_aReg##TestName##TestSuiteTable, ARRAY_SIZE(g_aReg##TestName##TestSuiteTable)); \
}


/// -------------------------------------------------
/// ���ò�����ע�ắ��
/// -------------------------------------------------
#define CALL_REGTESTSUITE_FUNC(ts)                  \
    extern void vReg##ts##TestSuiteFunc();          \
    vReg##ts##TestSuiteFunc()


/// -------------------------------------------------
/// ������ע�ắ��
/// -------------------------------------------------
extern void RegTestSuites(TestSuiteRegNode *pRegTable, int iRegCount);


/// -------------------------------------------------
/// �ⲿ���õĲ��������
/// -------------------------------------------------
extern int test_entry(int argc, char* argv[]);


#endif // #ifndef _TEST_TEST_H_

