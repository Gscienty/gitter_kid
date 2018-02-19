#include "shadow.h"
#include <iostream>
#include <algorithm>

inline int ParseTime (const std::string& timeStr) {
    if (timeStr.empty ()) {
        return -1;
    }

    return std::stoi (timeStr);
}

void ShadowItem::Parse (const std::string& line) {
    std::vector<std::string> vec;
    off_t offset = 0;
    off_t itemEnd = std::string::npos;

    while ((itemEnd = line.find (':', offset)) != std::string::npos) {
        vec.push_back (line.substr (offset, (std::size_t) (itemEnd - offset)));
        offset = itemEnd + 1;
    }

    if (offset < std::string::npos) {
        vec.push_back (line.substr (offset));
    }

    this->username = vec[0];
    this->passwd = vec[1];
    this->last = ParseTime (vec[2]);
    this->minInterval = ParseTime (vec[3]);
    this->maxInterval = ParseTime (vec[4]);
    this->waring = ParseTime (vec[5]);
    this->inactive = ParseTime (vec[6]);
    this->invaild = ParseTime (vec[7]);
}

inline std::string SerializeTime (const int time) {
    if (time == -1) return std::string("");

    return std::to_string (time);
}

std::string ShadowItem::Serialize () const {
    return this->username + ":"
        + this->passwd + ":"
        + SerializeTime (this->last) + ":"
        + SerializeTime (this->minInterval) + ":"
        + SerializeTime (this->maxInterval) + ":"
        + SerializeTime (this->waring) + ":"
        + SerializeTime (this->inactive) + ":"
        + SerializeTime (this->invaild) + ":";
}


ShadowItem::ShadowItem (const std::string& line) {
    this->Parse (line);
}

ShadowItem::ShadowItem () {
    this->username = "";
    this->passwd = "";
    this->last = -1;
    this->minInterval = -1;
    this->maxInterval = -1;
    this->waring = -1;
    this->inactive = -1;
    this->invaild = -1;
}

void ShadowPasswd::FullingTransfer (const std::string& str) {
    off_t hashSplitPos = str.find_last_of ('$');
    this->hash = str.substr (hashSplitPos + 1);
    off_t saltSplitPos = str.find_last_of ('$', hashSplitPos - 1);
    this->salt = str.substr (saltSplitPos + 1, hashSplitPos - saltSplitPos - 1);
    this->id = std::stoi (str.substr (1, saltSplitPos - 1), nullptr, 16);
}

ShadowPasswd::ShadowPasswd (const std::string& str) {


    FullingTransfer (str);
}

ShadowPasswd::ShadowPasswd () { }