#ifndef _G_USR_SHADOW_
#define _G_USR_SHADOW_

#include <string>
#include <vector>
#include "data_store.h"


class ShadowItem : public BaseItem {
private:
    std::string username;
    std::string passwd;
    int last;
    int minInterval;
    int maxInterval;
    int waring;
    int inactive;
    int invaild;

public:
    void Parse (std::string line);
    std::string Serialize () const;

    ShadowItem (const char *line) : ShadowItem (std::string (line)) { }
    ShadowItem (const std::string& line);
    ShadowItem ();
};

#endif
