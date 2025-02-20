#pragma once

#include <memory>
using namespace std;

namespace Gateway {

class CMediaTranspond
{
public:
    CMediaTranspond();
    ~CMediaTranspond();

};
typedef shared_ptr<CMediaTranspond> transpond_ptr;

} // namespace Gateway
