#pragma once

#include <string>
#include "types.h"
using namespace std;

namespace Zilu {
namespace Protocol {
namespace GB28181 {

// 通用消息项结构 - 所有消息的基本字段
struct common_item_t
{
    string      cmdtype;    // 命令类型
    string      sn;         // 序列号
    string      devid;      // 设备ID
};

// 通用应答消息格式
// 用于Broadcast、DeviceConfig、Catalog(目录收到应答)、Alarm、DeviceControl
struct common_answer_t : public common_item_t
{
    result_type_e   result;  // 处理结果
};

// 设备基本参数配置结构
struct manscdp_basicparam_cfg_t
{
    uint32_t    expire;              // 注册过期时间
    uint32_t    heartbeat_interval;  // 心跳间隔
    uint32_t    heartbeat_cnt;       // 心跳超时次数
    string      devname;             // 设备名称
};

}
}
}
