/// -------------------------------------------------
/// secure_main.cpp : ��ȫ����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "secure_main.h"
#include "Factory_if.h"
#include "Manager_if.h"


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CSecure, "secure")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CSecure)
    DCOP_IMPLEMENT_INTERFACE(ISecure)
    DCOP_IMPLEMENT_INTERFACE(IObject)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ʵ�ֶ�����
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT(CSecure)
    DCOP_IMPLEMENT_IDENTIFY_DYNAMIC("name", "id")
    DCOP_IMPLEMENT_CONFIG_THREADSAFE("threadsafe")
DCOP_IMPLEMENT_IOBJECT_END

/// -------------------------------------------------
/// ʵ����Ϣ�ַ�
/// -------------------------------------------------
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE(CSecure)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_START, OnStart)
    DCOP_IMPLEMENT_IOBJECT_MSG_PROC(DCOP_MSG_MANAGER_FINISH, OnFinish)
DCOP_IMPLEMENT_IOBJECT_MSG_HANDLE_END


/*******************************************************
  �� �� ��: CSecure::CSecure
  ��    ��: CSecure���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSecure::CSecure(Instance *piParent, int argc, char **argv)
{
    m_piControl = 0;

    DCOP_CONSTRUCT_INSTANCE(piParent);
    DCOP_CONSTRUCT_IOBJECT(argc, argv);
}

/*******************************************************
  �� �� ��: CSecure::~CSecure
  ��    ��: CSecure��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CSecure::~CSecure()
{
    Fini();

    DCOP_DESTRUCT_IOBJECT();
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CSecure::Init
  ��    ��: ��ʼ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::Init(IObject *root, int argc, void **argv)
{
    if (!root)
    {
        return FAILURE;
    }

    AutoObjLock(this);

    /// ��ѯ����
    DCOP_QUERY_OBJECT_START(root)
        DCOP_QUERY_OBJECT_ITEM(IControl,     DCOP_OBJECT_CONTROL,    m_piControl)
    DCOP_QUERY_OBJECT_END

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSecure::Fini
  ��    ��: ���ʱ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::Fini()
{
    DCOP_RELEASE_INSTANCE(m_piControl);
}

/*******************************************************
  �� �� ��: CSecure::Dump
  ��    ��: Dump
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::Dump(LOG_PRINT logPrint, LOG_PARA logPara, int argc, void **argv)
{
    if (!logPrint) return;

    AutoObjLock(this);

    for (IT_RULES it = m_rules.begin(); it != m_rules.end(); ++it)
    {
        logPrint(STR_FORMAT("ownerAttr: 0x%x \r\n", ((*it).second).m_ownerAttr), logPara);
        logPrint(STR_FORMAT("    ownerField: %d \r\n", ((*it).second).m_ownerField), logPara);
        DumpRight("    ownerRight", logPrint, logPara, ((*it).second).m_ownerRight);
        DumpRight("    visitorRight", logPrint, logPara, ((*it).second).m_visitorRight);
        logPrint(STR_FORMAT("    userAttr: 0x%x \r\n", ((*it).second).m_userAttr), logPara);
        logPrint(STR_FORMAT("    userField: %d \r\n", ((*it).second).m_userField), logPara);
        DumpRight("    userRight", logPrint, logPara, ((*it).second).m_userRight);
        logPrint(STR_FORMAT("    managerAttr: 0x%x \r\n", ((*it).second).m_managerAttr), logPara);
        logPrint(STR_FORMAT("    managerField: %d \r\n", ((*it).second).m_managerField), logPara);
        DumpRight("    managerRight", logPrint, logPara, ((*it).second).m_managerRight);
        logPrint(STR_FORMAT("    systemOperator: %d \r\n", ((*it).second).m_systemOperator), logPara);
    }
}

/*******************************************************
  �� �� ��: CSecure::OnStart
  ��    ��: ��ʼ����ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::OnStart(objMsg *msg)
{
    /// ע�ᰲȫ����
    if (m_piControl)
    {
        IControl::Node node[] = 
        {
            {0, DCOP_CTRL_NULL, DCOP_REQ, DCOP_GROUP_VISITOR, 0, InputCtrl},
        };
        DWORD dwRc = m_piControl->RegCtrlNode(this, 
                        DCOP_OBJECT_ACCESS, 
                        node, 
                        ARRAY_SIZE(node));
        CHECK_ERRCODE(dwRc, "Reg Ctrl Node To Access");
    }
}

/*******************************************************
  �� �� ��: CSecure::OnFinish
  ��    ��: ��������ʱ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::OnFinish(objMsg *msg)
{
}

/*******************************************************
  �� �� ��: CSecure::RegRule
  ��    ��: ע�ᰲȫ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::RegRule(Node *rules, DWORD count)
{
    if (!rules || !count) return FAILURE;

    AutoObjLock(this);

    for (DWORD i = 0; i < count; ++i)
    {
        IT_RULES it = m_rules.find(rules[i].m_ownerAttr);
        if (it != m_rules.end())
        {
            continue;
        }

        (void)m_rules.insert(MAP_RULES::value_type(rules[i].m_ownerAttr, rules[i]));
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CSecure::InputCtrl
  ��    ��: �������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::InputCtrl(objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        IObject *piCtrler)
{
    CSecure *pSecure = (CSecure *)piCtrler;
    if (!pSecure)
    {
        return FAILURE;
    }

    return pSecure->CheckAllRule(pInput, pOutput, bContinue);
}

/*******************************************************
  �� �� ��: CSecure::DumpRight
  ��    ��: DumpȨ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CSecure::DumpRight(const char *pcszTitle, 
                        LOG_PRINT logPrint, 
                        LOG_PARA logPara, 
                        DWORD right)
{
    if (!logPrint) return;

    logPrint(STR_FORMAT("%s: ", pcszTitle), logPara);

    static const char *Operation[] = 
    {
        "",
        "Create",
        "Destroy",
        "Add",
        "Del",
        "Set",
        "Get",
        "Dump",
        "Action",
        "Event",
    };

    bool bHaveRight = false;
    for (DWORD i = DCOP_CTRL_CREATE; i <= DCOP_CTRL_EVENT; ++i)
    {
        if (right & DCOP_SECURE_RIGHT(i))
        {
            logPrint(STR_FORMAT("%s ", Operation[i]), logPara);
            bHaveRight = true;
        }
    }

    if (!bHaveRight) logPrint("No Right ", logPara);

    logPrint("\r\n", logPara);
}

/*******************************************************
  �� �� ��: CSecure::GetRuleNode
  ��    ��: ��ȡ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
ISecure::Node *CSecure::GetRuleNode(DWORD attrID)
{
    AutoObjLock(this);

    IT_RULES it = m_rules.find(attrID);
    if (it == m_rules.end())
    {
        return 0;
    }

    return &((*it).second);
}

/*******************************************************
  �� �� ��: CSecure::CheckMsgOwner
  ��    ��: �����Ϣ�е�owner�ֶ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CSecure::CheckMsgOwner(DCOP_SESSION_HEAD *pSessionHead,
                        void *pSessionData,
                        DWORD ownerField)
{
    if (!pSessionHead || !pSessionData || !ownerField)
    {
        return false;
    }

    /// ��ȡ��Ϣ�е���������
    CDArray aCondHeads;
    IObjectMember::GetMsgHead(pSessionData, pSessionHead->m_type.m_valueLen, 0, &aCondHeads, 0, 0, 0);

    DCOP_CONDITION_HEAD *pCondHead = (DCOP_CONDITION_HEAD *)aCondHeads.Pos(0);
    if (!pCondHead)
    {
        return false;
    }

    /// ����ȷ�������ֶ��еĲ���������ͬʱ����
    if (pCondHead->m_condition == DCOP_CONDITION_ANY)
    {
        return false;
    }

    void *pCondParaData = *(void **)(pCondHead + 1);
    DWORD dwCondParaCount = pCondHead->m_paraCount;
    DWORD dwCondDataLen = pCondHead->m_paraLen;
    if (!pCondParaData || !dwCondParaCount || !dwCondDataLen)
    {
        return false;
    }

    CDStream sCondPara(dwCondParaCount * sizeof(DCOP_PARA_NODE));
    DCOP_PARA_NODE *pCondPara = (DCOP_PARA_NODE *)sCondPara.Buffer();
    if (!pCondPara)
    {
        return false;
    }

    void *pCondData = IObjectMember::GetMsgParaData(pCondParaData, 
                        dwCondParaCount, dwCondDataLen, 
                        pCondPara);
    if (!pCondData)
    {
        return false;
    }

    /// �����������ֶ��в���owner�ֶ�
    DWORD dwParaSize = 0;
    DWORD dwParaOffset = 0;
    for (DWORD i = 0; i < dwCondParaCount; ++i)
    {
        if ((pCondPara[i].m_paraID == ownerField) &&
            (pCondPara[i].m_opCode == DCOP_OPCODE_EQUAL))
        {
            dwParaSize = pCondPara[i].m_paraSize;
            break;
        }

        dwParaOffset += pCondPara[i].m_paraSize;
    }

    if (!dwParaSize)
    {
        return false;
    }

    /// ���������������в���ownerֵ
    /// (BYTE *)pCondData + dwParaOffset, dwParaSize

    return true;
}

/*******************************************************
  �� �� ��: CSecure::CheckAllRule
  ��    ��: ������й���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::CheckAllRule(objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue)
{
    if (!pInput) return FAILURE;

    bool bCheck = false;
    DWORD dwRc = FAILURE;

    /// ��ȡ��Ϣͷ
    CDArray aSessHeads;
    IObjectMember::GetMsgHead(pInput->GetDataBuf(), pInput->GetDataLen(), &aSessHeads, 0, 0, 0, 0);
    for (DWORD sessIdx = 0; sessIdx < aSessHeads.Count(); ++sessIdx)
    {
        /// ��ȡ�Ựͷ
        DCOP_SESSION_HEAD *pSessionHead = (DCOP_SESSION_HEAD *)aSessHeads.Pos(sessIdx);
        if (!pSessionHead)
        {
            continue;
        }

        /// ��ȡ�Ự����
        void *pSessionData = *(void **)(pSessionHead + 1);

        /// ���ݻỰ��ȡ����ڵ�
        ISecure::Node *pRuleNode = GetRuleNode(pSessionHead->m_attribute);
        if (!pRuleNode)
        {
            continue;
        }

        /// '������'�еĹ��ܺ����б�
        static CHECK_FUNC checkFunc[] = 
        {
            &CSecure::CheckOperatorRule,
            &CSecure::CheckOwnerRule,
            &CSecure::CheckVisitorRule,
            &CSecure::CheckUserRule,
            &CSecure::CheckManagerRule,
        };

        /// ���μ��
        for (DWORD i = 0; i < ARRAY_SIZE(checkFunc); ++i)
        {
            if (!checkFunc[i])
            {
                continue;
            }

            dwRc = (this->*(checkFunc[i]))(pSessionHead,
                        pSessionData,
                        pRuleNode,
                        pInput,
                        pOutput,
                        bContinue,
                        bCheck);
            if (bCheck) break;
        }

        PrintLog(STR_FORMAT("group:%d\r\n session:%d\r\n user:%d\r\n tty:%d\r\n attribute:0x%x\r\n dwRc:%d \r\n",
                        pSessionHead->m_group,
                        pSessionHead->m_session,
                        pSessionHead->m_user,
                        pSessionHead->m_tty,
                        pSessionHead->m_attribute,
                        dwRc), 
                        PrintToConsole, 0);

        /// �Ѿ��õ������
        if (bCheck) break;
    }

    return dwRc;
}

/*******************************************************
  �� �� ��: CSecure::CheckOperatorRule
  ��    ��: ���ϵͳ������Ȩ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::CheckOperatorRule(DCOP_SESSION_HEAD *pSessionHead,
                        void *pSessionData,
                        ISecure::Node *pRule,
                        objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        bool &bCheck)
{
    if (!pSessionHead || !pRule)
    {
        return FAILURE;
    }

    /// ϵͳ�û������Ǽ�������ϵ��û�
    if (pRule->m_systemOperator < DCOP_GROUP_MONITOR)
    {
        return FAILURE;
    }

    /// �������ϵͳ�û��������ˣ�����Ͳ�Ҫ��������û���
    bCheck = true;

    if (pSessionHead->m_group >= pRule->m_systemOperator)
    {
        return SUCCESS;
    }

    /// û��Ȩ�ޣ��ж�����
    bContinue = false;
    return ERRCODE_IO_NO_RIGHT_TO_OPERATE;
}

/*******************************************************
  �� �� ��: CSecure::CheckOwnerRule
  ��    ��: ��������߲���Ȩ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::CheckOwnerRule(DCOP_SESSION_HEAD *pSessionHead,
                        void *pSessionData,
                        ISecure::Node *pRule,
                        objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        bool &bCheck)
{
    if (!pSessionHead || !pRule)
    {
        return FAILURE;
    }

    /// ������ҵ�������û�
    if (pSessionHead->m_group != DCOP_GROUP_USER)
    {
        return FAILURE;
    }

    return FAILURE;
}

/*******************************************************
  �� �� ��: CSecure::CheckVisitorRule
  ��    ��: ���ι��߲���Ȩ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::CheckVisitorRule(DCOP_SESSION_HEAD *pSessionHead,
                        void *pSessionData,
                        ISecure::Node *pRule,
                        objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        bool &bCheck)
{
    if (!pSessionHead || !pRule)
    {
        return FAILURE;
    }

    /// ������δ��¼�û�
    if (pSessionHead->m_group != DCOP_GROUP_VISITOR)
    {
        return FAILURE;
    }

    /// ���δ��¼�û�Ȩ���趨
    if (pRule->m_visitorRight & DCOP_SECURE_RIGHT(pSessionHead->m_ctrl))
    {
        return SUCCESS;
    }

    /// û��Ȩ�ޣ��ж�����
    bContinue = false;
    return ERRCODE_IO_NO_RIGHT_TO_OPERATE;
}

/*******************************************************
  �� �� ��: CSecure::CheckUserRule
  ��    ��: ���һ���û�����Ȩ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::CheckUserRule(DCOP_SESSION_HEAD *pSessionHead,
                        void *pSessionData,
                        ISecure::Node *pRule,
                        objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        bool &bCheck)
{
    if (!pSessionHead || !pRule)
    {
        return FAILURE;
    }

    /// ������ҵ�������û�
    if (pSessionHead->m_group != DCOP_GROUP_USER)
    {
        return FAILURE;
    }

    return FAILURE;
}

/*******************************************************
  �� �� ��: CSecure::CheckManagerRule
  ��    ��: �������û�����Ȩ��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CSecure::CheckManagerRule(DCOP_SESSION_HEAD *pSessionHead,
                        void *pSessionData,
                        ISecure::Node *pRule,
                        objMsg *pInput,
                        objMsg *&pOutput,
                        bool &bContinue,
                        bool &bCheck)
{
    if (!pSessionHead || !pRule)
    {
        return FAILURE;
    }

    if (pSessionHead->m_group != DCOP_GROUP_USER)
    {
        return FAILURE;
    }

    return FAILURE;
}

