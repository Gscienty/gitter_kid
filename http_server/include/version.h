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

const std::map<std::string, http_server::version_t> rversions = {
    { "HTTP/1.1", version_1_1 }
};

}
