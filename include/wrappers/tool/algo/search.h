/// -------------------------------------------------
/// search.h : �����㷨����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_ALGO_SEARCH_H_
#define _TOOL_ALGO_SEARCH_H_

#ifdef __cplusplus
extern "C" {
#endif


/// ���ַ����ҵĵ����汾
int binary_search(int array[], int n, int v);

/// ���ַ����ҵĵݹ�汾
int binary_search_recurse(int array[], int low, int high, int v);


#ifdef __cplusplus
}
#endif

#endif // #ifndef _TOOL_ALGO_SEARCH_H_

