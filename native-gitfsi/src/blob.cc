#include "blob.h"

namespace gitter_kid {
namespace fsi {

blob::blob(std::basic_string<byte> &body)
    : _body(body) { }

obj_type blob::type() const {
    return obj_type_blob;
}

std::basic_string<byte> &blob::body() {
    return this->_body;
}

}
}
