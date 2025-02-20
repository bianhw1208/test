#include "LogWrapper.h"
#include "message_request.h"

namespace Gateway {
namespace SIP {

int CKeepaliveRequest::HandleResponse(int statcode)
{
    LOG_INFO("Keepalive: {}, statcode: {}", m_reqid, statcode);
    m_statcode = statcode;
    onRequestFinished();

    return 0;
}

int CKeepaliveRequest::GetStatCode()
{
    return m_statcode;
}

}
}
