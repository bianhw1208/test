#pragma once

#include <osipparser2/osip_message.h>

namespace Gateway {
namespace SIP {

// 基础事件处理器类 - 所有事件处理器的基类
class CBaseHandler
{
public:
    CBaseHandler() = default;
    virtual ~CBaseHandler() = default;

protected:
    // 发送简单响应消息
    int sendSimplyResp(const char* uname, struct eXosip_t* excontext, int tid, int status);

    // 发送呼叫确认消息
    int sendCallAck(struct eXosip_t *excontext, int did);

    // 从响应消息中获取状态码
    int getStatcodeFromResp(osip_message_t* resp);

    // 从请求消息中获取消息ID
    const char* getMsgIdFromReq(osip_message_t* req);
};

}
}