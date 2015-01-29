/// -------------------------------------------------
/// argcfg.h : �������ù���ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _TOOL_ARG_CFG_H_
#define _TOOL_ARG_CFG_H_

#include "dcop.h"


/// -------------------------------------------------
/// ������������
/// -------------------------------------------------
class CArgCfgType
{
public:
    typedef enum tagTYPE
    {
        TYPE_STRING,                        // �ַ�������
        TYPE_VALUE,                         // ��ֵ����
        TYPE_SWITCH                         // ����(ֻ���Ƿ����ù������ܾ���ֵ)
    }TYPE;

public:
    CArgCfgType()
    {
        m_cfgSimpleName = (const char *)NULL;
        m_cfgFullName   = (const char *)NULL;
        m_helpInfo      = (const char *)NULL;
        m_type          = TYPE_STRING;
        m_switch        = 0;
        m_must          = 0;
    }
    CArgCfgType(const char *szSimpleName, const char *szFullName, const char *szHelpInfo, BYTE byType, BYTE byMust)
    {
        m_cfgSimpleName = szSimpleName;
        m_cfgFullName   = szFullName;
        m_helpInfo      = szHelpInfo;
        m_type          = byType;
        m_must          = byMust;
    }
    ~CArgCfgType() {}

    const char *GetSimpleName() {return m_cfgSimpleName;}
    const char *GetFullName() {return m_cfgFullName;}
    const char *GetHelpInfo() {return m_helpInfo;}
    BYTE GetType() {return m_type;}
    bool bSwitch() {return ((m_switch)? true : false);}
    bool bMust() {return ((m_must)? true : false);}

private:
    const char *m_cfgSimpleName;            // ���������(һ����"-l"������ʽ)
    const char *m_cfgFullName;              // ������ȫ��(һ����"--long-time"����"-time"������ʽ)
    const char *m_helpInfo;                 // ������Ϣ
    BYTE m_type;                            // ����
    BYTE m_switch;                          // �Ƿ��ǿ���
    BYTE m_must;                            // �Ƿ����(0Ϊ�Ǳ���;��0Ϊ����)
    BYTE m_reserved;
};


/// -------------------------------------------------
/// �������������
/// -------------------------------------------------
class IArgCfgItemBase
{
public:
    IArgCfgItemBase()
    {
        m_cfgType = (CArgCfgType *)NULL;
        m_bConfig = false;
    }
    virtual ~IArgCfgItemBase() {}

    void SetCfgType(CArgCfgType *pCfgType) {m_cfgType = pCfgType;}
    CArgCfgType *GetCfgType() {return m_cfgType;}

    virtual int Cfg(int iPos, int argc, char **argv)
    {
        if (!m_cfgType)
        {
            return -1;
        }

        /// ������һ����������Ϊ��
        if (!(m_cfgType->GetSimpleName()) &&
            !(m_cfgType->GetFullName()))
        {
            return -1;
        }

        /// ƥ������һ��������������Ϊ������
        if ((m_cfgType->GetSimpleName() && !stricmp(argv[iPos], m_cfgType->GetSimpleName())) ||
            (m_cfgType->GetFullName() && !stricmp(argv[iPos], m_cfgType->GetFullName())))
        {
            m_bConfig = true;
            return 0;
        }

        return -1;
    }

    bool bConfiged() {return m_bConfig;}

private:
    CArgCfgType *m_cfgType;
    bool m_bConfig;
};


/// -------------------------------------------------
/// ����������ģ��
/// -------------------------------------------------
template<class T>
class CArgCfgItem : public IArgCfgItemBase
{
public:
    CArgCfgItem()
    {
        m_data = 0;
    }
    ~CArgCfgItem() {}

    IArgCfgItemBase *Init(T defaultValue, CArgCfgType *pCfgType)
    {
        m_data = defaultValue;
        SetCfgType(pCfgType);

        return (IArgCfgItemBase *)this;
    }

    virtual int Cfg(int iPos, int argc, char **argv)
    {
        CArgCfgType *pCfgType = GetCfgType();
        if (!pCfgType)
        {
            return -1;
        }

        /// ���û��ദ����������Ƿ�ƥ��
        if (IArgCfgItemBase::Cfg(iPos, argc, argv) < 0)
        {
            return -1;
        }

        if (pCfgType->GetType() != CArgCfgType::TYPE_VALUE)
        {
            return 0;
        }

        if (iPos < (argc - 1))
        {
            m_data = (T)atoi(argv[iPos + 1]);
            return 1;
        }

        return -1;
    }

    CArgCfgItem &operator=(T t)
    {
        m_data = t;
        return *this;
    }

    operator T()
    {
        CArgCfgType *pCfgType = GetCfgType();
        if (pCfgType && (pCfgType->GetType() == CArgCfgType::TYPE_SWITCH))
        {
            return (T)(bConfiged());
        }

        return m_data;
    }

private:
    T m_data;
};


/// -------------------------------------------------
/// ����������ģ��(ƫ�ػ�: ���ָ��)
/// -------------------------------------------------
template<class T>
class CArgCfgItem<T*> : public IArgCfgItemBase
{
public:
    CArgCfgItem()
    {
        m_ptr = NULL;
        m_dwMaxLen = 0;
    }
    ~CArgCfgItem()
    {
        if (m_ptr)
        {
            DCOP_Free(m_ptr);
        }
        m_ptr = NULL;
    }

    IArgCfgItemBase *Init(char *defaultString, DWORD dwMaxLen, CArgCfgType *pCfgType)
    {
        if (m_ptr || !dwMaxLen)
        {
            return NULL;
        }
    
        SetCfgType(pCfgType);
        if (pCfgType && (pCfgType->GetType() == CArgCfgType::TYPE_SWITCH))
        {
            return (IArgCfgItemBase *)this;
        }

        m_ptr = (T *)DCOP_Malloc(dwMaxLen);
        if (!m_ptr)
        {
            return (IArgCfgItemBase *)this;
        }

        (void)memset(m_ptr, 0, dwMaxLen);
        m_dwMaxLen = dwMaxLen;
        if (defaultString)
        {
            (void)strncpy(m_ptr, defaultString, m_dwMaxLen);
            m_ptr[m_dwMaxLen - 1] = '\0';
        }

        return (IArgCfgItemBase *)this;
    }

    virtual int Cfg(int iPos, int argc, char **argv)
    {
        if (!m_ptr)
        {
            return -1;
        }

        CArgCfgType *pCfgType = GetCfgType();
        if (!pCfgType)
        {
            return -1;
        }

        /// ���û��ദ����������Ƿ�ƥ��
        if (IArgCfgItemBase::Cfg(iPos, argc, argv) < 0)
        {
            return -1;
        }

        if (pCfgType->GetType() != CArgCfgType::TYPE_STRING)
        {
            return 0;
        }

        if (iPos < (argc - 1))
        {
            (void)strncpy(m_ptr, argv[iPos + 1], m_dwMaxLen);
            m_ptr[m_dwMaxLen - 1] = '\0';
            return 1;
        }

        return -1;
    }

    CArgCfgItem &operator=(char *pStr)
    {
        if (!m_ptr)
            return *this;

        if (pStr)
        {
            (void)strncpy(m_ptr, pStr, m_dwMaxLen);
            m_ptr[m_dwMaxLen - 1] = '\0';
        }
        else
        {
            (void)memset(m_ptr, 0, m_dwMaxLen);
        }

        return *this;
    }

    operator char*()
    {
        CArgCfgType *pCfgType = GetCfgType();
        if (pCfgType && (pCfgType->GetType() != CArgCfgType::TYPE_STRING))
        {
            return NULL;
        }

        return m_ptr;
    }

private:
    char *m_ptr;
    DWORD m_dwMaxLen;
};


/// -------------------------------------------------
/// �������ñ���
/// -------------------------------------------------
class CArgCfgTable
{
public:
    CArgCfgTable();
    ~CArgCfgTable();

    /// ���ô�ӡ�ص�
    void SetPrintFunc(LOG_PRINT logPrint, LOG_PARA logPara = 0);

    /// ע��������
    void Reg(IArgCfgItemBase **ppCfgItems, DWORD dwCfgCount);

    /// �������(ָ��bIgnoreInvalidArgΪfalse���ܺ�����Ч������������Ч�����᷵�ش���)
    DWORD Cfg(int argc, char **argv, bool bIgnoreInvalidArg = true);

    /// ��ӡ����
    void Help(const char *szHelpTitle);

    /// ��������
    bool Check();

private:
    LOG_PRINT m_logPrint;
    LOG_PARA m_logPara;
    IArgCfgItemBase **m_ppCfgItems;
    DWORD m_dwCfgCount;
};



/// -------------------------------------------------
/// ����������
/// -------------------------------------------------
#define DECLARE_CONFIG_ITEM(T, Member)              \
    CArgCfgItem<T> m_##Member;                      \
    static CArgCfgType m_cfgType_##Member


/// -------------------------------------------------
/// ʵ��������
/// -------------------------------------------------
#define IMPLEMENT_CONFIG_ITEM(CMyClass, Member, SimpleName, FullName, HelpInfo, Type, Must) \
    CArgCfgType CMyClass::m_cfgType_##Member(SimpleName, FullName, HelpInfo, Type, Must);


/// -------------------------------------------------
/// ��ʼ�����ÿ�ʼ(����)
/// -------------------------------------------------
#define INIT_CONFIG_START(CfgTable)                 \
    CArgCfgTable *__ptr_CfgTable = &CfgTable;       \
    IArgCfgItemBase *__ptr_CfgItems[] = {


/// -------------------------------------------------
/// ��ʼ��������(��ֵ)
/// -------------------------------------------------
#define INIT_CONFIG_ITEM_VAL(Member, Default)   \
    m_##Member.Init(Default, &m_cfgType_##Member),


/// -------------------------------------------------
/// ��ʼ��������(�ַ���)
/// -------------------------------------------------
#define INIT_CONFIG_ITEM_STR(Member, Default, MaxLen) \
    m_##Member.Init(Default, MaxLen, &m_cfgType_##Member),


/// -------------------------------------------------
/// ��ʼ�����ý���
/// -------------------------------------------------
#define INIT_CONFIG_END                             \
    };                                              \
    __ptr_CfgTable->Reg(__ptr_CfgItems, ARRAY_SIZE(__ptr_CfgItems));


#endif // #ifdef _TOOL_ARG_CFG_H_

