#pragma once

#include "version.h"
#include "request_method.h"
#include "http_buf.h"
#include <string>
#include <map>
#include <ios>

namespace http_server {

class request : private std::basic_ios<uint8_t> {
private:
    int _fd;
    http_server::http_buf _http_buf;

    http_server::version_t _version;
    http_server::request_method_t _method;
    std::string _uri;
    std::map<std::string, std::string> _header_parameters;
public:
    request(int fd);
    void set_cache(const http_server::request::char_type* s, size_t n);
    http_server::version_t& version();
    http_server::request_method_t& method();
    std::string& uri();
    std::map<std::string, std::string>& header_parameters();
    void read(http_server::request::char_type* s, std::streamsize n);
    void getline(std::string& line);
    void get_meta();
};

}
