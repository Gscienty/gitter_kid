#pragma once

#include "http_buf.h"
#include <ios>

namespace http_server {

class basic_omsg: public std::basic_ios<uint8_t> {
private:
    int _fd;
    http_buf _buf;
    bool _writed_metadata;

    uint8_t* _buffer;
    size_t _size;

protected:
    void write_crlf();
    virtual void write_metadata() = 0;
public:
    basic_omsg(int fd);
    bool writed_metadata() const;
    void flush();
    void write(const char_type* s, std::streamsize n);
};

}
