#include "object.h"
#include "blob.h"
#include "tree.h"
#include "commit.h"
#include <algorithm>

namespace gitterKid {
    namespace fsi {
        object::object(const repository& repo, std::string signture)
            : repo(repo), signture(signture), type(objectType::unknowType) {
            this->body = NULL;
            this->bodyBuffer = NULL;
        }
        
        object::~object() {

            switch (this->type) {
                case objectType::blobType:
                    if (this->body != NULL) { delete (blob *) this->body; }
                    if (this->bodyBuffer != NULL) { delete (std::vector<byte> *) this->bodyBuffer; }
                    break;
                case objectType::treeType:
                    if (this->body != NULL) { delete (blob *) this->body; }
                    if (this->bodyBuffer != NULL) { delete (std::vector<treeItem> *) this->bodyBuffer; }
                    break;
            }
        }

        objectType object::getType() const { return this->type; }

        objectType object::analysisType(std::vector<byte>& store, std::vector<byte>::iterator& spliter) {
            this->type = objectType::unknowType;
            if (store.empty()) { return objectType::unknowType; }
            if (spliter == store.end()) { return objectType::unknowType; }
            std::vector<byte>::iterator spaceIter = std::find(store.begin(), spliter, ' ');
            if (spaceIter == spliter) { return objectType::unknowType; }

            std::string contentSize;
            std::string type;

            contentSize.insert(contentSize.begin(), spaceIter + 1, spliter);
            type.insert(type.begin(), store.begin(), spaceIter);
            if (type.compare("blob") == 0) { this->type = objectType::blobType; }
            else if (type.compare("commit") == 0) { this->type = objectType::commitType; }
            else if (type.compare("tree") == 0) { this->type = objectType::treeType; }

            return this->type;
        }

        void object::initialize() {
            this->type = objectType::unknowType;
            std::vector<byte> store = this->getStore();
            if (store.empty()) { return; }
            
            std::vector<byte>::iterator spliter = std::find(store.begin(), store.end(), (byte) 0);
            this->body = NULL;
            switch (this->analysisType(store, spliter)) {
                case objectType::blobType:
                    this->bodyBuffer = new std::vector<byte>();
                    (*((std::vector<byte> *) this->bodyBuffer)).assign(spliter + 1, store.end());
                    this->body = new blob(*((std::vector<byte> *) this->bodyBuffer));
                    break;
                case objectType::treeType:
                    this->bodyBuffer = new std::vector<treeItem>();
                    this->body = new tree(*((std::vector<treeItem> *) this->bodyBuffer), spliter + 1, store.end());
                    break;
                case objectType::commitType:
                    this->bodyBuffer = new commitBody();
                    this->body = new commit(*((commitBody *) this->bodyBuffer), spliter + 1, store.end());
                    break;
            }
        }
    }
}