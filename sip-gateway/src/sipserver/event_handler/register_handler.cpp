#include "register_handler.h"
#include "LogWrapper.h"

#include <sstream>

//该系数是由UAS维护的，UAS在接收到UAC的未鉴权报文后，给UAC回复401，在该报文中必须要带相关认证系数和认证方法
//UAS赋值的认证随机数
#define NONCE "9bd055"
//UAS默认加密算法
#define ALGORITHTHM "MD5"

namespace Gateway {
    namespace SIP {
        int CRegisterHandler::HandleRequest(const sip_event_sptr& event)
        {
            eXosip_event_t* exosip_event = event->exevent;

            // 检查是否包含Authorization头域
            osip_message_t* request = exosip_event->request;
            osip_authorization_t* auth_header = nullptr;
            osip_message_get_authorization(request, 0, &auth_header);

            if (auth_header == nullptr) {
                // 发送401 Unauthorized 响应，需要添加WWW-Authenticate头域
                LOG_INFO("Sending 401 Unauthorized for REGISTER request, ID: {}", exosip_event->rid);
                
                osip_message_t* response = nullptr;
                eXosip_message_build_answer(event->excontext, exosip_event->tid, 401, &response);
                
                if (response != nullptr) {
                    //由SIP库生成认证方法和认证参数发送客户端
                    std::stringstream stream;
                    string nonce = NONCE;
                    string algorithm = ALGORITHTHM;
                    stream 
                        << "Digest realm=\"" << event->exevent->request->from->url->host
                        << "\",nonce=\"" << nonce
                        << "\",algorithm=" << algorithm;

                    osip_message_set_header(response, "WWW-Authenticate",
                        stream.str().c_str());
                    
                    eXosip_message_send_answer(event->excontext, exosip_event->tid, SIP_UNAUTHORIZED, response);
                    return 0;
                }
                
                return -1;
            }
            else {
                // 如果认证成功，发送200 OK 响应
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