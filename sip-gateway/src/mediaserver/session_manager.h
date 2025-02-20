#pragma once

#include "defs.h"

namespace Gateway {

class CSessionManager
{
public:
    CSessionManager();
    ~CSessionManager();

    static CSessionManager* instance();

    int CreateSession(const string &sess_id, session_ptr &s);

private:
    session_map     m_sessmap;

};

} // namespace Gateway
