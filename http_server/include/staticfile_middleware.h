#pragma once

#include "middleware.h"
#include <string>

namespace http_server {

class staticfile_middleware : public http_server::middleware {
private:
    const std::string _starts_with;
    const std::string _static_dir;
public:
    staticfile_middleware(std::string starts_with, std::string static_dir);
    virtual void runner(http_server::context& context) override;
    bool hit(std::string& uri) const;
    void not_found(http_server::response& response) const;
    static void add_mime(http_server::response& response, std::string& static_filename);
    void get_content(http_server::response& response, std::string& staticfilename) const;
};

}
