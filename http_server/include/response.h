#pragma once

#include "http_buf.h"
#include "status_code.h"
#include "version.h"
#include <map>
#include <string>
#include <ios>

namespace http_server {

class response : private std::basic_ios<uint8_t> {
private:
    int _fd;
    http_server::http_buf _http_buf;

    http_server::version_t _version;
    http_server::status_code_t _status_code;
    std::map<std::string, std::string> _header_parameters;
    std::basic_string<uint8_t> _data;

    void crlf();
public:
    response(int fd);
    void set_cache(const http_server::response::char_type* s, size_t n);
    http_server::version_t& version();
    http_server::status_code_t& status_code();
    std::map<std::string, std::string>& header_parameters();
    void write(const http_server::response::char_type* s, std::streamsize n);
    void write_metadata();
    void flush();
};

}
