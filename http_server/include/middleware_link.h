#pragma once

#include "context.h"
#include "middleware.h"
#include <list>
#include <functional>

namespace http_server {

class middleware_link {
private:
    std::list<std::function<void (http_server::context& context)>> _middlewares;
public:
    void append(std::function<void (http_server::context& context)> middleware);
    void append(http_server::middleware& middleware);

    void execute(http_server::context& context) const;
};

}
