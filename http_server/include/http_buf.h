#pragma once

#include <streambuf>
#include <cstdint>
#include <queue>
#include <utility>
#include <functional>

namespace http_server {

typedef uint8_t http_buf_direction_t;

const http_buf_direction_t in_buf = 0;
const http_buf_direction_t out_buf = 1;

class http_buf : public std::basic_streambuf<uint8_t> {
private:
    int _fd;
    http_server::http_buf_direction_t _direction;
    size_t _page_size;
    uint8_t *_activity_page;

    int __out_sync();
    int __in_sync();

protected:
    virtual int sync() override;
    virtual http_buf* setbuf(char_type* s, std::streamsize n) override;

    // put
    virtual http_server::http_buf::int_type overflow(int_type c) override;
    virtual std::streamsize xsputn(const http_server::http_buf::char_type* s,
                                   std::streamsize n) override;


    // get
    virtual http_server::http_buf::int_type underflow() override;
    virtual std::streamsize xsgetn(http_server::http_buf::char_type* s,
                                   std::streamsize n) override;
public:
    http_buf(int fd, http_buf_direction_t direction);
};

}
