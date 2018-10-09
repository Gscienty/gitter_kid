#pragma once

#include "version.h"
#include "request_method.h"
#include "http_buf.h"
#include "readable_buf.h"
#include <string>
#include <map>
#include <ios>
#include <chrono>

namespace http_server {

class request : private std::basic_ios<uint8_t> {
private:
    int _fd;
    http_server::readable_buf _http_buf;

    bool _flushed_metadata;

    http_server::version_t _version;
    http_server::request_method_t _method;
    std::string _uri;
    std::map<std::string, std::string> _header_parameters;

    size_t _content_length;

    bool __flush_metadata_implement();
public:
    request(int fd);
    void set_cache(const http_server::request::char_type* s, size_t n);
    http_server::version_t& version();
    http_server::request_method_t& method();
    std::string& uri();
    std::map<std::string, std::string>& header_parameters();
    size_t content_length() const;
    size_t read(http_server::request::char_type* s, std::streamsize n);
    void flush_metadata();
    void sync();

    std::mutex& read_mutex();
    void wait_readable(std::unique_lock<std::mutex>& locker);
};

}
