#include "sign.h"

namespace gitter_kid {
namespace fsi {


byte __to_byte(std::basic_string<byte>::iterator &base) {
    byte result = 0x00;

    if ('0' <= *base && *base <= '9') {
        result ^= __hex_val<'0', 4>(*base);
    }
    else {
        result ^= __hex_val<'a' - 10, 4>(*base);
    }

    if ('0' <= *(base + 1) && *(base + 1) <= '9') {
        result ^= __hex_val<'0', 0>(*(base + 1));
    }
    else {
        result ^= __hex_val<'a' - 10, 0>(*(base + 1));
    }

    return result;
}

sign_t::sign_t() {}

std::string &sign_t::str() {
    return this->_sign_str;
}

std::basic_string<byte> &sign_t::bytes() {
    return this->_sign_bytes;
}

}
}

