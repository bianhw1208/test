#pragma once
#include <sipserver/event_handler/sdp/call_defs.h>
#include "sipserver/manscdp/request_sender.h"
#include "config_manager.h"
#include "sipserver/manscdp/defs/defs.h"
#include "utils/ptz/defs.h"
#include "defs.h"
#include "Thread.h"

namespace Gateway {
namespace SIP {

// GB28181服务管理器类 - 整个GB28181服务的核心管理类
class SipSvrManager
{
public:
    SipSvrManager();
    ~SipSvrManager();

    // 获取单例实例
    static SipSvrManager* instance();

    // 初始化服务管理器
    int Init();
    
    // 启动服务
    int Start();

    // 注册成功回调
    int OnRegisterSuccess(int rid);

    // 处理设备控制命令
    int HandleDeviceControl(manscdp_devicecontrol_subcmd_e cmd, string &devid,
                          manscdp_switch_status_e onoff);

    // 处理云台控制命令
    int HandlePTZControl(control_cmd_t &ctrlcmd, string &devid);

    // 处理媒体请求(实时播放、下载、对讲、回放)
    int HandleMediaRequest(sdp_description_t &req_sdp, const sip_event_sptr &e);

private:
    // 注册到上级平台
    int register_to_platform();

    // 心跳处理线程
    void heartbeat_proc_to_platform(void *param);

    // 发送保活消息
    int keepalive_to_platform();

private:
    bool m_bRegok;           // 注册成功标志
    local_sip_t m_localsipcfg;    // 本地SIP配置
    remote_sip_t m_remotesipcfg;  // 远端SIP配置

    CMsgSender m_msgsender;       // 消息发送器
    Infra::CThread m_heartbeat;   // 心跳线程
};

}
}
