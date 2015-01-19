/// -------------------------------------------------
/// bytes.h : �ֽ���ش���
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_CPU_BYTES_H_
#define _TOOL_CPU_BYTES_H_

#include "dcop.h"


//////////////////////////////////////////////////////
/// ��ͬ�� CPU �в�ͬ���ֽ�������, ��Щ�ֽ�����ָ�������ڴ��б����˳��, �������������
/// �����������:
/// 1. Little endian:   �������ֽڴ洢����ʼ��ַ
/// 2. Big endian:      �������ֽڴ洢����ʼ��ַ

/// LE little-endian
/// ������˵�˼ά���ֽ���
/// ��ַ��λ�洢ֵ�ĵ�λ
/// ��ַ��λ�洢ֵ�ĸ�λ
/// ��ô����������˵�˼ά���ֽ�������Ϊ���˵ĵ�һ�۸���˵
/// ��λֵС����Ӧ�÷����ڴ��ַС�ĵط���Ҳ���ڴ��ַ��λ
/// ��֮����λֵ��Ӧ�÷����ڴ��ַ��ĵط���Ҳ���ڴ��ַ��λ

/// BE big-endian
/// ��ֱ�۵��ֽ���
/// ��ַ��λ�洢ֵ�ĸ�λ
/// ��ַ��λ�洢ֵ�ĵ�λ
/// Ϊʲô˵ֱ�ۣ���Ҫ���Ƕ�Ӧ��ϵ
/// ֻ��Ҫ���ڴ��ַ�����Ұ����ɵ͵��ߵ�˳��д��
/// ��ֵ����ͨ���ĸ�λ����λ��˳��д��
/// ���߶��գ�һ���ֽ�һ���ֽڵ�����ȥ

/// [����] ���ڴ���˫�� 0x01020304(DWORD) �Ĵ洢��ʽ
/// �ڴ��ַ    00  01  02  03
///     LE      04  03  02  01
///     BE      01  02  03  04

/// �����ֽ�˳���� TCP/IP �й涨�õ�һ�����ݱ�ʾ��ʽ���������� CPU ���͡�����ϵͳ���޹أ�
/// �Ӷ����Ա�֤�����ڲ�ͬ����֮�䴫��ʱ�ܹ�����ȷ���͡������ֽ�˳����� big endian ����ʽ��

/// Ϊ�˽���ת�� bsd socket �ṩ��ת���ĺ��� �������ĸ�
/// htons �� unsigned short ���ʹ�������ת����������
/// htonl �� unsigned long ���ʹ�������ת����������
/// ntohs �� unsigned short ���ʹ�������ת����������
/// ntohl �� unsigned long ���ʹ�������ת����������

/// ��ʹ�� little endian ��ϵͳ�� ��Щ��������ֽ������ת��
/// ��ʹ�� big endian ���͵�ϵͳ�� ��Щ�����ᶨ��ɿպ�

/// ͬ�� ��������򿪷�ʱ ���ǿ�ƽ̨����ʱ ҲӦ��ע�Ᵽֻ֤��һ���ֽ��� ��Ȼ�����Ľ��Ͳ�һ���ͻ���� bug!

/// ��ע��
/// 1 �������������ֽ�ת������ :htons ntohs htonl ntohl (s ���� short l �� long h �� host n �� network)
/// 2 ����ͬ�� CPU �����в�ͬ�Ĳ���ϵͳ���ֽ���Ҳ�ǲ�ͬ�ģ��μ��±�:
///     ������      ����ϵͳ        �ֽ�����
///     Alpha       ȫ��            Little endian
///     HP-PA       NT              Little endian
///     HP-PA       UNIX            Big endian
///     Intelx86    ȫ��            Little endian   <-----x86 ϵͳ��С���ֽ���ϵͳ
///     MIPS        NT              Little endian
///     MIPS        UNIX            Big endian
///     PowerPC     NT              Little endian
///     PowerPC     �� NT           Big endian      <-----PPC ϵͳ�Ǵ���ֽ���ϵͳ
///     RS/6000     UNIX            Big endian
///     SPARC       UNIX            Big endian
///     ARM ����    ȫ��            Little endian   <-----ARM ϵͳ��С���ֽ���ϵͳ

/// [����] http://blog.csdn.net/zhaojiangwei102/archive/2009/09/08/4532184.aspx
//////////////////////////////////////////////////////


//////////////////////////////////////////////////////
/// [��ʾ] Ϊ�����Ч��, ���ĵĺ�����У���������
//////////////////////////////////////////////////////


/// �����ֽ���ת������ṹ����
typedef struct tagBYTES_CHANGE_RULE
{
    DWORD size : 8;                         // �ֽ���
    DWORD pos : 24;                         // λ��
}BYTES_CHANGE_RULE;


#ifdef __cplusplus
extern "C" {
#endif


/// ��BUF�л�ȡ2�ֽ�(�ֽ���ת��)
extern WORD  Bytes_GetWord(const BYTE *cpbyBuf);

/// ��2�ֽ����õ�BUF��(�ֽ���ת��)
extern void  Bytes_SetWord(BYTE *pbyBuf, WORD wValue);

/// ��BUF�л�ȡ4�ֽ�(�ֽ���ת��)
extern DWORD Bytes_GetDword(const BYTE *cpbyBuf);

/// ��4�ֽ����õ�BUF��(�ֽ���ת��)
extern void  Bytes_SetDword(BYTE *pbyBuf, DWORD dwValue);

/// ���ݳ��ȴ�BUF�л�ȡDWORDֵ(���ȿ���Ϊ1��2��4������ֻȡǰ4���ֽ�)
extern DWORD Bytes_GetDwordValue(const BYTE *cpbyBuf, DWORD dwBufLen);

/// ���ݳ�������DWORDֵ��BUF��(���ȿ���Ϊ1��2��4������ֻдǰ4���ֽ�)
extern void  Bytes_SetDwordValue(DWORD dwValue, BYTE *pbyBuf, DWORD dwBufLen);

/// ��BUF�е����ݰ�������������ת��
extern void  Bytes_ChangeOrderByRule(const BYTES_CHANGE_RULE *adwRule, 
                    DWORD dwRuleCount, 
                    void *pBuf, 
                    DWORD dwBufLen);


#ifdef __cplusplus
}
#endif


#endif // #ifndef _TOOL_CPU_BYTES_H_

