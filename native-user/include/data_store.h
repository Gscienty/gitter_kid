#ifndef _G_USR_DATA_STORE_
#define _G_USR_DATA_STORE_

#include <string>
#include <vector>

class BaseItem {
public:
    virtual void Parse (std::string line) = 0;
    virtual std::string Serialize () const = 0;
};

template <class T>
class DataStore {
public:
    virtual void Initialize () = 0;
    virtual std::string GetName () const = 0;
    virtual std::vector<T>& Get () = 0;
    virtual void Put (std::vector<T> items) const = 0;
};

#endif