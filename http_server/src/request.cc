#include "request.h"

http_server::request::request(int fd)
    : _fd(fd)
    , _http_buf(fd, http_server::in_buf)
    , _version(http_server::version_1_1)
    , _method(http_server::request_method_get) {
    
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

void http_server::request::read(http_server::request::char_type* s, std::streamsize n) {
    this->rdbuf()->sgetn(s, n);
}

void http_server::request::getline(std::string& line) {
    http_server::request::char_type c;
    while ((c = this->rdbuf()->sbumpc()) != '\n') {
        if (this->_http_buf.readable() == false) {
            break;
        }
        line.push_back(c);
    }
}
