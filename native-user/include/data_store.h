#ifndef _G_USR_DATA_STORE_
#define _G_USR_DATA_STORE_

#include <string>
#include <vector>

class BaseItem {
protected:
    virtual void Parse (std::string line) = 0;
    virtual std::string Serialize () = 0;
};

template <class T>
class data_store {
public:
    std::string GetName () const = 0;
    std::vector<T> Get () const = 0;
    void Put (std::vector<T> items) const = 0;
};

#endif