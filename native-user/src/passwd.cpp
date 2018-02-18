#include "passwd.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>

void PasswdItem::Parse (const std::string& line) {
    std::vector<std::string> vec;
    off_t offset = 0;
    off_t itemEnd = line.npos;

    while ((itemEnd = line.find (':', offset)) != line.npos) {
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

std::string PasswdItem::Serialize () const {
    return this->username + ":"
    + this->passwd + ":"
    + std::to_string (this->uid) + ":"
    + std::to_string (this->gid) + ":"
    + this->gecos + ":"
    + this->home + ":"
    + this->shell;
}

PasswdItem::PasswdItem (const std::string& line) {
    this->Parse (line);
}

PasswdItem::PasswdItem () {
    this->username = "";
    this->passwd = "";
    this->uid = 0;
    this->gid = 0;
    this->gecos = "";
    this->home = "";
    this->shell = "";
}


std::string PasswdStore::GetName () const { return "passwd"; }

void PasswdStore::Initialize () {
    std::ifstream passwdFile (PasswdStore::path);
    if (passwdFile.is_open () == false) {
        return;
    }
    std::string line;
    while (std::getline (passwdFile, line)) {
        this->items.push_back (PasswdItem (line));
    }
}

std::vector<PasswdItem>& PasswdStore::Get () { return this->items; }

inline void Backup (const std::string& path) {
    std::ifstream originFile (path);
    std::ofstream backupFile (path + "_");
    std::string line;
    while (std::getline (originFile, line)) {
        backupFile << line << std::endl;
    }

    originFile.close ();
    backupFile.close ();
}

inline void RemoveBackup (const std::string& path) {
    std::remove ((path + "_").c_str ());
}

void PasswdStore::Put (std::vector <PasswdItem> items) const {
    Backup (PasswdStore::path);

    std::ofstream writer (PasswdStore::path);

    std::for_each (items.begin (), items.end (), [&] (const PasswdItem& item) -> void {
        writer << item.Serialize () << std::endl;
    });

    writer.close ();

    RemoveBackup (PasswdStore::path);
}