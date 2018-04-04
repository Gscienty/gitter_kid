#ifndef _GIT_FSI_TREE_
#define _GIT_FSI_TREE_

#include "define.h"
#include "content.h"
#include <vector>
#include <string>

namespace gitterKid {
    namespace fsi {
        
        class treeItem {
            private:
                std::string sign;
                std::string name;
                objectType type;
            public:
                treeItem(std::string sign, std::string name, objectType type)
                    : sign(sign), name(name), type(type) { }
                std::string getSign() const { return this->sign; }
                std::string getName() const { return this->name; }
                objectType getType() const { return this->type; }
        };

        class tree : public content {
            private:
                std::vector<treeItem> &items;
            public:
                objectType getType() const { return objectType::treeType; }
                tree(std::vector<treeItem> &items, std::vector<byte>::iterator spliter, std::vector<byte>::iterator end);
                tree(const content &origin);
                std::vector<treeItem> get() const { return this->items; }
        };
    }
}

#endif