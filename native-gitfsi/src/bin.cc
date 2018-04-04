#include "bin.h"
#include <iostream>

namespace gitterKid {
    namespace fsi {
        bin::bin(std::vector<byte> &body) : body(body) { }
        bin::bin(const content &origin) : body((*((bin *) &origin)).body) { }
    }
}