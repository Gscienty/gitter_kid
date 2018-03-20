#ifndef _G_USR_GROUP_
#define _G_USR_GROUP_

#include <string>
#include <vector>
#include "data_store.h"

class GroupItem : public BaseItem {
private:
    std::string name;
    std::string passwd;
    uint32_t gid;
    std::vector<std::string> users;

public:
    std::string &GetName () { return this->name; }
    std::string &GetPasswd () { return this->passwd; }
    uint32_t GetGID () const { return this->gid; }
    std::vector<std::string> &GetUsers () { return this->users; }

    std::string Serialize () const;
    void Parse (const std::string& line);

    GroupItem (const char *line) : GroupItem (std::string (line)) { }
    GroupItem (const std::string& line);
    GroupItem ();
};

class GroupStore : public DataStore<GroupItem> {
private:
    const std::string path = "/etc/group";
    std::vector<GroupItem> items;
public:
    void Reloading ();
    void Initialize ();
    std::string GetName () const;
    std::vector<GroupItem> &Get ();
    void Put (std::vector<GroupItem> items) const;
};

#endif