/// -------------------------------------------------
/// httpd_protocol.h : ���ı�Э��˽��ͷ�ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#ifndef _HTTPD_PROTOCOL_H_
#define _HTTPD_PROTOCOL_H_

#include "BaseClass.h"
#include "httpd_handle.h"
#include "stream/dstream.h"


/// -------------------------------------------------
/// ���ı��������ӿ���
/// -------------------------------------------------
INTF_VER(IHttpRequest, 1, 0, 0);
interface IHttpRequest : public Instance
{
    /// ��������
    virtual void Input(IHttpHandle *http, const char *req) = 0;
};


/// -------------------------------------------------
/// ���ı���Ӧ����ӿ���
/// -------------------------------------------------
INTF_VER(IHttpResponse, 1, 0, 0);
interface IHttpResponse : public Instance
{
    /// �����Ӧ
    virtual void Output(IHttpHandle *http, CDStream &rsp, DWORD *pdwHeadSize = 0) = 0;
};


/// -------------------------------------------------
/// ���ı��������ʵ����
/// -------------------------------------------------
class CHttpRequest : public IHttpRequest
{
public:
    /// ������ɶ���
    enum HEAD_LINE
    {
        HEAD_LINE_METHOD = 0,
        HEAD_LINE_URI,
        HEAD_LINE_PROTOCOL,
    };

    /// ���󷽷�(�ı�)
    static const char *Method[];

    /// Э��汾(�ı�)
    static const char *Protocol[];

public:
    static void StrDecode(char *str);
    static int hexit(char c);

public:
    CHttpRequest(Instance *piParent, int argc, char **argv);
    ~CHttpRequest();

    DCOP_DECLARE_INSTANCE;

    void Input(IHttpHandle *http, const char *req);

private:
    void Analyze(IHttpHandle *http, const char *req);
    DWORD GetHeadLine(IHttpHandle *http, const char *head);
    DWORD GetMethod(const char *head, IHttpHandle::METHOD &method);
    DWORD GetProtocol(const char *head, IHttpHandle::PROTOCOL &protocol);
    void GetHeadItem(IHttpHandle *http, CDString *head);
};


/// -------------------------------------------------
/// ���ı���Ӧ����ʵ����
/// -------------------------------------------------
class CHttpResponse : public IHttpResponse
{
public:
    /// Э��汾(������֧�ֵ���߰汾)
    static const char *Protocol;

    /// ״̬��(�ı�)
    static const char *Status[];

    /// ��������Ϣ
    static const char *Server;

    /// HTTP�Ѿ��涨��ʹ��RFC1123ʱ���ʽ
    static const char *RFC1123FMT;

    /// MIME(�ı�)
    static const char *MIME[];

    /// ����ѹ��(�ı�)
    static const char *Compress[];

public:
    static const char *GetStatus(IHttpHandle *http);
    static const char *GetContentType(IHttpHandle *http);
    static const char *GetContentType(const char *name);

public:
    CHttpResponse(Instance *piParent, int argc, char **argv);
    ~CHttpResponse();

    DCOP_DECLARE_INSTANCE;

    void Output(IHttpHandle *http, CDStream &rsp, DWORD *pdwHeadSize);

private:
    void GetHeadItem(IHttpHandle *http, CDStream &rsp);
    bool IsCompressSupport(IHttpHandle *http, IHttpHandle::COMPRESS compress);
    void DeflateContent(IHttpHandle *http);
    void GzipContent(IHttpHandle *http);
};


#endif // #ifndef _HTTPD_PROTOCOL_H_

