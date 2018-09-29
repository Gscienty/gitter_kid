#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace http_server {

typedef uint16_t status_code_t;

const status_code_t status_code_ok = 200;
const status_code_t status_code_notfound = 404;

const std::map<status_code_t, std::string> status_codes = {
    { 200, "OK" },
    { 404, "Not Found" }
};

}
