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

#endif