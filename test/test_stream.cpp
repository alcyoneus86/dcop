/// -------------------------------------------------
/// test_stream.cpp : ��Ҫ����������
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "test_stream.h"
#include "stream/dstream.h"


/// -------------------------------------------------
/// ��������
/// -------------------------------------------------
TEST_SUITE_TABLE(STREAM)
    TEST_SUITE_ITEM(CTestSuiteStream)
        TEST_CASE_ITEM(1)
            "1"
        TEST_CASE_ITEM_END
    TEST_SUITE_ITEM_END
TEST_SUITE_TABLE_END

/// -------------------------------------------------
/// ������
/// -------------------------------------------------
IMPLEMENT_REGTESTSUITE_FUNC(STREAM)


CTestSuiteStream::CTestSuiteStream()
{
    
}

CTestSuiteStream::~CTestSuiteStream()
{
    
}

int CTestSuiteStream::TestEntry(int argc, char* argv[])
{
    BYTE a1 = 1;
    WORD b1 = 2000;
    DWORD c1 = 0x30000;
    CDString d1 = "Test!";
    TestNode e1 = {1,2,3,4};

    BYTE a2 = 0;
    WORD b2 = 0;
    DWORD c2 = 0;
    CDString d2;
    TestNode e2;

    /// ������Ϊ�����ַ�����������������ʱĬ�ϲ�������������
    /// ��ˣ������ʱ�Ͳ�֪���ַ����ĳ�����(������Buf��ʽ���)��
    /// ���Ҫֱ������ַ����Ļ������ֶ�ָ������ʱ������������

    CDStream test;
    test.SetInputStrTerminal(true) << a1 << b1 << c1 << d1 << CBufferPara(&e1, sizeof(e1));

    if (test.AppendRecord() != SUCCESS)
    {
        return -1;
    }

    test << a1 << b1 << c1 << d1;

    CBufferPara f2((void *)&e2, sizeof(e2));
    /// test.SetOffset(0);
    test.SetCurRecord(0) >> a2 >> b2 >> c2 >> d2 >> f2;

    test.Dump(PrintToConsole);

    int rc = 0;

    TEST_CHECK(a1, a2, rc);
    TEST_CHECK(b1, b2, rc);
    TEST_CHECK(c1, c2, rc);
    TEST_CHECK(true, (d1 == d2), rc);
    TEST_CHECK(0, memcmp(&e1, &e2, sizeof(e2)), rc);

    return rc;
}

