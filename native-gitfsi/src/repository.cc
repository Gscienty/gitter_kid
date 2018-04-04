#include "repository.h"

namespace gitterKid {
    namespace fsi {
        repository::repository(std::string name, std::string path)
            : name(name), path(path) { }

        const std::string repository::getPath() const { return this->path; }
        const std::string repository::getName() const { return this->name; }
    }
}