#pragma once

#include <memory>
using namespace std;

namespace Zilu {
namespace Protocol {

class CMediaTranspond
{
public:
    CMediaTranspond();
    ~CMediaTranspond();

};
typedef shared_ptr<CMediaTranspond> transpond_ptr;

}
}
