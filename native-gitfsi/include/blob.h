#ifndef _GIT_FSI_BLOB_
#define _GIT_FSI_BLOB_

#include "define.h"
#include "content.h"
#include <vector>

namespace gitterKid {
    namespace fsi {
        class blob : public content {
            private:
                std::vector<byte> &body;
            public:
                objectType getType() const { return objectType::blobType; }
                blob(std::vector<byte> &body);
                blob(const content &origin);

                std::vector<byte> &get() { return this->body; }
        };
    }
}

#endif