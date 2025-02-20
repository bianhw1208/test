#include <eXosip2/eX_setup.h>
#include "base_handler.h"
#include "LogWrapper.h"

namespace Gateway {
namespace SIP {

int CBaseHandler::sendSimplyResp(const char *uname, eXosip_t *excontext, int tid, int status)
{
    osip_message_t * answer = nullptr;

    eXosip_lock(excontext);
    eXosip_message_build_answer(excontext, tid, status, &answer);
    int r = eXosip_message_send_answer(excontext, tid, status, nullptr);
    eXosip_unlock(excontext);

    LOG_DEBUG("sendSimplyResq: {} to uname: {}, ret: {}", status, uname, r);

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