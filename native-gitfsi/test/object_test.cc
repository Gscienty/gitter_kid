#include "object.h"
#include "gtest/gtest.h"

namespace gitterKid {
    namespace fsi {
        class mockObject : public object {
            private:
                size_t len;
                byte *bytes;
            protected:
                void fillContent() {
                    this->content.assign(bytes, bytes + len);
                };

            public:
                mockObject(byte *content, size_t len, const repository& repo, std::string signture)
                    : object(repo, signture), bytes(content), len(len) { }
        };

        TEST(gitterKid_fsi, type_test) {
            repository repo("", "");
            mockObject blobObj((byte *) "blob 0", 7, repo, std::string(""));
            blobObj.initialize();
            EXPECT_EQ(blobObj.getType(), objectType::blob);

            mockObject commitObj((byte *) "commit 0", 9, repo, std::string(""));
            commitObj.initialize();
            EXPECT_EQ(commitObj.getType(), objectType::commit);

            mockObject treeObj((byte *) "tree 0", 7, repo, std::string(""));
            treeObj.initialize();
            EXPECT_EQ(treeObj.getType(), objectType::tree);
        }
    }
}

int main() {
    int ret = RUN_ALL_TESTS();

    return 0;
}