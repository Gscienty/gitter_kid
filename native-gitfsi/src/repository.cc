#include "repository.h"
#include <sstream>

namespace gitter_kid {
namespace fsi {

repository::repository(std::string path)
    : _path(path) {}

const std::string &repository::path() {
    return this->_path;
}

std::string repository::looseobj_path(sign_t sign) {
    std::stringstream obj_path_builder;

    std::string path(this->_path.size() +
                     1 + 7 + 1 + 2 + 1 + 38, 0);
    obj_path_builder.rdbuf()
        ->pubsetbuf(const_cast<char *>(path.data()),
                    path.size());

    obj_path_builder.write(this->_path.data(),
                           this->_path.size());
    obj_path_builder.write("/objects/", 9);
    obj_path_builder.write(sign.str().data(), 2);
    obj_path_builder.put('/');
    obj_path_builder.write(sign.str().data() + 2,
                           38);

    return path;
}

}
}
