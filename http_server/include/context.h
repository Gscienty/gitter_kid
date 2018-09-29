#pragma once

#include "request.h"
#include "response.h"
#include "client.h"

namespace http_server {

struct context {
    http_server::client& client;
    http_server::request& request;
    http_server::response& response;

    bool run_next;
};

}
