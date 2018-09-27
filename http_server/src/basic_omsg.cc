#include "basic_omsg.h"
#include <malloc.h>

static std::string __crlf = "\r\n";

http_server::basic_omsg::basic_omsg(int fd)
    : _fd(fd)
    , _buf(fd)
    , _writed_metadata(false) {
    this->rdbuf(&this->_buf);
}

void http_server::basic_omsg::write_crlf() {
    this->rdbuf()->sputn(reinterpret_cast<const uint8_t *>("\r\n"), 2);
}

bool http_server::basic_omsg::writed_metadata() const {
    return this->_writed_metadata;
}

void http_server::basic_omsg::flush() {
    if (this->_writed_metadata == false) {
        this->_writed_metadata = true;
        this->write_metadata();
    }
    this->rdbuf()->pubsync();
}

void http_server::basic_omsg::write(const char_type* s, std::streamsize n) {
    if (this->_writed_metadata == false) {
        this->_writed_metadata = true;
        this->write_metadata();
    }
    this->rdbuf()->sputn(s, n);
}
