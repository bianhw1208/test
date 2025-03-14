#include <sipserver/defs.h>
#include "call_handler.h"
#include "./sdp/sdp.h"
#include "sipserver/sip_svr_manager.h"

namespace Gateway {
namespace SIP {


int CCallHandler::HandleIncomingCall(const sip_event_sptr &e)
{
    osip_body_t* body;
    osip_message_get_body(e->exevent->request, 0, &body);
    if (body == nullptr) {
        LOG_ERROR("HandleIncomingCall get body failed!");
        sendSimplyResp(e->name, e->excontext, e->exevent->tid, SIP_BAD_REQUEST);
        return -1;
    }

    LOG_DEBUG("HandleIncoming req_sdp body: {}", body->body);

    sdp_description_t req_sdp;
    CSdpHandle sdp_handle;
    int r = sdp_handle.ParseSdp(body->body, req_sdp);
    if (r != 0) {
        LOG_ERROR("ParseSdp failed, ret: {}", r);
        sendSimplyResp(e->name, e->excontext, e->exevent->tid, SIP_BAD_REQUEST);
        return -2;
    }

    if (req_sdp.s_sess_name == "Play") {
        on_call_play_req(e, req_sdp);
    }
    else if (req_sdp.s_sess_name == "Playback") {
        on_call_playback_req(e, req_sdp);
    }
    else if (req_sdp.s_sess_name == "Download") {
        on_call_download_req(e, req_sdp);
    }
    else if (req_sdp.s_sess_name == "Talk") {
        on_call_talk_req(e, req_sdp);
    }

    return 0;
}

int CCallHandler::on_call_play_req(const sip_event_sptr &e, const sdp_description_t &req_sdp)
{
#if 0
    int r = SipSvrManager::instance()->HandleMediaRequest(req_sdp, e);
#endif
    return 0;
}

int CCallHandler::on_call_playback_req(const sip_event_sptr &e, const sdp_description_t &req_sdp)
{
    return 0;
}

int CCallHandler::on_call_download_req(const sip_event_sptr &e, const sdp_description_t &req_sdp)
{
    return 0;
}

int CCallHandler::on_call_talk_req(const sip_event_sptr &e, const sdp_description_t &req_sdp)
{
    return 0;
}

}
}
