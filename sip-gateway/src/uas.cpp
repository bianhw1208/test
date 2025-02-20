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


//��ϵ������UASά���ģ�UAS�ڽ��յ�UAC��δ��Ȩ���ĺ󣬸�UAC�ظ�401���ڸñ����б���Ҫ�������֤ϵ������֤����
//UAS��ֵ����֤�����
#define NONCE "9bd055"
//UASĬ�ϼ����㷨
#define ALGORITHTHM "MD5"

static eXosip_t* s_excontext;

//SIP Fromͷ��
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
    //��������
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

//SIP Contractͷ��
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
    //Sip�㷵�ص�����ı�־ ��Ӧʱ���ؼ���
    int sipRequestId;
    //ά��һ��ע��
    string callId;
    //��Ϣ�����Ĺ��ܷ������ַ���
    string method;
    //��ַ����@������IP��ַ:���Ӷ˿ڣ�����sip:1111@127.0.0.1:5060
    CSipFromHeader from;
    //��ַ����@������IP��ַ:���Ӷ˿ڣ�����sip:1111@127.0.0.1:5060
    CSipFromHeader proxy;
    //��ַ����@������IP��ַ:���Ӷ˿ڣ�����sip:1111@127.0.0.1:5060
    CContractHeader contact;
    //��Ϣ����,һ��ΪDDCP��Ϣ��XML�ĵ�,���߾���Э��֡Ҫ��������ַ����ı�
    string content;
    //��Ӧ״̬��Ϣ
    string status;
    //��ʱ,ʱ�䵥λΪ��
    int expires;
};

struct SipAuthInfo
{
    //ƽ̨������
    string digestRealm;
    //ƽ̨�ṩ�������
    string nonce;
    //�û���
    string userName;
    //����
    string response;
    //��sip:ƽ̨��ַ��,����Ҫuac��ֵ
    string uri;
    //�����㷨MD5
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
    //��ȡexpires
    osip_header_t* header = NULL;
    {
        osip_message_header_get_byname(request, "expires",
            0, &header);
        if (NULL != header && NULL != header->hvalue)
        {
            regInfo.baseInfo.expires = atoi(header->hvalue);
        }
    }
    //contact�ֶ�
    osip_contact_t* contact = NULL;
    osip_message_get_contact(request, 0, &contact);
    if (NULL != contact)
    {
        regInfo.baseInfo.contact.SetContractHeader(contact->url->username,
            contact->url->host, contact->url->port,
            regInfo.baseInfo.expires);
    }
    //ע�᷵�� �ɷ��ͷ�ά��������ID ���շ����պ�ԭ�����ؼ���
    regInfo.baseInfo.sipRequestId = iReqId;
    //CALL_ID
    {
        stream.str("");
        stream << request->call_id->number;
        regInfo.baseInfo.callId = stream.str();
    }
    //����content��Ϣ
    osip_body_t* body = NULL;
    osip_message_get_body(request, 0, &body);
    if (body != NULL)
    {
        stream.str("");
        stream << body->body;
        regInfo.baseInfo.content = stream.str();
    }
    //��Ȩ��Ϣ
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

//��ӡ���յ�����Ӧ����
void printRegisterPkt(sipRegisterInfo& info)
{
    cout << "���յ����ģ�" << endl;
    cout << "=============================================="
        "==================" << endl;
    cout << "method:" << info.baseInfo.method << endl;
    cout << "from:    " << info.baseInfo.from.GetFormatHeader() << endl;
    cout << "to:" << info.baseInfo.proxy.GetFormatHeader() << endl;
    cout << "contact:" << info.baseInfo.contact.GetContractFormatHeader(false)
        << endl;

    //ע�᷵�� �ɷ��ͷ�ά��������ID ���շ����պ�ԭ�����ؼ���
    cout << "sipRequestId:" << info.baseInfo.sipRequestId << endl;
    //CALL_ID
    cout << "CallId:" << info.baseInfo.callId << endl;
    //����content��Ϣ
    cout << "body:" << info.baseInfo.content << endl;
    //��ȡexpires
    cout << "expires:" << info.baseInfo.expires << endl;
    //��Ȩ��Ϣ
    if (info.isAuthNull)
    {
        cout << "��ǰ����δ�ṩ��Ȩ��Ϣ!!!" << endl;
    }
    else
    {
        cout << "��ǰ���ļ�Ȩ��Ϣ����:" << endl;
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
            //��SIP��������֤��������֤�������Ϳͻ���
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
            cout << "����401����" << endl;
            cout << "========================================================"
                "========" << endl;
        }
        else if (iStatus == 200)
        {
            osip_message_set_header(answer, "Contact",
                info.baseInfo.contact.GetContractFormatHeader(true).c_str());
            cout << "========================================================="
                "=======" << endl;
            cout << "����200����" << endl;
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
            //������Ϣ��
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
    //��ӡ����
    printRegisterPkt(regInfo);
    //����Ӧ����
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
    //����ѭ����Ϣ��ʵ��Ӧ���п��Կ������߳�ͬʱ�����ź�
    eXosip_event_t* osipEventPtr = NULL;

    while (true)
    {
        // Wait the osip event.
        osipEventPtr = eXosip_event_wait(0, 0, 200);// 0�ĵ�λ���룬500�Ǻ���
        // If get nothing osip event,then continue the loop.
        if (NULL == osipEventPtr)
        {
            continue;
        }
        // �¼�����

        switch (osipEventPtr->type)
        {

            //��Ҫ������֤REGISTER��ʲô����
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