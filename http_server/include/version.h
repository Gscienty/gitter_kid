#pragma once

#include <string>
#include <map>
#include <cstdint>

namespace http_server {

typedef uint8_t version_t;

const version_t version_1_1 = 11;

const std::map<http_server::version_t, std::string> versions = {
    { version_1_1, "HTTP/1.1" }
};

}
