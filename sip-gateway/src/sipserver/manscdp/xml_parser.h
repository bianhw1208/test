#pragma once

#include <tinyxml2/tinyxml2.h>
#include "defs/defs.h"

using namespace std;

namespace Gateway {
namespace SIP {

// XML解析器类 - 用于解析MANSCDP XML消息
class CXmlParser
{
public:
    CXmlParser() = default;
    ~CXmlParser() = default;

    // 加载并解析XML数据
    // @param p_data: XML数据
    // @param p_size: 数据长度
    // @param doc: 输出的XML文档对象
    int Load(const char* p_data, int p_size, tinyxml_doc_t &doc);

    // 解析XML消息头部
    // @param doc: XML文档对象
    // @param bodyheader: 输出的消息头部结构
    int ParseXmlHeader(tinyxml_doc_t& doc, manscdp_msgbody_header_t& bodyheader);

    // 解析字符串类型的XML元素
    // @param ele: XML元素
    // @param name: 元素名称
    // @param textstr: 输出的字符串值
    // @param brequired: 是否必需
    // @param bto_utf8: 是否转换为UTF8编码
    int ParseEleStr(tinyxml_ele_t *ele, const char* name, string& textstr, bool brequired, bool bto_utf8= false);

    // 解析整数类型的XML元素
    // @param ele: XML元素
    // @param name: 元素名称
    // @param value: 输出的整数值
    // @param brequired: 是否必需
    int ParseEleInt(tinyxml_ele_t *ele, const char* name, int& value, bool brequired);

private:
    // 解析命令类别
    int parse_cmdcategory(tinyxml_ele_t *ele, manscdp_cmd_category_e& category, string& name);

    // 解析命令类型
    int parse_cmdtype(tinyxml_ele_t *ele, manscdp_msgbody_header_t &bh, string& textstr);

    // 解析浮点数类型的XML元素
    int parse_ele_double(tinyxml_ele_t *ele, const char* name, double& value, bool brequired);

    // 检查元素是否存在
    bool is_member(tinyxml_ele_t *ele, const char *name);
};

}
}
