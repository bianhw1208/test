#pragma once

#include <memory>
#include <map>
#include <condition_variable>
#include "defs.h"
using namespace std;

namespace Zilu {
namespace Protocol {
namespace GB28181 {

// 基础请求类 - 所有请求类型的基类
class CBaseRequest
{
public:
    // 构造函数,指定请求类型
    explicit CBaseRequest(outcoming_req_type_e reqtype);
    virtual ~CBaseRequest();

    // 处理响应消息
    virtual int HandleResponse(int statcode);

    // 设置是否等待响应
    void SetWait(bool bwait);

    // 等待响应结果
    void WaitResult();

    // 检查请求是否完成
    bool IsFinished();

    // 设置请求ID
    int SetReqid(string& id);

    // 获取请求类型
    outcoming_req_type_e GetReqType();

    // 获取请求发起时间
    time_t GetReqtime();

protected:
    // 标记请求完成
    int onRequestFinished();

private:
    // 完成等待,通知等待线程
    void finished();

protected:
    string m_reqid;               // 请求ID

private:
    bool m_bfinished;            // 请求是否完成标志
    bool m_bwait;                // 是否需要等待响应
    time_t m_reqtime;            // 请求发起时间
    outcoming_req_type_e m_reqtype;  // 请求类型
    mutex m_mutex;               // 互斥锁
    condition_variable m_cond;   // 条件变量,用于等待响应
};

typedef shared_ptr<CBaseRequest> CBaseRequestSPtr;
typedef map<string, CBaseRequestSPtr> BaseRequestMap;

// 请求映射表结构
struct base_request_map_t
{
    mutex m;                     // 保护请求映射表的互斥锁
    BaseRequestMap req_map;      // 请求ID到请求对象的映射表
};

}
}
}
