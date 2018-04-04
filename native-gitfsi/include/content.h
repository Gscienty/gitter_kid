#ifndef _GIT_FSI_CONTENT_
#define _GIT_FSI_CONTENT_

#include "define.h"
#include <vector>
#include <string>

namespace gitterKid {
    namespace fsi {

        enum objectType { unknowType, blobType, commitType, treeType, tagType, ofsDeltaType, refDeltaType };

        class content {
            public:
                virtual objectType getType() const = 0;
        };
    }
}

#endif