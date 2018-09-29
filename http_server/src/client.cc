#include "client.h"
#include <unistd.h>

http_server::client::client(int fd, ::sockaddr_in addr)
    : _fd(fd)
    , _addr(addr)
    , _send_buffer(nullptr)
    , _send_buffer_size(0)
    , _response(new http_server::response(fd))
    , _recv_buffer(nullptr)
    , _recv_buffer_size(0)
    , _request(new http_server::request(fd)) {}

http_server::client::~client() {
    this->__release_send();
    this->__release_recv();
}

void http_server::client::__release_send() {
    if (this->_send_buffer != nullptr) {
        this->_release_send_func(this->_send_buffer, this->_send_buffer_size);
        this->_send_buffer = nullptr;
        this->_send_buffer_size = 0;
        this->_response->set_cache(nullptr, 0);
    }
}

void http_server::client::__release_recv() {
    if (this->_recv_buffer != nullptr) {
        this->_release_recv_func(this->_recv_buffer, this->_recv_buffer_size);
        this->_recv_buffer = nullptr;
        this->_recv_buffer_size = 0;
        this->_response->set_cache(nullptr, 0);
    }
}

void http_server::client::set_send_buffer(uint8_t* buffer,
                                          size_t size,
                                          std::function<void (uint8_t*, size_t)> release_func) {
    if (this->_send_buffer != nullptr) {
        this->__release_send();
    }

    this->_send_buffer = buffer;
    this->_send_buffer_size = size;
    this->_release_send_func = release_func;

    this->_response->set_cache(this->_send_buffer, this->_send_buffer_size);
}

void http_server::client::set_recv_buffer(uint8_t* buffer,
                                          size_t size,
                                          std::function<void (uint8_t*, size_t)> release_func) {
    if (this->_recv_buffer != nullptr) {
        this->__release_recv();
    }

    this->_recv_buffer = buffer;
    this->_recv_buffer_size = size;
    this->_release_recv_func = release_func;

    this->_request->set_cache(this->_recv_buffer, this->_recv_buffer_size);
}

http_server::request& http_server::client::get_request() {
    return *this->_request;
}

http_server::response& http_server::client::get_response() {
    return *this->_response;
}

void http_server::client::close() {
    ::close(this->_fd);
    this->__release_recv();
    this->__release_send();

    this->_request.release();
    this->_response.release();
}
