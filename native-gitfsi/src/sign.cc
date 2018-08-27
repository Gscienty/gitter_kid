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

bool sign_t::operator<(const sign_t &other_sign) const {
    for (int i = 0; i < 20; i++) {
        if (this->_sign_bytes[i] < other_sign._sign_bytes[i]) {
            return true;
        }
        else if (this->_sign_bytes[i] > other_sign._sign_bytes[i]) {
            return false;
        }
    }
    return false;
}

}
}

