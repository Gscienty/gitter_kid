#ifndef _GIT_FSI_OBJECT_
#define _GIT_FSI_OBJECT_

#include "define.h"
#include "repository.h"
#include "content.h"
#include <string>
#include <vector>

namespace gitterKid {
    namespace fsi {

        class object {
            private:
                objectType analysisType(std::vector<byte>& store, std::vector<byte>::iterator& spliter);
            protected:
                const repository& repo;
                const std::string signture;
                objectType type;
                void *bodyBuffer; //lazy
                content *body;

                virtual std::vector<byte> getStore() = 0;
            public:
                object(const repository& repo, std::string signture);
                ~object();
                void initialize();
                objectType getType() const;

                template<class T> T get() const { return static_cast<T>(*this->body); }
        };
    }
}

#endif