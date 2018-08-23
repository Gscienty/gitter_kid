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
    obj_type analysis_type(std::basic_string<byte>& store,
                  std::basic_string<byte>::iterator& spliter);
protected:
    const repository& _repo;
    const std::string _sign;
    obj_type _type;
    void*_body_buffer; //lazy
    content *_body;

    virtual std::basic_string<byte> store() = 0;
public:
    object(const repository& repo, std::string signture);
    ~object();
    void initialize();
    obj_type type() const;

    template<class T> T get() const { return dynamic_cast<T>(*this->_body); }
};

}
}

#endif
