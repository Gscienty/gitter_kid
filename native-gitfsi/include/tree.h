#ifndef _GIT_FSI_TREE_
#define _GIT_FSI_TREE_

#include "define.h"
#include "content.h"
#include <vector>
#include <string>

namespace gitter_kid {
namespace fsi {

class tree_item {
private:
    std::string _sign;
    std::string _name;
    gitter_kid::fsi::obj_type _type;
public:
    tree_item(std::string sign, std::string name, obj_type type);
    std::string &sign();
    std::string &name();
    obj_type &type();
};

class tree : public content {
private:
    std::vector<tree_item> &_items;
public:
    virtual obj_type type() const override;
    tree(std::vector<tree_item> &items,
         std::basic_string<byte>::iterator spliter,
         std::basic_string<byte>::iterator end);
    std::vector<tree_item> &get();
};

}
}

#endif
