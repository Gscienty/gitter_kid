#include "tree.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace gitter_kid {
namespace fsi {

tree_item::tree_item(sign_t sign,
                     std::string name,
                     obj_type type)
    : _sign(sign)
    , _name(name)
    , _type(type) {}

sign_t &tree_item::sign() {
    return this->_sign;
}

std::string &tree_item::name() {
    return this->_name;
}

obj_type &tree_item::type() {
    return this->_type;
}

tree::tree(std::vector<tree_item> &items,
           std::basic_string<byte>::iterator spliter,
           std::basic_string<byte>::iterator end)
    : _items(items) {

    for(std::basic_string<byte>::iterator ch = spliter; ch != end;) {
        std::basic_string<byte>::iterator space_itr = std::find(ch, end, byte(' '));

        obj_type item_type = obj_type::obj_type_unknow;
        if (std::string(ch, space_itr).compare("100644")) {
            item_type = obj_type::obj_type_blob;
        }
        else {
            item_type = obj_type::obj_type_tree;
        }

        std::basic_string<byte>::iterator end_itr = std::find(space_itr + 1, end, byte(0));
        std::string name(space_itr + 1, end_itr);
        sign_t sign;
        sign.bytes_assign(end_itr + 1, end_itr + 21);

        ch = end_itr + 21;
        this->_items.push_back(tree_item(sign, name, item_type));
    }
}

obj_type tree::type() const {
    return obj_type::obj_type_tree;
}

std::vector<tree_item> &tree::items() {
    return this->_items;
}

}
}

