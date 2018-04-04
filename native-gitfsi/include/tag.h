#ifndef _GIT_FSI_TAG_
#define _GIT_FSI_TAG_

#include "define.h"
#include <string>
#include <vector>

namespace gitterKid {
    namespace fsi {
        class tagBody {
            private:
                std::string objectSignture;
                objectType type;
                std::string name;
                commitPersonLog tagger;
                std::string message;
        };

        class tag : public content {
            private:
                tagBody &body;
            public:
                objectType getType() const { return objectType::tagType; }
                tree(tagBody &body, std::vector<byte>::iterator spliter, std::vector<byte>::iterator end);
                tree(const content &origin);
                tagBody get() const { return this->items; }
        };
    }
}

#endif
