#ifndef _GIT_FSI_BLOB_
#define _GIT_FSI_BLOB_

#include "define.h"
#include "content.h"
#include <vector>

namespace gitterKid {
    namespace fsi {
        class bin : public content {
            private:
                std::vector<byte> body;
            public:
                objectType getType() const { return objectType::blob; }
                bin(const std::vector<byte> &body);

                std::vector<byte> &get() { return this->body; }
        };
    }
}

#endif