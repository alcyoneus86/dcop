/// -------------------------------------------------
/// httpd_protocol.cpp : ���ı�Э��ʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "httpd_protocol.h"
#include "Factory_if.h"
#include "zlib.h"
#include <time.h>
#include <ctype.h>


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CHttpRequest, "HttpRequest")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CHttpRequest)
    DCOP_IMPLEMENT_INTERFACE(IHttpRequest)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// ���󷽷�(�ı�)
/// -------------------------------------------------
const char *CHttpRequest::Method[] = 
{
    "GET",
    "POST",
    "HEAD",
    "PUT",
    "DELETE",
    "TRACE",
    "CONNECT",
    "OPTIONS"
};

/// -------------------------------------------------
/// Э��汾(�ı�)
/// -------------------------------------------------
const char *CHttpRequest::Protocol[] = 
{
    "HTTP/1.1",
    "HTTP/1.0"
};


/*******************************************************
  �� �� ��: CHttpRequest::CHttpRequest
  ��    ��: CHttpRequest���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpRequest::CHttpRequest(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);
}

/*******************************************************
  �� �� ��: CHttpRequest::~CHttpRequest
  ��    ��: CHttpRequest��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpRequest::~CHttpRequest()
{
    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CHttpRequest::Input
  ��    ��: ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpRequest::Input(IHttpHandle *http, const char *req)
{
    if (!http || !req)
    {
        return;
    }

    if (http->GetStatus() > IHttpHandle::STATUS_REQUEST)
    {
        return;
    }

    http->Content() << req;

    Analyze(http, CDString((const char *)http->Content().Buffer(), http->Content().Length()));
}

/*******************************************************
  �� �� ��: CHttpRequest::Analyze
  ��    ��: �����ı�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpRequest::Analyze(IHttpHandle *http, const char *req)
{
    if (!http || !req) return;

    /// �Ѿ���ʼ������˵���ǵȴ���������
    if (http->GetStatus() != IHttpHandle::STATUS_UNSTART)
    {
        if (http->bCompleted())
        {
            /// ��������������һ���ý��д�����
            http->SetStatus(IHttpHandle::STATUS_PROCESS);
        }

        return;
    }

    /// ������δ��ʼ����������'����'
    const char *blank = strstr(req, "\r\n\r\n");

    /// �Ҳ������У������ȴ��´�����
    if (!blank)
    {
        return;
    }

    /// �����ڿ�ʼ��˵�������ʽ����
    if (blank <= req)
    {
        http->SetStatus(IHttpHandle::STATUS_BAD_REQUEST);
        return;
    }

    DWORD headLen = (DWORD)(blank - req);

    /// �ҵ�'����'��'����'ǰ��'������'+'��Ϣͷ'
    CDString strHead(req, headLen);
    if (!strHead.Length())
    {
        return;
    }

    CDArray strList;
    strHead.Split("\n", strList);
    if (!strList.Count())
    {
        return;
    }

    /// ��һ����'������'
    CDString *pLineFirst = (CDString *)strList.Pos(0);
    if (!pLineFirst) return;
    DWORD dwRc = GetHeadLine(http, *pLineFirst);
    if (dwRc != SUCCESS)
    {
        http->SetStatus(IHttpHandle::STATUS_BAD_REQUEST);
        return;
    }

    /// �����Ƕ��'����ͷ'
    for (DWORD i = 1; i < strList.Count(); ++i)
    {
        GetHeadItem(http, (CDString *)strList.Pos(i));
    }

    /// '����'֮ǰ����Ϣ�Ѿ���������Ҫȥ��
    dwRc = http->Content().Remove(0, headLen + 4);
    if (dwRc != SUCCESS)
    {
        http->SetStatus(IHttpHandle::STATUS_BAD_REQUEST);
        return;
    }

    /// ��������
    http->ParseParams();

    /// �ɹ�����������Ϊ�����ȴ�����״̬
    http->SetStatus(IHttpHandle::STATUS_REQUEST);

    /// ��������������һ���ý��д�����
    if (http->bCompleted())
    {
        http->SetStatus(IHttpHandle::STATUS_PROCESS);
    }
}

/*******************************************************
  �� �� ��: CHttpRequest::GetHeadLine
  ��    ��: ��ȡ��ͷ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpRequest::GetHeadLine(IHttpHandle *http, const char *head)
{
    if (!http || !head) return FAILURE;
    DWORD dwRc = SUCCESS;

    CDString str(head);
    if (!str.Length())
    {
        return FAILURE;
    }

    str.TrimRight("\r\n");
    if (str.Get(0) == ' ')
    {
        return FAILURE;
    }

    CDArray strList;
    str.Split(" ", strList);
    if (!strList.Count())
    {
        return FAILURE;
    }

    /// ��ȡ���󷽷�
    CDString *pStr = (CDString *)strList.Pos(HEAD_LINE_METHOD);
    if (!pStr || !pStr->Length()) return FAILURE;
    IHttpHandle::METHOD method = IHttpHandle::METHOD_GET;
    dwRc = GetMethod(*pStr, method);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    http->SetMethod(method);

    /// ��ȡURI
    pStr = (CDString *)strList.Pos(HEAD_LINE_URI);
    if (!pStr || !pStr->Length()) return FAILURE;
    http->SetURI(*pStr);

    /// ��ȡЭ��汾
    pStr = (CDString *)strList.Pos(HEAD_LINE_PROTOCOL);
    if (!pStr || !pStr->Length()) return FAILURE;
    IHttpHandle::PROTOCOL protocol = IHttpHandle::PROTO_VER_1_1;
    dwRc = GetProtocol(*pStr, protocol);
    if (dwRc != SUCCESS)
    {
        return dwRc;
    }

    http->SetProtocol(protocol);

    return SUCCESS;
}

/*******************************************************
  �� �� ��: CHttpRequest::GetMethod
  ��    ��: ��ȡ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpRequest::GetMethod(const char *head, IHttpHandle::METHOD &method)
{
    if (!head) return FAILURE;

    for (DWORD i = 0; i < ARRAY_SIZE(Method); ++i)
    {
        if (!stricmp(Method[i], head))
        {
            method = (IHttpHandle::METHOD)i;
            return SUCCESS;
        }
    }

    return FAILURE;
}

/*******************************************************
  �� �� ��: CHttpRequest::GetProtocol
  ��    ��: ��ȡЭ��汾
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD CHttpRequest::GetProtocol(const char *head, IHttpHandle::PROTOCOL &protocol)
{
    if (!head) return FAILURE;

    for (DWORD i = 0; i < ARRAY_SIZE(Protocol); ++i)
    {
        if (!stricmp(Protocol[i], head))
        {
            protocol = (IHttpHandle::PROTOCOL)i;
            return SUCCESS;
        }
    }

    return FAILURE;
}

/*******************************************************
  �� �� ��: CHttpRequest::GetHeadItem
  ��    ��: ��ȡ��Ϣ��ͷ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpRequest::GetHeadItem(IHttpHandle *http, CDString *head)
{
    if (!http || !head) return;

    CDArray strList;
    head->Split(":", strList);
    if (!strList.Count())
    {
        return;
    }

    CDString *pStrName = (CDString *)strList.Pos(0);
    if (!pStrName)
    {
        return;
    }

    CDString *pStrValue = (CDString *)strList.Pos(1);
    if (!pStrValue)
    {
        return;
    }

    pStrName->Trim(" \t\r\n");
    pStrValue->Trim(" \t\r\n");

    if (!stricmp(*pStrName, "Connection"))
    {
        /// HTTP/1.1Ĭ�϶���Keep-Alive������ֻ�ж��Ƿ���Close
        http->SetConnection((!stricmp(*pStrValue, "Close"))? false : true);
        return;
    }

    if (!stricmp(*pStrName, "Host"))
    {
        http->SetHost(*pStrValue);
        return;
    }

    if (!stricmp(*pStrName, "Accept"))
    {
        http->SetAccept(*pStrValue);
        return;
    }

    if (!stricmp(*pStrName, "Accept-Charset"))
    {
        http->SetAcceptCharset(*pStrValue);
        return;
    }

    if (!stricmp(*pStrName, "Accept-Encoding"))
    {
        http->SetAcceptEncoding(*pStrValue);
        return;
    }

    if (!stricmp(*pStrName, "Accept-Language"))
    {
        http->SetAcceptLanguage(*pStrValue);
        return;
    }

    if (!stricmp(*pStrName, "Authorization"))
    {
        http->SetAuthorization(*pStrValue);
        return;
    }

    if (!stricmp(*pStrName, "User-Agent"))
    {
        http->SetUserAgent(*pStrValue);
        return;
    }
}

/*******************************************************
  �� �� ��: CHttpRequest::StrDecode
  ��    ��: �ַ������� (����'%3C'->'<')
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpRequest::StrDecode(char *str)
{
    if (!str) return;

    char *to = str;
    char *from = str;

    for (; *from != '\0'; ++to, ++from)
    {
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
        {
            *to = hexit(from[1]) * 16 + hexit(from[2]);
            from += 2;
        }
        else
        {
            *to = *from;
        }
    }

    *to = '\0';
}

/*******************************************************
  �� �� ��: CHttpRequest::hexit
  ��    ��: 16�����ַ������ֵ�ת��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int CHttpRequest::hexit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}


/// -------------------------------------------------
/// ʵ���೧
/// -------------------------------------------------
DCOP_IMPLEMENT_FACTORY(CHttpResponse, "HttpResponse")

/// -------------------------------------------------
/// ʵ�ֻ���
/// -------------------------------------------------
DCOP_IMPLEMENT_INSTANCE(CHttpResponse)
    DCOP_IMPLEMENT_INTERFACE(IHttpResponse)
    DCOP_IMPLEMENT_INTERFACE(Instance)
DCOP_IMPLEMENT_INSTANCE_END

/// -------------------------------------------------
/// Э��汾(������֧�ֵ���߰汾)
/// -------------------------------------------------
const char *CHttpResponse::Protocol = "HTTP/1.1";

/// -------------------------------------------------
/// ״̬��(�ı�)
/// -------------------------------------------------
const char *CHttpResponse::Status[] = 
{
    "",
    "",
    "",
    "",
    "",
    "200 OK",
    "400 Bad Request",
    "401 Unauthorized",
    "403 Forbidden",
    "404 Not Found",
    "500 Internal Server Error",
    "503 Server Unavailable"
};

/// -------------------------------------------------
/// ��������Ϣ
/// -------------------------------------------------
const char *CHttpResponse::Server = "DCOServer/0.1";

/// -------------------------------------------------
/// HTTP�Ѿ��涨��ʹ��RFC1123ʱ���ʽ
/// -------------------------------------------------
const char *CHttpResponse::RFC1123FMT = "%a, %d %b %Y %H:%M:%S GMT";

/// -------------------------------------------------
/// MIME(�ı�)
/// -------------------------------------------------
const char *CHttpResponse::MIME[] = 
{
    "text/html",
};

/// -------------------------------------------------
/// ����ѹ��(�ı�)
/// -------------------------------------------------
const char *CHttpResponse::Compress[] = 
{
    "",
    "gzip",
    "deflate"
};


/*******************************************************
  �� �� ��: CHttpResponse::CHttpCHttpResponseRequest
  ��    ��: CHttpResponse���캯��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpResponse::CHttpResponse(Instance *piParent, int argc, char **argv)
{
    DCOP_CONSTRUCT_INSTANCE(piParent);

}

/*******************************************************
  �� �� ��: CHttpResponse::~CHttpResponse
  ��    ��: CHttpResponse��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
CHttpResponse::~CHttpResponse()
{

    DCOP_DESTRUCT_INSTANCE();
}

/*******************************************************
  �� �� ��: CHttpResponse::Output
  ��    ��: �����Ӧ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpResponse::Output(IHttpHandle *http, CDStream &rsp, DWORD *pdwHeadSize)
{
    if (!http) return;

    /// ����ѹ����̬����
    /// if (http->bDynamic())
    /// ��ֻҪ�������ľͽ���ѹ��
    if (http->bCompleted())
    {
    #if _HTTPD_DEBUG_
        PrintLog(STR_FORMAT("<Output Compress Start> Status:%d", 
                http->GetStatus()), 
                PrintToConsole, 0);
    #endif
        if (IsCompressSupport(http, IHttpHandle::COMPRESS_DEFLATE))
        {
            DeflateContent(http);
        }
        else if (IsCompressSupport(http, IHttpHandle::COMPRESS_GZIP))
        {
            GzipContent(http);
        }
        else
        {
        }
    #if _HTTPD_DEBUG_
        PrintLog(STR_FORMAT("<Output Compress End> Status:%d", 
                http->GetStatus()), 
                PrintToConsole, 0);
    #endif
    }

    /// �ߵ���Ӧ������״̬
    if (http->GetStatus() == IHttpHandle::STATUS_RESPONSE)
    {
        http->SetStatus(IHttpHandle::STATUS_OK);
    }

    /// ������֧�ֵ����Э��汾
    rsp << Protocol;

    /// '״̬��'
    rsp << STR_FORMAT(" %s\r\n", GetStatus(http));

    /// 'Ӧ��ͷ'
    GetHeadItem(http, rsp);

    /// '����'
    rsp << "\r\n";

    if (pdwHeadSize)
    {
        *pdwHeadSize = rsp.Length();
    }

    /// �����������Ӧ���ȶ�ȡ���������

    /// û�л�Ӧ��Ϣ�Ͳ���ȡ������
    DWORD dwContentLen = http->GetContentLength();
    if (!dwContentLen)
    {
        return;
    }

    /// ���ͷ�����ݴ�С�����������зֶζ�ȡ����������ȡʵ�����ݳ���
    /// [ע] (���ݹ���ʱ��ʵ������������Ӧ�÷ŵ�ֻ����Դ·��������ѭ��Load)
    if (dwContentLen > http->Content().Length())
    {
        dwContentLen = http->Content().Length();
    }

    /// '��Ϣ��'
    if ((rsp.Length() + dwContentLen) > DSTREAM_DMEM_MAX_LEN)
    {
        rsp.SetMemMaxLen(rsp.Length() + dwContentLen);
    }

    rsp << CBufferPara(http->Content().Buffer(), dwContentLen);
}

/*******************************************************
  �� �� ��: CHttpResponse::GetStatus
  ��    ��: ��ȡ״̬��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
const char *CHttpResponse::GetStatus(IHttpHandle *http)
{
    if (!http) return Status[IHttpHandle::STATUS_BAD_REQUEST];

    IHttpHandle::STATUS status = http->GetStatus();
    if ((DWORD)status >= (DWORD)ARRAY_SIZE(Status))
    {
        return Status[IHttpHandle::STATUS_INTERNAL_SERVER_ERROR];
    }

    if ( !(*(Status[status])) )
    {
        return Status[IHttpHandle::STATUS_INTERNAL_SERVER_ERROR];;
    }

    return Status[status];
}

/*******************************************************
  �� �� ��: CHttpResponse::GetContentType
  ��    ��: ��ȡ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
const char *CHttpResponse::GetContentType(IHttpHandle *http)
{
    if (!http) return MIME[IHttpHandle::MIME_TEXT_HTML];

    if (http->bDynamic())
    {
        IHttpHandle::MIME content_type = http->GetContentType();
        if ((DWORD)content_type >= (DWORD)ARRAY_SIZE(MIME))
        {
            return MIME[IHttpHandle::MIME_TEXT_HTML];
        }

        return MIME[content_type];
    }

    return GetContentType(http->GetURI());
}

/*******************************************************
  �� �� ��: CHttpResponse::GetContentType
  ��    ��: ��ȡ��������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
const char *CHttpResponse::GetContentType(const char *name)
{
    if (!name) return "text/html";

    const char* dot = strrchr( name, '.' );
    if ( dot == (char*) 0 )
        return "text/html";
    if ( strcmp( dot, ".html" ) == 0 || strcmp( dot, ".htm" ) == 0 )
        return "text/html";
    if ( strcmp( dot, ".xhtml" ) == 0 || strcmp( dot, ".xht" ) == 0 )
        return "application/xhtml+xml";
    if ( strcmp( dot, ".jpg" ) == 0 || strcmp( dot, ".jpeg" ) == 0 )
        return "image/jpeg";
    if ( strcmp( dot, ".gif" ) == 0 )
        return "image/gif";
    if ( strcmp( dot, ".png" ) == 0 )
        return "image/png";
    if ( strcmp( dot, ".ico" ) == 0 )
        return "image/x-icon";
    if ( strcmp( dot, ".webp" ) == 0 )
        return "image/webp";
    if ( strcmp( dot, ".css" ) == 0 )
        return "text/css";
    if ( strcmp( dot, ".xml" ) == 0 || strcmp( dot, ".xsl" ) == 0 )
        return "text/xml";
    if ( strcmp( dot, ".au" ) == 0 )
        return "audio/basic";
    if ( strcmp( dot, ".wav" ) == 0 )
        return "audio/wav";
    if ( strcmp( dot, ".avi" ) == 0 )
        return "video/x-msvideo";
    if ( strcmp( dot, ".mov" ) == 0 || strcmp( dot, ".qt" ) == 0 )
        return "video/quicktime";
    if ( strcmp( dot, ".mpeg" ) == 0 || strcmp( dot, ".mpe" ) == 0 )
        return "video/mpeg";
    if ( strcmp( dot, ".vrml" ) == 0 || strcmp( dot, ".wrl" ) == 0 )
        return "model/vrml";
    if ( strcmp( dot, ".midi" ) == 0 || strcmp( dot, ".mid" ) == 0 )
        return "audio/midi";
    if ( strcmp( dot, ".mp3" ) == 0 )
        return "audio/mpeg";
    if ( strcmp( dot, ".ogg" ) == 0 )
        return "application/ogg";
    if ( strcmp( dot, ".pac" ) == 0 )
        return "application/x-ns-proxy-autoconfig";
    return "text/html";
}

/*******************************************************
  �� �� ��: CHttpResponse::GetHeadItem
  ��    ��: ��ȡ��Ϣ��ͷ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpResponse::GetHeadItem(IHttpHandle *http, CDStream &rsp)
{
    if (!http) return;

    time_t now;
    char timebuf[64];

    rsp << STR_FORMAT("Server: %s\r\n", Server);

    now = time( (time_t*) 0 );
    (void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
    rsp << STR_FORMAT("Date: %s\r\n", timebuf);

    rsp << STR_FORMAT("Content-Type: %s\r\n", GetContentType(http));

    rsp << STR_FORMAT("Content-Length: %lu\r\n", http->GetContentLength());

    if (http->GetContentEncoding() && ((DWORD)http->GetContentEncoding() < (DWORD)ARRAY_SIZE(Compress)))
        rsp << STR_FORMAT("Content-Encoding: %s\r\n", Compress[http->GetContentEncoding()]);
}

/*******************************************************
  �� �� ��: CHttpResponse::IsCompressSupport
  ��    ��: �Ƿ�֧��ָ����ѹ����ʽ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
bool CHttpResponse::IsCompressSupport(IHttpHandle *http, IHttpHandle::COMPRESS compress)
{
    if (!http || !compress || ((DWORD)compress >= (DWORD)ARRAY_SIZE(Compress)))
    {
        return false;
    }

    const char *pEncoding = http->GetAcceptEncoding();
    if (!pEncoding)
    {
        return false;
    }

    if (!strstr(pEncoding, Compress[compress]))
    {
        return false;
    }

    return true;
}

/*******************************************************
  �� �� ��: CHttpResponse::DeflateContent
  ��    ��: deflateѹ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpResponse::DeflateContent(IHttpHandle *http)
{
    if (!http || !http->Content().Length())
    {
        return;
    }

    unsigned long zipLen = compressBound(http->Content().Length());
    CDStream sZip((DWORD)zipLen);
    if (!sZip.Buffer())
    {
        return;
    }

    /// zlib��ʽֻ��һ��2�ֽڵ�ͷ������ʶ����һ��zlib�������ṩ��ѹ��Ϣ��
    /// ����һ��4�ֽڵ�β�����������ڽ�ѹ��У�����������Եġ�
    /// IE��deflate���벻ʶ��zlib��ʽ��������Ҫȥ��zlibͷ����ֻ�����м��ѹ������
    int rc = compress((Bytef *)sZip.Buffer(), &zipLen, (Bytef *)http->Content().Buffer(), http->Content().Length());
    if ((rc != Z_OK) || (zipLen <= 6))
    {
        return;
    }

    http->Content().Clear();
    http->Content() << CBufferPara((BYTE *)sZip.Buffer() + 2, (DWORD)zipLen - 6);
    http->SetContentEncoding(IHttpHandle::COMPRESS_DEFLATE);
    http->SetContentLength(http->Content().Length());
}

/*******************************************************
  �� �� ��: CHttpResponse::GzipContent
  ��    ��: gzipѹ������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void CHttpResponse::GzipContent(IHttpHandle *http)
{
    if (!http || !http->Content().Length())
    {
        return;
    }

    unsigned long zipLen = (unsigned long)http->Content().Length();
    CDStream sZip((DWORD)zipLen);
    if (!sZip.Buffer())
    {
        return;
    }

    z_stream strm;

    /// ��ʼ��strm�ṹ�е�zalloc, zfree, opaque,Ҫ��ʹ��Ĭ�ϵ��ڴ�������
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;

    /// �����������������
    strm.avail_in = http->Content().Length();
    strm.avail_out = zipLen;
    strm.next_in = (Bytef *)http->Content().Buffer();
    strm.next_out = (Bytef *)sZip.Buffer();

    int err = -1;
    /// ��ʼ��zlib��״̬���ɹ�����Z_OK
    /// deflateInit:zlib��ʽ��deflateInit2:gzip��ʽ
    ///     err = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    err = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS+16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (err != Z_OK)
    {
        (void)deflateEnd(&strm);
        return;
    }

    /// Z_FINISH����������룬��deflate()������
    err = deflate(&strm, Z_FINISH);
    /// Z_STREAM_END�������е����붼�Ѿ����꣬���е����Ҳ������
    if (err != Z_STREAM_END)
    {
        (void)deflateEnd(&strm);
        return;
    }
    
    /// deflateEnd�ͷ���Դ����ֹ�ڴ�й©
    (void)deflateEnd(&strm);

    /// strm.avail_out�������������ʣ��Ŀ��пռ��С
    if (zipLen <= strm.avail_out)
    {
        return;
    }

    zipLen -= strm.avail_out;

    http->Content().Clear();
    http->Content() << CBufferPara((BYTE *)sZip.Buffer(), (DWORD)zipLen);
    http->SetContentEncoding(IHttpHandle::COMPRESS_GZIP);
    http->SetContentLength(http->Content().Length());
}

