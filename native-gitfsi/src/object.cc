#include "object.h"
#include "blob.h"
#include "tree.h"
#include "commit.h"
#include <algorithm>

namespace gitter_kid {
namespace fsi {

object::object()
    : _body_buffer(nullptr)
    , _body(nullptr)
    , _type(obj_type::obj_type_unknow) {}

object::object(std::basic_string<byte> &buffer)
    : _body_buffer(nullptr)
    , _body(nullptr)
    , _type(obj_type::obj_type_unknow) {

    this->_type = obj_type::obj_type_unknow;
    if (buffer.empty()) { return; }

    std::basic_string<byte>::iterator spliter = std::find(buffer.begin(),
                                                          buffer.end(),
                                                          byte(0));

    this->_body = nullptr;
    switch (this->analysis_type(buffer, spliter)) {
    case obj_type::obj_type_blob:
            this->_body_buffer = new std::basic_string<byte>(spliter + 1,
                                                             buffer.end());
            this->_body =
                new blob(*reinterpret_cast<std::basic_string<byte> *>(this->_body_buffer));
            this->_type = obj_type::obj_type_blob;
            break;
    case obj_type::obj_type_tree:
            this->_body_buffer = new std::vector<tree_item>();
            this->_body =
                new tree(*reinterpret_cast<std::vector<tree_item> *>(this->_body_buffer),
                         spliter + 1,
                         buffer.end());
            this->_type = obj_type::obj_type_tree;
            break;
    case obj_type::obj_type_commit:
            this->_body_buffer = new commit_body();
            this->_body =
                new commit(*reinterpret_cast<commit_body *>(this->_body_buffer),
                           spliter + 1,
                           buffer.end());
            this->_type = obj_type::obj_type_commit;
            break;
    default:
            break;
    }
}

object::object(std::basic_string<byte> &buffer, obj_type type)
    : _body_buffer(nullptr)
    , _body(nullptr) 
    , _type(type) {

    switch (this->_type) {
    case obj_type::obj_type_blob:
        this->_body_buffer = new std::basic_string<byte>(buffer.begin(),
                                                         buffer.end());
        this->_body = new blob(*reinterpret_cast<std::basic_string<byte> *>(this->_body_buffer));
        break;
    case obj_type::obj_type_tree:
        this->_body_buffer = new std::vector<tree_item>();
        this->_body =
            new tree(*reinterpret_cast<std::vector<tree_item> *>(this->_body_buffer),
                     buffer.begin(),
                     buffer.end());
        break;
    case obj_type::obj_type_commit:
        this->_body_buffer = new commit_body();
        this->_body = new commit(*reinterpret_cast<commit_body *>(this->_body_buffer),
                                 buffer.begin(),
                                 buffer.end());
        break;
    default:
        return;
    }
}

object::~object() {
    if (this->_body != nullptr) {
        delete this->_body;
    }

    if (this->_body_buffer != nullptr) {
        switch (this->_type) {
        case obj_type::obj_type_blob:
            delete reinterpret_cast<std::basic_string<byte> *>(this->_body_buffer);
            break;
        case obj_type::obj_type_commit:
            delete reinterpret_cast<commit_body *>(this->_body_buffer);
            break;
        case obj_type::obj_type_tree:
            delete reinterpret_cast<std::vector<tree_item> *>(this->_body_buffer);
            break;
        default:
            break;
        }
    }
}

obj_type object::type() const {
    return this->_type;
}

obj_type object::analysis_type(std::basic_string<byte> &store,
                               std::basic_string<byte>::iterator &spliter) {
    if (this->_type != obj_type::obj_type_unknow) {
        return this->_type;
    }

    if (store.empty()) {
        return this->_type;
    }
    if (spliter == store.end()) {
        return this->_type;
    }

    std::basic_string<byte>::iterator space_iter = std::find(store.begin(),
                                                             spliter,
                                                             byte(' '));
    if (spliter == store.end()){
        return this->_type;
    }


    std::string content_size(space_iter + 1, spliter);
    std::string type(store.begin(), space_iter);

    if (type.compare("blob") == 0) {
        this->_type = obj_type::obj_type_blob;
    }
    else if (type.compare("commit") == 0) {
        this->_type = obj_type::obj_type_commit;
    }
    else if (type.compare("tree") == 0) {
        this->_type = obj_type::obj_type_tree;
    }

    return this->_type;
}

}
}
