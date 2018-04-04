#ifndef _GIT_FSI_CONTENT_
#define _GIT_FSI_CONTENT_

#include "define.h"
#include <vector>
#include <string>

namespace gitterKid {
    namespace fsi {

        enum objectType { unknowType, blobType, commitType, treeType, tagType, ofsDeltaType, refDeltaType };

        class treeItem {
            private:
                std::string sign;
                std::string name;
                objectType type;
            public:
                treeItem(std::string sign, std::string name, objectType type)
                    : sign(sign), name(name), type(type) { }
                std::string &getSign() { return this->sign; }
                std::string &getName() { return this->name; }
                objectType getType() { return this->type; }
        };

        class content {
            public:
                virtual objectType getType() const = 0;
        };
    }
}

#endif