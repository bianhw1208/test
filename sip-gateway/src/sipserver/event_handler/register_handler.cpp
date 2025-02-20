#include "register_handler.h"
#include "LogWrapper.h"

namespace Gateway {
    namespace SIP {
        int CRegisterHandler::HandleRequest(const sip_event_sptr& event)
        {
            eXosip_event_t* exosip_event = event->exevent;

            // ��������Ƿ����Authorizationͷ��
            osip_message_t* request = exosip_event->request;
            osip_authorization_t* auth_header = nullptr;
            osip_message_get_authorization(request, 0, &auth_header);

            if (auth_header == nullptr) {
                // ����401 Unauthorized ��Ӧ
                LOG_INFO("Sending 401 Unauthorized for REGISTER request, ID: {}", exosip_event->rid);
                return SendRegisterResponse(event, SIP_UNAUTHORIZED);
            }
            else {
                // ������������֤��REGISTER����
                LOG_INFO("Handling authenticated REGISTER request, ID: {}", exosip_event->rid);

                // ���������Ӹ�����߼�������ע������������֤�û���Ϣ��

                // ������֤�ɹ�������200 OK ��Ӧ
                return SendRegisterResponse(event, SIP_OK);
            }
        }

        int CRegisterHandler::SendRegisterResponse(const sip_event_sptr& event, int status)
        {
            eXosip_event_t* exosip_event = event->exevent;

            // ������Ӧ
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