/// -------------------------------------------------
/// test_algo.h : ��Ҫ�����㷨����
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_ALGO_H_
#define _TEST_ALGO_H_

#include "test.h"


/// �㷨������
class CTestSuiteAlgo : public ITestSuite
{
public:
    CTestSuiteAlgo();
    ~CTestSuiteAlgo();

    int TestEntry(int argc, char* argv[]);

private:
    int BinarySearchEntry();
    int BaseNCase(unsigned int N, unsigned int num);
    int BaseNEntry();
};


#endif // #ifndef _TEST_ALGO_H_

