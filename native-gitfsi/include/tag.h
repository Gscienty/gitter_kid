#ifndef _GIT_FSI_TAG_
#define _GIT_FSI_TAG_

#include "content.h"
#include "commit.h"
#include "define.h"
#include <string>
#include <vector>

namespace gitter_kid {
namespace fsi {

class tag_body {
private:
    std::string _obj_sign;
    gitter_kid::fsi::obj_type _type;
    std::string _name;
    gitter_kid::fsi::commit_metadata _tagger;
    std::string _message;
public:
    std::string &obj_sign() { return this->_obj_sign; }
    gitter_kid::fsi::obj_type &type() { return this->_type; }
    std::string &name() { return this->_name; }
    gitter_kid::fsi::commit_metadata &tagger() { return this->_tagger; }
    std::string &message() { return this->_message; }
};

class tag : public gitter_kid::fsi::content {
private:
    gitter_kid::fsi::tag_body &_body;
public:
    virutal gitter_kid::fsi::obj_type type() const override {
        return gitter_kid::fsi::obj_type::obj_type_tag;
    }
    tag(gitter_kid::fsi::tag_body &body,
        std::basic_string<byte>::iterator spliter,
        std::basic_string<byte>::iterator end);
    gitter_kid::fsi::tag_body &get() { return this->_body; }
};

}
}

#endif
