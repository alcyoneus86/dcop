/// -------------------------------------------------
/// httpd_if.h : ���ı����빫��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _HTTPD_IF_H_
#define _HTTPD_IF_H_

#include "Object_if.h"
#include "ObjModel_if.h"
#include "user_if.h"
#include "sock.h"
#include "stream/dstream.h"


/////////////////////////////////////////////////////
///                'HTTP���'˵��
/// -------------------------------------------------
/// 'HTTP���'(IHttpHandle)����HTTP:
///     �� '����' -> '����' -> '��Ӧ' ������
///     �������ݵı��档
/////////////////////////////////////////////////////


/// ����HTTP����ĺ�
#define CREATE_HTTP_HANDLE() \
    IHttpHandle::CreateInstance(__FILE__, __LINE__)


/// -------------------------------------------------
/// ���ı��������ӿ���
/// -------------------------------------------------
INTF_VER(IHttpHandle, 1, 0, 0);
class IHttpHandle : public Instance
{
public:
    static IHttpHandle *CreateInstance(const char *file, int line);

public:
    /// ���󷽷�(��ֵ)
    enum METHOD
    {
        METHOD_GET = 0,                     // �����ȡRequest-URI����ʶ����Դ
        METHOD_POST,                        // ��Request-URI����ʶ����Դ�󸽼��µ�����
        METHOD_HEAD,                        // �����ȡ��Request-URI����ʶ����Դ����Ӧ��Ϣ��ͷ
        METHOD_PUT,                         // ����������洢һ����Դ������Request-URI��Ϊ���ʶ
        METHOD_DELETE,                      // ���������ɾ��Request-URI����ʶ����Դ
        METHOD_TRACE,                       // ��������������յ���������Ϣ����Ҫ���ڲ��Ի����
        METHOD_CONNECT,                     // ��������ʹ��
        METHOD_OPTIONS                      // �����ѯ�����������ܣ����߲�ѯ����Դ��ص�ѡ�������
    };

    /// Э��汾(��ֵ)
    enum PROTOCOL
    {
        PROTO_VER_1_1 = 0,                  // HTTP/1.1
        PROTO_VER_1_0                       // HTTP/1.0
    };

    /// ״̬��(��ֵ)
    enum STATUS
    {
        STATUS_UNSTART = 0,
        STATUS_REQUEST,
        STATUS_PROCESS,
        STATUS_JSON,
        STATUS_RESPONSE,

        STATUS_OK,                          // Ok : �ͻ�������ɹ�
        STATUS_BAD_REQUEST,                 // Bad Request : �ͻ����������﷨���󣬲��ܱ������������
        STATUS_UNAUTHORIZED,                // Unauthorized : ����δ����Ȩ�����״̬��������WWW-Authenticate��ͷ��һ��ʹ��
        STATUS_FORBIDDEN,                   // Forbidden : �������յ����󣬵��Ǿܾ��ṩ����
        STATUS_NOT_FOUND,                   // Not Found : ������Դ�����ڣ�eg�������˴����URL
        STATUS_INTERNAL_SERVER_ERROR,       // Internal Server Error : ��������������Ԥ�ڵĴ���
        STATUS_SERVER_UNAVAILABLE,          // Server Unavailable : ��������ǰ���ܴ���ͻ��˵�����һ��ʱ�����ָܻ�����
    };

    /// MIME(��ֵ)
    enum MIME
    {
        MIME_TEXT_HTML = 0,
    };

    /// ����ѹ��(��ֵ)
    enum COMPRESS
    {
        COMPRESS_NO = 0,
        COMPRESS_GZIP,
        COMPRESS_DEFLATE
    };

public:
    /// �Ƿ�����
    virtual bool bCompleted() = 0;

    /// �Ƿ�̬����
    virtual bool bDynamic() = 0;

    /// ���úͻ�ȡ���󷽷�
    virtual void SetMethod(METHOD method) = 0;
    virtual METHOD GetMethod() = 0;

    /// ���úͻ�ȡ����URI·��
    virtual void SetURI(const char *URI) = 0;
    virtual const char *GetURI() = 0;

    /// ���úͻ�ȡЭ��汾
    virtual void SetProtocol(PROTOCOL protocol) = 0;
    virtual PROTOCOL GetProtocol() = 0;

    /// ���úͻ�ȡ��ȡ״̬��
    virtual void SetStatus(STATUS status) = 0;
    virtual STATUS GetStatus() = 0;

    /// ���úͻ�ȡ����״̬
    virtual void SetConnection(bool keep) = 0;
    virtual bool GetConnection() = 0;

    /// ���úͻ�ȡ��������
    virtual void SetHost(const char *HOST) = 0;
    virtual const char *GetHost() = 0;

    /// ���úͻ�ȡ�ɽ�������
    virtual void SetAccept(const char *accept) = 0;
    virtual const char *GetAccept() = 0;

    /// ���úͻ�ȡ�ɽ����ַ���
    virtual void SetAcceptCharset(const char *accept_charset) = 0;
    virtual const char *GetAcceptCharset() = 0;

    /// ���úͻ�ȡ�ɽ��ܱ���
    virtual void SetAcceptEncoding(const char *accept_encoding) = 0;
    virtual const char *GetAcceptEncoding() = 0;

    /// ���úͻ�ȡ�ɽ�������
    virtual void SetAcceptLanguage(const char *accept_language) = 0;
    virtual const char *GetAcceptLanguage() = 0;

    /// ���úͻ�ȡȨ��֤��
    virtual void SetAuthorization(const char *authorization) = 0;
    virtual const char *GetAuthorization() = 0;

    /// ���úͻ�ȡ�ն���Ϣ
    virtual void SetUserAgent(const char *user_agent) = 0;
    virtual const char *GetUserAgent() = 0;

    /// �������ݶ���
    virtual CDStream &Content() = 0;

    /// ���úͻ�ȡ��������
    virtual void SetContentType(MIME content_type) = 0;
    virtual MIME GetContentType() = 0;

    /// ���úͻ�ȡ���ݳ���
    virtual void SetContentLength(DWORD content_length) = 0;
    virtual DWORD GetContentLength() = 0;

    /// ���úͻ�ȡ���ݱ���
    virtual void SetContentEncoding(COMPRESS content_encoding) = 0;
    virtual COMPRESS GetContentEncoding() = 0;

    /// ���úͻ�ȡ��������
    virtual void SetContentLanguage(const char *content_language) = 0;
    virtual const char *GetContentLanguage() = 0;

    /// ���úͻ�ȡ����޸�ʱ��
    virtual void SetLastModified(const char *last_modified) = 0;
    virtual const char *GetLastModified() = 0;

    /// ���úͻ�ȡ����ʱ��
    virtual void SetExpires(const char *expires) = 0;
    virtual const char *GetExpires() = 0;

    /// �����ͻ�ȡ����
    virtual void ParseParams() = 0;
    virtual const char *GetSysParam(const char *param_name) = 0;
    virtual CDArray &GetReqParamList() = 0;
    virtual CDArray &GetCondParamList() = 0;
    virtual BYTE GetCondition() = 0;

    /// ʹ�ܶ�̬����
    virtual void EnableDynamic() = 0;
};


/// -------------------------------------------------
/// ���ı��������ӿ���
/// -------------------------------------------------
INTF_VER(IHttpProcess, 1, 0, 0);
interface IHttpProcess : public IObject
{
    /// ��Դ·���ڵ�
    struct ResPathNode
    {
        const char *m_resPath;                  // ��Դ·��
        DWORD m_attrID;                         // ����ID
    };

    /// ��Դ���ͽڵ�
    struct ResTypeNode
    {
        const char *m_resType;                  // ��Դ����
        IObject *m_objProc;                     // �������
    };

    /// ע����Դ·��������ֵ
    virtual DWORD RegResPath(ResPathNode *pNode, DWORD dwCount) = 0;

    /// ע����Դ���ͺʹ������
    virtual DWORD RegResType(ResTypeNode *pNode, DWORD dwCount) = 0;

    /// ��ȡ��Դ·����������
    virtual IObject *GetResOwner(const char *URI, DWORD &rdwAttrID) = 0;

    /// ������Ϣ(�������ڲ�����ַ�)
    virtual DWORD Input(objMsg *msg, DWORD clientIP, WORD clientPort) = 0;
};


/// -------------------------------------------------
/// ���ı�����ӿڶ���
/// -------------------------------------------------
INTF_VER(IHttpServer, 1, 0, 0);
interface IHttpServer : public IObject
{
    /// �Ự�ڵ�
    struct SessionNode
    {
        char m_szUserName[IUser::NAME_SIZE];
        DWORD m_dwUserID;
        DWORD m_dwUserGroup;
        DWORD m_dwSessID;
        objSock *m_pSock;
        IHttpHandle *m_pHttp;
        bool m_bSetClientInfo;
    };

    /// ����http������Ự�ڵ���
    virtual void SaveHttpToSession(
                        DWORD dwSessionID, 
                        IHttpHandle *pHttp
                        ) = 0;
};


#endif // #ifndef _HTTPD_IF_H_

