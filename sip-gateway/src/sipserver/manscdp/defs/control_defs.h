#pragma once

#include "common_defs.h"

namespace Gateway {
namespace SIP {

struct manscdp_device_config_dialogs_t
{
    struct request_t : public common_item_t {
    public:
        request_t() {
            cmdtype = "DeviceConfig";
        }
        manscdp_basicparam_cfg_t        basicparam;
        ///TODO add SVACEncodeConfig so and on.
    };
    struct response_t : public common_answer_t {
    public:
        response_t() {
            cmdtype = "DeviceConfig";
        }
    };

public:
    request_t           request;
    response_t          response;
};

}
}
