#include "gtest/gtest.h"
#include "object.h"
#include "blob.h"
#include "inflate.h"
#include <fstream>
#include <cstring>
#include <iomanip>

std::string path;

TEST(object, commit) {
    using namespace gitter_kid::fsi;

    byte content[] = {
        0x78, 0x01, 0x75, 0x8e, 0x4d, 0x0e, 0x82, 0x40, 0x0c, 0x85, 0x5d, 0xcf, 0x29, 0x1a, 0x4c, 0x0c,
        0x2c, 0x74, 0x61, 0x74, 0x83, 0x84, 0xab, 0x4c, 0x98, 0x1f, 0x61, 0x14, 0x67, 0xc8, 0x4c, 0x59,
        0x10, 0xc2, 0xdd, 0x6d, 0x21, 0x26, 0xba, 0xb0, 0x8b, 0xa6, 0xef, 0xa5, 0xef, 0x6b, 0x55, 0x1f,
        0x14, 0x9c, 0xaf, 0x97, 0xdd, 0xde, 0x79, 0xdd, 0x8f, 0xc6, 0x42, 0xa6, 0xc8, 0x3a, 0x75, 0x99,
        0x10, 0xbe, 0x79, 0xd9, 0x34, 0x34, 0xda, 0x42, 0xeb, 0x10, 0x6d, 0x94, 0x4f, 0x67, 0x60, 0xfe,
        0xb2, 0xef, 0xc9, 0x91, 0x16, 0x1c, 0x28, 0x4b, 0xee, 0x79, 0x42, 0x43, 0x53, 0x93, 0x9c, 0x96,
        0x09, 0xa3, 0xf3, 0x6d, 0xa5, 0x26, 0xb4, 0x35, 0x1c, 0x54, 0x30, 0x53, 0x21, 0x80, 0xaa, 0x04,
        0xc9, 0x22, 0x5f, 0x1d, 0x98, 0x61, 0x11, 0x22, 0xa8, 0x87, 0xc4, 0x69, 0xb0, 0xb0, 0x91, 0x78,
        0xcc, 0x0b, 0xd0, 0xc1, 0x27, 0x24, 0x3e, 0x87, 0xa2, 0xc5, 0x31, 0x7a, 0xf8, 0x2c, 0x4a, 0x5e,
        0xbc, 0x09, 0x8a, 0xfe, 0x3f, 0xb8, 0x3d, 0xc5, 0x97, 0x8a, 0x5f, 0x08, 0x76, 0x2e, 0x1d, 0xeb,
        0xf5, 0x89, 0x15, 0xb1, 0x10, 0xe7, 0x0d, 0x19, 0xd5, 0x56, 0xb1
    };
    
    std::basic_string<byte> obj_content(content, content + sizeof(content));

    std::basic_string<byte> deflate_buf = __inflate(obj_content, 4095);

    object obj(deflate_buf);

    EXPECT_EQ(obj_type::obj_type_blob, obj.type());
    EXPECT_EQ(254, obj.get<blob>().body().size());
}

int main(int, char **argv) {
    size_t size = strlen(argv[1]);
    path.assign(argv[1], argv[1] + size);

    return RUN_ALL_TESTS();
}
