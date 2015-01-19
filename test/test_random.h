/// -------------------------------------------------
/// test_random.h : ��Ҫ�������������
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_RANDOM_H_
#define _TEST_RANDOM_H_

#include "test.h"
#include "array/darray.h"


/// ����random
class CTestSuiteRandom : public ITestSuite
{
public:
    static const DWORD RAND_SIZE = 16;

public:
    CTestSuiteRandom();
    ~CTestSuiteRandom();

    int TestEntry(int argc, char* argv[]);

private:
    DWORD Append(void *pBuf);

private:
    CDArray m_aRandomList;
};


#endif // #ifndef _TEST_RANDOM_H_

