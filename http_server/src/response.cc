#include "response.h"

http_server::response::response(int fd)
    : http_server::basic_omsg(fd)
    , _version(http_server::version_1_1)
    , _status_code(http_server::status_code_ok) {}

std::map<std::string, std::string>& http_server::response::header_parameters() {
    return this->_header_parameters;
}

http_server::status_code_t& http_server::response::status_code() {
    return this->_status_code;
}

http_server::version_t& http_server::response::version() {
    return this->_version;
}

void http_server::response::write_metadata() {
    // write http version
    auto& v = http_server::versions.at(this->_version);
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(v.data()), v.size());
    this->rdbuf()->sputc(' ');
    auto code = std::to_string(this->_status_code);
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(code.data()), code.size());
    this->rdbuf()->sputc(' ');
    auto desc = http_server::status_codes.at(this->_status_code);
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(desc.data()), desc.size());
    this->write_crlf();

    for (auto& param : this->_header_parameters) {
        this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(param.first.data()), param.first.size());
        this->rdbuf()->sputc(':');
        this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>(param.second.data()), param.second.size());
        this->write_crlf();
    }

    this->write_crlf();
}
