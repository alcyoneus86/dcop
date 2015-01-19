/// -------------------------------------------------
/// test_stream.h : ��Ҫ����������
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TEST_STREAM_H_
#define _TEST_STREAM_H_

#include "test.h"


/// ����stream
class CTestSuiteStream : public ITestSuite
{
public:
    struct TestNode
    {
        DWORD m1;
        WORD m2;
        BYTE m3;
        BYTE m4;
    };

public:
    CTestSuiteStream();
    ~CTestSuiteStream();

    int TestEntry(int argc, char* argv[]);
};


#endif // #ifndef _TEST_STREAM_H_

