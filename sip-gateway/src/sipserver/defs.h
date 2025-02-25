#pragma once

#include <memory>
#include <functional>
#include "eXosip2/eXosip.h"
using namespace std;

namespace Gateway {
namespace SIP {

struct sip_event_t;
typedef shared_ptr <sip_event_t> sip_event_sptr;
typedef function<int(const sip_event_sptr &)> event_proc;

struct sip_event_t
{
    int                 value;
    const char*         name;
    event_proc          proc;
    struct eXosip_t*    excontext;
    eXosip_event_t*     exevent;
    uint64_t            eventid;

    sip_event_t() = default;
};

}
} // namespace Gateway::SIP
