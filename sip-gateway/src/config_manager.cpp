#include "config_manager.h"

namespace Gateway {
CConfigManager *CConfigManager::instance()
{
    static CConfigManager _ins;
    return &_ins;
}

bool CConfigManager::Load(const std::string &path)
{
    using namespace ltmit;
    CIniParser parser;

    auto get_data_with_default = [](CIniParser& parser, std::string const& section, std::string const& key, std::string& default_val) {
        iniValue_t temp_val;
        temp_val.val = default_val;
        if (parser.get_data(section, key, temp_val) == 0) {
            default_val = temp_val.val;
        }
        };
    std::string errmsg;
    int rt = parser.parseFile(path.c_str());
    if (rt) {
        return ERROR_CFG_PARSE_FILE_FAILED;
    }

    try {
        get_data_with_default(parser, "local_sip", "svr_id", m_localcfg.svr_id);
        get_data_with_default(parser, "local_sip", "svr_domain", m_localcfg.svr_domain);
        get_data_with_default(parser, "local_sip", "svr_ip", m_localcfg.svr_ip);
        m_localcfg.svr_port = parser.cxx_getint("local_sip", "svr_port");

        get_data_with_default(parser, "remote_sip", "svr_id", m_remotecfg.svr_id);
        get_data_with_default(parser, "remote_sip", "svr_domain", m_remotecfg.svr_domain);
        get_data_with_default(parser, "remote_sip", "svr_ip", m_remotecfg.svr_ip);
        get_data_with_default(parser, "remote_sip", "passwd", m_remotecfg.passwd);
        m_remotecfg.svr_port = parser.cxx_getint("remote_sip", "svr_port");

        //log 

        m_portpool.start = parser.cxx_getint("media_port_pool", "start");
        m_portpool.end = parser.cxx_getint("media_port_pool", "end");
    }
    catch (CIniParser::sec_not_found& e) {
        format_string(errmsg, "INI sec not found! sec=%s, key=%s", e.sec.data(), e.key.data());
        return ERROR_CFG_SEC_NOT_FOUND;
    }
    catch (CIniParser::valtype_mismatch& e) {
        format_string(errmsg, "INI valtype convert fail! cannot convert [%s].%s to %s",
            e.sec.data(), e.key.data(), e.failed_type.data());
        return ERROR_CFG_VALUE_TYPE;
    }
    catch (...) {
        format_string(errmsg, "other exception!");
        return ERROR_CFG_OTHER_UNKNOWN;
    }
    return true;
}

bool CConfigManager::GetLocalSip(local_sip_t &localcfg)
{
    localcfg = m_localcfg;
    return true;
}

bool CConfigManager::GetRemoteSip(remote_sip_t &remotecfg)
{
    remotecfg = m_remotecfg;
    return true;
}

bool CConfigManager::GetLogConfig(log_config_t &logcfg)
{
    logcfg = m_logcfg;
    return true;
}

bool CConfigManager::GetMediaPortPool(media_port_pool_t &portpool)
{
    portpool = m_portpool;
    return true;
}

}
