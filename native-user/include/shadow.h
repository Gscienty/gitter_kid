#ifndef _G_USR_SHADOW_
#define _G_USR_SHADOW_

#include <string>
#include <vector>
#include "data_store.h"

class ShadowPasswd {
private:
    const char ALPHA[64] { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '.', '/'
    };

    std::string passwd;

    int id;
    std::string salt;
    std::string hash;

    void FullingTransfer (const std::string &str);
    void GenerateSalt ();
    void GeneratePasswd (const std::string &str);

public:
    bool operator== (const char *str);
    bool operator== (const std::string &str);

    ShadowPasswd &operator= (ShadowPasswd &&shadowPasswd);

    std::string Get () const;
    void SetPasswd (const std::string &str, int id = 6);

    ShadowPasswd (const std::string &str);
    ShadowPasswd (const char *str);
    ShadowPasswd (const ShadowPasswd &passwd);
    ShadowPasswd ();
};

class ShadowItem : public BaseItem {
private:
    std::string username;
    ShadowPasswd passwd;
    int last;
    int minInterval;
    int maxInterval;
    int waring;
    int inactive;
    int invaild;

public:
    std::string &GetUserName () { return this->username; }
    int GetLast() { return this->last; }
    int GetMinInterval () { return this->minInterval; }
    int GetMaxInterval () { return this->maxInterval; }
    int GetWaring () { return this->waring; }
    int GetInactive () { return this->inactive; }
    int GetInvaild () { return this->invaild; }

    void Parse (const std::string &line);
    std::string Serialize () const;

    ShadowItem (const char *line) : ShadowItem (std::string (line)) { }
    ShadowItem (const std::string &line);
    ShadowItem ();
};

class ShadowStore : public DataStore<ShadowItem> {
private:
    const std::string path = "./test/etc/shadow";
    std::vector <ShadowItem> items;
public:
    void Reloading ();
    void Initialize ();
    std::string GetName() const { return "shadow"; }
    std::vector<ShadowItem> &Get ();
    void Put (std::vector<ShadowItem> items) const;
};

#endif
