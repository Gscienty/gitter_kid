#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace http_server {

typedef uint8_t request_method_t;

const http_server::request_method_t request_method_get       = 0;
const http_server::request_method_t request_method_post      = 1;
const http_server::request_method_t request_method_put       = 2;
const http_server::request_method_t request_method_delete    = 4;

const std::map<std::string, http_server::request_method_t> request_methods = {
    { "GET",    http_server::request_method_get     },
    { "POST",   http_server::request_method_post    },
    { "PUT",    http_server::request_method_put     },
    { "DELETE", http_server::request_method_delete  }
};

}
