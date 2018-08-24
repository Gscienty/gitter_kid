#ifndef _GIT_FSI_REPOSITORY_
#define _GIT_FSI_REPOSITORY_

#include <string>

namespace gitter_kid {
namespace fsi {
class repository {
protected:
    const std::string _name;
public:
    repository(std::string name);
    std::string &name();
};
}
}

#endif
