#include "object.h"
#include <algorithm>

namespace gitterKid {
    namespace fsi {
        object::object(const repository& repo, std::string signture)
            : repo(repo), signture(signture), type(objectType::unknow) { }

        objectType object::getType() const { return this->type; }

        void object::analysisType(std::vector<byte>::iterator& spliter) {
            this->type = objectType::unknow;
            if (this->content.empty()) { return; }
            if (spliter == this->content.end()) { return; }
            std::vector<byte>::iterator spaceIter = std::find(this->content.begin(), spliter, ' ');
            if (spaceIter == spliter) { return; }

            std::string contentSize;
            std::string type;

            contentSize.insert(contentSize.begin(), spaceIter + 1, spliter);
            type.insert(type.begin(), this->content.begin(), spaceIter);
            if (type.compare("blob") == 0) { this->type = objectType::blob; }
            else if (type.compare("commit") == 0) { this->type = objectType::commit; }
            else if (type.compare("tree") == 0) { this->type = objectType::tree; }
        }

        void object::initialize() {
            this->content.clear();
            this->type = objectType::unknow;
            this->fillContent();
            if (this->content.empty()) { return; }
            
            std::vector<byte>::iterator spliter = std::find(this->content.begin(), this->content.end(), (byte) 0);
            this->analysisType(spliter);
        }
    }
}