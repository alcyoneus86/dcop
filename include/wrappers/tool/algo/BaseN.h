/// -------------------------------------------------
/// BaseN.h : 10��������N������֮��ı��뻻��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_ALGO_BASEN_H_
#define _TOOL_ALGO_BASEN_H_

#ifdef __cplusplus
extern "C" {
#endif


/// 10����ת��ΪN����
int DCOP_BaseN(unsigned int N, unsigned int num, unsigned int *array, unsigned int len, unsigned int *high);

/// N����ת��Ϊ10����
int DCOP_Base10(unsigned int N, unsigned int *array, unsigned int len, unsigned int *num);


#ifdef __cplusplus
}
#endif

#endif // #ifndef _TOOL_ALGO_BASEN_H_

