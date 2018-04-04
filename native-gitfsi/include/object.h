#ifndef _GIT_FSI_OBJECT_
#define _GIT_FSI_OBJECT_

#include "define.h"
#include "repository.h"
#include <string>
#include <vector>

namespace gitterKid {
    namespace fsi {

        enum objectType { unknow, blob, commit, tree, tag, ofsDelta, refDelta };

        class object {
            private:
                void analysisType(std::vector<byte>::iterator& spliter);
            protected:
                const repository& repo;
                const std::string signture;
                std::vector<byte> content;
                objectType type;

                virtual void fillContent() = 0;
            public:
                object(const repository& repo, std::string signture);
                void initialize();
                objectType getType() const;
        };
    }
}

#endif