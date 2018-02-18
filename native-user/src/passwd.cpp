#include "passwd.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>

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

std::string PasswdItem::Serialize () const {
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
    this->username = "";
    this->passwd = "";
    this->uid = 0;
    this->gid = 0;
    this->gecos = "";
    this->home = "";
    this->shell = "";
}


std::string PasswdStore::GetName () const { return "passwd"; }

std::vector <PasswdItem> PasswdStore::Get () const {
    std::vector <PasswdItem> result;

    std::ifstream passwdFile (this->path);
    if (passwdFile.is_open () == false) {
        return result;
    }
    std::string line;
    while (std::getline (passwdFile, line)) {
        result.push_back (PasswdItem (line));
    }

    return result;
}

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
    Backup (this->path);

    std::ofstream writer (this->path);

    std::for_each (items.begin (), items.end (), [&] (const PasswdItem& item) -> void {
        writer << item.Serialize () << std::endl;
    });

    writer.close ();

    RemoveBackup (this->path);
}