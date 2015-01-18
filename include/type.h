/// -------------------------------------------------
/// type.h : �������Ͷ��幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _DCOP_TYPE_H_
#define _DCOP_TYPE_H_


///              linux64     windows64     linux32     windows32
/// char            1            1            1            1
/// short           2            2            2            2
/// int             4            4            4            4
/// long            8            4            4            4
/// long long       8            8            8            8
/// size_t          8            8            4            4


//////////////////////////////////////////////////////
/// �����������Ͷ��� - begin
//////////////////////////////////////////////////////
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
#if __linux__ // linux��Ϊ��֧��64λ��ʹ��int; windowsΪ�˺�MFC���ݣ�����ʹ��long
typedef unsigned int    DWORD;
#else
typedef unsigned long   DWORD;
#endif
typedef int             BOOL;
#ifndef NULL
#define NULL            (void *)0
#endif
#ifndef TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif
#ifndef DCOP_OPEN
#define DCOP_OPEN       1
#endif
#ifndef DCOP_CLOSE
#define DCOP_CLOSE      0
#endif
#ifndef DCOP_ENABLE
#define DCOP_ENABLE     1
#endif
#ifndef DCOP_DISABLE
#define DCOP_DISABLE    0
#endif
#ifndef DCOP_YES
#define DCOP_YES        1
#endif
#ifndef DCOP_NO
#define DCOP_NO         0
#endif
//////////////////////////////////////////////////////
/// �����������Ͷ��� - end
//////////////////////////////////////////////////////


/// ��ȡ�����С
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)       (sizeof(a)/sizeof((a)[0]))
#endif


/// ��ȡ���ֵ
#ifndef MAX
#define MAX(a, b)           (((a) > (b))? (a) : (b))
#endif

/// ��ȡ��Сֵ
#ifndef MIN
#define MIN(a, b)           (((a) < (b))? (a) : (b))
#endif


/// �ṹ�г�Աƫ��
#ifndef OFFSET_OF
#define OFFSET_OF(t, m)     ((size_t)&(((t *)0)->m))
#endif


/// �ṹ�г�Ա��С
#ifndef SIZE_OF
#define SIZE_OF(t, m)       (sizeof(((t *)0)->m))
#endif


/// һǧ
#define THOUSAND            1000

/// ����
#define MILLION             1000000

/// ʮ��
#define BILLION             1000000000


//////////////////////////////////////////////////////
/// WIN32ƽ̨��������Ͷ��� - begin
//////////////////////////////////////////////////////

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_WINDOWS_)
#ifndef RP_FRAMEWORK_DLL

#ifndef FRAMEWORK_DLL

#define RP_FRAMEWORK_DLL
#define RP_FRAMEWORK_DLLV(type) type

#else // !FRAMEWORK_DLL

#define RP_FRAMEWORK_DLLV(type) RP_FRAMEWORK_DLL

#ifdef RP_FRAMEWORK_EXPORTS

#define RP_FRAMEWORK_DLL __declspec(dllexport) 

#else // !RP_FRAMEWORK_EXPORTS

#define RP_FRAMEWORK_DLL __declspec(dllimport)

#endif // RP_FRAMEWORK_EXPORTS

#endif // FRAMEWORK_DLL
#endif // RP_FRAMEWORK_DLL

#else // !WIN32 ...
#define RP_FRAMEWORK_DLL
#define RP_FRAMEWORK_DLLV(type) type
#endif // #if defined(WIN32) ...

//////////////////////////////////////////////////////
/// WIN32ƽ̨��������Ͷ��� - end
//////////////////////////////////////////////////////


#endif // #ifndef _DCOP_TYPE_H_

