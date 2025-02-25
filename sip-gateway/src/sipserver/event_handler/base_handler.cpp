#include <eXosip2/eX_setup.h>
#include "base_handler.h"
#include "LogWrapper.h"

namespace Gateway {
namespace SIP {

int CBaseHandler::sendSimplyResp(const char * eventname, eXosip_t *excontext, int tid, int status)
{
    osip_message_t * answer = nullptr;

    eXosip_lock(excontext);
    eXosip_message_build_answer(excontext, tid, status, &answer);
    int r = eXosip_message_send_answer(excontext, tid, status, answer);
    eXosip_unlock(excontext);

    // 从From头域获取设备ID
    /*std::string deviceid;
    if (e->exevent->request->from && e->exevent->request->from->url) {
        deviceid = e->exevent->request->from->url->username;
    }*/

    if (!r){
        LOG_DEBUG("sendSimplyResq: {} to tid: {}, event name: {} sueccess!", status, tid, eventname);
    }
    else {
        LOG_ERROR("sendSimplyResq: {} to tid: {}, event name: {} failed!", status, tid, eventname);
    }
    return r;
}

int CBaseHandler::sendCallAck(eXosip_t *excontext, int did)
{
    eXosip_lock(excontext);

    osip_message_t* ack;
    eXosip_call_build_ack(excontext, did, &ack);
    eXosip_call_send_ack(excontext, did, ack);

    eXosip_unlock(excontext);

    return 0;
}

int CBaseHandler::getStatcodeFromResp(osip_message_t *resp)
{
    return resp != nullptr ? resp->status_code : -1;
}

const char *CBaseHandler::getMsgIdFromReq(osip_message_t *req)
{
    osip_generic_param_t* tag = nullptr;
    osip_to_get_tag(req->from, &tag);
    if (nullptr == tag || nullptr == tag->gvalue) {
        return "";
    }
    return (const char*)tag->gvalue;
}

}
}