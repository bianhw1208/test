#pragma once

#include <string>
#include <memory>
#include <map>
#include "base_request.h"
#include "message_request.h"

using namespace std;

namespace Gateway {
namespace SIP {

// 请求管理器类 - 管理所有外发请求的生命周期
class CRequestManager
{
public:
    CRequestManager() = default;
    ~CRequestManager() = default;

    // 初始化请求管理器
    int Init();

    // 创建新的请求
    // @param reqtype: 请求类型
    // @param reqsn: 请求序列号
    // @param bwait: 是否等待响应
    // @return: 请求对象指针
    CBaseRequestSPtr CreateRequest(outcoming_req_type_e reqtype, const string& reqsn, bool bwait);

    // 添加请求到管理器
    // @param reqid: 请求ID
    // @param request: 请求对象
    int AddRequest(string& reqid, CBaseRequestSPtr& req);

    // 处理请求响应
    // @param reqid: 请求ID
    // @param statcode: 响应状态码
    int HandleResponse(const string& reqid, int statcode);

    // 获取请求对象
    // @param reqid: 请求ID
    // @return: 请求对象指针
    CBaseRequestSPtr GetRequest(const string& reqid);

    // 移除请求
    // @param reqid: 请求ID
    int RemoveRequest(const string& reqid);

    // 获取单例实例
    static CRequestManager * instance();

    // 启动请求管理器
    int Start();

    // 根据序列号获取消息请求
    int GetMsgRequestBySn(const string& reqsn, CMessageRequestSptr& msgreq);

    // 从管理器中删除请求
    int DelRequest(outcoming_req_type_e reqtype, string& reqid);

    // 处理消息响应
    int HandleMsgResponse(string& reqid, int status_code);

private:
    // 检查请求超时
    int check_requet_timeout();

    // 获取请求映射表
    base_request_map_t * get_base_request_map(outcoming_req_type_e requtype);

    // 处理响应
    int handle_response(base_request_map_t& reqmap, string& reqid, int status_code);

private:
    base_request_map_t m_messagemap;  // 请求映射表
    mutex m_mutex;                // 互斥锁,保护请求映射表
};

}
}
