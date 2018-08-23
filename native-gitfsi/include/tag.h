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
    obj_type _type;
    std::string _name;
    commit_metadata _tagger;
    std::string _message;
public:
    std::string &obj_sign();
    obj_type &type();
    std::string &name();
    commit_metadata &tagger();
    std::string &message();
};

class tag : public content {
private:
    gitter_kid::fsi::tag_body &_body;
public:
    virtual obj_type type() const override;
    tag(gitter_kid::fsi::tag_body &body,
        std::basic_string<byte>::iterator spliter,
        std::basic_string<byte>::iterator end);
    tag_body &get();
};

}
}

#endif
