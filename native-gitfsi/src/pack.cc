#include "pack.h"
#include <sstream>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace gitter_kid {
namespace fsi {

pack::pack(repository &repo, std::string sign) {
    std::stringstream path_builder;

    // build index file (.idx) path
    this->_idx_path.resize(repo.path().size() + 63, 0);
    path_builder.rdbuf()->pubsetbuf(const_cast<char *>(this->_idx_path.data()),
                                    this->_idx_path.size());

    path_builder.write(repo.path().data(), repo.path().size());
    path_builder.write("/objects/pack/pack-", 19);
    path_builder.write(sign.data(), 40);
    path_builder.write(".idx", 4);

    // build pack file (.pack) path
    this->_pack_path.resize(repo.path().size() + 64, 0);
    path_builder.rdbuf()->pubsetbuf(const_cast<char *>(this->_pack_path.data()),
                                    this->_pack_path.size());
    path_builder.write(repo.path().data(), repo.path().size());
    path_builder.write("/objects/pack/pack-", 19);
    path_builder.write(sign.data(), 40);
    path_builder.write(".pack", 5);
}

std::string &pack::idx_path() {
    return this->_idx_path;
}

std::string &pack::pack_path() {
    return this->_pack_path;
}

uint32_t pack::__count(std::ifstream &idx_file) {
    if (!idx_file.is_open()) {
        return 0;
    }

    uint32_t result = 0;
    // set off
    idx_file.seekg(8, std::ios::beg);

    for (int i = 0; i < 256; i++) {
        uint32_t n;
        idx_file.read(reinterpret_cast<char *>(&n), 4);
        result = std::max(result, ntohl(n));
    }

    return result;
}

inline uint32_t __inl_nth_off(std::ifstream &idx_file,
                          uint32_t items_count,
                          uint32_t nth) {
    idx_file.seekg(8 + 1024 + 24 * items_count + 4 * nth, std::ios::beg);
    uint32_t result;
    idx_file.read(reinterpret_cast<char *>(&result), 4);

    return ntohl(result);
}

inline void __inl_nth_sign(std::ifstream &idx_file,
                           uint32_t nth,
                           sign_t &sign) {
    idx_file.seekg(8 + 1024 + 20 * nth, std::ios::beg);
    std::basic_string<byte> bytes_sign(20, 0);
    idx_file.read(reinterpret_cast<char *>(const_cast<byte *>(bytes_sign.data())), 20);

    sign.bytes_assign(bytes_sign.begin(), bytes_sign.end());
}

void pack::__sorted_indexes(std::ifstream &idx_file,
                       uint32_t items_count,
                       size_t pack_size) {

    this->_indexes.resize(items_count);
    for (uint32_t i = 0; i < items_count; i++) {
        this->_indexes[i].nth = i;
        this->_indexes[i].off = __inl_nth_off(idx_file, items_count, i);
        this->_indexes[i].len = 0;
        __inl_nth_sign(idx_file, i, this->_indexes[i].sign);
    }

    std::sort(this->_indexes.begin(),
              this->_indexes.end(),
              [] (const __pack_idx_s &a, const __pack_idx_s &b) -> bool {

                return a.off < b.off;

              });

    this->_indexes.back().len = pack_size - 20 - this->_indexes.back().off;
    for (uint32_t i = 0; i < items_count - 1; i++) {
        this->_indexes[i].len = this->_indexes[i + 1].off - this->_indexes[i].off;
    }
}

void pack::__build_rdtree_indexes() {
    std::for_each(this->_indexes.begin(),
                  this->_indexes.end(),
                  [this] (const __pack_idx_s &idx) -> void {

                    this->_sign_indexes.insert(std::make_pair(idx.sign, idx));

                  });
}

size_t pack::__pack_size() const {
    struct stat st;
    if (stat(this->_pack_path.data(), &st) != 0) {
        return 0;
    }
    return st.st_size;
}

void pack::idx_init() {
    // read idx file
    std::ifstream idx_file(this->_idx_path, std::ios::binary);
    // calculate items count
    uint32_t items_count = this->__count(idx_file);
    // build sorted vector & rd-tree (pack's index)
    this->__sorted_indexes(idx_file, items_count, this->__pack_size());
    this->__build_rdtree_indexes();
}

std::map<sign_t, __pack_idx_s> &pack::sign_index() {
    return this->_sign_indexes;
}

std::vector<__pack_idx_s> &pack::off_index() {
    return this->_indexes;
}

}
}
