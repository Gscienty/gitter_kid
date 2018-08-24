#ifndef _GIT_FSI_CONTENT_
#define _GIT_FSI_CONTENT_

#include "define.h"
#include <vector>
#include <string>

namespace gitterKid {
namespace fsi {

enum obj_type {
    obj_type_unknow,
    obj_type_blob,
    obj_type_commit,
    obj_type_tree,
    obj_type_tag,
    obj_type_ofsdelta,
    obj_type_refdelta
};

class content {
public:
    virtual obj_type type() const = 0;
};

}
}

#endif
