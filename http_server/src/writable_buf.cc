#include "writable_buf.h"
#include <unistd.h>

http_server::writable_buf::writable_buf(int fd)
    : _fd(fd)
    , _page_size(0)
    , _activity_page(nullptr) {}

int http_server::writable_buf::sync() {
    if (this->pptr() == this->pbase()) {
        return 0;
    }

    ::write(this->_fd, this->_activity_page, this->pptr() - this->pbase());

    this->setp(this->_activity_page, this->_activity_page + this->_page_size);

    return 0;
}

http_server::writable_buf* http_server::writable_buf::setbuf(char_type* s, std::streamsize n) {
    this->_activity_page = s;
    this->_page_size = n;
    this->setp(s, s + n);
    return this;
}

http_server::writable_buf::int_type http_server::writable_buf::overflow(int_type c) {
    this->sync();

    this->_activity_page[0] = uint8_t(c);
    this->pbump(1);

    return c;
}

std::streamsize http_server::writable_buf::xsputn(const http_server::writable_buf::char_type* s, std::streamsize n) {
    // declare remain puts bytes count
    size_t remain_puts_bytes = n;
    http_server::writable_buf::char_type* s_pos = const_cast<http_server::writable_buf::char_type *>(s);

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
