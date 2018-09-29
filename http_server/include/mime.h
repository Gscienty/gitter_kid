#pragma once

#include <map>
#include <string>

namespace http_server {

const std::map<std::string, std::string> mimes = {
    { "html", "text/html" }
};

}
