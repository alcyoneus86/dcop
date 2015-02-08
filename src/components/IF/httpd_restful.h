/// -------------------------------------------------
/// httpd_restful.h : ���ı�����˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _HTTPD_RESTFUL_H_
#define _HTTPD_RESTFUL_H_

#include "httpd_if.h"
#include "Manager_if.h"
#include "ObjModel_if.h"
#include "access_if.h"


/////////////////////////////////////////////////////
///                'RESTful'˵��
/// -------------------------------------------------
/// 'REST'(Representational State Transfer)�ķ�����:
/// "���ֲ�״̬ת��"�����һ���ܹ�����RESTԭ�򣬾ͳ���
/// ΪRESTful�ܹ���Ҫ���RESTful�ܹ�����õķ�������ȥ
/// ���Representational State Transfer�������:
/// 1. ��Դ��Resources��
///    REST������"���ֲ�״̬ת��"�У�ʡ�������"����
///    ��"��ʵָ����"��Դ"��Resources����"���ֲ�"����ν
///    "��Դ"�����������ϵ�һ��ʵ�壬����˵�������ϵ�һ
///    ��������Ϣ����������һ���ı���һ��ͼƬ��һ�׸�����
///    һ�ַ�����֮����һ�������ʵ�塣�������һ��URI
///    ��ͳһ��Դ��λ����ָ������ÿ����Դ��Ӧһ���ض���
///    URI��Ҫ��ȡ�����Դ����������URI�Ϳ��ԣ����URI��
///    ����ÿһ����Դ�ĵ�ַ���һ�޶���ʶ�����
///    ��ν"����"�������뻥������һϵ�е�"��Դ"����������
///    ����URI��
/// 2. ���ֲ㣨Representation��
///    "��Դ"��һ����Ϣʵ�壬�������ж������ڱ�����ʽ��
///    ���ǰ�"��Դ"������ֳ�������ʽ����������"���ֲ�"
///    ��Representation�������磬�ı�������txt��ʽ���֣�
///    Ҳ������HTML��ʽ��XML��ʽ��JSON��ʽ���֣���������
///    ���ö����Ƹ�ʽ��ͼƬ������JPG��ʽ���֣�Ҳ������
///    PNG��ʽ���֡�URIֻ������Դ��ʵ�壬������������ʽ��
///    �ϸ��˵����Щ��ַ����".html"��׺���ǲ���Ҫ�ģ�
///    ��Ϊ�����׺����ʾ��ʽ������"���ֲ�"���룬��URI
///    Ӧ��ֻ����"��Դ"��λ�á����ľ��������ʽ��Ӧ����
///    HTTP�����ͷ��Ϣ����Accept��Content-Type�ֶ�ָ����
///    �������ֶβ��Ƕ�"���ֲ�"��������
/// 3. ״̬ת����State Transfer��
///    ����һ����վ���ʹ����˿ͻ��˺ͷ�������һ������
///    ���̡�����������У��Ʊ��漰�����ݺ�״̬�ı仯��
///    ������ͨ��Э��HTTPЭ�飬��һ����״̬Э�顣����
///    ζ�ţ����е�״̬�������ڷ������ˡ���ˣ������
///    ������Ҫ����������������ͨ��ĳ���ֶΣ��÷�����
///    �˷���"״̬ת��"��State Transfer����������ת��
///    �ǽ����ڱ��ֲ�֮�ϵģ����Ծ���"���ֲ�״̬ת��"��
///    �ͻ����õ����ֶΣ�ֻ����HTTPЭ�顣������˵������
///    HTTPЭ�����棬�ĸ���ʾ������ʽ�Ķ��ʣ�GET��POST��
///    PUT��DELETE�����Ƿֱ��Ӧ���ֻ���������GET����
///    ��ȡ��Դ��POST�����½���Դ��Ҳ�������ڸ�����Դ����
///    PUT����������Դ��DELETE����ɾ����Դ��
/// �ۺ�����Ľ��ͣ������ܽ�һ��ʲô��RESTful�ܹ���
/// ��1��ÿһ��URI����һ����Դ��
/// ��2���ͻ��˺ͷ�����֮�䣬����������Դ��ĳ�ֱ��ֲ㣨Representation����
/// ��3���ͻ���ͨ���ĸ�HTTP���ʣ��Է���������Դ���в�����ʵ��"���ֲ�״̬ת��"��
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// ���ı��������ʵ����
/// -------------------------------------------------
class CHttpProcess : public IHttpProcess
{
public:
    CHttpProcess(Instance *piParent, int argc, char **argv);
    ~CHttpProcess();

    DCOP_DECLARE_INSTANCE;
    DCOP_DECLARE_IOBJECT;

    DWORD Init(IObject *root, int argc, void **argv);
    void Fini();

    DWORD RegResPath(ResPathNode *pNode, DWORD dwCount);
    DWORD RegResType(ResTypeNode *pNode, DWORD dwCount);
    IObject *GetResOwner(const char *URI, DWORD &rdwAttrID);
    DWORD Input(objMsg *msg, DWORD clientIP, WORD clientPort);

    void Proc(objMsg *msg);

    IModel *GetModel() {return m_piModel;}

private:
    DWORD Dispatch(const char *resType, objMsg *msg);
    void  DefaultProc(IHttpHandle *http);

private:
    char m_szDir[DCOP_FILE_NAME_LEN];               // ·��
    char m_szHome[DCOP_FILE_NAME_LEN];              // ��ҳ

    ResPathNode *m_pPathNode;                       // ��Դ·������
    DWORD m_dwPathCount;                            // ��Դ·������
    ResTypeNode *m_pTypeNode;                       // ��Դ��������
    DWORD m_dwTypeCount;                            // ��Դ��������

    IManager *m_piManager;                          // ������
    IModel *m_piModel;                              // ģ��
    IAccess *m_piAccess;                            // �������
};


#endif // #ifndef _HTTPD_RESTFUL_H_

