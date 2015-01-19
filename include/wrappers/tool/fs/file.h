/// -------------------------------------------------
/// file.h : �ļ�������װ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_FS_FILE_H_
#define _TOOL_FS_FILE_H_

#include "dcop.h"


/////////////////////////////////////////////////////
/// 'r'  ֻ����ʽ�򿪣����ļ�ָ��ָ���ļ�ͷ������ļ������ڣ���File���ؿա�
/// 'r+' ��д��ʽ�򿪣����ļ�ָ��ָ���ļ�ͷ������ļ������ڣ���File���ؿա� 
/// 'w'  д�뷽ʽ�򿪣����ļ�ָ��ָ���ļ�ͷ�����ļ���С��Ϊ�㡣����ļ����������Դ���֮�� 
/// 'w+' ��д��ʽ�򿪣����ļ�ָ��ָ���ļ�ͷ�����ļ���С��Ϊ�㡣����ļ����������Դ���֮�� 
/// 'a'  д�뷽ʽ�򿪣����ļ�ָ��ָ���ļ�ĩβ������ļ����������Դ���֮�� 
/// 'a+' ��д��ʽ�򿪣����ļ�ָ��ָ���ļ�ĩβ������ļ����������Դ���֮�� 
/// 'x'  ��������д�뷽ʽ�򿪣����ļ�ָ��ָ���ļ�ͷ������ļ��Ѵ��ڣ��� fopen() ����ʧ�ܲ����� FALSE�� 
/// 'x'  ��������д�뷽ʽ�򿪣����ļ�ָ��ָ���ļ�ͷ������ļ��Ѵ��ڣ��� fopen() ����ʧ�ܲ����� FALSE��
/////////////////////////////////////////////////////


/// �ļ�������(��·��)
#define DCOP_FILE_NAME_LEN                  256


#ifdef __cplusplus
    extern "C" {
#endif

DWORD DCOP_SaveToFile(const char *szFile, const void *cpBuf, DWORD dwCount, DWORD dwOffset);
DWORD DCOP_RestoreFromFile(const char *szFile, void *cpBuf, DWORD dwCount, DWORD dwOffset);
BOOL  DCOP_IsFileExist(const char *szFile);
DWORD DCOP_GetFileLen(const char *szFile);

#ifdef __cplusplus
    }
#endif


#endif // #ifndef _TOOL_FS_FILE_H_

