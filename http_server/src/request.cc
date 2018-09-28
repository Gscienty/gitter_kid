#include "request.h"
#include <algorithm>

http_server::request::request(int fd)
    : _fd(fd)
    , _http_buf(fd, http_server::in_buf)
    , _version(http_server::version_1_1)
    , _method(http_server::request_method_get) {
    
    this->rdbuf(&this->_http_buf);
}

void http_server::request::set_timeout(const uint64_t timeout) {
    this->_http_buf.set_read_timeout(timeout);
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

void http_server::request::read(http_server::request::char_type* s, std::streamsize n) {
    this->rdbuf()->sgetn(s, n);
}

void http_server::request::getline(std::string& line) {
    line.clear();
    if (this->_http_buf.readable() == false) {
        return;
    }

    while (this->_http_buf.readable()) {
        uint8_t c = this->_http_buf.sbumpc();
        if (c == '\r' || c == '\n') {
            break;
        }
        line.push_back(c);
    }
    
    if (this->_http_buf.readable()) {
        uint8_t c = this->_http_buf.sgetc();
        if (c == '\r' || c == '\n') {
            this->_http_buf.sbumpc();
        }
    }
}

void http_server::request::get_meta() {
    std::string line;
    bool firstline_flag = true;
    while (this->getline(line), this->_http_buf.readable() && line.empty() == false) {
        if (firstline_flag) {
            firstline_flag = false;

            std::string::iterator item_begin = line.begin();
            std::string::iterator item_end = std::find(line.begin(), line.end(), ' ');

            std::string method_str(item_begin, item_end);
            if (http_server::request_methods.find(method_str) == http_server::request_methods.end()) {
                this->_method = http_server::request_method_get;
            }
            else {
                this->_method = http_server::request_methods.at(method_str);
            }

            item_begin = item_end;
            item_begin++;
            item_end = std::find(item_begin, line.end(), ' ');
            this->_uri.assign(item_begin, item_end);

            item_begin = item_end;
            item_begin++;
            item_end = line.end();

            std::string version_str(item_begin, item_end);
            if (http_server::rversions.find(version_str) == http_server::rversions.end()) {
                this->_version = http_server::version_1_1;
            }
            else {
                this->_version = http_server::rversions.at(version_str);
            }
        }
        else {
            std::string::iterator delimiter_itr = std::find(line.begin(), line.end(), ':');
            std::string key(line.begin(), delimiter_itr);
            std::string val(delimiter_itr + 1, line.end());
            this->_header_parameters.insert(std::make_pair(key, val));
        }
    }
}
