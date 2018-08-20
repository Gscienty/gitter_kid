#ifndef _GIT_FSI_BLOB_
#define _GIT_FSI_BLOB_

#include "define.h"
#include "content.h"
#include <string>

namespace gitterKid {
namespace fsi {
class blob : public content {
private:
    std::basic_string<byte> body;
public:
    virtual objectType getType() const override { return objectType::blobType; }
    blob(std::basic_string<byte> &body);

    std::basic_string<byte> &get() { return this->body; }
};
}
}

#endif
