/// -------------------------------------------------
/// ObjTimer_wheel.h : ��ʱ����ʱ����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJTIMER_WHEEL_H_
#define _OBJTIMER_WHEEL_H_


/////////////////////////////////////////////////////
///                    ʱ����
/// -------------------------------------------------
/// https://www.ibm.com/developerworks/cn/linux/l-cn-timers
/// ʱ���� (Timing-Wheel) �㷨������һ�Ժ㶨�ٶ���ת��
/// ������ǹ��ǹ��ײ����ײ��ǹ�ţ����ǹ�������ӵ�����
/// �ᱻ��������֮���Ӧ���ǣ����� PerTickBookkeeping��
/// ����ʵĹ��������� Tick Ϊ��λ����ʱ�ӣ��������
/// ���κζ�ʱ�����ڣ��������Ӧ�� ExpiryProcessing ��
/// �趨һ��ѭ��Ϊ N �� Tick ��Ԫ����ǰʱ������ S ��ѭ
/// ��֮��ָ��Ԫ�� i (i>=0 and i<= N - 1)����ǰʱ�� 
/// (Current Time)Tc ���Ա�ʾΪ��Tc = S*N + i �������
/// ʱ����һ��ʱ���� (Time Interval) Ϊ Ti �Ķ�ʱ����
/// �趨���������Ԫ�� n(Next) �У��� n = ��Tc + Ti��
/// mod N = (S*N + i + Ti) mod N = (i + Ti) mod N ��
/// ������ǵ� N �㹻�Ĵ���Ȼ StartTimer��StopTimer��
/// PerTickBookkeeping ʱ���㷨���Ӷȷֱ�Ϊ O(1)��O(1)��
/// O(1) ��
/// �����Ҫ֧�ֵĶ�ʱ����Χ�ǳ��Ĵ������ʵ�ַ�ʽ��
/// ��������������������Ϊ���������ķǳ��ɹ۵��ڴ棬
/// ������Ҫ��ʾ�Ķ�ʱ����ΧΪ��0 �C 2^3-1ticks�����
/// ��ʱ������Ҫ 2^32 ��Ԫ�ؿռ䣬������ڴ�ռ��ʹ��
/// ���ǳ����Ӵ�
/// ˮ���У�Ϊ�˱�ʾ������Χ���ֳ��˲�ͬ�ĵ�λ������ 
/// 1000��100��10 �ȵȣ����Ƶģ���ʾһ�� 32bits �ķ�Χ��
/// Ҳ����Ҫ 2^32 ��Ԫ�ص����顣
/////////////////////////////////////////////////////

#include "ObjTimer_if.h"
#include "list/dllist.h"


/// ��ʱ����Сֵ
#define TIMER_VALUE_MIN         100

/// ��ʱ��ʱ����
class CTimerWheel : public ITimer::IWheel
{
public:
    /// TimerNode�Ƕ�ʱ�����нڵ�
    ///     ��ӵ�������ʱӦ�����»�ȡʱ���ֵ
    struct TimerNode : ITimer::Node
    {
        DLL_ENTRY(TimerNode)    m_field;            // �����ֶ�
        DWORD                   m_cycle;            // ѭ��Ȧ��
        DWORD                   m_extra;            // ����ʱ��
    };
    /// ��ʱ����ֵm_extra������: 
    ///     ����ֵ���ǵͼ��ֵĵ�ǰλ�õ�ֵ����Ҫ�����õ�����ֵ:
    ///     ��һ�̶ֿ�ֵ(�н�1~2��)��ʵ���϶����˵ͼ�ֵ�ĵ�ǰֵ(����ֵ)��
    ///     ��������һ�ֺ�����һ�α���ĸ���ֵ�����Ǿͱ������:
    ///     "��һ�̶ֿ�ֵ - ����ֵ + ����ֵ + ��ʱʱ�� - ��һ����Ȧֵ"
    ///     �� "��һ�̶ֿ�ֵ" == "��һ����Ȧֵ"����������Ĺ�ʽ����:
    ///     "��ʱʱ��"�Լ���

    typedef DLL_HEAD(TimerNode) TimerSlot;          // ʱ���ֲ�λ

public:
    CTimerWheel();
    ~CTimerWheel();

    DWORD Init(DWORD dwWheelID,
            DWORD dwSlotCount,
            CTimerWheel *pHigherWheel,
            CTimerWheel *pLowerWheel,
            DWORD dwHashBase,
            TIMEOUT_PROC fnTimeoutProc,
            void *pTimeoutPara);

    void OnTick();

    DWORD Add(ITimer::Handle handle);
    void Del(ITimer::Handle handle);

    DWORD GetWheelID() {return m_dwWheelID;}
    DWORD GetHashBase() {return m_dwHashBase;}
    DWORD GetScalePoint() {return m_dwScalePoint;}
    DWORD GetSlotCount() {return m_dwSlotCount;}

private:
    void SlotTimeout(TimerSlot *pSlot);
    void HandleTimeout(ITimer::Handle handle);
    
private:
    CTimerWheel *           m_pHigherWheel;         // �߿̶�����
    CTimerWheel *           m_pLowerWheel;          // �Ϳ̶�����
    DWORD                   m_dwWheelID;            // ʱ����ID
    DWORD                   m_dwHashBase;           // HASH��ֵ
    TimerSlot *             m_pSlotTable;           // ʱ���ֲ�λ��
    DWORD                   m_dwSlotCount;          // ʱ���ֲ�λ����
    DWORD                   m_dwScalePoint;         // ʱ���̶ֿ�ָ��
    TIMEOUT_PROC            m_fnTimeoutProc;        // ��ʱ����ص�
    void *                  m_pTimeoutPara;         // ��ʱ����ص�����
};


#endif // #ifndef _OBJTIMER_WHEEL_H_

