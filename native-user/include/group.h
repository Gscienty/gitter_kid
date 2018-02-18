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

public:
    std::string Serialize () const;
    void Parse (std::string line);

    GroupItem (std::string line);
    GroupItem ();
};

class GroupStore : public DataStore<GroupItem> {
private:
    const std::string path = "./test/etc/group";
    vector<GroupItem> items;
public:
    void Initialize ();
    std::string GetName () const;
    std::vector<GroupItem>& Get ();
    void Put (std::vector<GroupItem> items) const;
};

#endif