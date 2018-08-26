#ifndef _GIT_FSI_REPOSITORY_
#define _GIT_FSI_REPOSITORY_

#include <string>
#include "sign.h"

namespace gitter_kid {
namespace fsi {

class repository {
private:
    const std::string _path;

public:
    repository(std::string path);
    const std::string &path();
    std::string looseobj_path(sign_t sign);
};

}
}

#endif
