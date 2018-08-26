#include "sign.h"

namespace gitter_kid {
namespace fsi {

sign_t::sign_t() {}

sign_t::sign_t(std::string sign) {
    this->str_assign(sign.begin(), sign.end());
}

sign_t::sign_t(char *sign) {
    this->str_assign(sign, sign + 40);
}

std::string &sign_t::str() {
    return this->_sign_str;
}

std::basic_string<byte> &sign_t::bytes() {
    return this->_sign_bytes;
}

}
}

