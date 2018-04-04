#include "tree.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace gitterKid {
    namespace fsi {
        tree::tree(std::vector<treeItem> &items, std::vector<byte>::iterator spliter, std::vector<byte>::iterator end)
            : items(items) {
            for(std::vector<byte>::iterator ch = spliter; ch != end;) {
                std::vector<byte>::iterator subEnd = std::find(ch, end, (byte) ' ');
                std::string type;
                type.insert(type.begin(), ch, subEnd);
                ch = subEnd + 1;
                
                subEnd = std::find(ch, end, (byte) 0);
                std::string name;
                name.insert(name.begin(), ch, subEnd);
                ch = subEnd + 1;

                std::stringstream signStream;

                for (int i = 0; i < 20; i++) {
                    signStream << std::hex << (unsigned int) ((*ch & 0xF0) >> 4) << std::hex << (unsigned int) (*ch & 0x0F);
                    ch++;
                }
                objectType innerType = objectType::unknowType;
                if (type.compare("100644")) {
                    innerType = objectType::blobType;
                }
                else {
                    innerType = objectType::treeType;
                }
                this->items.push_back(treeItem(signStream.str(), name, innerType));
            }
        }

        tree::tree(const content &origin) : items((*((tree *) &origin)).items) { }
    }
}