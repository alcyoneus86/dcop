/// -------------------------------------------------
/// config.h : ���ö��幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _DCOP_CONFIG_H_
#define _DCOP_CONFIG_H_


//////////////////////////////////////////////////////
/// ����ϵͳƽ̨��(����ֻ�ڷ�װ��ʹ��) - begin
//////////////////////////////////////////////////////
#define DCOP_OS_NONE        0
#define DCOP_OS_WINDOWS     1
#define DCOP_OS_LINUX       2

#if defined(_WINDOWS) || defined(_WINDOWS_) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    #define DCOP_OS         DCOP_OS_WINDOWS
#elif defined(__linux) || defined(__linux__)
    #define DCOP_OS         DCOP_OS_LINUX
#else
    #define DCOP_OS         DCOP_OS_NONE
#endif
//////////////////////////////////////////////////////
/// ����ϵͳƽ̨��(����ֻ�ڷ�װ��ʹ��) - end
//////////////////////////////////////////////////////



/////////////////////////////////////////////////////
///             stlʹ�ö���
/// -------------------------------------------------
/// ����ʹ�ñ�׼��stl, ���Բ�ʹ��#include <>, ��ʹ��
/// #include "", ��Ϊ����������Ĭ�ϵ�Ŀ¼û�б�׼stl,
///     CB: Ĭ�ϵ���Ŀ¼��$(BCB)\include��, ���Ұ�
///         �����ı�׼stlͷ�ļ��������.h, ������CB��
///         Ҫ��ʾָ��·��$(BCB)\include\Stlport.
/// ��ʹ��STL���ļ���ͷ��Ҫ��������Ӧ�ĺ�,��: INC_MAP
/////////////////////////////////////////////////////
#if defined(INC_STRING)
    #ifdef _MSC_VER
        #pragma warning(disable:4786)
    #endif
    #include "string"
#endif

#if defined(INC_MAP)
    #ifdef _MSC_VER
        #pragma warning(disable:4786)
    #endif
#include "map"
#endif

#if defined(INC_SET)
#include "set"
#endif

#if defined(INC_DEQUE)
#include "deque"
#endif

#if defined(INC_LIST)
#include "list"
#endif
//////////////////////////////////////////////////////
/// stlʹ�ö��� - end
//////////////////////////////////////////////////////


#endif // #ifndef _DCOP_CONFIG_H_

