#ifndef _GIT_FSI_PACK_
#define _GIT_FSI_PACK_

#include "sign.h"
#include "object.h"
#include <cstdint>
#include <unistd.h>
#include <string>
#include <map>
#include <fstream>
#include <vector>

namespace gitter_kid {
namespace fsi {

struct __pack_idx_s {
    int nth;
    size_t off;
    size_t len;
    sign_t sign;
};

struct __pack_segment_s {
    std::basic_string<byte> buf;
    uint8_t type;
    size_t item_len;
    size_t off;
};

struct __pack_item_s {
    std::basic_string<byte> buf;
    uint8_t type;
    sign_t sign;
    size_t negative_off;
    size_t off;
    size_t origin_len;
};

class pack {
private:
    std::string _pack_path;
    std::string _idx_path;

    std::vector<__pack_idx_s> _indexes;
    std::map<sign_t, __pack_idx_s> _sign_indexes;

    uint32_t __count(std::ifstream &idx_file);
    void __sorted_indexes(std::ifstream &idx_file,
                     uint32_t items_count,
                     size_t pack_size);
    void __build_rdtree_indexes();
    size_t __pack_size() const;
    std::basic_string<byte> __delta_patch(std::basic_string<byte> &base,
                                          __pack_item_s delta);
    size_t __indexes_findlen(pack &_pack, size_t off);
    __pack_item_s __refdelta_patch(std::vector<pack> &pack_collection,
                                   const __pack_item_s &packitem);
    __pack_item_s __ofsdelta_patch(std::vector<pack> &pack_collection,
                                   pack &_pack,
                                   const __pack_item_s &packitem);
    __pack_segment_s __get_segment(std::ifstream &pack_file, size_t off, size_t len);
    __pack_item_s __get_item(__pack_segment_s &seg);
    object __get(std::vector<pack> &repo, const __pack_idx_s &index);
public:
    pack(std::string pack_path, std::string sign);

    void idx_init();

    std::string &pack_path();
    std::string &idx_path();

    std::map<sign_t, __pack_idx_s> &sign_index();
    std::vector<__pack_idx_s> &off_index();

    object get(std::vector<pack> &pack_collection, sign_t sign);
};

}
}

#endif
