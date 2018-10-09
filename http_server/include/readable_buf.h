#pragma once

#include <streambuf>
#include <cstdint>

#include <mutex>
#include <condition_variable>

namespace http_server {

class readable_buf : public std::basic_streambuf<uint8_t> {
private:
    int _fd;
    size_t _page_size;
    uint8_t* _activity_page;

    std::mutex _use_page_mutex;
    std::condition_variable _readable_cond;

protected:
    virtual int sync() override;
    virtual readable_buf* setbuf(char_type* s, std::streamsize n) override;
    virtual http_server::readable_buf::int_type underflow() override;
    virtual std::streamsize xsgetn(http_server::readable_buf::char_type* s,
                                   std::streamsize n) override;
public:
    readable_buf(int fd);

    std::mutex& use_page_mutex();
    void wait_readable(std::unique_lock<std::mutex> &locker);
};

}
