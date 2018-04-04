#include "object.h"
#include "blob.h"
#include "gtest/gtest.h"
#include <string>
#include <iostream>

namespace gitterKid {
    namespace fsi {
        class mockObject : public object {
            private:
                size_t len;
                byte *bytes;
            protected:
                std::vector<byte> getStore() {
                    std::vector<byte> ret;
                    ret.assign(bytes, bytes + len);
                    return ret;
                };

            public:
                mockObject(byte *content, size_t len, const repository& repo, std::string signture)
                    : object(repo, signture), bytes(content), len(len) { }
        };

        TEST(gitterKid_fsi_object, type_test) {
            repository repo("", "");
            mockObject blobObj((byte *) "blob 0", 7, repo, std::string(""));
            blobObj.initialize();
            EXPECT_EQ(blobObj.getType(), objectType::blobType);

            mockObject commitObj((byte *) "commit 0", 9, repo, std::string(""));
            commitObj.initialize();
            EXPECT_EQ(commitObj.getType(), objectType::commitType);

            mockObject treeObj((byte *) "tree 0", 7, repo, std::string(""));
            treeObj.initialize();
            EXPECT_EQ(treeObj.getType(), objectType::treeType);
        }

        TEST(gitterKid_fsi_object, blob_test) {
            repository repo("", "");
            byte blobContent[] = {
                'b', 'l', 'o', 'b', ' ', '1', '6', 0,
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
            };

            mockObject blobObj((byte *) blobContent, 24, repo, std::string(""));
            blobObj.initialize();

            EXPECT_EQ(16, blobObj.get<blob>().get().size());
            std::string content;
            content.insert(content.begin(), blobObj.get<blob>().get().begin(), blobObj.get<blob>().get().end());
            EXPECT_EQ(0, std::string("0123456789abcdef").compare(content));
        }

        TEST(gitterKid_fsi_object, tree_test) {
            repository repo("", "");
            byte treeContent[] = {
                't', 'r', 'e', 'e', ' ', '1', '5', '3', 0,
                '1', '0', '0', '0', '0', ' ', 'f', 'i', 'l', 'e', '1',
                0,
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'j',
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',

                '1', '0', '0', '0', '1', ' ', 'f', 'i', 'l', 'e', '2',
                0,
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'j',
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',

                '1', '0', '0', '0', '2', ' ', 'f', 'i', 'l', 'e', '3',
                0,
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'j',
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
            };


            mockObject treeObj((byte *) treeContent, 105, repo, std::string(""));
            treeObj.initialize();

        }
    }
}

int main() {
    int ret = RUN_ALL_TESTS();

    return 0;
}