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
    commit_metadata() { }
    commit_metadata(std::string name,
                    std::string mail,
                    uint64_t timestamp,
                    std::string timezone)
        : _name(name)
        , _mail(mail)
        , _timestamp(timestamp)
        , _timezone(timezone) { }
    
    std::string &name() { return this->_name; }
    std::string &mail() { return this->_mail; }
    uint64_t &timestamp() { return this->_timestamp; }
    std::string &timezone() { return this->_timezone; }
};
    
class commit_body {
private:
    std::vector<std::string> _parents;
    std::string _tree_sign;
    std::string _message;
    gitter_kid::fsi::commit_metadata _author;
    gitter_kid::fsi::commit_metadata _committer;
public:
    commit_body() { }

    std::vector<std::string> &parents() { return this->_parents; }
    std::string &tree_sign() { return this->_tree_sign; }
    std::string &message() { return this->_message; }
    gitter_kid::fsi::commit_metadata &author() { return this->_author; }
    gitter_kid::fsi::commit_metadata &committer() const { return this->_committer; }

};

class commit : public gitter_kid::fsi::content {
private:
    gitter_kid::fsi::commit_body _body;

public:
    virtual gitter_kid::fsi::obj_type type() const override {
        return gitter_kid::fsi::obj_type::obj_type_commit;
    }
    commit(gitter_kid::fsi::commit_body &body,
           std::basic_string<byte>::iterator spliter,
           std::basic_string<byte>::iterator end);
    gitter_kid::fsi::commit_body &body() { return this->_body; }
};
    
}
}

#endif
