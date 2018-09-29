#pragma once

#include "request.h"
#include "response.h"
#include <netinet/in.h>
#include <queue>
#include <memory>
#include <cstdint>
#include <functional>

namespace http_server {

class client {
private:
    int _fd;
    sockaddr_in _addr;

    uint8_t* _send_buffer;
    size_t _send_buffer_size;
    std::function<void (uint8_t*, size_t)> _release_send_func;
    std::unique_ptr<http_server::response> _response;
    void __release_send();

    uint8_t* _recv_buffer;
    size_t _recv_buffer_size;
    std::function<void (uint8_t*, size_t)> _release_recv_func;
    std::unique_ptr<http_server::request> _request;
    void __release_recv();

public:
    client(int fd, ::sockaddr_in addr);
    virtual ~client();
    void set_send_buffer(uint8_t* buffer,
                         size_t size,
                         std::function<void (uint8_t*, size_t)> release_func);
    void set_recv_buffer(uint8_t* buffer,
                         size_t size,
                         std::function<void (uint8_t*, size_t)> release_func);
    http_server::request& get_request();
    http_server::response& get_response();
    void close();
};

}
