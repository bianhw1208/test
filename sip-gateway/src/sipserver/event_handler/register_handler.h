#pragma once

#include "base_handler.h"
#include "sipserver/defs.h"

namespace Gateway {
namespace SIP {

    class CRegisterHandler : public CBaseHandler
    {
    public:
        CRegisterHandler() = default;
        virtual ~CRegisterHandler() = default;

        // ����REGISTER����
        int HandleRequest(const sip_event_sptr& event);

    private:
        // ����ע����Ӧ
        int SendRegisterResponse(const sip_event_sptr& event, int status);
    };

}
}