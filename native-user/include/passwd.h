#ifndef _G_USR_PASSWD_
#define _G_USR_PASSWD_

#include <string>
#include <vector>
#include "data_store.h"

class PasswdItem : protected BaseItem {
private:
    std::string username;
    std::string home;
    std::string gecos;
    unsigned int uid;
    unsigned int gid;
    std::string passwd;
    std::string shell;
protected:
public:
    std::string Serialize ();
    void Parse (std::string line);
    PasswdItem (std::string line);
    PasswdItem ();
};

#endif