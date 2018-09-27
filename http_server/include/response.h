#pragma once

#include "basic_omsg.h"
#include "status_code.h"
#include "version.h"
#include <map>
#include <string>

namespace http_server {

class response : public http_server::basic_omsg {
private:
    http_server::version_t _version;
    http_server::status_code_t _status_code;
    std::map<std::string, std::string> _header_parameters;
protected:
    virtual void write_metadata() override;
public:
    response(int fd);
    http_server::version_t& version();
    http_server::status_code_t& status_code();
    std::map<std::string, std::string>& header_parameters();
};

}
