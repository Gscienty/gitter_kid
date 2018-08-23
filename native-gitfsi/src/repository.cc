#include "repository.h"

namespace gitter_kid {
namespace fsi {

repository::repository(std::string name)
    : _name(name) {}

const std::string &repository::name() {
    return this->_name;
}

}
}
