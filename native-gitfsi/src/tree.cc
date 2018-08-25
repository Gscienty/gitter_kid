#include "tree.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace gitter_kid {
namespace fsi {

tree_item::tree_item(std::string sign,
                     std::string name,
                     obj_type type)
    : _sign(sign)
    , _name(name)
    , _type(type) {}

std::string &tree_item::sign() {
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
        std::basic_string<byte>::iterator sub_end = std::find(ch, end, byte(' '));
        std::string type;
        type.insert(type.begin(), ch, sub_end);
        ch = sub_end + 1;
        
        sub_end = std::find(ch, end, byte(0));
        std::string name(ch, sub_end);
        ch = sub_end + 1;

        std::string sign;
        sign.resize(40);
        std::stringstream sign_stream;
        sign_stream.rdbuf()->pubsetbuf(const_cast<char *>(sign.data()), 40);

        for (int i = 0; i < 20; i++) {
            sign_stream << std::hex << uint16_t((*ch & 0xF0) >> 4) << uint16_t(*ch & 0x0F);
            ch++;
        }
        obj_type item_type = obj_type::obj_type_unknow;
        if (type.compare("100644")) {
            item_type = obj_type::obj_type_blob;
        }
        else {
            item_type = obj_type::obj_type_tree;
        }
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

