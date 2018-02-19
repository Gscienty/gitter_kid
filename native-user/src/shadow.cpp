#include "shadow.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <crypt.h>

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
        + this->passwd.Get () + ":"
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
    this->passwd = str;

    off_t hashSplitPos = str.find_last_of ('$');
    this->hash = str.substr (hashSplitPos + 1);
    off_t saltSplitPos = str.find_last_of ('$', hashSplitPos - 1);
    this->salt = str.substr (saltSplitPos + 1, hashSplitPos - saltSplitPos - 1);
    this->id = std::stoi (str.substr (1, saltSplitPos - 1), nullptr, 16);
}

void ShadowPasswd::GenerateSalt () {
    std::random_device r;
    std::default_random_engine eng (r ());
    std::uniform_int_distribution<int> uniform_dist (0, 63);

    std::stringstream builder;

    for (int i = 0; i < 8; i++) { builder << ShadowPasswd::ALPHA[uniform_dist (eng)]; }

    this->salt = builder.str ();
}

ShadowPasswd::ShadowPasswd (const std::string& str) {
    int splitCount = 0;

    std::for_each (str.begin (), str.end (), [&] (const char chr) -> void {
        if (chr == '$') {
            splitCount++;
        }
    });

    if (splitCount == 3) {
        FullingTransfer (str);
    }
    else {
        this->SetPasswd (str);
    }
}


ShadowPasswd::ShadowPasswd (const char *str)  : ShadowPasswd (std::string (str)) { }

ShadowPasswd::ShadowPasswd () { }

ShadowPasswd::ShadowPasswd (const ShadowPasswd &passwd) {
    this->passwd = passwd.passwd;
    this->id = passwd.id;
    this->salt = passwd.salt;
    this->hash = passwd.hash;

}

std::string ShadowPasswd::Get () const { return this->passwd; }
void ShadowPasswd::SetPasswd (const std::string &str, int id) {
    this->GenerateSalt ();
    this->id = id;

    std::stringstream builder;
    builder << "$" << std::hex << this->id << "$" << this->salt << "$";

    this->FullingTransfer (crypt (str.c_str (), builder.str ().c_str ()));
}

ShadowPasswd &ShadowPasswd::operator= (ShadowPasswd &&shadowPasswd) {
    ShadowPasswd &passwd = *(new ShadowPasswd (shadowPasswd));
    return passwd;
}

bool ShadowPasswd::operator== (const char *str) {
    std::stringstream builder;
    builder << "$" << std::hex << this->id << "$" << this->salt << "$";
    return this->passwd.compare(std::string (crypt (str, builder.str ().c_str ()))) == 0;
}
bool ShadowPasswd::operator== (const std::string& str) { return this->operator== (str.c_str ()); }