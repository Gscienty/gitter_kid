#include "response.h"

http_server::response::response(int fd)
    : _fd(fd)
    , _http_buf(fd, http_server::out_buf)
    , _version(http_server::version_1_1)
    , _status_code(http_server::status_code_ok) {
    
    this->rdbuf(&this->_http_buf);
}

std::map<std::string, std::string>& http_server::response::header_parameters() {
    return this->_header_parameters;
}

http_server::status_code_t& http_server::response::status_code() {
    return this->_status_code;
}

http_server::version_t& http_server::response::version() {
    return this->_version;
}

void http_server::response::set_cache(const http_server::response::char_type* s, size_t n) {
    this->rdbuf()->pubsetbuf(const_cast<http_server::response::char_type *>(s), n);
}

void http_server::response::crlf() {
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>("\r\n"), 2);
}

void http_server::response::write_metadata() {
    // write http version
    auto& v = http_server::versions.at(this->_version);
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(v.data()), v.size());
    this->rdbuf()->sputc(' ');
    // write http code
    auto code = std::to_string(this->_status_code);
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(code.data()), code.size());
    this->rdbuf()->sputc(' ');
    // write description
    auto desc = http_server::status_codes.at(this->_status_code);
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(desc.data()), desc.size());
    this->crlf();

    // response head parameters;
    for (auto& param : this->_header_parameters) {
        this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(param.first.data()), param.first.size());
        this->rdbuf()->sputc(':');
        this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(param.second.data()), param.second.size());
        this->crlf();
    }

    this->crlf();
}

void http_server::response::write(const http_server::response::char_type* s, std::streamsize n) {
    this->_header_parameters["Content-Length"] = std::to_string(n);
    this->write_metadata();

    this->rdbuf()->sputn(s, n);
}

void http_server::response::flush() {
    this->rdbuf()->pubsync();
}
