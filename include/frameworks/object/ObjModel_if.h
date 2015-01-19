/// -------------------------------------------------
/// ObjModel_if.h : ����ģ�ͽ�ģ����ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _OBJMODEL_IF_H_
#define _OBJMODEL_IF_H_

#include "Object_if.h"


/// -------------------------------------------------
/// ����ģ�ͼ���ģ�Ķ���˵��
/// -------------------------------------------------
/// ģ��: �������ԵĶ��壬����: '����'��'����'��'�¼�'
/// ��ģ: ���ǰ�����ƹ�ϵ����Щ��Щģ���������ڴ���
/// -------------------------------------------------


/// -------------------------------------------------
/// ����IModel�汾��
/// -------------------------------------------------
INTF_VER(IModel, 1, 0, 0);


/// ������󳤶�
#define DCOP_TABLE_NAME_LEN                 32

/// �ֶ�����󳤶�
#define DCOP_FIELD_NAME_LEN                 16


/// -------------------------------------------------
/// ģ�ͽӿڶ���
/// -------------------------------------------------
interface IModel : public IObject
{
    /// ģ������
    enum TYPE
    {
        TYPE_DATA = 0,                      // [0]����
        TYPE_METHOD,                        // [1]����
        TYPE_EVENT                          // [2]�¼�
    };

    /// �ؼ�������
    enum KEY
    {
        KEY_NO = 0,                         // [0]�ǹؼ���
        KEY_YES                             // [1]�ǹؼ���
    };

    /// �ֶ�����
    enum ENUM_FIELD
    {
        FIELD_NULL = 0,                     // [0]��Ч�ֶ�
        FIELD_IDENTIFY,                     // [1]ID (�������룬�Զ�����Ψһ��ʶ����Χ: 1~Nʮ����)
        FIELD_BYTE,                         // [2]�ֽ� (16�����޷���)
        FIELD_WORD,                         // [3]�� (16�����޷���)
        FIELD_DWORD,                        // [4]˫�� (16�����޷���)
        FIELD_SHORT,                        // [5]short (ʮ�����з���)
        FIELD_INTEGER,                      // [6]int (ʮ�����з���)
        FIELD_NUMBER,                       // [7]������ (��Χ: 0~Nʮ����)
        FIELD_BOOL,                         // [8]bool (��0Ϊtrue;0Ϊfalse)
        FIELD_CHAR,                         // [9]char (�ַ�)
        FIELD_STRING,                       // [10]�ַ���
        FIELD_BUFFER,                       // [11]���ݶ�
        FIELD_DATE,                         // [12]����(������)
        FIELD_TIME,                         // [13]ʱ��(ʱ����)
        FIELD_IP,                           // [14]IP��ַ
        FIELD_PTR,                          // [15]ָ��
        FIELD_TIMER,                        // [16]��ʱ��
        FIELD_PASS,                         // [17]У����

        FIELD_NUM
    };

    /// �ֶλ���
    struct Field
    {
        char m_fieldName[DCOP_FIELD_NAME_LEN];
        BYTE m_isKey;                       // �Ƿ�ؼ���(Ψһ�ԣ���������������)
        BYTE m_fieldType;                   // �ֶ�����
        WORD m_fieldSize;                   // �ֶδ�С
        DWORD m_defaultValue;               // Ĭ��ֵ(������ֵ,0��ʾ��Ĭ��ֵ)
        DWORD m_minValue;                   // ��Сֵ(������ֵ,0��ʾ����Сֵ)
        DWORD m_maxValue;                   // ���ֵ(������ֵ,0��ʾ�����ֵ)
    };

    /// �ֶι���
    struct Relation
    {
        DWORD m_relID;                      // ����ID(ָ����Ҫ������ȥ���ֶ�ID)(��Ҫ������Ψһֵ�ֶ�)
        DWORD m_attrID;                     // ����������ID
        DWORD m_fieldID;                    // �������ֶ�ID
    };

    /// ע����ֶ�
    virtual DWORD RegTable(
                    char tableName[DCOP_TABLE_NAME_LEN],
                    DWORD objID,            // ����ID
                    DWORD attrID,           // ����ID
                    TYPE attrType,          // ��������
                    Field *pFields,         // �ֶ��б�
                    DWORD dwFieldCount,     // �ֶθ���
                    DWORD dwDefRecCount,    // Ĭ�ϼ�¼����
                    Relation *pShips = 0,   // �����б�
                    DWORD dwShipCount = 0   // ��������
                    ) = 0;

    /// ��ȡ����
    virtual const char *GetTableName(DWORD attrID) = 0;

    /// ��ȡ����������ID
    virtual DWORD GetObjID(DWORD attrID) = 0;

    /// ��ȡ������
    virtual TYPE GetType(DWORD attrID) = 0;

    /// ��ȡ�ֶ�
    virtual Field *GetFields(DWORD attrID, DWORD &rdwFieldCount) = 0;

    /// ��ȡĬ�ϼ�¼��
    virtual DWORD GetDefRecCount(DWORD attrID) = 0;

    /// ��ȡ�����ֶ�
    virtual Relation *GetShips(DWORD attrID, DWORD &rdwShipCount) = 0;
};


#endif // #ifndef _OBJMODEL_IF_H_

