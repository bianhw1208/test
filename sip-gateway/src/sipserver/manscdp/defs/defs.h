#pragma once

#include <string>
#include <vector>
#include "control_defs.h"
#include "query_defs.h"
#include "notify_defs.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

namespace Gateway {
namespace SIP {

// MANSCDP命令类别枚举
enum manscdp_cmd_category_e
{
    MANSCDP_CMD_CATEGORY_CONTROL,   // 控制类命令
    MANSCDP_CMD_CATEGORY_QUERY,     // 查询类命令
    MANSCDP_CMD_CATEGORY_NOTIFY,    // 通知类命令
    MANSCDP_CMD_CATEGORY_RESPONSE,  // 响应类命令

    MANSCDP_CMD_CATEGORY_MAX,
    MANSCDP_CMD_CATEGORY_UNKNOWN = MANSCDP_CMD_CATEGORY_MAX
};

// MANSCDP命令类型枚举
enum manscdp_cmdtype_e
{
    // 控制类命令
    MANSCDP_CONTROL_CMD_DEVICE_CONTROL,  // 设备控制
    MANSCDP_CONTROL_CMD_DEVICE_CONFIG,   // 设备配置

    // 查询类命令
    MANSCDP_QUERY_CMD_DEVICE_STATUS,     // 设备状态查询
    MANSCDP_QUERY_CMD_CATALOG,           // 设备目录查询
    MANSCDP_QUERY_CMD_DEVICE_INFO,       // 设备信息查询
    MANSCDP_QUERY_CMD_RECORD_INFO,       // 录像文件查询
    MANSCDP_QUERY_CMD_ALARM,             // 报警查询
    MANSCDP_QUERY_CMD_CONFIG_DOWNLOAD,   // 设备配置查询
    MANSCDP_QUERY_CMD_PRESET_QUERY,      // 预置位查询
    MANSCDP_QUERY_CMD_MOBILE_POSITION,   // 移动设备位置查询

    // 通知类命令
    MANSCDP_NOTIFY_CMD_KEEPALIVE,        // 设备状态信息(心跳)
    MANSCDP_NOTIFY_CMD_ALARM,            // 报警通知
    MANSCDP_NOTIFY_CMD_MEDIA_STATUS,     // 媒体状态通知
    MANSCDP_NOTIFY_CMD_BROADCASE,        // 语音广播通知
    MANSCDP_NOTIFY_CMD_MOBILE_POSITION,  // 移动设备位置通知

    MANSCDP_CMD_TYPE_MAX,
    MANSCDP_CMD_TYPE_UNKNOWN = MANSCDP_CMD_TYPE_MAX
};

// 设备控制子命令枚举
enum manscdp_devicecontrol_subcmd_e
{
     PTZCmd = 1,         // 云台控制
     TeleBoot,           // 远程重启
     RecordCmd,          // 录像控制
     GuardCmd,           // 布防/撤防
     AlarmCmd = 5,       // 报警控制
     IFrameCmd,          // 强制I帧
     DragZoomIn,         // 拉框放大
     DragZoomOut,        // 拉框缩小
     HomePosition        // 看守位
};

// 设备开关状态枚举
enum manscdp_switch_status_e
{
    SWITCH_ON,           // 开启状态
    SWITCH_OFF          // 关闭状态
};

// 设备配置子命令枚举
enum manscdp_deviceconfig_subcmd_e
{
    BasicParam = 1,      // 基本参数配置
    SVACEncodeConfig,    // SVAC编码配置
    SVACDecodeConfig     // SVAC解码配置
};

// 设备控制子命令列表类型
typedef vector<manscdp_devicecontrol_subcmd_e> manscdp_devicecontrol_subcmd_t;
// 设备配置子命令列表类型
typedef vector<manscdp_deviceconfig_subcmd_e> manscdp_deviceconfig_subcmd_t;
// 配置下载子命令列表类型
typedef vector<string> manscdp_configdownload_subcmd_t;

// XML文档类型别名
typedef tinyxml2::XMLDocument   tinyxml_doc_t;
// XML元素类型别名
typedef tinyxml2::XMLElement    tinyxml_ele_t;

///< MANSCDP xml��Ϣ ��ѡ������
/*
 * <?xml version="1.0"?>
 * <Query>
 * <CmdType>Catalog</CmdType>
 * <SN>27</SN>
 * <DeviceID>1234</DeviceID>
 * </Query>
 */
struct manscdp_msgbody_header_t
{
    manscdp_cmd_category_e              cmd_category;    // 命令类别
    manscdp_cmdtype_e                   cmd_type;        // 命令类型
    string                              sn;              // 序列号
    string                              devid;           // 设备ID
    manscdp_deviceconfig_subcmd_t       devcfg_subcmd;   // 设备配置子命令列表
    manscdp_devicecontrol_subcmd_t      devctl_subcmd;   // 设备控制子命令列表
    manscdp_configdownload_subcmd_t     cfgdownload_subcmd; // 配置下载子命令列表
};

}
}
