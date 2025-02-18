#pragma once

#include <string>
#include "eXosip2/eXosip.h"
#include "utils/safe_queue.h"
#include "defs.h"
#include "event_handler/handler_manager.h"

namespace Zilu {
namespace Protocol {
namespace GB28181 {

// SIP服务器类 - 处理SIP协议通信的核心类
class CSipServer
{
public:
    CSipServer();

    ~CSipServer();

    // 获取单例实例
    static CSipServer *instance();

    // 初始化SIP服务器
    int Init();

    // 设置本地SIP配置
    // @param sip_id: 本地SIP服务器ID
    // @param host: 本地服务器IP
    // @param port: 本地服务器端口
    int SetLocalConfig(const std::string &sip_id, const std::string &host, uint16_t port);

    // 启动SIP服务器
    // @param user_agent: 用户代理标识
    int Start(const std::string &user_agent);

    // 退出服务器
    int Quit();

    // 获取eXosip上下文
    eXosip_t *GetExosipContext()
    { return m_excontext; }

    // 获取本地SIP ID
    const std::string& GetSipId() { return m_sipId; }

    // 获取本地IP地址
    const std::string& GetHost() { return m_sipHost; }

    // 获取本地端口
    uint16_t GetPort() { return m_sipPort; }

public:
    // 接收SIP事件的线程函数
    int DoReceiveEvents();

    // 处理SIP事件的线程函数
    int DoProcessEvents();

private:
    // 创建新的事件对象
    sip_event_sptr new_event(eXosip_t *exosip_context, eXosip_event_t *exosip_event);

private:
    uint16_t m_sipPort;           // SIP服务器端口
    uint64_t m_eventId;           // 事件ID计数器
    std::string m_sipId;          // SIP服务器ID
    std::string m_sipHost;        // SIP服务器IP
    eXosip_t *m_excontext;        // eXosip上下文
    CSafeQueue <sip_event_sptr> m_eventQueue;  // 事件队列
    CEventHandlerManager m_eventHandle;         // 事件处理器管理器
};

}
}
}
