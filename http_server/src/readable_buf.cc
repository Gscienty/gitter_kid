#include "readable_buf.h"
#include <unistd.h>
#include <iostream>
#include <algorithm>

http_server::readable_buf::readable_buf(int fd)
    : _fd(fd)
    , _page_size(0)
    , _activity_page(nullptr) {

    this->setg(nullptr, nullptr, nullptr);
}

int http_server::readable_buf::sync() {
    std::unique_lock<std::mutex> locker(this->_use_page_mutex);

    size_t remain_unread_size = this->egptr() - this->gptr();
    if (remain_unread_size != 0) {
        std::copy(this->gptr(), this->egptr(), this->_activity_page);
    }

    size_t newly_bytes_size = ::read(this->_fd,
                                     this->_activity_page + remain_unread_size,
                                     this->_page_size - remain_unread_size);

    size_t readable_size = remain_unread_size + newly_bytes_size;

    this->setg(this->_activity_page,
               this->_activity_page,
               this->_activity_page + readable_size);

    if (readable_size == 0 && remain_unread_size == 0) {
        return -1;
    }

    this->_readable_cond.notify_one();
    return 0;
}

http_server::readable_buf* http_server::readable_buf::setbuf(char_type* s, std::streamsize n) {
    std::lock_guard<std::mutex> locker(this->_use_page_mutex);
    this->_activity_page = s;
    this->_page_size = n;
    this->setg(s, s, s);
    return this;
}

http_server::readable_buf::int_type http_server::readable_buf::underflow() {
    return traits_type::eof();
}

void http_server::readable_buf::wait_readable(std::unique_lock<std::mutex>& locker) {
    this->_readable_cond.wait(locker, [this] () -> bool { return this->gptr() != this->egptr(); });
}

std::streamsize http_server::readable_buf::xsgetn(http_server::readable_buf::char_type* s, std::streamsize n) {
    http_server::readable_buf::char_type* s_pos = s;
    size_t remain_size = n;
    if (remain_size == 0) {
        return 0;
    }

    std::unique_lock<std::mutex> locker(this->_use_page_mutex);

    
    while (remain_size != 0) {
        this->wait_readable(locker);

        size_t remain_unread_size = this->egptr() - this->gptr();
        if (remain_unread_size == 0) { break; }

        size_t readable_size = std::min(remain_size, remain_unread_size);
        
        std::copy(this->gptr(), this->gptr() + readable_size, s_pos);
        s_pos += readable_size;
        this->gbump(readable_size);

        remain_size -= readable_size;
    }

    return n - remain_size;
}

std::mutex& http_server::readable_buf::use_page_mutex() {
    return this->_use_page_mutex;
}
