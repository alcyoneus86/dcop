/// -------------------------------------------------
/// ObjTimer_wheel.h : ��ʱ����ʱ����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "ObjTimer_wheel.h"


/*******************************************************
  �� �� ��: ITimer::IWheel::CreateInstance
  ��    ��: ����ʱ����ʵ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
ITimer::IWheel *ITimer::IWheel::CreateInstance(DWORD dwWheelID,
                        DWORD dwSlotCount,
                        IWheel *pHigherWheel,
                        IWheel *pLowerWheel,
                        DWORD dwHashBase,
                        IWheel::TIMEOUT_PROC fnTimeoutProc,
                        void *pTimeoutPara)
{
    CTimerWheel *pTimerWheel = new CTimerWheel();
    if (!pTimerWheel)
    {
        return NULL;
    }

    pTimerWheel->Init(dwWheelID, dwSlotCount, 
                        (CTimerWheel *)pHigherWheel, (CTimerWheel *)pLowerWheel, 
                        dwHashBase, fnTimeoutProc, pTimeoutPara);
    return pTimerWheel;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::~IWheel
  ��    ��: ITimer::IWheel����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
ITimer::IWheel::~IWheel()
{
}

/*******************************************************
  �� �� ��: ITimer::IWheel::Alloc
  ��    ��: ������
  ��    ��: 
  ��    ��: 
  ��    ��: ��ʱ�����
  �޸ļ�¼: 
 *******************************************************/
ITimer::Handle ITimer::IWheel::Alloc(void *pValue, DWORD dwLen)
{
    if (!pValue || !dwLen)
    {
        return NULL;
    }

    /// ����ڵ�ռ�
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)DCOP_Malloc(sizeof(CTimerWheel::TimerNode) + dwLen);
    if (!pNode)
    {
        return NULL;
    }

    /// ��ʼ������ֵ
    (void)memset(pNode, 0, sizeof(CTimerWheel::TimerNode) + dwLen);
    (void)memcpy(pNode + 1, pValue, dwLen);

    /// ת����ʱ�����
    ITimer::Handle hTimer = (ITimer::Handle)pNode;
    pNode->m_timer = hTimer;

    /// ���ؾ��
    return hTimer;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::Free
  ��    ��: �ͷž��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void ITimer::IWheel::Free(ITimer::Handle handle)
{
    if (!handle)
    {
        return;
    }

    DCOP_Free(handle);
}

/*******************************************************
  �� �� ��: CTimerWheel::SetValue
  ��    ��: �����û�ֵ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void ITimer::IWheel::SetValue(ITimer::Handle handle, void *pValue, DWORD dwLen)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return;
    }

    (void)memcpy(pNode + 1, pValue, dwLen);
}

/*******************************************************
  �� �� ��: ITimer::IWheel::GetValue
  ��    ��: ͨ�������ȡ��ʱ��ֵ
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: ��ʱ��ֵ
  �޸ļ�¼: 
 *******************************************************/
void *ITimer::IWheel::GetValue(ITimer::Handle handle)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return 0;
    }

    return pNode + 1;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::GetWheel
  ��    ��: ͨ�������ȡ��ʱ���ֶ���
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: ��ʱ���ֶ���
  �޸ļ�¼: 
 *******************************************************/
ITimer::IWheel *ITimer::IWheel::GetWheel(ITimer::Handle handle)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return 0;
    }

    return pNode->m_wheel;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::SetTimeBase
  ��    ��: ����ʱ���ֵ
  ��    ��: handle  - ��ʱ�����
            tbase   - ʱ���ֵ
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void ITimer::IWheel::SetTimeBase(ITimer::Handle handle, DWORD tbase)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return;
    }

    pNode->m_tbase = tbase;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::GetTimeBase
  ��    ��: ��ȡʱ���ֵ
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: ʱ���ֵ
  �޸ļ�¼: 
 *******************************************************/
DWORD ITimer::IWheel::GetTimeBase(ITimer::Handle handle)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return 0;
    }

    return pNode->m_tbase;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::GetIndex
  ��    ��: ��ȡʱ���ֵ
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: ��λ����
  �޸ļ�¼: 
 *******************************************************/
DWORD ITimer::IWheel::GetIndex(ITimer::Handle handle)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return 0;
    }

    return pNode->m_index;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::GetTimeBase
  ��    ��: ��ȡʱ���ֵ
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: ѭ������
  �޸ļ�¼: 
 *******************************************************/
DWORD ITimer::IWheel::GetCycle(ITimer::Handle handle)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return 0;
    }

    return pNode->m_cycle;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::GetHandle
  ��    ��: ��ȡʵ�ʾ��ֵ(���㸴�ƶ�ʱ����Ϣ)
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: ʵ�ʾ��ֵ(Allocʱ�ľ��ֵ)
  �޸ļ�¼: 
 *******************************************************/
ITimer::Handle ITimer::IWheel::GetHandle(ITimer::Handle handle)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return 0;
    }

    return pNode->m_timer;
}

/*******************************************************
  �� �� ��: ITimer::IWheel::GetString
  ��    ��: ��ȡ��ʾ�ַ���
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: ʱ����ʾ�ַ���
  �޸ļ�¼: 
 *******************************************************/
void ITimer::IWheel::GetString(ITimer::Handle handle, CDString &str)
{
    ITimer::Node *pTimer = handle;
    if (!pTimer)
    {
        str << "no timer";
        return;
    }

    ITimer::IWheel *pWheel = ITimer::IWheel::GetWheel(handle);
    if (!pWheel)
    {
        str << STR_FORMAT("expired/%d", pTimer->m_tbase);
        return;
    }

    ITimer::Handle hTimer = ITimer::IWheel::GetHandle(handle);
    if (!hTimer)
    {
        str << STR_FORMAT("expired/%d", pTimer->m_tbase);
        return;
    }

    DWORD dwTimeBase = ITimer::IWheel::GetTimeBase(hTimer);
    if (dwTimeBase != hTimer->m_tbase)
    {
        str << STR_FORMAT("expired/%d", pTimer->m_tbase);
        return;
    }

    DWORD dwIndex = ITimer::IWheel::GetIndex(hTimer);
    if (dwIndex != hTimer->m_index)
    {
        str << STR_FORMAT("expired/%d", pTimer->m_tbase);
        return;
    }
    
    DWORD dwCycle = ITimer::IWheel::GetCycle(hTimer);
    DWORD dwSlotCount = pWheel->GetSlotCount();
    DWORD dwScalePoint = pWheel->GetScalePoint();
    DWORD dwLeft = dwCycle * dwSlotCount + ((dwIndex > dwScalePoint)? 
            (dwIndex - dwScalePoint) : (dwSlotCount - dwScalePoint + dwIndex));
    str << STR_FORMAT("%d/%d", dwLeft, dwTimeBase);
}

/*******************************************************
  �� �� ��: CTimerWheel::CTimerWheel
  ��    ��: CTimerWheel���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTimerWheel::CTimerWheel()
{
    m_pHigherWheel      = 0;
    m_pLowerWheel       = 0;
    m_dwWheelID         = 0;
    m_dwHashBase        = 0;
    m_pSlotTable        = 0;
    m_dwSlotCount       = 0;
    m_dwScalePoint      = 0;
    m_fnTimeoutProc     = 0;
    m_pTimeoutPara      = 0;
}

/*******************************************************
  �� �� ��: CTimerWheel::~CTimerWheel
  ��    ��: CTimerWheel��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CTimerWheel::~CTimerWheel()
{
    m_pHigherWheel      = 0;
    m_pLowerWheel       = 0;
    m_dwWheelID         = 0;
    m_dwHashBase        = 0;

    if (m_pSlotTable)
    {
        for (DWORD i = 0; i < m_dwSlotCount; ++i)
            DLL_CLEAR((&(m_pSlotTable[i])), TimerNode, m_field, DCOP_Free);

        DCOP_Free(m_pSlotTable);
        m_pSlotTable = 0;
    }

    m_dwSlotCount       = 0;
    m_dwScalePoint      = 0;
    m_fnTimeoutProc     = 0;
    m_pTimeoutPara      = 0;
}

/*******************************************************
  �� �� ��: CTimerWheel::Init
  ��    ��: ��ʼ�����
  ��    ��: dwWheelID       - ��ʱ����ID
            dwSlotCount     - ��λ����
            pHigherWheel    - �߼���Ķ�ʱ����
            pLowerWheel     - �ͼ���Ķ�ʱ����
            dwHashBase      - HASH��ֵ
            fnTimeoutProc   - ��ʱ����ص�
            pTimeoutPara    - ��ʱ�ص�����
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CTimerWheel::Init(DWORD dwWheelID, DWORD dwSlotCount, 
                    CTimerWheel *pHigherWheel, CTimerWheel *pLowerWheel,
                    DWORD dwHashBase, TIMEOUT_PROC fnTimeoutProc, void *pTimeoutPara)
{
    if (m_pSlotTable)
    {
        return FAILURE;
    }

    if (!dwSlotCount)
    {
        return FAILURE;
    }

    m_pSlotTable = (TimerSlot *)DCOP_Malloc(dwSlotCount * sizeof(TimerSlot));
    if (!m_pSlotTable)
    {
        return FAILURE;
    }

    m_dwWheelID = dwWheelID;
    (void)memset(m_pSlotTable, 0, dwSlotCount * sizeof(TimerSlot));
    m_dwSlotCount       = dwSlotCount;
    m_pHigherWheel      = pHigherWheel;
    m_pLowerWheel       = pLowerWheel;
    m_dwHashBase        = dwHashBase;
    m_fnTimeoutProc     = fnTimeoutProc;
    m_pTimeoutPara      = pTimeoutPara;

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTimerWheel::OnTick
  ��    ��: ��ʱ��TICK����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimerWheel::OnTick()
{
    if (!m_pSlotTable) return;

    m_dwScalePoint++;
    if (m_dwScalePoint >= m_dwSlotCount)
    {
        m_dwScalePoint %= m_dwSlotCount;
        if (m_pHigherWheel)
        {
            m_pHigherWheel->OnTick();
        }
    }

    if (!DLL_EMPTY(&(m_pSlotTable[m_dwScalePoint])))
    {
        SlotTimeout(&(m_pSlotTable[m_dwScalePoint]));
    }
}

/*******************************************************
  �� �� ��: CTimerWheel::Add
  ��    ��: ��Ӿ������ʱ����
            [��ʱ��ֻ�ܴӸ߼������ͼ����������]
            [�߼��ֵ�'HashBase'==�ͼ��ֵ�'SlotCount*HashBase']
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: �ɹ�����ʧ�ܵĴ�����
  �޸ļ�¼: 
 *******************************************************/
DWORD CTimerWheel::Add(ITimer::Handle handle)
{
    if (!m_pSlotTable) return FAILURE;

    /////////////////////////////////////////////////
    /// ��ʱ�����о���ͳ�ʱֵ
    /////////////////////////////////////////////////
    TimerNode *pNode = (TimerNode *)handle;
    if (!pNode || !pNode->m_tbase)
    {
        return FAILURE;
    }

    /////////////////////////////////////////////////
    /// ����ֵ���ϸ���ֵ���ٳ�ȥ�߼�ֵ
    /////////////////////////////////////////////////
    DWORD dwBase = m_dwSlotCount * m_dwHashBase;
    DWORD dwTime = (pNode->m_tbase + pNode->m_extra) % dwBase;
    DWORD dwLoop = (pNode->m_tbase + pNode->m_extra) / dwBase;

    /////////////////////////////////////////////////
    /// ����ֵʹ�ù��˾�ʧЧ����
    /////////////////////////////////////////////////
    pNode->m_extra = 0;

    /////////////////////////////////////////////////
    /// ����и߼��֣���ѭ����Ч
    /////////////////////////////////////////////////
    if (m_pHigherWheel)
    {
        dwLoop = 0;
    }

    /////////////////////////////////////////////////
    /// �������С�ڱ�����һ���̶ȣ���ͼ��ִ���
    /////////////////////////////////////////////////
    if (!dwLoop && (dwTime < m_dwHashBase))
    {
        if (dwTime && m_pLowerWheel)
        {
            /// �ͼ��ִ����С����
            return m_pLowerWheel->Add(handle);
        }
        else
        {
            /// �޾��ȸ�С�ĵͼ���
            if (m_pHigherWheel)
            {
                /// ����Ǹ߼��ֹ����ģ��ͳ�ʱ�������سɹ�
                HandleTimeout(handle);
                return SUCCESS;
            }

            /// ֱ�������һ����ǰ���Ȳ������ʱ�䣬ֻ�з��ش�����
            return FAILURE;
        }
    }

    /////////////////////////////////////////////////
    /// �ͼ�ֵ�ĵ�ǰλ����Ϊ����ֵ��ͬʱ����ֵ���ϸø���ֵ
    /////////////////////////////////////////////////
    if (m_pLowerWheel)
    {
        pNode->m_extra = m_pLowerWheel->GetScalePoint() * m_pLowerWheel->GetHashBase();
        dwTime += pNode->m_extra;
    }

    /////////////////////////////////////////////////
    /// ��ȡ�����̶ֿ�����ֵ����������
    /////////////////////////////////////////////////
    DWORD dwIndex = ((dwTime / m_dwHashBase) + m_dwScalePoint) % m_dwSlotCount;
    pNode->m_wheel = this;
    pNode->m_index = dwIndex;
    pNode->m_cycle = dwLoop;
    DLL_INSERT_TAIL((&(m_pSlotTable[dwIndex])), pNode, m_field);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CTimerWheel::Del
  ��    ��: �Ӷ�ʱ������ɾ�����
  ��    ��: handle  - ��ʱ�����
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimerWheel::Del(ITimer::Handle handle)
{
    if (!m_pSlotTable) return;

    TimerNode *pNode = (TimerNode *)handle;
    if (!pNode)
    {
        return;
    }

    if (!pNode->m_wheel)
    {
        return;
    }

    if (pNode->m_wheel->GetWheelID() != m_dwWheelID)
    {
        return;
    }

    if (!DLL_NEXT(pNode, m_field) || !DLL_PREV(pNode, m_field))
    {
        return;
    }

    DLL_REMOVE(&(m_pSlotTable[pNode->m_index]), pNode, m_field);
    pNode->m_wheel = NULL;
}

/*******************************************************
  �� �� ��: CTimerWheel::SlotTimeout
  ��    ��: ��λ��ʱ����
  ��    ��: pSlot   - ��λ
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimerWheel::SlotTimeout(TimerSlot *pSlot)
{
    TimerNode *pNode = DLL_FIRST(pSlot);
    while (pNode)
    {
        TimerNode *pNodeTmp = pNode;
        pNode = DLL_NEXT_LOOP(pSlot, pNode, m_field);
        if (pNodeTmp->m_cycle)
        {
            /// ��Ҫ���ѭ���������������ϱ���
            pNodeTmp->m_cycle--;
            continue;
        }

        /// ���������Ƴ������ͼ�����ӻ��߳�ʱ����
        DLL_REMOVE(pSlot, pNodeTmp, m_field);
        pNodeTmp->m_wheel = NULL;
        if (m_pLowerWheel)
        {
            (void)m_pLowerWheel->Add((ITimer::Handle)pNodeTmp);
        }
        else
        {
            HandleTimeout((ITimer::Handle)pNodeTmp);
        }
    }

}

/*******************************************************
  �� �� ��: CTimerWheel::HandleTimeout
  ��    ��: �����ʱ����
  ��    ��: handle  - ���
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CTimerWheel::HandleTimeout(ITimer::Handle handle)
{
    CTimerWheel::TimerNode *pNode = (CTimerWheel::TimerNode *)handle;
    if (!pNode)
    {
        return;
    }

    /// �ÿ���ʱ����ֵ����ֹ�´��ظ�ʹ��
    pNode->m_extra = 0;

    if (m_fnTimeoutProc) (*m_fnTimeoutProc)(handle, m_pTimeoutPara);
}

