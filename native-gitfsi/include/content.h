#ifndef _GIT_FSI_CONTENT_
#define _GIT_FSI_CONTENT_

#include "define.h"
#include <vector>

namespace gitterKid {
    namespace fsi {

        enum objectType { unknow, blob, commit, tree, tag, ofsDelta, refDelta };

        class content {
            public:
                virtual objectType getType() const = 0;
        };
    }
}

#endif