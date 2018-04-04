#include "blob.h"

namespace gitterKid {
    namespace fsi {
        blob::blob(std::vector<byte> &body) : body(body) { }
        blob::blob(const content &origin) : body((*((blob *) &origin)).body) { }
    }
}