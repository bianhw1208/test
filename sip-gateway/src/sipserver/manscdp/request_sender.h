#pragma once

#include <string>
#include <memory>
#include <atomic>
#include "defs/defs.h"
#include "xml_assembly.h"
#include "request_manager/request_manager.h"
#include "osipparser2/osip_message.h"
#include "config_manager.h"
#include "sipserver/request_manager/base_request.h"
using namespace std;

namespace Gateway {
namespace SIP {

// 消息发送器类 - 负责发送各类SIP消息
class CMsgSender
{
public:
    CMsgSender();
    ~CMsgSender();

    // 注册到上级平台
    int RegisterOnline(remote_sip_t &remote);

    // 发送保活消息
    int Keepalive(remote_sip_t &remote, local_sip_t &local, bool bwait, CBaseRequestSPtr &request);

    // 初始化发送器
    int Init();

    // 发送设备状态查询请求
    // @param devid: 设备ID
    // @return: 请求ID,用于跟踪响应
    int SendDeviceStatusQuery(const std::string &devid);

    // 发送设备目录查询请求
    int SendCatalogQuery(const std::string &devid);

    // 发送设备信息查询请求
    int SendDeviceInfoQuery(const std::string &devid);

    // 发送录像文件查询请求
    // @param devid: 设备ID
    // @param starttime: 开始时间
    // @param endtime: 结束时间
    int SendRecordInfoQuery(const std::string &devid, const std::string &starttime,
                          const std::string &endtime);

    // 发送报警查询请求
    int SendAlarmQuery(const std::string &devid);

    // 发送配置下载请求
    int SendConfigDownload(const std::string &devid);

    // 发送预置位查询请求
    int SendPresetQuery(const std::string &devid);

    // 发送设备控制命令
    int SendDeviceControl(const std::string &devid, manscdp_devicecontrol_subcmd_e subcmd,
                         manscdp_switch_status_e onoff);

    // 发送PTZ控制命令
    int SendPTZCmd(const std::string &devid, const std::string &ptzcmd);

private:
    // 生成消息序列号
    string generate_sn();

    // 发送消息
    int send_message(remote_sip_t &remote, const char *body, CBaseRequestSPtr &request);

    // 从请求中获取请求ID
    const char * get_reqid_from_request(osip_message_t* msg);

private:
    atomic<uint64_t > m_sn;  // 消息序列号计数器
    CXmlAssembly m_xmlassembly;     // XML组装器
    CRequestManager m_reqmanager;    // 请求管理器
};

}
}