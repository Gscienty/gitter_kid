#ifndef _G_USR_PASSWD_
#define _G_USR_PASSWD_

#include <string>
#include <vector>
#include "data_store.h"

class PasswdItem : public BaseItem {
private:
    std::string username;
    std::string home;
    std::string gecos;
    unsigned int uid;
    unsigned int gid;
    std::string passwd;
    std::string shell;
public:
    std::string Serialize () const;
    void Parse (std::string line);

    PasswdItem (std::string line);
    PasswdItem ();
};

class PasswdStore : public DataStore <PasswdItem> {
private:
    std::string path = "./test/etc/passwd";

public:
    std::string GetName () const;
    std::vector <PasswdItem> Get () const;
    void Put (std::vector <PasswdItem> items) const;
};

#endif