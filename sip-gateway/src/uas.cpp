#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <osipparser2/osip_message.h>
#include <osipparser2/osip_parser.h>
#include <osipparser2/osip_port.h>

#include <eXosip2/eXosip.h>
#include <eXosip2/eX_setup.h>
#include <eXosip2/eX_register.h>
#include <eXosip2/eX_options.h>
#include <eXosip2/eX_message.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

#define LISTEN_ADDR ("192.168.1.10")
#define UASPORT (5070)


//该系数是由UAS维护的，UAS在接收到UAC的未鉴权报文后，给UAC回复401，在该报文中必须要带相关认证系数和认证方法
//UAS赋值的认证随机数
#define NONCE "9bd055"
//UAS默认加密算法
#define ALGORITHTHM "MD5"

static eXosip_t* s_excontext;

//SIP From头部
class CSipFromHeader
{
public:
    CSipFromHeader()
    {
    }
    ~CSipFromHeader()
    {
    }
    void SetHeader(string addrCod, string addrI, string addrPor)
    {
        addrCode = addrCod;
        addrIp = addrI;
        addrPort = addrPor;
    }
    string GetFormatHeader()
    {
        std::stringstream stream;
        stream << "<sip: " << addrCode << "@" << addrIp << ":" << addrPort
            << ">";
        return stream.str();
    }
    //主机名称
    string GetRealName()
    {
        std::stringstream stream;
        stream << addrIp;
        return stream.str();
    }
private:
    string addrCode;
    string addrIp;
    string addrPort;
};

//SIP Contract头部
class CContractHeader : public CSipFromHeader
{
public:
    CContractHeader()
    {
    }
    ~CContractHeader()
    {
    }
    void SetContractHeader(string addrCod, string addrI, string addrPor,
        int expire)
    {
        SetHeader(addrCod, addrI, addrPor);
        expires = expire;
    }
    string GetContractFormatHeader(bool bExpires)
    {
        if (!bExpires)
        {
            return GetFormatHeader();
        }
        else
        {
            string sTmp = GetFormatHeader();
            std::stringstream stream;
            stream << ";" << "expires=" << expires;
            sTmp += stream.str();
            return sTmp;
        }

    }
private:
    int expires;
};

struct SipContextInfo
{
    //Sip层返回的请求的标志 响应时返回即可
    int sipRequestId;
    //维护一次注册
    string callId;
    //消息所属的功能方法名字符串
    string method;
    //地址编码@域名或IP地址:连接端口，例如sip:1111@127.0.0.1:5060
    CSipFromHeader from;
    //地址编码@域名或IP地址:连接端口，例如sip:1111@127.0.0.1:5060
    CSipFromHeader proxy;
    //地址编码@域名或IP地址:连接端口，例如sip:1111@127.0.0.1:5060
    CContractHeader contact;
    //消息内容,一般为DDCP消息体XML文档,或者具体协议帧要求的其他字符串文本
    string content;
    //响应状态信息
    string status;
    //超时,时间单位为秒
    int expires;
};

struct SipAuthInfo
{
    //平台主机名
    string digestRealm;
    //平台提供的随机数
    string nonce;
    //用户名
    string userName;
    //密码
    string response;
    //“sip:平台地址”,不需要uac赋值
    string uri;
    //加密算法MD5
    string algorithm;
};

struct sipRegisterInfo
{
    SipContextInfo baseInfo;
    SipAuthInfo authInfo;
    bool isAuthNull;
};

void parserRegisterInfo(osip_message_t* request, int iReqId,
    sipRegisterInfo& regInfo)
{
    std::stringstream stream;
    regInfo.baseInfo.method = request->sip_method;
    regInfo.baseInfo.from.SetHeader(request->from->url->username,
        request->from->url->host, request->from->url->port);
    regInfo.baseInfo.proxy.SetHeader(request->to->url->username,
        request->to->url->host, request->to->url->port);
    //获取expires
    osip_header_t* header = NULL;
    {
        osip_message_header_get_byname(request, "expires",
            0, &header);
        if (NULL != header && NULL != header->hvalue)
        {
            regInfo.baseInfo.expires = atoi(header->hvalue);
        }
    }
    //contact字段
    osip_contact_t* contact = NULL;
    osip_message_get_contact(request, 0, &contact);
    if (NULL != contact)
    {
        regInfo.baseInfo.contact.SetContractHeader(contact->url->username,
            contact->url->host, contact->url->port,
            regInfo.baseInfo.expires);
    }
    //注册返回 由发送方维护的请求ID 接收方接收后原样返回即可
    regInfo.baseInfo.sipRequestId = iReqId;
    //CALL_ID
    {
        stream.str("");
        stream << request->call_id->number;
        regInfo.baseInfo.callId = stream.str();
    }
    //解析content消息
    osip_body_t* body = NULL;
    osip_message_get_body(request, 0, &body);
    if (body != NULL)
    {
        stream.str("");
        stream << body->body;
        regInfo.baseInfo.content = stream.str();
    }
    //鉴权信息
    osip_authorization_t* authentication = NULL;
    {
        osip_message_get_authorization(request, 0, &authentication);
        if (NULL == authentication)
        {
            regInfo.isAuthNull = true;
        }
        else
        {
            regInfo.isAuthNull = false;
            stream.str("");
            stream << authentication->username;
            regInfo.authInfo.userName = stream.str();
            stream.str("");
            stream << authentication->algorithm;
            regInfo.authInfo.algorithm = stream.str();
            stream.str("");
            stream << authentication->realm;
            regInfo.authInfo.digestRealm = stream.str();
            stream.str("");
            stream << authentication->nonce;
            regInfo.authInfo.nonce = stream.str();
            stream.str("");
            stream << authentication->response;
            regInfo.authInfo.response = stream.str();
            stream.str("");
            stream << authentication->uri;
            regInfo.authInfo.uri = stream.str();
        }
    }
    authentication = NULL;
}

//打印接收到的响应报文
void printRegisterPkt(sipRegisterInfo& info)
{
    cout << "接收到报文：" << endl;
    cout << "=============================================="
        "==================" << endl;
    cout << "method:" << info.baseInfo.method << endl;
    cout << "from:    " << info.baseInfo.from.GetFormatHeader() << endl;
    cout << "to:" << info.baseInfo.proxy.GetFormatHeader() << endl;
    cout << "contact:" << info.baseInfo.contact.GetContractFormatHeader(false)
        << endl;

    //注册返回 由发送方维护的请求ID 接收方接收后原样返回即可
    cout << "sipRequestId:" << info.baseInfo.sipRequestId << endl;
    //CALL_ID
    cout << "CallId:" << info.baseInfo.callId << endl;
    //解析content消息
    cout << "body:" << info.baseInfo.content << endl;
    //获取expires
    cout << "expires:" << info.baseInfo.expires << endl;
    //鉴权信息
    if (info.isAuthNull)
    {
        cout << "当前报文未提供鉴权信息!!!" << endl;
    }
    else
    {
        cout << "当前报文鉴权信息如下:" << endl;
        cout << "username:" << info.authInfo.userName << endl;
        cout << "algorithm:" << info.authInfo.algorithm << endl;
        cout << "Realm:" << info.authInfo.digestRealm << endl;
        cout << "nonce:" << info.authInfo.nonce << endl;
        cout << "response:" << info.authInfo.response << endl;
        cout << "uri:" << info.authInfo.uri << endl;
    }
    cout << "=================================================="
        "==============" << endl;
    return;
}
void sendRegisterAnswer(sipRegisterInfo& info)
{
    osip_message_t* answer = NULL;
    int iStatus;
    if (info.isAuthNull)
    {
        iStatus = 401;
    }
    else
    {
        iStatus = 200;
    }
    eXosip_lock(s_excontext);
    {
        int result = ::eXosip_message_build_answer(s_excontext, info.baseInfo.sipRequestId, iStatus, &answer);
        if (iStatus == 401)
        {
            //由SIP库生成认证方法和认证参数发送客户端
            std::stringstream stream;
            string nonce = NONCE;
            string algorithm = ALGORITHTHM;
            stream << "Digest realm=\"" << info.baseInfo.from.GetRealName()
                << "\",nonce=\"" << nonce
                << "\",algorithm=" << algorithm;

            osip_message_set_header(answer, "WWW-Authenticate",
                stream.str().c_str());
            cout << "======================================================="
                "=========" << endl;
            cout << "发送401报文" << endl;
            cout << "========================================================"
                "========" << endl;
        }
        else if (iStatus == 200)
        {
            osip_message_set_header(answer, "Contact",
                info.baseInfo.contact.GetContractFormatHeader(true).c_str());
            cout << "========================================================="
                "=======" << endl;
            cout << "发送200报文" << endl;
            cout << "=========================================================="
                "======" << endl;
            //string_t b = "<sip: 100110000101000000@192.168.31.18:5061>;expires=600";
            //osip_message_set_header(answer, "Contact", b.c_str());
        }
        else
        {
            //Do nothing
        }

        if (OSIP_SUCCESS != result)
        {
            ::eXosip_message_send_answer(s_excontext, info.baseInfo.sipRequestId, 400, NULL);
        }
        else
        {
            //发送消息体
            ::eXosip_message_send_answer(s_excontext, info.baseInfo.sipRequestId, iStatus,
                answer);
        }
        if (0 == info.baseInfo.expires)
        {
            eXosip_register_remove(s_excontext, info.baseInfo.sipRequestId);
        }
    }eXosip_unlock(s_excontext);
}
void OnRegister(eXosip_event_t* osipEvent)
{
    sipRegisterInfo regInfo;
    parserRegisterInfo(osipEvent->request, osipEvent->tid, regInfo);
    //打印报文
    printRegisterPkt(regInfo);
    //发送应答报文
    sendRegisterAnswer(regInfo);

}

int main1()
{
    s_excontext = eXosip_malloc();
    int result = OSIP_SUCCESS;
    // init exosip.
    if (OSIP_SUCCESS != (result = eXosip_init(s_excontext)))
    {
        printf("eXosip_init failure.\n");
        return 1;
    }
    cout << "eXosip_init success." << endl;
    //
    //        if (null_ptr != this->receiveSipMessageCallback || null_ptr
    //                != this->sendSipMessageCallback)
    //        {
    //            if (OSIP_SUCCESS != (result = ::eXosip_set_cbsip_message(
    //                    &Sip::MessageCallback)))
    //            {
    //                return;
    //            }
    //        }
    eXosip_set_user_agent(s_excontext, "Protocol/1.0");

    if (OSIP_SUCCESS != eXosip_listen_addr(s_excontext, IPPROTO_UDP, LISTEN_ADDR, UASPORT, AF_INET, 0))
    {
        printf("eXosip_listen_addr failure.\n");
        return 1;
    }

    if (OSIP_SUCCESS != eXosip_set_option(s_excontext, EXOSIP_OPT_SET_IPV4_FOR_GATEWAY, LISTEN_ADDR))
    {
        return -1;
    }
    //开启循环消息，实际应用中可以开启多线程同时接收信号
    eXosip_event_t* osipEventPtr = NULL;

    while (true)
    {
        // Wait the osip event.
        osipEventPtr = eXosip_event_wait(s_excontext, 0, 200);// 0的单位是秒，500是毫秒
        // If get nothing osip event,then continue the loop.
        if (NULL == osipEventPtr)
        {
            continue;
        }
        // 事件处理

        switch (osipEventPtr->type)
        {

            //需要继续验证REGISTER是什么类型
        /*case EXOSIP_REGISTRATION_NEW:
            OnRegister(osipEventPtr);
            break;*/
        case EXOSIP_MESSAGE_NEW:
        {
            if (!strncmp(osipEventPtr->request->sip_method, "REGISTER",
                strlen("REGISTER")))
            {
                OnRegister(osipEventPtr);
            }
            else if (!strncmp(osipEventPtr->request->sip_method, "MESSAGE",
                strlen("MESSAGE")))
            {
                //
            }
        }
        break;
        default:
            cout
                << "The sip event type that not be precessed.the event "
                "type is : "
                << osipEventPtr->type;
            break;
        }
        //ProcessSipEvent(this->osipEventPtrParam);
        eXosip_event_free(osipEventPtr);
        osipEventPtr = NULL;
    }

    return 0;
}