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
    void SetUserName (const std::string& value) { this->username = value; }
    void SetHome (const std::string& value) { this->home = value; }
    void SetGecos (const std::string& value) { this->gecos = value; }
    void SetUID (unsigned int value) { this->uid = value; }
    void SetGID (unsigned int value) { this->gid = value; }
    void SetPasswd (const std::string& value) { this->passwd = value; }
    void SetShell (const std::string& value) { this->shell = value; }

    std::string &GetUserName () { return this->username; }
    std::string &GetHome () { return this->home; }
    std::string &GetGecos () { return this->gecos; }
    std::string &GetPasswd () { return this->passwd; }
    std::string &GetShell () { return this->shell; }
    unsigned int GetGID () { return this->gid; }
    unsigned int GetUID () { return this->uid; }

    std::string Serialize () const;
    void Parse (const std::string& line);

    PasswdItem (const char *line) : PasswdItem (std::string (line)) { }
    PasswdItem (const std::string& line);
    PasswdItem ();
};

class PasswdStore : public DataStore<PasswdItem> {
private:
    const std::string path = "./test/etc/passwd";
    std::vector <PasswdItem> items;

public:
    void Reloading ();
    void Initialize ();
    std::string GetName () const;
    std::vector<PasswdItem> &Get ();
    void Put (std::vector<PasswdItem> items) const;
};

#endif