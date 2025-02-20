#include "sipserver/request_manager/request_manager.h"
#include "mediaserver/session_manager.h"
#include "sip_svr_manager.h"
#include "config_manager.h"
#include "sip_server.h"
#include "mediaserver/defs.h"
#include "Thread.h"

namespace Gateway {
namespace SIP {

std::string g_sipfrom_ipport; /*本级域 ip-port 格式*/
std::string g_sipfrom_domain; /*本级域 ip-domain 格式*/

std::string g_sipproxy_ipport; /*上级域 ip-port 格式*/
std::string g_sipproxy_domain; /*上级域 ip-domain 格式*/

eXosip_t *g_excontext;

SipSvrManager *SipSvrManager::instance()
{
    static SipSvrManager _ins;
    return &_ins;
}

SipSvrManager::SipSvrManager() : m_bRegok(false), m_heartbeat("gb_heartbeat")
{

}

SipSvrManager::~SipSvrManager()
{

}

int SipSvrManager::Init()
{
    CConfigManager::instance()->GetLocalSip(m_localsipcfg);
    CConfigManager::instance()->GetRemoteSip(m_remotesipcfg);

    //本级域
    char sipfrom[128] = {0};
    snprintf(sipfrom, sizeof(sipfrom), "sip:%s@%s:%d", m_localsipcfg.svr_id.c_str(),
             m_localsipcfg.svr_ip.c_str(), m_localsipcfg.svr_port);
    g_sipfrom_ipport = sipfrom;
    g_sipfrom_domain = "sip:" + m_localsipcfg.svr_id + "@" + m_localsipcfg.svr_domain;

    //上级域
    char sipproxy[128] = {0};
    snprintf(sipproxy, sizeof(sipproxy), "sip:%s@%s:%d", m_remotesipcfg.svr_id.c_str(),
            m_remotesipcfg.svr_ip.c_str(), m_remotesipcfg.svr_port);
    g_sipproxy_ipport = sipproxy;
    g_sipproxy_domain = "sip:"+m_remotesipcfg.svr_id + "@" + m_remotesipcfg.svr_domain;

    return 0;
}

int SipSvrManager::Start()
{
    CSipServer::instance()->Init();
    CSipServer::instance()->SetLocalConfig(m_localsipcfg.svr_id, m_localsipcfg.svr_ip, m_localsipcfg.svr_port);
    CSipServer::instance()->Start("Protocol/1.0");

    g_excontext = CSipServer::instance()->GetExosipContext();

    CRequestManager::instance()->Start();

#if 0
    register_to_platform();

    //启动心跳
    Infra::ThreadProc proc = bind(&SipSvrManager::heartbeat_proc_to_platform, this, std::placeholders::_1);
    m_heartbeat.start(proc);
#endif
    return 0;
}

int SipSvrManager::register_to_platform()
{
    m_msgsender.RegisterOnline(m_remotesipcfg);
    return 0;
}

int SipSvrManager::OnRegisterSuccess(int rid)
{
    LOG_INFO("rid: {} register success!", rid);
    m_bRegok = true;
    return 0;
}

void SipSvrManager::heartbeat_proc_to_platform(void *param)
{
    LOG_INFO("heartbead thread entry.");

    int timeoutcnt = 0;     //<超时3次未回复 重新向上级注册

    while (m_heartbeat.looping())
    {
        if (m_bRegok) {
            if (0 != keepalive_to_platform()) {
                timeoutcnt++;
            }
            if (timeoutcnt >= 3) {
                m_bRegok = false;
            }
        }
        else {
            register_to_platform();
        }
        Infra::CThread::sleep(50 * 1000);
    }
}

int SipSvrManager::keepalive_to_platform()
{
    CBaseRequestSPtr request;
    int r = m_msgsender.Keepalive(m_remotesipcfg, m_localsipcfg, true, request);
    if (r != 0) {
        return -1;
    }
    LOG_INFO("keepalive wait result.");
    request->WaitResult();
    LOG_INFO("keepalive wait over.");
    auto keepalive_req = dynamic_cast<CKeepaliveRequest*>(request.get());
    r = keepalive_req->GetStatCode();
    return r == 200 ? 0 : -2;
}

int SipSvrManager::HandleDeviceControl(manscdp_devicecontrol_subcmd_e cmd, string &devid,
                                            manscdp_switch_status_e onoff)
{
    return 0;
}

int SipSvrManager::HandlePTZControl(control_cmd_t &ctrlcmd, string &devid)
{
    return 0;
}

int SipSvrManager::HandleMediaRequest(sdp_description_t &req_sdp, const sip_event_sptr &e)
{
    const string& devid = req_sdp.u_uri;
    session_ptr s;
    CSessionManager::instance()->CreateSession(req_sdp.o_sessid, s);
    s->cid = e->exevent->cid;
    s->did = e->exevent->did;
    s->devid = s->devid;

    return 0;
}

}
}
