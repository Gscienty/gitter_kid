#ifndef _GIT_FSI_REPOSITORY_
#define _GIT_FSI_REPOSITORY_

#include <string>

namespace gitterKid {
    namespace fsi {
        class repository {
            protected:
                const std::string name;
            public:
                repository(std::string name);
                const std::string getName() const;
        };
    }
}

#endif
