#ifndef _G_USR_SHADOW_
#define _G_USR_SHADOW_

#include <string>
#include <vector>
#include "data_store.h"

class ShadowPasswd {
private:
    std::string passwd;

    int id;
    std::string salt;
    std::string hash;

    void FullingTransfer (const std::string& str);

public:
    bool operator== (const char *str);
    bool operator== (const std::string& str);
    bool operator== (const ShadowPasswd& passwd);

    ShadowPasswd (const std::string& str);
    ShadowPasswd (const char *str) : ShadowPasswd (std::string (str)) { }
    ShadowPasswd ();
};

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
    void Parse (const std::string& line);
    std::string Serialize () const;

    ShadowItem (const char *line) : ShadowItem (std::string (line)) { }
    ShadowItem (const std::string& line);
    ShadowItem ();
};

#endif
