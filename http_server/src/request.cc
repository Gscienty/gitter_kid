#include "request.h"
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <algorithm>

http_server::request::request(int fd)
    : _fd(fd)
    , _http_buf(fd)
    , _flushed_metadata(false)
    , _version(http_server::version_1_1)
    , _method(http_server::request_method_get)
    , _content_length(0) {
    
    this->rdbuf(&this->_http_buf);
}

void http_server::request::set_cache(const http_server::request::char_type* s, size_t n) {
    this->rdbuf()->pubsetbuf(const_cast<http_server::request::char_type *>(s), n) ;
}

http_server::version_t& http_server::request::version() {
    return this->_version;
}

http_server::request_method_t &http_server::request::method() {
    return this->_method;
}

std::string& http_server::request::uri() {
    return this->_uri;
}

std::map<std::string, std::string>& http_server::request::header_parameters() {
    return this->_header_parameters;
}

void http_server::request::flush_metadata() {
    if (this->_flushed_metadata == false) {
        this->_flushed_metadata = true;
        this->__flush_metadata_implement();

        auto content_length_findret = this->_header_parameters.find("Content-Length");
        if (content_length_findret != this->_header_parameters.end()) {
            this->_content_length = std::stol(content_length_findret->second);
        }
    }
}

size_t http_server::request::read(http_server::request::char_type* s, std::streamsize n) {
    this->flush_metadata();

    size_t realable_readsize = std::min(static_cast<size_t>(n), this->_content_length);
    this->rdbuf()->sgetn(s, realable_readsize);

    return realable_readsize;
}

static const char __crlf_status[] = { '\r', '\n', '\r', '\n' };

bool http_server::request::__flush_metadata_implement() {
    std::unique_lock<std::mutex> locker(this->_http_buf.use_page_mutex());
    int alpha_status = 0;
    std::stringstream metadata_stream;

    bool first_line = true;
    while (alpha_status != 4) {
        this->wait_readable(locker);
        uint8_t c = this->_http_buf.sbumpc();
        if (c == __crlf_status[alpha_status]) {
            alpha_status++;
            if (alpha_status == 1) {
                std::string line = metadata_stream.str();
                metadata_stream.str("");

                if (first_line) {
                    first_line = false;

                    size_t item_begin = 0;
                    size_t item_end = line.find(" ", item_begin);
                    std::string method = line.substr(item_begin, item_end - item_begin);
                    auto method_findret = http_server::request_methods.find(method);
                    if (method_findret == http_server::request_methods.end()) {
                        this->_method = http_server::request_method_get;
                    }
                    else {
                        this->_method = method_findret->second;
                    }

                    item_begin = item_end + 1;
                    item_end = line.find(" ", item_begin);
                    this->_uri = line.substr(item_begin, item_end - item_begin);

                    item_begin = item_end + 1;
                    std::string version = line.substr(item_begin);
                    auto version_findret = http_server::rversions.find(version);
                    if (version_findret == http_server::rversions.end()) {
                        this->_version = http_server::version_1_1;
                    }
                    else {
                        this->_version = version_findret->second;
                    }
                }
                else {
                    size_t delimiter_pos = line.find(":");
                    std::string key = line.substr(0, delimiter_pos);
                    std::string value = line.substr(delimiter_pos + 1);

                    this->_header_parameters.insert(std::make_pair(key, value));
                }
            }
        }
        else {
            alpha_status = 0;
            metadata_stream << c;
        }

    }

    return true;
}

void http_server::request::sync() {
    this->_http_buf.pubsync();
}

std::mutex& http_server::request::read_mutex() {
    return this->_http_buf.use_page_mutex();
}

void http_server::request::wait_readable(std::unique_lock<std::mutex>& locker) {
    this->_http_buf.wait_readable(locker);
}

size_t http_server::request::content_length() const {
    return this->_content_length;
}
