#include "server.h"
#include <sys/socket.h>
#include <sys/epoll.h>

http_server::server::server(::sockaddr_in &addr, size_t backlog)
    : _backlog(backlog) {
    this->_epoll_fd = ::epoll_create(backlog);

    this->_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    ::bind(this->_fd, reinterpret_cast<::sockaddr *>(&addr), sizeof(::sockaddr_in));

    ::epoll_event event;
    event.events = EPOLLIN;
}
