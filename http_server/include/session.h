#pragma once

#include "middleware_link.h"
#include <netinet/in.h>
#include <memory>
#include <atomic>

namespace http_server {

class session {
private:
    const http_server::middleware_link& _middlewares;
    std::atomic_bool _using_flag;

    uint8_t* _send_buffer;
    size_t _send_buffer_size;
    uint8_t* _recv_buffer;
    size_t _recv_buffer_size;
public:
    session(const http_server::middleware_link& middlewares,
            size_t send_buffer_size,
            size_t recv_buffer_size);

    std::atomic_bool& using_flag();
    void run(int client_fd, ::sockaddr_in addr);
};

}
