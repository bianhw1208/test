#pragma once

#include <string>
#include "defs.h"
using namespace std;

namespace Gateway {

class CPtzParser
{
public:
    CPtzParser() = default;
    ~CPtzParser() = default;

    int ParseControlCmd(control_cmd_t &ctrlcmd, const string &cmdstr);

private:
    void parse_ptz(const char *b, control_cmd_t &ctrlcmd);
    void parse_fi(const char *b, control_cmd_t &ctrlcmd);
    void parse_preset(const char *b, control_cmd_t &ctrlcmd);
    void parse_patrol(const char *b, control_cmd_t &ctrlcmd);
    void parse_scan(const char *b, control_cmd_t &ctrlcmd);

private:
    uint8_t m_b4;
    uint8_t m_b5;
    uint8_t m_b6;
    uint8_t m_b7;
};

}
