#pragma once

#include <string>
#include "base_request.h"

namespace Gateway {
namespace SIP {

// 消息请求类 - 处理消息类型的请求
class CMessageRequest : public CBaseRequest
{
public:
    // 构造函数,指定请求类型和序列号
    CMessageRequest(outcoming_req_type_e reqtype, const string& reqsn) : CBaseRequest(reqtype), m_reqsn(reqsn) {}
    virtual ~CMessageRequest() {}

    // 获取请求序列号
    const string& GetReqSn() {
        return m_reqsn;
    }

private:
    string m_reqsn;              // 请求序列号
};
typedef shared_ptr<CMessageRequest> CMessageRequestSptr;

// 保活请求类 - 处理保活消息
class CKeepaliveRequest : public CMessageRequest
{
public:
    CKeepaliveRequest(const string& reqsn) : CMessageRequest(OUTCOMING_REQU_TYPE_KEEPALIVE, reqsn) {}
    virtual ~CKeepaliveRequest() {}

    // 处理保活响应
    virtual int HandleResponse(int statcode);

    // 获取状态码
    // -1: keepalive failed; 200: OK
    int GetStatCode();

private:
    int m_statcode;             // 保活响应状态码
};

}
}
