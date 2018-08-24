#ifndef _GIT_FSI_OBJECT_
#define _GIT_FSI_OBJECT_

#include "define.h"
#include "repository.h"
#include "content.h"
#include <string>
#include <vector>

namespace gitter_kid {
namespace fsi {

class object {
    private:
        gitter_kid::fsi::obj_type
        analysis_type(std::basic_string<byte>& store,
                     std::basic_string<byte>::iterator& spliter);
    protected:
        const gitter_kid::fsi::repository& _repo;
        const std::string _sign;
        gitter_kid::fsi::obj_type type;
        void *_body_buffer; //lazy
        gitter_kid::fsi::content *_body;

        virtual std::basic_string<byte> store() = 0;
    public:
        object(const gitter_kid::fsi::repository& repo, std::string signture);
        ~object();
        void initialize();
        gitter_kid::fsi::obj_type type() const;

        template<class T> T get() const { return dynamic_cast<T>(*this->_body); }
};

}
}

#endif
