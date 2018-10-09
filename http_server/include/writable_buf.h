#pragma once

#include <streambuf>
#include <cstdint>

namespace http_server {

class writable_buf : public std::basic_streambuf<uint8_t> {
private:
    int _fd;
    size_t _page_size;
    uint8_t* _activity_page;

protected:
    virtual int sync() override;
    virtual writable_buf* setbuf(char_type* s, std::streamsize n) override;
    virtual http_server::writable_buf::int_type overflow(int_type c) override;
    virtual std::streamsize xsputn(const http_server::writable_buf::char_type* s,
                                   std::streamsize n) override;
public:
    writable_buf(int fd);
};

}
