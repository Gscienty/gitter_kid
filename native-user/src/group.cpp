#include "group.h"
#include <iostream>
#include <fstream>
#include <algorithm>

std::string GroupItem::Serialize () const {
    std::string base = this->name + ":"
        + this->passwd + ":"
        + std::to_string (this->gid);
    if (this->users.size () == 0) {
        return base;
    }

    base += ":" + this->users[0];

    std::for_each (this->users.begin () + 1, this->users.end (), [&] (const std::string &user) -> void {
        base += "," + user;
    });

    return base;
}

void GroupItem::Parse (const std::string& line) {
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

    this->name = vec[0];
    this->passwd = vec[1];
    this->gid = std::stoi (vec[2], nullptr);

    if (vec.size () == 4) {
        offset = 0;
        itemEnd = std::string::npos;
        std::string users = vec[3];

        while ((itemEnd = users.find (',', offset)) != std::string::npos) {
            this->users.push_back (users.substr (offset, (std::size_t) (itemEnd - offset)));
            offset = itemEnd + 1;
        }

        if (offset < std::string::npos) {
            this->users.push_back (users.substr (offset));
        }
    }
}

GroupItem::GroupItem (const std::string& line) {
    this->Parse (line);
}

GroupItem::GroupItem () {
    this->name = "";
    this->passwd = "";
    this->gid = 0;
}

void GroupStore::Reloading () {
    this->items.clear ();
    this->Initialize ();
}


void GroupStore::Initialize () {
    if (this->items.empty () == false) {
        return ;
    }
    std::ifstream groupFile (GroupStore::path);
    if (groupFile.is_open () == false) {
        return;
    }

    std::string line;
    while (std::getline (groupFile, line)) {
        this->items.push_back (line);
    }

    groupFile.close ();
}

std::string GroupStore::GetName () const { return "group"; }

std::vector<GroupItem>& GroupStore::Get () { return this->items; }

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

void GroupStore::Put (std::vector<GroupItem> items) const {
    Backup (GroupStore::path);

    std::ofstream writer (GroupStore::path);

    std::for_each (items.begin (), items.end (), [&] (const GroupItem& item) -> void {
        writer << item.Serialize () << std::endl;
    });

    writer.close ();

    RemoveBackup (GroupStore::path);
}