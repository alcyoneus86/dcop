/// -------------------------------------------------
/// test_mem.h : ��Ҫ�����ڴ����
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_MEM_H_
#define _TEST_MEM_H_

#include "test.h"


/// �����ڴ�������ͷż�¼������
class CTestMem
{
public:
    CTestMem() : m_iTest(0) {}
    ~CTestMem() {}

    void SetTest(int iTest) { m_iTest = iTest; }
    int GetTest() { return m_iTest; }

private:
    int m_iTest;
};


/// �����ڴ�������ͷż�¼������
class CTestSuiteMem : public ITestSuite
{
public:
    CTestSuiteMem();
    ~CTestSuiteMem();

    int TestEntry(int argc, char* argv[]);

};


#endif // #ifndef _TEST_MEM_H_

