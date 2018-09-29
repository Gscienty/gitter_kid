#include "staticfile_middleware.h"
#include "mime.h"
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <sys/mman.h>

#include <iostream>

http_server::staticfile_middleware::
staticfile_middleware(std::string starts_with, std::string static_dir)
    : _starts_with(starts_with)
    , _static_dir(static_dir) {}

void http_server::staticfile_middleware::runner(http_server::context& context) {
    if (this->hit(context.request.uri()) == false) {
        context.run_next = true;
        return;
    }

    std::string static_filename(this->_static_dir.begin(), this->_static_dir.end());
    static_filename.append(context.request.uri().begin() + this->_starts_with.size(),
                           context.request.uri().end());

    if (::access(static_filename.c_str(), F_OK) != 0) {
        this->not_found(context.response);
    }
    else {
        http_server::staticfile_middleware::add_mime(context.response, static_filename);
        this->get_content(context.response, static_filename);
    }
    context.run_next = false;
    context.response.flush();
}

bool http_server::staticfile_middleware::hit(std::string& uri) const {
    if (this->_starts_with.size() > uri.size()) {
        return false;
    }

    auto starts_with_itr = this->_starts_with.begin();
    auto uri_itr = uri.begin();

    while (starts_with_itr != this->_starts_with.end() &&
           *starts_with_itr++ == *uri_itr++);

    return starts_with_itr == this->_starts_with.end();
}

void http_server::staticfile_middleware::add_mime(http_server::response& response, std::string& staticfilename) {
    auto ext = std::find(staticfilename.rbegin(), staticfilename.rend(), '.');
    if (ext == staticfilename.rend()) {
        response.header_parameters()["Content-Type"] = "text/plain";
        return;
    }
    std::string ext_name(ext.base() + 1, staticfilename.end());
    if (http_server::mimes.find(ext_name) == http_server::mimes.end()) {
        response.header_parameters()["Content-Type"] = "text/plain";
        return;
    }

    response.header_parameters()["Content-Type"] = http_server::mimes.at(ext_name);
}

void http_server::staticfile_middleware::
get_content(http_server::response& response, std::string& staticfilename) const {
    int fd = ::open(staticfilename.c_str(), O_RDONLY);
    lseek(fd, 0, SEEK_END);
    size_t filesize = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);
    int page_count = filesize / _SC_PAGE_SIZE + (filesize % _SC_PAGE_SIZE == 0 ? 0 : 1);
    
    void* page_ptr = mmap(NULL,
                          page_count * _SC_PAGE_SIZE,
                          PROT_READ,
                          MAP_SHARED,
                          fd,
                          0);

    response.write(reinterpret_cast<uint8_t *>(page_ptr), filesize);

    munmap(page_ptr, page_count * _SC_PAGE_SIZE);
    ::close(fd);
}

void http_server::staticfile_middleware::
not_found(http_server::response& response) const {
    response.status_code() = http_server::status_code_notfound;
    response.header_parameters()["Content-Type"] = "text/html";
    response.write(reinterpret_cast<const uint8_t *>("<h1>Not Found</h1>"), 18);
}
