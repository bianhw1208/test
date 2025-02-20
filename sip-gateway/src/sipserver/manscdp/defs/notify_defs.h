#pragma once

#include "common_defs.h"

namespace Gateway {
namespace SIP {

// 保活消息对话结构
struct manscdp_keepalive_dialogs_t
{
    // 保活请求结构
    struct request_t : public common_item_t {
        struct info {
            string      devid;      // 设备ID
        };
    public:
        request_t() {
            cmdtype = "Keepalive";  // 设置命令类型为保活
        }
        result_type_e       result;         // 处理结果
        list<info>          error_devlist;  // 错误设备列表
    };

    // 保活响应结构
    struct response_t : public common_answer_t {
        response_t() {
            cmdtype = "Keepalive";  // 设置命令类型为保活
        }
    };

public:
    request_t       request;    // 请求消息
    response_t      response;   // 响应消息
};

// 报警通知对话结构
struct manscdp_alarm_notify_dialogs_t
{
    // 报警通知请求结构
    struct request_t : public common_item_t {
        struct info {
            struct param {
                uint32_t        eventtype;  // 事件类型
            };
        public:
            uint32_t        alarmtype;      // 报警类型
            param           alarmtype_param; // 报警类型参数
        };
    public:
        request_t() {
            cmdtype = "Alarm";  // 设置命令类型为报警
        }
        string              alarm_priority;    // 报警级别
        string              alarm_method;      // 报警方式
        string              alarmtime;         // 报警时间
        string              alarm_description; // 报警描述
        double              longitude;         // 经度
        double              latitude;          // 纬度
        list<info>          info_list;         // 报警信息列表
    };

    // 报警通知响应结构
    struct response_t : public common_answer_t {
        response_t() {
            cmdtype = "Alarm";  // 设置命令类型为报警
        }
    };

public:
    request_t           request;   // 请求消息
    response_t          response;  // 响应消息
};

// 媒体状态通知对话结构
struct manscdp_media_status_dialogs_t
{
    // 媒体状态通知请求结构
    struct request_t : public common_item_t {
    public:
        request_t() {
            cmdtype = "MediaStatus";  // 设置命令类型为媒体状态
        }
        string      notifytype;       // 通知类型
    };

    // 媒体状态通知响应结构
    struct response_t : public common_answer_t {
    public:
        response_t() {
            cmdtype = "MediaStatus";  // 设置命令类型为媒体状态
        }
    };

public:
    request_t       request;    // 请求消息
    response_t      response;   // 响应消息
};

// 语音广播通知对话结构
struct manscdp_broadcast_dialogs_t
{
    // 语音广播通知请求结构
    struct request_t : public common_item_t {
    public:
        request_t() {
            cmdtype = "Broadcast";    // 设置命令类型为广播
        }
        string          sourceid;     // 源设备ID
        string          targetid;     // 目标设备ID
    };

    // 语音广播通知响应结构
    struct response_t : public common_answer_t {
    public:
        response_t() {
            cmdtype = "Broadcast";    // 设置命令类型为广播
        }
    };

public:
    request_t       request;    // 请求消息
    response_t      response;   // 响应消息
};

}
}