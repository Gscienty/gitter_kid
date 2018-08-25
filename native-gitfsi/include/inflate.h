#ifndef _GIT_FSI_INFLATE_
#define _GIT_FSI_INFLATE_

#include "define.h"
#include "zlib.h"
#include <string>

namespace gitter_kid {
namespace fsi {

std::basic_string<byte> __inflate(std::basic_string<byte> &, size_t inflate_buf_len);

}
}

#endif
