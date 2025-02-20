#pragma once

#include <string>
#include <map>
#include "LogWrapper.h"

using namespace std;

namespace Gateway {
namespace SIP {

// 媒体操作类型枚举
enum media_operate_type_e
{
    MOT_UNKNOWN  =0,  // 未知类型
    MOT_PLAY,        // 实时播放
    MOT_DOWNLOAD,    // 文件下载
    MOT_PLAYBACK,    // 历史回放
    MOT_TALK,        // 语音对讲
};

// SDP会话描述结构
struct sdp_description_t
{
    string v_version;     // 版本号

    // o=34020000002000000001 B8975BC37429408E83BBAB32535D07B2 0 IN IP4 172.18.3.203
    string o_username;    // 所有者/创建者
    string o_sessid;      // 会话ID
    string o_sess_version;// 会话版本
    string o_nettype;     // 网络类型
    string o_addrtype;    // 地址类型
    string o_addr;        // IP地址

    // s=Play
    string s_sess_name;   // 会话名称,对应media_operate_type_e

    // u=34020000001320000002
    string u_uri;         // URI

    // c=IN IP4 172.18.3.204
    string c_nettype;     // 连接网络类型
    string c_addrtype;    // 连接地址类型
    string c_addr;        // 连接IP地址

    // t= 0 0
    string t_startime;    // 会话开始时间
    string t_endtime;     // 会话结束时间

    // m=video 38930 RTP/AVP 96 98 97
    string m_media;       // 媒体类型
    string m_port;        // 媒体端口
    string m_tmode;       // 传输模式(TCP/RTP/AVP或RTP/AVP)
    string m_payloadtype; // 负载类型

    // a=recvonly
    // a=rtpmap:96 PS/90000
    map<string, string> a_attrimap;  // 属性字段映射

    // y=0200003000
    string y_ssrc;        // SSRC标识

    // f=v/1/5/3///a///
    string f_media_desc;  // 媒体描述

    // 打印SDP信息
    void print() {
        LOG_DEBUG("o_username: {}", o_username);
        LOG_DEBUG("o_sessid: {}", o_sessid);
        LOG_DEBUG("o_sess_version: {}", o_sess_version);
        LOG_DEBUG("o_nettype: {}", o_nettype);
        LOG_DEBUG("o_addrtype: {}", o_addrtype);
        LOG_DEBUG("o_addr: {}", o_addr);
        LOG_DEBUG("s_sess_name: {}", s_sess_name);
        LOG_DEBUG("u_uri: {}", u_uri);
        LOG_DEBUG("c_nettype: {}", c_nettype);
        LOG_DEBUG("c_addrtype: {}", c_addrtype);
        LOG_DEBUG("c_addr: {}", c_addr);
        LOG_DEBUG("t_startime: {}", t_startime);
        LOG_DEBUG("t_endtime: {}", t_endtime);
        LOG_DEBUG("m_media: {}", m_media);
        LOG_DEBUG("m_port: {}", m_port);
        LOG_DEBUG("m_tmode: {}", m_tmode);
        LOG_DEBUG("m_payloadtype: {}", m_payloadtype);
        LOG_DEBUG("y_ssrc: {}", y_ssrc);
        LOG_DEBUG("f_media_desc: {}", f_media_desc);

        for (auto a : a_attrimap) {
            LOG_DEBUG("attri key: {}, value: {}", a.first, a.second);
        }
    }
};

}
}
