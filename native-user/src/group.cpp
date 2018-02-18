#include "group.h"

std::string GroupItem::Serialize () const {
    return this->name + ":"
        + this->passwd + ":"
        + std::to_string (this->gid);
}

void GroupItem::Parse (std::string line) {
    std::vector<std::string> vec;
    off_t offset = 0;
    off_t itemEnd = line.npos;

    while ((itemEnd = line.find (':', offset)) != line.npos) {
        vec.push_back (line.substr (offset, (std::size_t) (itemEnd - offset)));
        offset = itemEnd + 1;
    }

    if (offset < line.npos) {
        vec.push_back (line.substr (offset));
    }

    this->name = vec[0];
    this->passwd = vec[1];
    this->gid = std::stoi (vec[2], nullptr);
}


GroupItem::GroupItem (std::string line) {
    this->Parse (line);
}
GroupItem::GroupItem () {
    this->name = "";
    this->passwd = "";
    this->gid = 0;
}