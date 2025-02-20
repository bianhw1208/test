#pragma once

#include "sipserver/defs.h"
#include "base_handler.h"
#include "sipserver/manscdp/xml_parser.h"
#include "sipserver/manscdp/defs/defs.h"
#include "utils/ptz/ptz_parser.h"

namespace Gateway {
namespace SIP {

// 消息处理器类 - 处理SIP MESSAGE类型的消息
class CMessageHandler : public CBaseHandler
{
public:
    CMessageHandler();

    ~CMessageHandler() final = default;

    // 处理接收到的请求消息
    int HandleIncomingReq(const sip_event_sptr& e);

    // 消息事件处理函数类型定义
    typedef function<int(const sip_event_sptr&, tinyxml_doc_t&, manscdp_msgbody_header_t&)> msg_event_proc;

public:
    // 处理外发请求的成功响应
    int HandleResponseSuccess(const sip_event_sptr& e);
    // 处理外发请求的失败响应
    int HandleResponseFailure(const sip_event_sptr& e);

private:
    // 处理控制类消息请求
    int handle_incoming_req_control(const sip_event_sptr& e, tinyxml_doc_t& doc,
                                    manscdp_msgbody_header_t& bh);

    // 处理查询类消息请求                              
    int handle_incoming_req_query(const sip_event_sptr& e, tinyxml_doc_t& doc,
                                    manscdp_msgbody_header_t& bh);

    // 处理通知类消息请求
    int handle_incoming_req_notify(const sip_event_sptr& e, tinyxml_doc_t& doc,
                                   manscdp_msgbody_header_t& bh);

    // 处理响应类消息请求
    int handle_incoming_req_response(const sip_event_sptr& e, tinyxml_doc_t& doc,
                                    manscdp_msgbody_header_t& bh);

private:
    // PTZ控制命令处理
    int on_devctrl_ptzcmd(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 远程重启命令处理
    int on_devctrl_teleboot(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 录像控制命令处理
    int on_devctrl_recordcmd(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 布防/撤防命令处理
    int on_devctrl_guardcmd(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 报警命令处理
    int on_devctrl_alarmcmd(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 强制I帧命令处理
    int on_devctrl_iframecmd(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 拉框放大命令处理
    int on_devctrl_dragzoomin(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 拉框缩小命令处理
    int on_devctrl_dragzoomout(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    // 看守位命令处理
    int on_devctrl_homeposition(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);

private:
    int on_devcfg_basicparam(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_devcfg_svacencodeconfig(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_devcfg_svacdeconfigconfig(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);

    int devctrl_subcmd(const sip_event_sptr &e, string &devid, manscdp_devicecontrol_subcmd_e subcmd,
                       manscdp_switch_status_e onoff);

private:
    int on_query_device_status(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_query_catalog(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_query_device_info(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_query_record_info(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_query_alarm(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_query_config_download(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_query_preset_query(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);
    int on_query_mobile_position(const sip_event_sptr& e, tinyxml_doc_t& doc, manscdp_msgbody_header_t& bh);

private:
    CXmlParser                                          m_xmlparser;
    CPtzParser                                          m_ptzparser;
    ///< control type
    map<manscdp_devicecontrol_subcmd_e, msg_event_proc> m_devctlproc;
    map<manscdp_deviceconfig_subcmd_e, msg_event_proc>  m_devcfgproc;

    ///< query type
    map<manscdp_cmdtype_e, msg_event_proc>              m_queryproc;
};

}
}