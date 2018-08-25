#include "inflate.h"

namespace gitter_kid {
namespace fsi {

std::basic_string<byte> __inflate(std::basic_string<byte> &deflate_bytes,
                                  size_t inflate_bytes_len) {
    std::basic_string<byte> inflate_bytes(inflate_bytes_len, byte(0));

    z_stream inflated_stream;
    inflated_stream.zalloc = nullptr;
    inflated_stream.zfree = nullptr;
    inflated_stream.opaque = nullptr;
    inflated_stream.avail_in = deflate_bytes.size();
    inflated_stream.next_in = const_cast<byte *>(deflate_bytes.data());

    int retval = inflateInit(&inflated_stream);
    if (retval != Z_OK) {
        return std::basic_string<byte>();
    }

    std::basic_string<byte> result;

    do {
        size_t capacity_size = inflate_bytes.capacity();
        inflated_stream.avail_out = capacity_size;
        inflated_stream.next_out = const_cast<byte *>(inflate_bytes.data());
        retval = inflate(&inflated_stream, Z_NO_FLUSH);

        switch (retval) {
        case Z_NEED_DICT:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&inflated_stream);
            return result;
        }

        if (capacity_size < inflated_stream.avail_out) {
            break;
        }

        size_t inflated_size = capacity_size - inflated_stream.avail_out;
        result.insert(result.end(),
                      inflate_bytes.begin(),
                      inflate_bytes.begin() + inflated_size);

    } while (inflated_stream.avail_out == 0);
    inflateEnd(&inflated_stream);

    return result;
}

}
}
