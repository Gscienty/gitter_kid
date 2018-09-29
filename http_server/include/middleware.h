#pragma once

#include "context.h"

namespace http_server {

class middleware {
public:
    virtual void runner(http_server::context& context) = 0;
};

}
