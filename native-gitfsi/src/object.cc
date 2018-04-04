#include "object.h"
#include "bin.h"
#include <algorithm>

namespace gitterKid {
    namespace fsi {
        object::object(const repository& repo, std::string signture)
            : repo(repo), signture(signture), type(objectType::unknow) {
            this->body = NULL;
        }
        
        object::~object() {
            if (this->body != NULL) {
                delete this->body;
            }
        }

        objectType object::getType() const { return this->type; }

        objectType object::analysisType(std::vector<byte>& store, std::vector<byte>::iterator& spliter) {
            this->type = objectType::unknow;
            if (store.empty()) { return objectType::unknow; }
            if (spliter == store.end()) { return objectType::unknow; }
            std::vector<byte>::iterator spaceIter = std::find(store.begin(), spliter, ' ');
            if (spaceIter == spliter) { return objectType::unknow; }

            std::string contentSize;
            std::string type;

            contentSize.insert(contentSize.begin(), spaceIter + 1, spliter);
            type.insert(type.begin(), store.begin(), spaceIter);
            if (type.compare("blob") == 0) { this->type = objectType::blob; }
            else if (type.compare("commit") == 0) { this->type = objectType::commit; }
            else if (type.compare("tree") == 0) { this->type = objectType::tree; }

            return this->type;
        }

        void object::initialize() {
            this->type = objectType::unknow;
            std::vector<byte> store = this->getStore();
            if (store.empty()) { return; }
            
            std::vector<byte>::iterator spliter = std::find(store.begin(), store.end(), (byte) 0);
            std::vector<byte> body(spliter + 1, store.end());
            switch (this->analysisType(store, spliter)) {
                case objectType::blob:
                    this->body = new bin(body);
                    break;
                default:
                    this->body = NULL;
            }
        }
    }
}