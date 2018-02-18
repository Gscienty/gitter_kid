#include "passwd.h"
#include <iostream>
#include <algorithm>
#include <functional>

void foreach_iter (const std::string& str) {
    std::cout << str << std::endl;
}

void PasswdItem::Parse (std::string line) {
    std::vector<std::string> vec;
    std::size_t offset = 0;
    std::size_t itemEnd = std::string::npos;

    while ((itemEnd = line.find (':', offset)) != std::string::npos) {
        vec.push_back (line.substr (offset, itemEnd - offset));
        offset = itemEnd + 1;
    }
    if (offset < line.npos) {
        vec.push_back (line.substr (offset));
    }

    this->username = vec[0];
    this->passwd = vec[1];
    this->uid = std::stoi (vec[2], nullptr);
    this->gid = std::stoi (vec[3], nullptr);
    this->gecos = vec[4];
    this->home = vec[5];
    this->shell = vec[6];
}

std::string PasswdItem::Serialize () {
    return this->username + ":"
    + this->passwd + ":"
    + std::to_string (this->uid) + ":"
    + std::to_string (this->gid) + ":"
    + this->gecos + ":"
    + this->home + ":"
    + this->shell;
}

PasswdItem::PasswdItem (std::string line) {
    this->Parse (line);
}

PasswdItem::PasswdItem () {
    this->uid = 0;
    this->gid = 0;
}