#include "middleware_link.h"

void http_server::middleware_link::append(std::function<void (http_server::context& context)> middleware) {
    this->_middlewares.push_back(middleware);
}

void http_server::middleware_link::append(http_server::middleware& middleware) {
    this->_middlewares.push_back(std::bind(&http_server::middleware::runner, &middleware, std::placeholders::_1));
}

void http_server::middleware_link::execute(http_server::context& context) const {
    for (auto& middleware : this->_middlewares) {
        if (context.run_next == false) {
            break;
        }
        middleware(context);
    }
}
