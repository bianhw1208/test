#pragma once
namespace Zilu {
namespace Protocol {
namespace GB28181 {

// 设备ID类型枚举
enum deviceid_type_e
{
    // TODO: 添加设备ID类型定义
};

// 状态类型枚举
enum status_type_e
{
    STATUS_UNKNOWN = 0,  // 未知状态
    ON,                  // 开启状态
    OFF,                // 关闭状态
};

// 结果类型枚举
enum result_type_e
{
    RESULT_UNKNOWN = 0,  // 未知结果
    OK,                  // 成功
    ERROR               // 失败
};

// PTZ控制类型枚举
enum ptz_type_e
{
    // TODO: 添加PTZ控制类型定义
};

// 录像类型枚举
enum record_type_e
{
    RECORD_UNKNOWN = 0,  // 未知类型
    RECORD,              // 开始录像
    STOP_RECORD         // 停止录像
};

// 布防/撤防类型枚举
enum guard_type_e
{
    GUARD_UNKNON =0,    // 未知类型
    SET_GUARD,          // 布防
    RESET_GUARD         // 撤防
};

// 项目类型枚举
enum item_type_e
{
    // TODO: 添加项目类型定义
};

// 文件类型枚举
enum item_file_type_e
{
    // TODO: 添加文件类型定义
};

}
}
}
