#include "LogWrapper.h"
#include "message_request.h"

namespace Zilu {
namespace Protocol {
namespace GB28181 {

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
}
