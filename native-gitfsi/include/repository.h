#ifndef _GIT_FSI_REPOSITORY_
#define _GIT_FSI_REPOSITORY_

#include "pack.h"
#include "object.h"
#include "sign.h"
#include <string>
#include <vector>

namespace gitter_kid {
namespace fsi {

class repository {
private:
    const std::string _path;
    std::vector<pack> _packs;
public:
    repository(std::string path);
    const std::string &path();
    std::string looseobj_path(sign_t sign);
    void initialize_packs();

    object get(sign_t sign);
};

}
}

#endif
