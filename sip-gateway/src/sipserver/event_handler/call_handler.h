#pragma once

#include "base_handler.h"
#include "./sdp/call_defs.h"

namespace Zilu {
namespace Protocol {
namespace GB28181 {

// 呼叫处理器类 - 处理SIP INVITE类型的消息
class CCallHandler : public CBaseHandler
{
public:
    CCallHandler() {};
    virtual ~CCallHandler() {};

    // 处理接收到的呼叫请求
    int HandleIncomingCall(const sip_event_sptr &e);

private:
    // 处理实时播放请求
    int on_call_play_req(const sip_event_sptr &e, const sdp_description_t &req_sdp);
    // 处理历史回放请求
    int on_call_playback_req(const sip_event_sptr &e, const sdp_description_t &req_sdp);
    // 处理文件下载请求
    int on_call_download_req(const sip_event_sptr &e, const sdp_description_t &req_sdp);
    // 处理语音对讲请求
    int on_call_talk_req(const sip_event_sptr &e, const sdp_description_t &req_sdp);
};

}
}
}