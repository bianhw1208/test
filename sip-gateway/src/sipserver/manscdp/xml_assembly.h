#pragma once

#include <string>
#include "defs/defs.h"
using namespace std;

namespace Gateway {
namespace SIP {

// XML组装器类 - 用于组装MANSCDP XML消息
class CXmlAssembly
{
public:
    CXmlAssembly();
    ~CXmlAssembly() = default;

public:
    // 组装设备状态响应消息
    int assemble_resp_device_status(string& body, const manscdp_device_status_dialog_t& d);
    
    // 组装目录查询响应消息
    int assemble_resp_query_catalog(string& body, const manscdp_cataloginfo_dialog_t &d);
    
    // 组装设备信息响应消息
    int assemble_resp_device_info(string& body, const manscdp_devinfo_dialog_t& d);
    
    // 组装录像信息响应消息
    int assemble_resp_record_info(string& body, const manscdp_recordinfo_dialog_t& d);
    
    // 组装报警查询响应消息
    int assemble_resp_query_alarm(string& body, const manscdp_alarm_query_dialog_t& d);
    
    // 组装配置下载响应消息
    int assemble_resp_config_download(string& body, const manscdp_config_download_dialog_t& d);
    
    // 组装预置位查询响应消息
    int assemble_resp_preset_query(string& body, const manscdp_preset_query_dialog_t& d);

private:
    // 插入子元素
    tinyxml_ele_t *insert_child(tinyxml_ele_t *root, const char *key, const string &value);

    // 转换为字符串
    int tostring(string& s);

    // 组装通用项目
    template <typename T>
    int assemble_common_item(tinyxml_ele_t *root, const T& d);

private:
    tinyxml_doc_t m_tinyxml_doc;  // XML文档对象
};

}
}
