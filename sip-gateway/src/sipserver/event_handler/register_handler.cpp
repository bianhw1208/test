#include "register_handler.h"
#include "LogWrapper.h"

namespace Gateway {
    namespace SIP {
        int CRegisterHandler::HandleRequest(const sip_event_sptr& event)
        {
            eXosip_event_t* exosip_event = event->exevent;

            // 检查请求是否带有Authorization头部
            osip_message_t* request = exosip_event->request;
            osip_authorization_t* auth_header = nullptr;
            osip_message_get_authorization(request, 0, &auth_header);

            if (auth_header == nullptr) {
                // 发送401 Unauthorized 响应
                LOG_INFO("Sending 401 Unauthorized for REGISTER request, ID: {}", exosip_event->rid);
                return SendRegisterResponse(event, SIP_UNAUTHORIZED);
            }
            else {
                // 处理带有身份验证的REGISTER请求
                LOG_INFO("Handling authenticated REGISTER request, ID: {}", exosip_event->rid);

                // 这里可以添加更多的逻辑来处理注册请求，例如验证用户信息等

                // 假设验证成功，发送200 OK 响应
                return SendRegisterResponse(event, SIP_OK);
            }
        }

        int CRegisterHandler::SendRegisterResponse(const sip_event_sptr& event, int status)
        {
            eXosip_event_t* exosip_event = event->exevent;

            // 发送响应
            int ret = sendSimplyResp(event->name, event->excontext, exosip_event->tid, status);
            if (ret != 0) {
                LOG_ERROR("Failed to send REGISTER response, status: {}", status);
                return -1;
            }

            LOG_INFO("Sent REGISTER response with status: {}", status);
            return 0;
        }

    }
}