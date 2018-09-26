#pragma once

#include <streambuf>
#include <cstdint>
#include <queue>
#include <utility>
#include <functional>

namespace http_server {

class http_buf : public std::basic_streambuf<uint8_t> {
private:
    int _fd;
    size_t _page_size;
    uint8_t *_activity_page;
protected:
    virtual http_buf* setbuf(char_type* s, std::streamsize n) override;
    virtual int sync() override;
    virtual int_type overflow(int_type c) override;
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n) override;
public:
    http_buf(int fd);
};

}
