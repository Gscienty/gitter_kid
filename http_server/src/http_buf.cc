#include "http_buf.h"
#include <sys/mman.h>
#include <iostream>
#include <algorithm>
#include <sys/socket.h>
#include <unistd.h>

http_server::http_buf::http_buf(int fd, http_server::http_buf_direction_t direction)
    : _fd(fd)
    , _direction(direction)
    , _page_size(0)
    , _activity_page(nullptr) {}

http_server::http_buf* http_server::http_buf::setbuf(http_server::http_buf::char_type* s, std::streamsize n) {
    this->_activity_page = s;
    this->_page_size = n;

    switch (this->_direction) {
    case http_server::out_buf:
        this->setp(s, s + n);
        break;

    case http_server::in_buf:
        this->setg(s, s, s);
        break;
    }
    return this;
}

int http_server::http_buf::__out_sync() {
    if (this->pptr() == this->pbase()) {
        return 0;
    }

    ::write(this->_fd, this->_activity_page, this->pptr() - this->pbase());

    this->setp(this->_activity_page, this->_activity_page + this->_page_size);

    return 0;
}

int http_server::http_buf::__in_sync() {
    size_t remain_unread_size = this->egptr() - this->gptr();
    if (remain_unread_size != 0) {
        std::copy(this->gptr(), this->egptr(), this->_activity_page);
    }

    size_t newly_bytes_size = ::read(this->_fd,
                                     this->_activity_page + remain_unread_size,
                                     this->_page_size - remain_unread_size);

    this->setg(this->_activity_page,
               this->_activity_page,
               this->_activity_page + remain_unread_size + newly_bytes_size);

    return 0;
}

int http_server::http_buf::sync() {

    switch (this->_direction) {
    case http_server::out_buf:
        return this->__out_sync();
    case http_server::in_buf:
        return this->__in_sync();
    default:
        return -1;
    }

}

http_server::http_buf::int_type
http_server::http_buf::
overflow(http_server::http_buf::int_type c) {
    this->sync();

    this->_activity_page[0] = uint8_t(c);
    this->pbump(1);

    return c;
}

std::streamsize
http_server::http_buf::
xsputn(const http_server::http_buf::char_type* s, std::streamsize n) {
    // declare remain puts bytes count
    size_t remain_puts_bytes = n;
    http_server::http_buf::char_type* s_pos = const_cast<http_server::http_buf::char_type *>(s);

    if (this->pptr() == nullptr) {
        this->overflow(*s_pos++);
        remain_puts_bytes--;
    }

    while (remain_puts_bytes != 0) {
        // get remain _activity_page remain bytes count
        size_t buffer_remain_bytes = this->epptr() - this->pptr();

        if (buffer_remain_bytes <= 0) {
            //overflow
            this->overflow(*s_pos);
            s_pos++;
            remain_puts_bytes--;
            buffer_remain_bytes = this->epptr() - this->pptr();
        }

        // calculate will write bytes count
        size_t write_count = std::min(buffer_remain_bytes, remain_puts_bytes);

        std::copy(s_pos, s_pos + write_count, this->pptr());

        s_pos += write_count;
        remain_puts_bytes -= write_count;
        this->pbump(write_count);
    }
    return n - remain_puts_bytes;
}

http_server::http_buf::int_type http_server::http_buf::underflow() {
    this->sync();

    if (this->gptr() == this->egptr()) {
        return traits_type::eof();
    }

    return this->_activity_page[0];
}

std::streamsize 
http_server::http_buf::
xsgetn(http_server::http_buf::char_type* s, std::streamsize n) {
    size_t remain_gets_bytes = n;
    http_server::http_buf::char_type* s_pos = s; 
    
    if (this->gptr() == this->egptr()) {
        this->underflow();
    }

    while (remain_gets_bytes != 0) {
        size_t buffer_remain_bytes = this->egptr() - this->gptr() ;
        if (buffer_remain_bytes <= 0) {
            this->underflow();
        }

        size_t read_count = std::min(buffer_remain_bytes, remain_gets_bytes);

        std::copy(this->gptr(), this->gptr() + read_count, s_pos);
        s_pos += read_count;
        remain_gets_bytes -= read_count;
        this->gbump(read_count);
    }

    return n - remain_gets_bytes;
}
