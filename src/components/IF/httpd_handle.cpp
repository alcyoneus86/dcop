/// -------------------------------------------------
/// httpd_handle.cpp : ���ı����ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "httpd_handle.h"
#include "BaseMessage.h"


/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CHttpHandle)
    DCOP_IMPLEMENT_INTERFACE(IHttpHandle)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END


/*******************************************************
  �� �� ��: IHttpHandle::CreateInstance
  ��    ��: ����HTTP���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
IHttpHandle *IHttpHandle::CreateInstance(const char *file, int line)
{
    #undef new
    CHttpHandle *pHttpHandle = new (file, line) CHttpHandle(NULL, 0, 0);
    #define new new(__FILE__, __LINE__)

    return pHttpHandle;
}

/*******************************************************
  �� �� ��: CHttpHandle::CHttpHandle
  ��    ��: CHttpHandle���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpHandle::CHttpHandle(Instance *piParent, int argc, char **argv)
{
    m_method = METHOD_GET;
    m_protocol = PROTO_VER_1_1;

    m_dynamic = false;
    m_status = STATUS_UNSTART;
    m_keep = true;

    m_content_type = MIME_TEXT_HTML;
    m_content_length = 0;
    m_content_encoding = COMPRESS_NO;

    m_condition = DCOP_CONDITION_ANY;

    DCOP_CONSTRUCT_INSTANCE(piParent);
}

/*******************************************************
  �� �� ��: CHttpHandle::~CHttpHandle
  ��    ��: CHttpHandle��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpHandle::~CHttpHandle()
{
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CHttpHandle::bCompleted
  ��    ��: �Ƿ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CHttpHandle::bCompleted()
{
    if (!m_status)
    {
        return false;
    }

    return (m_content.Length() >= m_content_length);
}

/*******************************************************
  �� �� ��: CHttpHandle::ParseParams
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpHandle::ParseParams()
{
    /// ��ͬ���󷽷���������λ�ò�һ��
    if (m_method == METHOD_GET)
    {
        const char *split = strchr(m_URI, '?');
        if (split)
        {
            m_params.Set(split + 1);
            m_URI.Remove((DWORD)(split - (const char *)m_URI));
        }
    }
    else if (m_method == METHOD_POST)
    {
        m_params.Set((const char *)m_content.Buffer(), m_content.Length());
        m_content.Clear();
    }
    else
    {
    }

    if (!m_params.Length())
    {
        return;
    }

    /// �����ĸ�ʽ�� "�������" @/$ "��������" | "ϵͳ����"
    /// "�������"��"��������"���·��������
    /// "@����"����������������"$����"�ǹؼ���ƥ���Ψһ����
    /// "ϵͳ����"�Ǹ�http�������ʹ�õ���������
    CDArray param_list;
    m_params.Split("@$|", param_list);
    if (!param_list.Count())
    {
        return;
    }

    CDString *pStr = (CDString *)param_list.Pos(HTTP_PARAM_SYS);
    if (!pStr)
    {
        return;
    }

    m_sys_params = *pStr;
    m_sys_params.Split("&", m_sys_param_list);

    pStr = (CDString *)param_list.Pos(HTTP_PARAM_REQ);
    if (!pStr)
    {
        return;
    }

    m_req_params = *pStr;
    m_req_params.Split("&", m_req_param_list);

    pStr = (CDString *)param_list.Pos(HTTP_PARAM_COND);
    if (!pStr)
    {
        return;
    }

    char ch = *(char *)(pStr + 1);
    if (ch == '$') m_condition = DCOP_CONDITION_ONE;
    m_cond_params = *pStr;
    m_cond_params.Split("&", m_cond_param_list);
}

/*******************************************************
  �� �� ��: CHttpHandle::GetSysParam
  ��    ��: ��ȡϵͳ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
const char *CHttpHandle::GetSysParam(const char *param_name)
{
    if (!param_name || !(*param_name))
    {
        return m_params;
    }

    if (!m_sys_param_list.Count())
    {
        return NULL;
    }

    /// ����һ�㲻�࣬��λ��������ʹ��forѭ�����ұ�map�ȷ�ʽ����
    DWORD dwCount = m_sys_param_list.Count();
    for (DWORD i = 0; i < dwCount; ++i)
    {
        CDString *pStr = (CDString *)m_sys_param_list.Pos(i);
        if (!pStr)
        {
            continue;
        }

        const char *cszSplit = strchr(*pStr, '=');
        if (!cszSplit || (cszSplit < (const char *)(*pStr)))
        {
            continue;
        }

        CDString strParamName(*pStr, (DWORD)(cszSplit - (const char *)(*pStr)));
        if (!stricmp(strParamName, param_name))
        {
            return cszSplit + 1;
        }
    }

    return NULL;
}

