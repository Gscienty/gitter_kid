#include "session.h"
#include <malloc.h>

http_server::session::session(const http_server::middleware_link& middlewares,
                              size_t send_buffer_size,
                              size_t recv_buffer_size)
    : _middlewares(middlewares)
    , _send_buffer(reinterpret_cast<uint8_t *>(malloc(send_buffer_size)))
    , _send_buffer_size(send_buffer_size)
    , _recv_buffer(reinterpret_cast<uint8_t *>(malloc(recv_buffer_size)))
    , _recv_buffer_size(recv_buffer_size) {}

std::atomic_bool& http_server::session::using_flag() {
    return this->_using_flag;
}

void http_server::session::run(int client_fd, ::sockaddr_in addr) {
    http_server::client client(client_fd, addr);
    client.set_recv_buffer(this->_recv_buffer,
                           this->_recv_buffer_size,
                           [] (uint8_t *, size_t) -> void {  });
    client.set_send_buffer(this->_send_buffer,
                           this->_send_buffer_size,
                           [] (uint8_t *, size_t) -> void {  });

    http_server::context context = {
        client,
        client.get_request(),
        client.get_response(),
        true
    };

    context.request.flush_metadata();

    this->_middlewares.execute(context);

    context.response.flush();
}

