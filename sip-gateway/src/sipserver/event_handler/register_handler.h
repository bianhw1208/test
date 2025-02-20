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

        // 处理REGISTER请求
        int HandleRequest(const sip_event_sptr& event);

    private:
        // 发送注册响应
        int SendRegisterResponse(const sip_event_sptr& event, int status);
    };

}
}