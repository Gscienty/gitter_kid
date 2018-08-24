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
    treeItem(std::string sign, std::string name, gitter_kid::fsi::obj_type type)
        : _sign(sign), _name(name), _type(type) { }
    std::string &sign() { return this->_sign; }
    std::string &name() const { return this->_name; }
    gitter_kid::fsi::obj_type &type() { return this->_type; }
};

class tree : public gitter_kid::fsi::content {
private:
    std::vector<gitter_kid::fsi::tree_item> _items;
public:
    virtual gitter_kid::fsi::obj_type type() const override {
        return gitter_kid::fsi::obj_type::obj_type_tree;
    }
    tree(std::basic_string<byte>::iterator spliter, std::basic_string<byte>::iterator end);
    std::vector<gitter_kid::fsi::tree_item> &get() { return this->_items; }
};

}
}

#endif
