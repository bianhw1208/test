#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <unistd.h>
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

//���ؼ���IP
#define LISTEN_ADDR "192.168.50.57"
//���ؼ����˿�
#define UACPORT "5061"
#define UACPORTINT 5061
//��UAC��ַ����
#define UACCODE "100110000201000000"
//����UAC����
#define UACPWD "12345"
//Զ��UAS IP
#define UAS_ADDR "192.168.50.57"
//Զ��UAS �˿�
#define UAS_PORT "5060"
//��ʱ
#define EXPIS 300

//��ǰ����״̬ 1 �Ѿ�ע�� 0 δע��
static int iCurrentStatus;
//ע��ɹ�HANDLE
static int iHandle = -1;

eXosip_t* s_excontext;

//SIP From/To ͷ��
class CSipFromToHeader
{
public:
    CSipFromToHeader(){}
    ~CSipFromToHeader()
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
        stream << "sip: " << addrCode << "@" << addrIp << ":" << addrPort;
        return stream.str();
    }
    //��������
    string GetCode()
    {
        std::stringstream stream;
        stream << addrCode;
        return stream.str();
    }
    //������ַ
    string GetAddr()
    {
        std::stringstream stream;
        stream << addrIp;
        return stream.str();
    }
    //�˿�
    string GetPort()
    {
        std::stringstream stream;
        stream << addrPort;
        return stream.str();
    }

private:
    string addrCode;
    string addrIp;
    string addrPort;
};

//SIP Contractͷ��
class CContractHeader : public CSipFromToHeader
{
public:
    CContractHeader()
    {
    }
    ~CContractHeader()
    {
    }
    void SetContractHeader(string addrCod, string addrI, string addrPor)
    {
        SetHeader(addrCod, addrI, addrPor);
    }
    string GetContractFormatHeader()
    {

        std::stringstream stream;
        stream << "<sip:" << GetCode() << "@" << GetAddr() << ":" << GetPort() << ">";
        return stream.str();
    }
};

//����ע����Ϣ
int SendRegister(int& registerId, CSipFromToHeader & from, CSipFromToHeader & to,
        CContractHeader & contact, const string & userName, const string & pwd,
        const int expires, int iType)
{
    cout << "=============================================" << endl;
    if (iType == 0)
    {
        cout << "ע��������Ϣ��" << endl;
    }
    else if (iType == 1)
    {
        cout << "ˢ��ע����Ϣ��" << endl;
    }
    else
    {
        cout << "ע����Ϣ:" << endl;
    }
    cout << "registerId " << registerId << endl;
    cout << "from " << from.GetFormatHeader() << endl;
    cout << "to " << to.GetFormatHeader() << endl;
    cout << "contact" << contact.GetContractFormatHeader() << endl;
    cout << "userName" << userName << endl;
    cout << "pwd" << pwd << endl;
    cout << "expires" << expires << endl;
    cout << "=============================================" << endl;
    //������ע��
    static osip_message_t * regMsg = 0;
    int ret;

    ::eXosip_add_authentication_info(s_excontext, userName.c_str(), userName.c_str(), pwd.c_str(), "MD5", NULL);
    eXosip_lock(s_excontext);
    //����ע����Ϣ 401��Ӧ��eXosip2���Զ�����
    if (0 == registerId)
    {
        // ע����Ϣ�ĳ�ʼ��
        registerId = ::eXosip_register_build_initial_register(
            s_excontext, from.GetFormatHeader().c_str(), to.GetFormatHeader().c_str(),
            contact.GetContractFormatHeader().c_str(), expires, &regMsg);
        if (registerId <= 0)
        {
            return -1;
        }
    }
    else
    {
        // ����ע����Ϣ
        ret = ::eXosip_register_build_register(s_excontext, registerId, expires, &regMsg);
        if (ret != OSIP_SUCCESS)
        {
            return ret;
        }
        //���ע��ԭ��
        if (expires == 0)
        {
            osip_contact_t * contact = NULL;
            char tmp[128];

            osip_message_get_contact(regMsg, 0, &contact);
            {
                sprintf(tmp, "<sip:%s@%s:%s>;expires=0",
                        contact->url->username, contact->url->host,
                        contact->url->port);
            }
            //osip_contact_free(contact);
            //reset contact header
            osip_list_remove(&regMsg->contacts, 0);
            osip_message_set_contact(regMsg, tmp);
            osip_message_set_header(regMsg, "Logout-Reason", "logout");
        }
    }
    // ����ע����Ϣ
    ret = ::eXosip_register_send_register(s_excontext, registerId, regMsg);
    if (ret != OSIP_SUCCESS)
    {
        registerId = 0;
    }eXosip_unlock(s_excontext);

    return ret;
}

//ע��
void Register()
{
    if (iCurrentStatus == 1)
    {
        cout << "��ǰ�Ѿ�ע��" << endl;
        return;
    }
    CSipFromToHeader stFrom;
    stFrom.SetHeader(UACCODE, UAS_ADDR, UAS_PORT);
    CSipFromToHeader stTo;
    stTo.SetHeader(UACCODE, UAS_ADDR, UAS_PORT);
    CContractHeader stContract;
    stContract.SetContractHeader(UACCODE, LISTEN_ADDR, UACPORT);
    //����ע����Ϣ
    int registerId = 0;
    if (0 > SendRegister(registerId, stFrom, stTo, stContract, UACCODE, UACPWD,
            3000, 0))
    {
        cout << "����ע��ʧ��" << endl;
        return;
    }
    iCurrentStatus = 1;
    iHandle = registerId;
}
//ˢ��ע��
void RefreshRegister()
{
    if (iCurrentStatus == 0)
    {
        cout << "��ǰδע�ᣬ������ˢ��" << endl;
        return;
    }
    CSipFromToHeader stFrom;
    stFrom.SetHeader(UACCODE, UAS_ADDR, UAS_PORT);
    CSipFromToHeader stTo;
    stTo.SetHeader(UACCODE, UAS_ADDR, UAS_PORT);
    CContractHeader stContract;
    stContract.SetContractHeader(UACCODE, LISTEN_ADDR, UACPORT);
    //����ע����Ϣ
    if (0 > SendRegister(iHandle, stFrom, stTo, stContract, UACCODE, UACPWD,
            3000, 1))
    {
        cout << "����ˢ��ע��ʧ��" << endl;
        return;
    }
}
//ע��
void UnRegister()
{
    if (iCurrentStatus == 0)
    {
        cout << "��ǰδע�ᣬ������ע��" << endl;
        return;
    }
    CSipFromToHeader stFrom;
    stFrom.SetHeader(UACCODE, UAS_ADDR, UAS_PORT);
    CSipFromToHeader stTo;
    stTo.SetHeader(UACCODE, UAS_ADDR, UAS_PORT);
    CContractHeader stContract;
    stContract.SetContractHeader(UACCODE, LISTEN_ADDR, UACPORT);
    //����ע����Ϣ
    if (0 > SendRegister(iHandle, stFrom, stTo, stContract, UACCODE, UACPWD,
            0, 2))
    {
        cout << "����ע��ʧ��" << endl;
        return;
    }
    iCurrentStatus = 0;
    iHandle = -1;
}
static void help()
{
    const char* b =
    "-------------------------------------------------------------------------------\n"
    "SIP Library test process - uac v 1.0 (June 13, 2014)\n\n"
    "SIP UAC�� ע��,ˢ��ע��,ע��ʵ��\n\n"
    "Author: ��������\n\n"
    "���͵�ַ:http://blog.csdn.net/hiwubihe QQ:1269122125\n\n"
    "-------------------------------------------------------------------------------\n"
    "\n"
    "              0:Register\n"
    "              1:RefreshRegister\n"
    "              2:UnRegister\n"
    "              3:clear scream\n"
    "              4:exit\n"
    "-------------------------------------------------------------------------------\n"
    "\n";
    fprintf(stderr, b, strlen(b));
    cout << "please select method :";
}
//�������߳�
void* serverHandle(void* pUser)
{
    sleep(3);
    help();
    char ch = getchar();
    getchar();
    while (1)
    {
        switch (ch)
        {
        case '0':
            //ע��
            Register();
            break;
        case '1':
            //ˢ��ע��
            RefreshRegister();
            break;
        case '2':
            //ע��
            UnRegister();
            break;
        case '3':
            if (system("clear") < 0)
            {
                cout << "clear scream error" << endl;
                exit(1);
            }
            break;
        case '4':
            cout << "exit sipserver......" << endl;
            getchar();
            exit(0);
        default:
            cout << "select error" << endl;
            break;
        }
        cout << "press any key to continue......" << endl;
        getchar();
        help();
        ch = getchar();
        getchar();
    }
    return NULL;
}

//�¼������߳�
void* eventHandle(void* pUser)
{
    eXosip_event_t * osipEventPtr = (eXosip_event_t*)pUser;
    switch (osipEventPtr->type)
    {
    //��Ҫ������֤REGISTER��ʲô����
    case EXOSIP_REGISTRATION_SUCCESS:
    case EXOSIP_REGISTRATION_FAILURE:
    {
        cout << "�յ�״̬��:" << osipEventPtr->response->status_code << "����" << endl;
        if (osipEventPtr->response->status_code == 401)
        {
            cout << "���ͼ�Ȩ����" << endl;
        }
        else if (osipEventPtr->response->status_code == 200)
        {
            cout << "���ճɹ�" << endl;
        }
        else
        {}
     }
        break;
    default:
        cout << "The sip event type that not be precessed.the event "
            "type is : " << osipEventPtr->type << endl;
        break;
    }
    eXosip_event_free(osipEventPtr);
    return NULL;
}

int main2()
{
    s_excontext = eXosip_malloc();
    iCurrentStatus = 0;
    //�⴦����
    int result = OSIP_SUCCESS;
    //��ʼ����
    if (OSIP_SUCCESS != (result = eXosip_init(s_excontext)))
    {
        printf("eXosip_init failure.\n");
        return 1;
    }
    cout << "eXosip_init success." << endl;
    eXosip_set_user_agent(s_excontext, NULL);
    //����
    if (OSIP_SUCCESS != eXosip_listen_addr(s_excontext, IPPROTO_UDP, NULL, UACPORTINT,
            AF_INET, 0))
    {
        printf("eXosip_listen_addr failure.\n");
        return 1;
    }
    //���ü�������
    if (OSIP_SUCCESS != eXosip_set_option(s_excontext, EXOSIP_OPT_SET_IPV4_FOR_GATEWAY, LISTEN_ADDR))
    {
        return -1;
    }
    //���������߳�
    pthread_t pthser;
    if (0 != pthread_create(&pthser, NULL, serverHandle, NULL))
    {
        printf("����������ʧ��\n");
        return -1;
    }
    //�¼����ڵȴ�
    eXosip_event_t * osipEventPtr = NULL;
    //�����¼�ѭ��
    while (true)
    {
        //�ȴ��¼� 0�ĵ�λ���룬500�Ǻ���
        osipEventPtr = ::eXosip_event_wait(s_excontext, 0, 200);
        //����eXosip��Ĭ�ϴ���
        {
            eXosip_lock(s_excontext);
            //һ�㴦��401/407���ÿ�Ĭ�ϴ���
            eXosip_default_action(s_excontext, osipEventPtr);
            eXosip_unlock(s_excontext);
        }
        //�¼��ռ����ȴ�
        if (NULL == osipEventPtr)
        {
            continue;
        }
        //�����̴߳����¼������¼�������Ͻ��¼�ָ���ͷ�
        pthread_t pth;
        if (0 != pthread_create(&pth, NULL, eventHandle, (void*)osipEventPtr))
        {
            printf("�����̴߳����¼�ʧ��\n");
            continue;
        }
        osipEventPtr = NULL;
    }
}