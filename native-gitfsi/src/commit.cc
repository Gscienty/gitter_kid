#include "commit.h"
#include <algorithm>
#include <string>

namespace gitter_kid {
namespace fsi {

commit_metadata::commit_metadata() {}

commit_metadata::commit_metadata(std::string name,
                                 std::string mail,
                                 uint64_t timestamp,
                                 std::string timezone)
    : _name(name)
    , _mail(mail)
    , _timestamp(timestamp)
    , _timezone(timezone) {}

commit_metadata::commit_metadata(std::basic_string<byte>::iterator begin,
                                 std::basic_string<byte>::iterator end) {
    std::basic_string<byte> fields[4];
    std::basic_string<byte>::iterator ch = begin;

    int i;
    for (i = 0; ch != end && i < 4; i++) {
        auto space_itr = std::find(ch, end, byte(' '));
        if (space_itr != end) {
            fields[i].assign(ch, space_itr);
            ch = space_itr + 1;
        }
        else {
            ch = end;
        }
    }

    this->_name.assign(fields[0].begin(), fields[0].end());
    this->_mail.assign(fields[1].begin(), fields[1].end());
    this->_timestamp = std::stol(std::string(fields[2].begin(), fields[2].end()));
    this->_timezone.assign(fields[3].begin(), fields[3].end());
}

std::string &commit_metadata::name() {
    return this->_name;
}

std::string &commit_metadata::mail() {
    return this->_mail;
}

uint64_t &commit_metadata::timestamp() {
    return this->_timestamp;
}

std::string &commit_metadata::timezone() {
    return this->_timezone;
}

commit_body::commit_body() {}

std::vector<std::string> &commit_body::parents() {
    return this->_parents;
}

std::string &commit_body::tree_sign() {
    return this->_tree_sign;
}

std::string &commit_body::message() {
    return this->_message;
}

commit_metadata &commit_body::author() {
    return this->_author;
}

commit_metadata &commit_body::committer() {
    return this->_committer;
}

commit::commit(commit_body &body,
               std::basic_string<byte>::iterator spliter,
               std::basic_string<byte>::iterator end)
    : _body(body) {
    for (std::basic_string<byte>::iterator ch = spliter; ch != end; ) {
        std::basic_string<byte>::iterator end_line_iter = std::find(ch, end, byte('\n'));

        if (end_line_iter == ch) {
            this->_body.message().assign(ch + 1, end);
            break;
        }

        std::basic_string<byte>::iterator space_iter = std::find(ch, end, byte(' '));
        std::string tag(ch, space_iter);

        if (tag.compare("tree") == 0) {
            this->_body.tree_sign().assign(space_iter + 1, end_line_iter);
        }
        else if (tag.compare("author") == 0) {
            this->_body.author() = commit_metadata(space_iter + 1, end_line_iter);
        }
        else if (tag.compare("committer") == 0) {
            this->_body.committer() = commit_metadata(space_iter + 1, end_line_iter);
        }
        else if (tag.compare("parent") == 0) {
            this->_body.parents().push_back(std::string(space_iter + 1, end_line_iter));
        }

        ch = end_line_iter + 1;
    }
}

obj_type commit::type() const {
    return obj_type::obj_type_commit;
}

commit_body &commit::body() {
    return this->_body;
}

}
}
