/// -------------------------------------------------
/// httpd_restful.h : 超文本处理私有头文件
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
///                'RESTful'说明
/// -------------------------------------------------
/// 'REST'(Representational State Transfer)的翻译是:
/// "表现层状态转化"。如果一个架构符合REST原则，就称它
/// 为RESTful架构。要理解RESTful架构，最好的方法就是去
/// 理解Representational State Transfer这个词组:
/// 1. 资源（Resources）
///    REST的名称"表现层状态转化"中，省略了主语。"表现
///    层"其实指的是"资源"（Resources）的"表现层"。所谓
///    "资源"，就是网络上的一个实体，或者说是网络上的一
///    个具体信息。它可以是一段文本、一张图片、一首歌曲、
///    一种服务，总之就是一个具体的实体。你可以用一个URI
///    （统一资源定位符）指向它，每种资源对应一个特定的
///    URI。要获取这个资源，访问它的URI就可以，因此URI就
///    成了每一个资源的地址或独一无二的识别符。
///    所谓"上网"，就是与互联网上一系列的"资源"互动，调用
///    它的URI。
/// 2. 表现层（Representation）
///    "资源"是一种信息实体，它可以有多种外在表现形式。
///    我们把"资源"具体呈现出来的形式，叫做它的"表现层"
///    （Representation）。比如，文本可以用txt格式表现，
///    也可以用HTML格式、XML格式、JSON格式表现，甚至可以
///    采用二进制格式；图片可以用JPG格式表现，也可以用
///    PNG格式表现。URI只代表资源的实体，不代表它的形式。
///    严格地说，有些网址最后的".html"后缀名是不必要的，
///    因为这个后缀名表示格式，属于"表现层"范畴，而URI
///    应该只代表"资源"的位置。它的具体表现形式，应该在
///    HTTP请求的头信息中用Accept和Content-Type字段指定，
///    这两个字段才是对"表现层"的描述。
/// 3. 状态转化（State Transfer）
///    访问一个网站，就代表了客户端和服务器的一个互动
///    过程。在这个过程中，势必涉及到数据和状态的变化。
///    互联网通信协议HTTP协议，是一个无状态协议。这意
///    味着，所有的状态都保存在服务器端。因此，如果客
///    户端想要操作服务器，必须通过某种手段，让服务器
///    端发生"状态转化"（State Transfer）。而这种转化
///    是建立在表现层之上的，所以就是"表现层状态转化"。
///    客户端用到的手段，只能是HTTP协议。具体来说，就是
///    HTTP协议里面，四个表示操作方式的动词：GET、POST、
///    PUT、DELETE。它们分别对应四种基本操作：GET用来
///    获取资源，POST用来新建资源（也可以用于更新资源），
///    PUT用来更新资源，DELETE用来删除资源。
/// 综合上面的解释，我们总结一下什么是RESTful架构：
/// （1）每一个URI代表一种资源；
/// （2）客户端和服务器之间，传递这种资源的某种表现层（Representation）；
/// （3）客户端通过四个HTTP动词，对服务器端资源进行操作，实现"表现层状态转化"。
/////////////////////////////////////////////////////


/// -------------------------------------------------
/// 超文本处理对象实现类
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
    char m_szDir[DCOP_FILE_NAME_LEN];               // 路径
    char m_szHome[DCOP_FILE_NAME_LEN];              // 主页

    ResPathNode *m_pPathNode;                       // 资源路径数组
    DWORD m_dwPathCount;                            // 资源路径数量
    ResTypeNode *m_pTypeNode;                       // 资源类型数组
    DWORD m_dwTypeCount;                            // 资源类型数量

    IManager *m_piManager;                          // 管理器
    IModel *m_piModel;                              // 模型
    IAccess *m_piAccess;                            // 接入管理
};


#endif // #ifndef _HTTPD_RESTFUL_H_

