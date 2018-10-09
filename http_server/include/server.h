#pragma once

#include <netinet/in.h>

namespace http_server {

class server {
private:
    int _fd;
    size_t _backlog;
    int _epoll_fd;
public:
    server(::sockaddr_in &addr, size_t backlog);
};

}
