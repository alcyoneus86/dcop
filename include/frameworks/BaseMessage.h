/// -------------------------------------------------
/// BaseMessage.h : ������Ϣ���Ͷ��幫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _BASEMESSAGE_H_
#define _BASEMESSAGE_H_

#include "BaseID.h"
#include "cpu/bytes.h"


/// -------------------------------------------------
/// ���ļ���Ҫ�漰'��Ϣ����'��'��Ϣ�ṹ'����:
///     '��Ϣ����': ����������Ϣ
///     '��Ϣ�ṹ': ����������Ϣ
/// -------------------------------------------------
/// ����'��Ϣ�ṹ':
///     (û��һ���̶�����Ϣ�ṹ�����ǲ��ö����Ϣ�ṹ
/// ������ϣ��ﵽ���ֲ�����Ŀ�ģ�����һ���﷨�ṹ - 
/// �ɲο�SQL��� select 'fieldA fieldB fieldC' from 'table1' where 'fieldD >= 6')
/// -------------------------------------------------


/// -------------------------------------------------
/// ����'��Ϣ�ṹ'���:
/// -------------------------------------------------
/// '����'/'����'��������,'���'/'����'����Ϣ�����ṹ:
///     DCOP_SESSION_HEAD + DCOP_REQUEST_HEAD + Para + Data
/// -------------------------------------------------
/// 'ɾ��'/'ͳ��'�������Լ�¼����Ϣ�����ṹ:
///     DCOP_SESSION_HEAD + DCOP_CONDITION_HEAD + Para + Data + DCOP_CONDITION_HEAD + Data + ...
/// -------------------------------------------------
/// '�༭'/'��ѯ'�������Լ�¼����Ϣ�����ṹ:
///     DCOP_SESSION_HEAD + DCOP_CONDITION_HEAD + Para + Data + DCOP_REQUEST_HEAD + Para + Data
///     DCOP_SESSION_HEAD + DCOP_CONDITION_HEAD + Para + Data + DCOP_CONDITION_HEAD + Data + DCOP_REQUEST_HEAD + Para + Data
/// -------------------------------------------------
/// '��Ӧ'����Ϣ�����ṹ: (������¼��Paraֻ��Ҫ����һ��)
///     DCOP_SESSION_HEAD + DCOP_RESPONSE_HEAD + Para + Data + DCOP_RESPONSE_HEAD + Data + DCOP_RESPONSE_HEAD + Data + ...
/// -------------------------------------------------
/// '�¼�'����Ϣ�����ṹ:
///     DCOP_SESSION_HEAD + DCOP_EVENT_HEAD + Para + Data + DCOP_EVENT_HEAD + Data + DCOP_EVENT_HEAD + Data + ...
/// -------------------------------------------------
/// ��ͬ���͵Ķ����Ϣͷ�ϲ���һ����Ϣ����:
///     DCOP_SESSION_HEAD + ... + DCOP_SESSION_HEAD + ...
/// -------------------------------------------------


/// -------------------------------------------------
/// '��Ϣ�ṹ'��'Para'�Լ�'Value'�Ĺ�ϵ
/// -------------------------------------------------
///     'Para'���������ֶΣ�ʹ��'DCOP_PARA_NODE'�ṹ
///     ��'��Ϣ�ṹ'��ʹ��'paraCount'������
/// -------------------------------------------------
///     'Value'�Ǵ����������ݣ����밴���ֶ��б�������
///     ��'��Ϣ�ṹ'��ʹ��'paraLen'������
/// -------------------------------------------------
///     [1] һ����Ϣ���л��߼�¼��������һ��'Para'����
///     [2] ��ͬ�����ݿ�ʹ���ʼ��'��Ϣ�ṹ'�е�����
///     [3] ��ͬ���ֶοɵ�����'��Ϣ�ṹ'��������������
///         ͬһ����¼����
///     [4] '����'�ṹ��һ��'�Ự'ֻ����һ����'����'��
///         '��Ӧ'��'�¼�'�����ж��
/// -------------------------------------------------


/// -------------------------------------------------
/// ͷ������ : ÿһ����Ϣͷ���е�һ����ͬ��Ա��������ʶһ����Ϣͷ������������
/// -------------------------------------------------
typedef struct tagDCOP_MSG_HEAD
{
    BYTE m_headType;                        // ͷ������(��'DCOP_MSG_HEAD_TYPE')
    BYTE m_headSize;                        // ͷ����С(��'DCOP_MSG_HEAD_SIZE')
    WORD m_valueLen;                        // ͷ�������ֵ�ĳ���
}DCOP_MSG_HEAD;


/// -------------------------------------------------
/// �Ự��Ϣͷ
/// -------------------------------------------------
///     �Ự : �û���¼��ĻỰ��Ϣ
///     ���� : ���������ֵ(����Դ��ģʱȷ��)
///     ���� : ��Ϣ�����к�
///     ���� : ���� / ���� / �¼�
/// -------------------------------------------------
typedef struct tagDCOP_SESSION_HEAD
{
    DCOP_MSG_HEAD m_type;                   // ��������
    WORD m_ver;                             // �汾(��'DCOP_SESSION_VER')
    WORD m_count;                           // Я������Ϣ�ڵ�����
    DWORD m_session;                        // �Ự��
    DWORD m_user;                           // �û�ID
    DWORD m_tty;                            // �ն�����ID
    DWORD m_attribute;                      // ����ID
    WORD m_index;                           // ����ֵ
    BYTE m_ctrl;                            // ����(��'DCOP_CTRL_TYPE')
    BYTE m_ack;                             // Ӧ��(��'DCOP_ACK_TYPE')
}DCOP_SESSION_HEAD;


/// -------------------------------------------------
/// ������Ϣͷ
/// -------------------------------------------------
typedef struct tagDCOP_CONDITION_HEAD
{
    DCOP_MSG_HEAD m_type;                   // ��������
    BYTE m_condition;                       // ����(��'DCOP_CONDITION_TYPE')
    BYTE m_paraCount;                       // ��������
    WORD m_paraLen;                         // ��������
}DCOP_CONDITION_HEAD;


/// -------------------------------------------------
/// ������Ϣͷ
/// -------------------------------------------------
typedef struct tagDCOP_REQUEST_HEAD
{
    DCOP_MSG_HEAD m_type;                   // ��������
    WORD m_paraCount;                       // ��������
    WORD m_paraLen;                         // ��������
}DCOP_REQUEST_HEAD;


/// -------------------------------------------------
/// ��Ӧ��Ϣͷ
/// -------------------------------------------------
typedef struct tagDCOP_RESPONSE_HEAD
{
    DCOP_MSG_HEAD m_type;                   // ��������
    DWORD m_retCode;                        // ������(0Ϊ��ȷ,��0Ϊ������)
    DWORD m_recordCount;                    // ��¼����
    DWORD m_recordIndex;                    // ���μ�¼����
    WORD m_paraCount;                       // ��������
    WORD m_paraLen;                         // ��������
}DCOP_RESPONSE_HEAD;


/// -------------------------------------------------
/// �¼���Ϣͷ
/// -------------------------------------------------
typedef struct tagDCOP_EVENT_HEAD
{
    DCOP_MSG_HEAD m_type;                   // ��������
    DWORD m_recordCount;                    // ��¼����
    DWORD m_recordIndex;                    // ���μ�¼����
    WORD m_paraCount;                       // ��������
    WORD m_paraLen;                         // ��������
}DCOP_EVENT_HEAD;


/// -------------------------------------------------
/// �����ڵ�
/// -------------------------------------------------
typedef struct tagDCOP_PARA_NODE
{
    DWORD m_paraID;                         // ����ID(��1��ʼ�ı��,0��ʾ���в���;�����������������,��'DCOP_SPECPARA_TYPE')
    BYTE m_opCode;                          // ������(��'DCOP_OPCODE_TYPE')
    BYTE m_paraType;                        // ��������(���嶨���ʵ��ʹ���йأ����ʹ�õĶ������Խ�ģ���Ǿ��������ֶ�)
    WORD m_paraSize;                        // ������С
}DCOP_PARA_NODE;


/// -------------------------------------------------
/// ͷ�����Ͷ���
/// -------------------------------------------------
enum DCOP_MSG_HEAD_TYPE
{
    DCOP_MSG_HEAD_SESSION = 0,              // �Ự��Ϣͷ
    DCOP_MSG_HEAD_CONDITION,                // ������Ϣͷ
    DCOP_MSG_HEAD_REQUEST,                  // ������Ϣͷ
    DCOP_MSG_HEAD_RESPONSE,                 // ��Ӧ��Ϣͷ
    DCOP_MSG_HEAD_EVENT,                    // �¼���Ϣͷ

    DCOP_MSG_HEAD_COUNT                     // ��Ϣͷ����Ŀ
};


/// -------------------------------------------------
/// ͷ���汾����[���б仯���������޸�]
/// -------------------------------------------------
const BYTE DCOP_MSG_HEAD_SIZE[] = 
{
    sizeof(DCOP_SESSION_HEAD),              // ��ͨ��Ϣͷ��С
    sizeof(DCOP_CONDITION_HEAD),            // ������Ϣͷ��С
    sizeof(DCOP_REQUEST_HEAD),              // ������Ϣͷ��С
    sizeof(DCOP_RESPONSE_HEAD),             // ��Ӧ��Ϣͷ��С
    sizeof(DCOP_EVENT_HEAD)                 // �¼���Ϣͷ��С
};


/// -------------------------------------------------
/// SESSIONͷ���ֽ���ת������
/// -------------------------------------------------
#define BYTES_CHANGE_SESSION_HEAD_ORDER(pHead)      \
    const BYTES_CHANGE_RULE DCOP_SESSION_HEAD_BORULE[] = \
    { \
        {SIZE_OF(DCOP_MSG_HEAD, m_valueLen), OFFSET_OF(DCOP_MSG_HEAD, m_valueLen)}, \
        {SIZE_OF(DCOP_SESSION_HEAD, m_ver), OFFSET_OF(DCOP_SESSION_HEAD, m_ver)}, \
        {SIZE_OF(DCOP_SESSION_HEAD, m_count), OFFSET_OF(DCOP_SESSION_HEAD, m_count)}, \
        {SIZE_OF(DCOP_SESSION_HEAD, m_session), OFFSET_OF(DCOP_SESSION_HEAD, m_session)}, \
        {SIZE_OF(DCOP_SESSION_HEAD, m_user), OFFSET_OF(DCOP_SESSION_HEAD, m_user)}, \
        {SIZE_OF(DCOP_SESSION_HEAD, m_tty), OFFSET_OF(DCOP_SESSION_HEAD, m_tty)}, \
        {SIZE_OF(DCOP_SESSION_HEAD, m_attribute), OFFSET_OF(DCOP_SESSION_HEAD, m_attribute)}, \
        {SIZE_OF(DCOP_SESSION_HEAD, m_index), OFFSET_OF(DCOP_SESSION_HEAD, m_index)} \
    }; \
    Bytes_ChangeOrderByRule(DCOP_SESSION_HEAD_BORULE, ARRAY_SIZE(DCOP_SESSION_HEAD_BORULE), \
                        pHead, DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_SESSION])


/// -------------------------------------------------
/// CONDITIONͷ���ֽ���ת������
/// -------------------------------------------------
#define BYTES_CHANGE_CONDITION_HEAD_ORDER(pHead)    \
    const BYTES_CHANGE_RULE DCOP_CONDITION_HEAD_BORULE[] = \
    { \
        {SIZE_OF(DCOP_MSG_HEAD, m_valueLen), OFFSET_OF(DCOP_MSG_HEAD, m_valueLen)}, \
        {SIZE_OF(DCOP_CONDITION_HEAD, m_paraLen), OFFSET_OF(DCOP_CONDITION_HEAD, m_paraLen)} \
    }; \
    Bytes_ChangeOrderByRule(DCOP_CONDITION_HEAD_BORULE, ARRAY_SIZE(DCOP_CONDITION_HEAD_BORULE), \
                        pHead, DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_CONDITION])


/// -------------------------------------------------
/// REQUESTͷ���ֽ���ת������
/// -------------------------------------------------
#define BYTES_CHANGE_REQUEST_HEAD_ORDER(pHead)      \
    const BYTES_CHANGE_RULE DCOP_REQUEST_HEAD_BORULE[] = \
    { \
        {SIZE_OF(DCOP_MSG_HEAD, m_valueLen), OFFSET_OF(DCOP_MSG_HEAD, m_valueLen)}, \
        {SIZE_OF(DCOP_REQUEST_HEAD, m_paraCount), OFFSET_OF(DCOP_REQUEST_HEAD, m_paraCount)}, \
        {SIZE_OF(DCOP_REQUEST_HEAD, m_paraLen), OFFSET_OF(DCOP_REQUEST_HEAD, m_paraLen)} \
    }; \
    Bytes_ChangeOrderByRule(DCOP_REQUEST_HEAD_BORULE, ARRAY_SIZE(DCOP_REQUEST_HEAD_BORULE), \
                        pHead, DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_REQUEST])


/// -------------------------------------------------
/// RESPONSEͷ���ֽ���ת������
/// -------------------------------------------------
#define BYTES_CHANGE_RESPONSE_HEAD_ORDER(pHead)     \
    const BYTES_CHANGE_RULE DCOP_RESPONSE_HEAD_BORULE[] = \
    { \
        {SIZE_OF(DCOP_MSG_HEAD, m_valueLen), OFFSET_OF(DCOP_MSG_HEAD, m_valueLen)}, \
        {SIZE_OF(DCOP_RESPONSE_HEAD, m_retCode), OFFSET_OF(DCOP_RESPONSE_HEAD, m_retCode)}, \
        {SIZE_OF(DCOP_RESPONSE_HEAD, m_recordCount), OFFSET_OF(DCOP_RESPONSE_HEAD, m_recordCount)}, \
        {SIZE_OF(DCOP_RESPONSE_HEAD, m_recordIndex), OFFSET_OF(DCOP_RESPONSE_HEAD, m_recordIndex)}, \
        {SIZE_OF(DCOP_RESPONSE_HEAD, m_paraCount), OFFSET_OF(DCOP_RESPONSE_HEAD, m_paraCount)}, \
        {SIZE_OF(DCOP_RESPONSE_HEAD, m_paraLen), OFFSET_OF(DCOP_RESPONSE_HEAD, m_paraLen)} \
    }; \
    Bytes_ChangeOrderByRule(DCOP_RESPONSE_HEAD_BORULE, ARRAY_SIZE(DCOP_RESPONSE_HEAD_BORULE), \
                        pHead, DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_RESPONSE])


/// -------------------------------------------------
/// EVENTͷ���ֽ���ת������
/// -------------------------------------------------
#define BYTES_CHANGE_EVENT_HEAD_ORDER(pHead)        \
    const BYTES_CHANGE_RULE DCOP_EVENT_HEAD_BORULE[] = \
    { \
        {SIZE_OF(DCOP_MSG_HEAD, m_valueLen), OFFSET_OF(DCOP_MSG_HEAD, m_valueLen)}, \
        {SIZE_OF(DCOP_EVENT_HEAD, m_recordCount), OFFSET_OF(DCOP_EVENT_HEAD, m_recordCount)}, \
        {SIZE_OF(DCOP_EVENT_HEAD, m_recordIndex), OFFSET_OF(DCOP_EVENT_HEAD, m_recordIndex)}, \
        {SIZE_OF(DCOP_EVENT_HEAD, m_paraCount), OFFSET_OF(DCOP_EVENT_HEAD, m_paraCount)}, \
        {SIZE_OF(DCOP_EVENT_HEAD, m_paraLen), OFFSET_OF(DCOP_EVENT_HEAD, m_paraLen)} \
    }; \
    Bytes_ChangeOrderByRule(DCOP_EVENT_HEAD_BORULE, ARRAY_SIZE(DCOP_EVENT_HEAD_BORULE), \
                        pHead, DCOP_MSG_HEAD_SIZE[DCOP_MSG_HEAD_EVENT])


/// -------------------------------------------------
/// PARA�ڵ��ֽ���ת������
/// -------------------------------------------------
#define BYTES_CHANGE_PARA_NODE_ORDER(pNode, Count)  \
    const BYTES_CHANGE_RULE DCOP_PARA_NODE_BORULE[] = \
    { \
        {SIZE_OF(DCOP_PARA_NODE, m_paraID), OFFSET_OF(DCOP_PARA_NODE, m_paraID)}, \
        {SIZE_OF(DCOP_PARA_NODE, m_paraSize), OFFSET_OF(DCOP_PARA_NODE, m_paraSize)}, \
    }; \
    for (DWORD i = 0; i < Count; ++i) \
    Bytes_ChangeOrderByRule(DCOP_PARA_NODE_BORULE, ARRAY_SIZE(DCOP_PARA_NODE_BORULE), \
                        &(pNode[i]), sizeof(DCOP_PARA_NODE))


/// -------------------------------------------------
/// �Ự�汾
/// -------------------------------------------------
#define DCOP_SESSION_VER 1


/// -------------------------------------------------
/// ���ƶ���
/// -------------------------------------------------
enum DCOP_CTRL_TYPE
{
    DCOP_CTRL_NULL = 0,                     // ��Ч����
    DCOP_CTRL_CREATE,                       // ��������
    DCOP_CTRL_DESTROY,                      // ���ٲ���
    DCOP_CTRL_ADD,                          // ��Ӳ���
    DCOP_CTRL_DEL,                          // ɾ������
    DCOP_CTRL_SET,                          // ���ò���
    DCOP_CTRL_GET,                          // ��ȡ����
    DCOP_CTRL_DUMP,                         // DUMP����
    DCOP_CTRL_METHOD,                       // '����'
    DCOP_CTRL_EVENT,                        // '�¼�'(�����¼�����Դ�¼�;��������Ĳ���������¼�����Դ�¼�)
};


/// -------------------------------------------------
/// Ӧ����
/// -------------------------------------------------
enum DCOP_ACK_TYPE
{
    DCOP_REQ = 0,                           // ����     (0b00)
    DCOP_EVT,                               // �¼�     (0b01)
    DCOP_RSP_CON,                           // ��Ӧ���� (0b10)
    DCOP_RSP_END                            // ��Ӧ���� (0b11)
};
#define DCOP_RSP(ack) (((ack) & DCOP_RSP_CON) != 0)


/// -------------------------------------------------
/// ������Ϣ����������
/// -------------------------------------------------
enum DCOP_CONDITION_TYPE
{
    DCOP_CONDITION_ANY = 0,                 // ���������ֶε�����ƥ��
    DCOP_CONDITION_ALL,                     // ���������ֶε�����ƥ��
    DCOP_CONDITION_ONE                      // �����ֶε���һ���ֶν��о�ȷƥ��
};


/// -------------------------------------------------
/// �����������
/// -------------------------------------------------
/// �����ֶ���һ������ֶι���������һ�ű��ϣ�Ȼ�����ֱ�ӻ�ȡ�����������ֶΣ����: 
///     "��λ16λ(�̶�ֵ) + �����ֶ�(���255) + �����ֶ�(���255)"
/// [ע] ����������ֶ�IDһ�㲻����255��
/// -------------------------------------------------
enum DCOP_SPECPARA_TYPE
{
    DCOP_FIELD_BASE = 0,
    DCOP_FIELD_SPECPARA,
    DCOP_FIELD_RELATION,

    DCOP_FIELD_LOW0 = 0x000000ff,
    DCOP_FIELD_LOW1 = 0x0000ff00,
    DCOP_FIELD_HIGH = 0xffff0000,

    DCOP_SPECPARA_COUNT = (DCOP_FIELD_SPECPARA << 16),      // ��¼ͳ��
    DCOP_SPECPARA_OFFSET,                                   // ��¼ƫ��
    DCOP_SPECPARA_LIMIT,                                    // ��¼����
};
#define DCOP_SPECPARA(fieldID, specType) (((fieldID) & DCOP_FIELD_HIGH) == ((specType) << 16))


/// -------------------------------------------------
/// ����������
/// -------------------------------------------------
enum DCOP_OPCODE_TYPE
{
    DCOP_OPCODE_NONE = 0,                   // �޲���
    DCOP_OPCODE_ADD,                        // ��       - '+'
    DCOP_OPCODE_SUB,                        // ��       - '-'
    DCOP_OPCODE_MUL,                        // ��       - '*'
    DCOP_OPCODE_DIV,                        // ��       - '/'
    DCOP_OPCODE_EQUAL,                      // ����     - '='
    DCOP_OPCODE_MORE_EQUAL,                 // ���ڵ��� - '>='
    DCOP_OPCODE_MORE,                       // ����     - '>'
    DCOP_OPCODE_LESS,                       // С��     - '<'
    DCOP_OPCODE_LESS_EQUAL,                 // С�ڵ��� - '<='
    DCOP_OPCODE_NOT_EQUAL,                  // ������   - '!='
    DCOP_OPCODE_INCLUDE,                    // ����     - '#'
    DCOP_OPCODE_NOT_INCLUDE                 // ������   - '!#'
};


/// -------------------------------------------------
/// �κ���Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_ANY                                (0x00000000)


/// -------------------------------------------------
/// ������Ϣ����(��Ҫ���ڶ�����������:'����'/'����'/'�¼�')
/// -------------------------------------------------
#define DCOP_MSG_OBJECT                             (0x00000001)
#define DCOP_MSG_OBJECT_REQUEST                     ((DCOP_MSG_OBJECT) + 1)        // 0x00000002 : �����������
#define DCOP_MSG_OBJECT_RESPONSE                    ((DCOP_MSG_OBJECT) + 2)        // 0x00000003 : ����������Ӧ
#define DCOP_MSG_OBJECT_EVENT                       ((DCOP_MSG_OBJECT) + 3)        // 0x00000004 : ����֪ͨ�¼�


/// -------------------------------------------------
/// ������Ϣ���͵����
/// -------------------------------------------------
///     ��16λ : ����ID
///     ��16λ : ƫ��ֵ
/// -------------------------------------------------


/// -------------------------------------------------
/// ״̬����Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_MANAGER                            ((DCOP_OBJECT_MANAGER) << 16)   // 0x00030000
#define DCOP_MSG_MANAGER_START                      ((DCOP_MSG_MANAGER) + 1)        // 0x00030001 : ��̬��ʼ����
#define DCOP_MSG_MANAGER_FINISH                     ((DCOP_MSG_MANAGER) + 2)        // 0x00030002 : ��̬��������
#define DCOP_MSG_MANAGER_LOAD                       ((DCOP_MSG_MANAGER) + 3)        // 0x00030003 : ��̬���ض���
#define DCOP_MSG_MANAGER_UNLOAD                     ((DCOP_MSG_MANAGER) + 4)        // 0x00030004 : ��̬ж�ض���


/// -------------------------------------------------
/// ״̬����Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_STATUS                             ((DCOP_OBJECT_STATUS) << 16)    // 0x00080000
#define DCOP_MSG_STATUS_START                       ((DCOP_MSG_STATUS) + 1)         // 0x00080001 : ״̬����ʼ
#define DCOP_MSG_STATUS_NEW                         ((DCOP_MSG_STATUS) + 2)         // 0x00080002 : ״̬��Ǩ��
#define DCOP_MSG_STATUS_TIMEOUT                     ((DCOP_MSG_STATUS) + 3)         // 0x00080003 : ״̬����ʱ
#define DCOP_MSG_STATUS_STOP                        ((DCOP_MSG_STATUS) + 4)         // 0x00080004 : ״̬������


/// -------------------------------------------------
/// ��Ӧ����Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_RESPONSE                           ((DCOP_OBJECT_RESPONSE) << 16)  // 0x00090000
#define DCOP_MSG_RESPONSE_TIMER_1S                  ((DCOP_MSG_RESPONSE) + 1)       // 0x00090001 : ��Ӧ��1S��ʱ����Ϣ


/// -------------------------------------------------
/// ģ����Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_MODEL                              ((DCOP_OBJECT_MODEL) << 16)     // 0x000a0000
#define DCOP_MSG_MODEL_REG                          ((DCOP_MSG_MODEL) + 1)          // 0x000a0001 : ģ��ע����Ϣ


/// -------------------------------------------------
/// �Ự��Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_SESSION                            ((DCOP_OBJECT_SESSION) << 16)   // 0x00140000
#define DCOP_MSG_SESSION_TIMER_1S                   ((DCOP_MSG_SESSION) + 1)        // 0x00140001 : �Ự1S��ʱ����Ϣ
#define DCOP_MSG_SESSION_TIMEOUT                    ((DCOP_MSG_SESSION) + 2)        // 0x00140002 : �Ự��ʱ��Ϣ


/// -------------------------------------------------
/// ���ı���Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_HTTPD                              ((DCOP_OBJECT_HTTPD) << 16)     // 0x00180000
#define DCOP_MSG_HTTPD_REQUEST                      ((DCOP_MSG_HTTPD) + 1)          // 0x00180001 : Http������Ϣ
#define DCOP_MSG_HTTPD_PROCESS                      ((DCOP_MSG_HTTPD) + 2)          // 0x00180002 : Http������Ϣ
#define DCOP_MSG_HTTPD_RESPONSE                     ((DCOP_MSG_HTTPD) + 3)          // 0x00180003 : Http��Ӧ��Ϣ


/// -------------------------------------------------
/// �ⲿ�Զ�����Ϣ����
/// -------------------------------------------------
#define DCOP_MSG_CUSTOM                             ((DCOP_OBJECT_CUSTOM) << 16) // 0x00FF0000


#endif // #ifndef _BASEMESSAGE_H_

