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
    void*_body_buffer; //lazy
    content *_body;
    obj_type _type;
    obj_type analysis_type(std::basic_string<byte>& store,
                  std::basic_string<byte>::iterator& spliter);
public:
    object(std::basic_string<byte> &buffer);
    virtual ~object();

    obj_type type() const;
    template<class T> T &get() { return *dynamic_cast<T *>(this->_body); }
};

}
}

#endif
