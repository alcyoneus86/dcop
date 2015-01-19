/// -------------------------------------------------
/// ObjTimer_if.h : ��ʱ�����󹫹�ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJTIMER_IF_H_
#define _OBJTIMER_IF_H_

#include "Object_if.h"
#include "string/dstring.h"


/////////////////////////////////////////////////////
///                    ʹ��˵��
/// -------------------------------------------------
/// 1. ��ʱ��ʱ�䵥λΪ����(ms)������Ϊ1s��100ms����:
///    30s(��30s)���ϵĳ�ʱʱ�䣬ʹ�þ���Ϊ1s��
///    30s���ڵĳ�ʱʱ�䣬ʹ�þ���Ϊ100ms��
/// 2. �����ǳ�ʱ��ʱ������ѭ����ʱ������Ҫ����ֹͣ��
///    ��������Ѿ���ʱ�����Ǿ��û���ͷš�
/// 3. ���������Ҫ�ڱ����г�ʱ���еģ�
///    ����ʹ��ʱ����: IWheel
/////////////////////////////////////////////////////


/// ����ITimer�汾��
INTF_VER(ITimer, 1, 0, 0);


/// ��ʱ���ӿ�
interface ITimer : public IObject
{
    /////////////////////////////////////////////////
    /// ���Ķ�ʱ���ӿ�
    /// ---------------------------------------------
    /// �ɶ�ʱ�������Լ��ṩ��Ψһһ�����õĶ�ʱ������
    /////////////////////////////////////////////////

    /// ��ʱ������
    enum TYPE
    {
        TYPE_LOOP = 0,                              // ѭ����ʱ��
        TYPE_NOLOOP                                 // ��ʱ��ʱ��
    };

    /// ��ʱ�����
    interface IWheel;
    typedef struct Node
    {
        IWheel *                m_wheel;            // ʱ���ֶ���
        Node *                  m_timer;            // ��ʱ�����
        DWORD                   m_index;            // ��λ����ֵ
        DWORD                   m_tbase;            // ʱ�����ֵ
    } * Handle;                                     // ��ʱ�����

    /// ����һ����ʱ��(����NULLΪʧ�ܣ�����Ϊ�ɹ�)
    virtual Handle Start(
                        TYPE timerType,             // ��ʱ������
                        DWORD dwEvent,              // ��ʱ���¼�
                        DWORD dwTimeOut,            // ��ʱ����ʱʱ��
                        IObject *recver             // ��ʱ�����ն���
                        ) = 0;

    /// ֹͣһ����ʱ��
    virtual void Stop(
                        Handle hTimer               // ��ʱ�����
                        ) = 0;


    /////////////////////////////////////////////////
    /// ʱ���ֶ��нӿ�
    /// ---------------------------------------------
    /// �����������ʹ����Щ�ӿ��������Լ��Ķ�ʱ������
    /////////////////////////////////////////////////

    interface IWheel
    {
        /// ��ʱ����ص�����
        typedef void (*TIMEOUT_PROC)(ITimer::Handle handle, void *para);

        /// ����ʱ����ʵ��
        static IWheel *CreateInstance(DWORD dwWheelID,
                        DWORD dwSlotCount,
                        IWheel *pHigherWheel,
                        IWheel *pLowerWheel,
                        DWORD dwHashBase,
                        TIMEOUT_PROC fnTimeoutProc,
                        void *pTimeoutPara);

        /// ���䶨ʱ�����(�����ŵ���ʱ���ڵ�ֵ�ĵ�ַ�ͳ���)
        static ITimer::Handle Alloc(void *pValue, DWORD dwLen);

        /// �ͷŶ�ʱ�����(ֹͣ���ͷž��)
        static void Free(ITimer::Handle handle);

        /// ���ö�ʱ���ڵ�ֵ(�����Alloc�����õĲ��ԣ����øýӿ���������)
        static void SetValue(ITimer::Handle handle, void *pValue, DWORD dwLen);

        /// ���ݶ�ʱ�������ȡ��ʱ���ڵ�ֵ
        static void *GetValue(ITimer::Handle handle);

        /// ���ݶ�ʱ�������ȡ��ʱ���ֶ���
        static IWheel *GetWheel(ITimer::Handle handle);

        /// ����ʱ���ֵ����ʱ�������(�����test_timer.cpp��ʹ��)
        static void SetTimeBase(ITimer::Handle handle, DWORD tbase);

        /// ��ȡʱ���ֵ
        static DWORD GetTimeBase(ITimer::Handle handle);

        /// ��ȡ��λ����ֵ
        static DWORD GetIndex(ITimer::Handle handle);

        /// ��ȡѭ��Ȧ��
        static DWORD GetCycle(ITimer::Handle handle);

        /// ��ȡʵ�ʾ��ֵ(���㸴�ƶ�ʱ����Ϣ)
        static ITimer::Handle GetHandle(ITimer::Handle handle);

        /// ��ȡ��ʾ�ַ���
        static void GetString(ITimer::Handle handle, CDString &str);

        /// ������
        virtual ~IWheel();

        /// ��ʱ������
        virtual void OnTick() = 0;

        /// ���붨ʱ�����
        virtual DWORD Add(ITimer::Handle handle) = 0;

        /// ɾ����ʱ�����
        virtual void Del(ITimer::Handle handle) = 0;

        /// ��ȡ��ʱ����ID
        virtual DWORD GetWheelID() = 0;

        /// ��ȥHASH��ֵ(�����test_timer.cpp��ʹ��)
        virtual DWORD GetHashBase() = 0;

        /// ��ȡ�̶ȵ�(�����test_timer.cpp��ʹ��)
        virtual DWORD GetScalePoint() = 0;

        /// ��ȡ��λ��
        virtual DWORD GetSlotCount() = 0;
    };

};


#endif // #ifndef _OBJTIMER_IF_H_

