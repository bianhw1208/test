#pragma once

#include "call_defs.h"

namespace Zilu {
namespace Protocol {
namespace GB28181 {

// SDP消息处理类 - 处理SDP消息的解析和组装
class CSdpHandle
{
public:
    CSdpHandle();
    ~CSdpHandle();

    // 解析SDP消息
    int ParseSdp(const char *buf, sdp_description_t &sdp_desc);
    // 组装SDP消息
    int AssembleSdp(string &sdp, const sdp_description_t &sdp_desc);
};

}
}
}