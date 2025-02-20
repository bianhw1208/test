#pragma once

#include <string>
#include "ini_utils.hpp"

namespace Gateway {

// 本地SIP服务器配置结构
struct local_sip_t {
    uint16_t        svr_port;       // 服务器端口
    std::string     svr_id;         // 服务器ID
    std::string     svr_ip;         // 服务器IP
    std::string     svr_domain;     // 服务器域
};

// 远端SIP服务器配置结构
struct remote_sip_t : public local_sip_t {
    std::string     passwd;         // 认证密码
};

// 媒体端口池配置结构
struct media_port_pool_t {
    uint32_t start;                 // 起始端口
    uint32_t end;                   // 结束端口
};

// 日志配置结构
struct log_config_t {
    bool        enable;             // 是否启用日志
    int         level;              // 日志级别
    std::string path;               // 日志路径
    std::string logName;            // 日志文件名
};

// 配置管理器类 - 管理所有服务配置
class CConfigManager
{
public:
    CConfigManager() = default;
    ~CConfigManager() = default;

    // 获取单例实例
    static CConfigManager* instance();

    // 加载配置文件
    bool Load(const std::string& path);

    // 获取本地SIP配置
    bool GetLocalSip(local_sip_t& localcfg);

    // 获取远端SIP配置
    bool GetRemoteSip(remote_sip_t& remotecfg);

    // 获取日志配置
    bool GetLogConfig(log_config_t& logcfg);

    // 获取媒体端口池配置
    bool GetMediaPortPool(media_port_pool_t& portpool);

private:
    local_sip_t m_localcfg;         // 本地SIP配置
    remote_sip_t m_remotecfg;       // 远端SIP配置
    log_config_t m_logcfg;          // 日志配置
    media_port_pool_t m_portpool;   // 媒体端口池配置
};

} // namespace Gateway