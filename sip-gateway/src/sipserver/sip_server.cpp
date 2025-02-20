#include "sip_server.h"
#include <netinet/in.h>
#include "LogWrapper.h"
#include "threadpool.h"
#include "event_handler/handler_manager.h"

namespace Gateway {
namespace SIP {

// 获取单例实例
CSipServer *CSipServer::instance()
{
    static CSipServer server;
    return &server;
}

// 构造函数,初始化基本参数
CSipServer::CSipServer() : m_sipPort(5060), m_eventId(1000000000), m_excontext(nullptr) {}

CSipServer::~CSipServer() = default;

// 初始化SIP服务器
int CSipServer::Init()
{
    // 分配eXosip上下文
    m_excontext = eXosip_malloc();

    // 初始化eXosip
    int ret = eXosip_init(m_excontext);
    if (ret != 0) {
        LOG_ERROR("eXosip_init failed, ret = ", ret);
        return -1;
    }
    LOG_INFO("eXosip_init successfully!");
    return 0;
}

// 设置本地SIP配置
int CSipServer::SetLocalConfig(const std::string &sip_id, const std::string &host, uint16_t port)
{
    m_sipId = sip_id;    // SIP服务器ID
    m_sipHost = host;    // 服务器IP
    m_sipPort = port;    // 服务器端口

    return 0;
}

// 启动SIP服务器
int CSipServer::Start(const std::string &user_agent)
{
    // 监听指定地址和端口
    int ret = eXosip_listen_addr(m_excontext, IPPROTO_UDP,
            m_sipHost.c_str(), m_sipPort, AF_INET, 0);
    if (ret != 0) {
        eXosip_quit(m_excontext);
        
        LOG_ERROR("eXosip_listen_addr failed, ret: {}, errno: {}", ret, errno);
        return -1;
    }

    // 设置用户代理标识
    if (user_agent.length() > 0)
        eXosip_set_user_agent(m_excontext, user_agent.c_str());

    // 启动事件接收和处理线程
    Infra::ThreadTask recvTask = std::bind(&CSipServer::DoReceiveEvents, this);
    Infra::ThreadTask procTask1 = std::bind(&CSipServer::DoProcessEvents, this);

    Infra::CThreadPool::instance()->run(recvTask);
    Infra::CThreadPool::instance()->run(procTask1);

    return 0;
}

int CSipServer::Quit()
{
    eXosip_quit(m_excontext);
    return 0;
}

// 接收SIP事件
int CSipServer::DoReceiveEvents()
{
    while (1) {
        // 等待并接收事件
        eXosip_event_t *event = eXosip_event_wait(m_excontext, 0, 50);
        if (event == nullptr)
            continue;

        eXosip_lock(m_excontext);
        eXosip_default_action(m_excontext, event);
        eXosip_unlock(m_excontext);

        // 创建新的事件对象并加入队列
        sip_event_sptr e = new_event(m_excontext, event);
        m_eventQueue.Push(e);

        // 释放eXosip事件, 挪至DoProcessEvents中释放
        //eXosip_event_free(event);
    }
    return 0;
}

// 处理SIP事件
int CSipServer::DoProcessEvents()
{
    while (1) {
        // 从队列获取事件
        sip_event_sptr e;
        if (!m_eventQueue.Pop(e))
            continue;

        // 调用事件处理器处理事件
        if (e->proc) {
            LOG_INFO("do process event, id: {}, type: {}, method: {}", e->id, e->name, e->exevent->request->sip_method);
            e->proc(e);

            // 释放eXosip事件
            eXosip_event_free(e->exevent);
        }
    }
    return 0;
}

// 创建新的事件对象
sip_event_sptr CSipServer::new_event(eXosip_t *exosip_context, eXosip_event_t *exosip_event)
{
    if (exosip_event == nullptr)
        return nullptr;

    if (exosip_event->type < EXOSIP_REGISTRATION_SUCCESS || exosip_event->type > EXOSIP_NOTIFICATION_GLOBALFAILURE)
        return nullptr;

    sip_event_sptr event(new sip_event_t);// = std::make_shared(SipEvent)();

    // 获取事件处理器
    CEventHandlerManager::EventNameProcPair pair = m_eventHandle.GetEventProc(exosip_event->type);
    if (pair.name == nullptr)
        return nullptr;

    event->name = pair.name;
    event->proc = pair.proc;
    event->excontext = exosip_context;
    event->exevent = exosip_event;
    event->id = m_eventId++;
    LOG_INFO("receive new event, id: {}, type: {}, method: {}", event->id, event->name, event->exevent->request->sip_method);
    return event;
}

}
}
