#ifndef _GIT_FSI_COMMIT_
#define _GIT_FSI_COMMIT_

#include "content.h"
#include <cstdint>

namespace gitter_kid {
namespace fsi {

class commit_metadata {
private:
    std::string _name;
    std::string _mail;
    uint64_t _timestamp;
    std::string _timezone;
public:
    commit_metadata();
    commit_metadata(std::string name,
                    std::string mail,
                    uint64_t timestamp,
                    std::string timezone);
    commit_metadata(std::basic_string<byte>::iterator begin,
                    std::basic_string<byte>::iterator end);
    
    std::string &name();
    std::string &mail();
    uint64_t &timestamp();
    std::string &timezone();
};
    
class commit_body {
private:
    std::vector<std::string> _parents;
    std::string _tree_sign;
    std::string _message;
    gitter_kid::fsi::commit_metadata _author;
    gitter_kid::fsi::commit_metadata _committer;
public:
    commit_body();

    std::vector<std::string> &parents();
    std::string &tree_sign();
    std::string &message();
    commit_metadata &author();
    commit_metadata &committer();

};

class commit : public content {
private:
    gitter_kid::fsi::commit_body &_body;

public:
    commit(gitter_kid::fsi::commit_body &body,
           std::basic_string<byte>::iterator spliter,
           std::basic_string<byte>::iterator end);
    virtual obj_type type() const override;
    commit_body &body();
};
    
}
}

#endif
