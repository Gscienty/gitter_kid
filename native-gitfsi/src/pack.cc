#include "pack.h"
#include "inflate.h"
#include "commit.h"
#include <sstream>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace gitter_kid {
namespace fsi {

pack::pack(std::string repo_path, std::string sign) {
    std::stringstream path_builder;

    // build index file (.idx) path
    this->_idx_path.resize(repo_path.size() + 63, 0);
    path_builder.rdbuf()->pubsetbuf(const_cast<char *>(this->_idx_path.data()),
                                    this->_idx_path.size());

    path_builder.write(repo_path.data(), repo_path.size());
    path_builder.write("/objects/pack/pack-", 19);
    path_builder.write(sign.data(), 40);
    path_builder.write(".idx", 4);

    // build pack file (.pack) path
    this->_pack_path.resize(repo_path.size() + 64, 0);
    path_builder.rdbuf()->pubsetbuf(const_cast<char *>(this->_pack_path.data()),
                                    this->_pack_path.size());
    path_builder.write(repo_path.data(), repo_path.size());
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

/**
 * Get packet's items count
 * Args:
 *      std::ifstream &idx_file: index file stream (input)
 * Returns:
 *      items count
 */
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

/**
 * Get packet's nth item offset
 * Args:
 *      std::ifstream &idx_file: index file stream (input)
 *      uint32_t items_count: items count
 *      uint32_t nth: nth item
 */
inline uint32_t __inl_nth_off(std::ifstream &idx_file,
                          uint32_t items_count,
                          uint32_t nth) {
    idx_file.seekg(8 + 1024 + 24 * items_count + 4 * nth, std::ios::beg);
    uint32_t result;
    idx_file.read(reinterpret_cast<char *>(&result), 4);

    return ntohl(result);
}

/**
 * assign nth index to sign from index file
 * Args:
 *      sign_t &sign: assigned sign
 *      uint32_t nth: nth index
 *      std::ifstream &idx_file: pack index file
 */
inline void __inl_assign_nth_sign(sign_t &sign,
                                  uint32_t nth,
                                  std::ifstream &idx_file) {
    idx_file.seekg(8 + 1024 + 20 * nth, std::ios::beg);
    std::basic_string<byte> bytes_sign(20, 0);
    idx_file.read(reinterpret_cast<char *>(const_cast<byte *>(bytes_sign.data())), 20);

    sign.bytes_assign(bytes_sign.begin(), bytes_sign.end());
}

/**
 * sort pack's indexes
 * Args:
 *      std::ifstream &idx_file: index file stream (input)
 *      uint32_t items_count: items count
 *      size_t pack_size: pack file's size
 */
void pack::__sorted_indexes(std::ifstream &idx_file,
                       uint32_t items_count,
                       size_t pack_size) {

    this->_indexes.resize(items_count);
    for (uint32_t i = 0; i < items_count; i++) {
        this->_indexes[i].nth = i;
        this->_indexes[i].off = __inl_nth_off(idx_file, items_count, i);
        this->_indexes[i].len = 0;
        __inl_assign_nth_sign(this->_indexes[i].sign, i, idx_file);
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

/**
 * build index tree (reference sign)
 */
void pack::__build_rdtree_indexes() {
    std::for_each(this->_indexes.begin(),
                  this->_indexes.end(),
                  [this] (const __pack_idx_s &idx) -> void {

                    this->_sign_indexes.insert(std::make_pair(idx.sign, idx));

                  });
}

/**
 * get pack's size
 * Result:
 *      size_t pack's size
 */
size_t pack::__pack_size() const {
    struct stat st;
    if (stat(this->_pack_path.data(), &st) != 0) {
        return 0;
    }
    return st.st_size;
}

/**
 * initialize this pack's index
 */
void pack::idx_init() {
    // read idx file
    std::ifstream idx_file(this->_idx_path, std::ios::binary);
    // calculate items count
    uint32_t items_count = this->__count(idx_file);
    // build sorted vector & rd-tree (pack's index)
    this->__sorted_indexes(idx_file, items_count, this->__pack_size());
    this->__build_rdtree_indexes();
}

/**
 * getter/setter sign index (red/black tree)
 */
std::map<sign_t, __pack_idx_s> &pack::sign_index() {
    return this->_sign_indexes;
}


/**
 * getter/setter offset index (vector)
 */
std::vector<__pack_idx_s> &pack::off_index() {
    return this->_indexes;
}

__pack_segment_s pack::__get_segment(std::ifstream &pack_file, size_t off, size_t len) {
    pack_file.seekg(off, std::ios::beg);

    uint8_t p_byte = pack_file.get();
    uint8_t type = (p_byte >> 4) & 0x07;

    uint32_t size = p_byte & 0x0F;
    uint32_t shift = 4;
    while (p_byte & 0x80) {
        p_byte = pack_file.get();
        size += (uint32_t(p_byte & 0x7F)) << shift;
        shift += 7;
    }

    if (len == 0) {
        len = size * 3;
    }

    std::basic_string<byte> buf(len, 0);
    pack_file.read(reinterpret_cast<char *>(const_cast<byte *>(buf.data())), buf.size());

    return { buf, type, size, off };
}


size_t pack::__indexes_findlen(pack &_pack, size_t off) {
    int i = 0;
    int j = _pack._indexes.size();
    while (i < j) {
        int mid = (i + j) / 2;
        if (off < _pack._indexes[mid].off) {
            j = mid - 1;
        }
        else if (off > _pack._indexes[mid].off) {
            i = mid + 1;
        }
        else {
            return _indexes[mid].len;
        }
    }
    return 0;
}


__pack_item_s pack::__ofsdelta_patch(std::vector<pack> &pack_collection,
                                    pack &_pack,
                                    const __pack_item_s &packitem) {
    std::ifstream packfile(_pack.pack_path());

    __pack_segment_s base_segment =
        this->__get_segment(packfile,
                            packitem.off - packitem.negative_off,
                            this->__indexes_findlen(_pack,
                                                    packitem.off - packitem.negative_off));

    if (base_segment.buf.empty()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }

    __pack_item_s base_packitem = this->__get_item(base_segment);
    if (base_packitem.buf.empty()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }

    if (base_packitem.type == 6) {
        base_packitem = this->__ofsdelta_patch(pack_collection, _pack, base_packitem);
    }
    else if (base_packitem.type == 7) {
        base_packitem = this->__refdelta_patch(pack_collection, base_packitem);
    }

    if (base_packitem.buf.empty()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }

    std::basic_string<byte> patched_buf = this->__delta_patch(base_packitem.buf, packitem);

    return { patched_buf, base_packitem.type, sign_t(), 0, 0, patched_buf.size() };
}


__pack_item_s pack::__refdelta_patch(std::vector<pack> &pack_collection,
                                     const __pack_item_s &packitem) {
    std::vector<pack>::iterator pack_itr = pack_collection.begin();
    std::map<sign_t, __pack_idx_s>::iterator find_result = this->_sign_indexes.end();

    for (; pack_itr != pack_collection.end(); pack_itr++) {
        find_result = pack_itr->_sign_indexes.find(packitem.sign);
        if (find_result != pack_itr->_sign_indexes.end()) {
            break;
        }
    }

    if (pack_itr == pack_collection.end()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }

    std::ifstream base_packfile(pack_itr->_pack_path);
    __pack_segment_s base_segment = pack_itr->__get_segment(base_packfile,
                                                            find_result->second.off,
                                                            find_result->second.len);
    if (base_segment.buf.empty()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }
    __pack_item_s base_packitem = this->__get_item(base_segment);
    if (base_packitem.buf.empty()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }

    if (base_packitem.type == 6) {
        base_packitem = this->__ofsdelta_patch(pack_collection,
                                               *pack_itr,
                                               base_packitem);
    }
    else if (base_packitem.type == 7) {
       base_packitem = this->__refdelta_patch(pack_collection, base_packitem);
    }

    if (base_packitem.buf.empty()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }

    std::basic_string<byte> patched_buf = this->__delta_patch(base_packitem.buf,
                                                              packitem);
    if (patched_buf.empty()) {
        return { std::basic_string<byte>(), 0, sign_t(), 0, 0, 0 };
    }

    return { patched_buf, base_packitem.type, sign_t(), 0, 0, patched_buf.size() };
}


__pack_item_s pack::__get_item(__pack_segment_s &seg) {
    if (seg.type < 5) {
        std::basic_string<byte> deflate_bytes = __inflate(seg.buf, seg.item_len);
        return { deflate_bytes, seg.type, sign_t(), 0, seg.off, 0 };
    }
    else if (seg.type == 6) { // ofs delta
        auto itr = seg.buf.begin();

        int nbytes = 1;
        size_t negative_off = size_t(*itr & 0x7F);

        while (*itr & 0x80) {
            itr++;
            nbytes++;
            negative_off = (negative_off << 7) | size_t(*itr & 0x7F);
        }
        if (nbytes >= 2) {
            for (int i = 1; i < nbytes; i++) {
                negative_off += (1 << (7 * i));
            }
        }

        std::basic_string<byte> inflate_bytes(itr + 1, seg.buf.end());
        std::basic_string<byte> deflate_bytes = __inflate(inflate_bytes,
                                                          seg.buf.size() - nbytes);
        return { deflate_bytes, seg.type, sign_t(), negative_off, seg.off, seg.item_len };
    }
    else if (seg.type == 7) { // ref delta
        std::string sign(seg.buf.begin(), seg.buf.begin() + 20);
        std::basic_string<byte> inflate_bytes(seg.buf.begin() + 20, seg.buf.end());
        std::basic_string<byte> deflate_bytes = __inflate(inflate_bytes, seg.item_len);

        return { deflate_bytes, seg.type, sign_t(sign), 0, seg.off, seg.item_len };
    }

    return { std::basic_string<byte>(), 0, 0, 0, 0, 0 };
}


std::basic_string<byte> pack::__delta_patch(std::basic_string<byte> &base,
                                            __pack_item_s delta) {
    std::basic_string<byte>::iterator itr = delta.buf.begin();
    while (*itr & 0x80) { itr++; }
    itr++;

    size_t size = 0;

    int i = 0;
    size_t tmp;
    do {
        tmp = *itr++;
        size |= (tmp & 0x7F) << i;
        i += 7;
    } while (tmp & 0x80 && itr != delta.buf.end());

    std::basic_string<byte> ret;
    while (itr != delta.buf.end()) {
        uint8_t cmd = *itr++;
        if (cmd & 0x80) {
            size_t cp_off = 0;
            size_t cp_size = 0;
            if (cmd & 0x01) { cp_off = *itr++; }
            if (cmd & 0x02) { cp_off |= *itr++ << 8; }
            if (cmd & 0x04) { cp_off |= *itr++ << 16; }
            if (cmd & 0x08) { cp_off |= ((unsigned) *itr++ << 24); }
            if (cmd & 0x10) { cp_size = *itr++; }
            if (cmd & 0x20) { cp_size |= *itr++ << 8; }
            if (cmd & 0x40) { cp_size |= *itr++ << 16; }
            if (cp_size == 0) { cp_size = 0x10000; }

            ret.insert(ret.end(), base.begin() + cp_off, base.begin() + cp_off + cp_size);
            size -= cp_size;
        }
        else if (cmd) {
            if (cmd > size) { break; }
            ret.insert(ret.end(), itr, itr + cmd);
            itr += cmd;
            size -= cmd;
        }
        else {
            return std::basic_string<byte>();
        }
    }
    if (itr != delta.buf.end() || size != 0) {
        return std::basic_string<byte>();
    }

    return ret;
}


object pack::__get(std::vector<pack> &pack_collection,
                   const __pack_idx_s &index) {
    std::ifstream pack_file(this->_pack_path);
    if (!pack_file.is_open()) {
        return object();
    }
    __pack_segment_s segment = this->__get_segment(pack_file, index.off, index.len);
    __pack_item_s packitem = this->__get_item(segment);

apply_delta:
    switch (packitem.type) {
    case 0x01:
        return object(packitem.buf, obj_type::obj_type_commit);
    case 0x02:
        return object(packitem.buf, obj_type::obj_type_tree);
    case 0x03:
        return object(packitem.buf, obj_type::obj_type_blob);
    case 0x06:
        packitem = this->__ofsdelta_patch(pack_collection, *this, packitem);
        goto apply_delta;
    case 0x07:
        packitem = this->__refdelta_patch(pack_collection, packitem);
        goto apply_delta;
    default:
        return object();
    }
}

object pack::get(std::vector<pack> &pack_collection, sign_t sign) {
    auto find_result = this->_sign_indexes.find(sign);
    if (find_result == this->_sign_indexes.end()) {
        // return unknow object (not found)
        return object();
    }

    // return packed object
    return this->__get(pack_collection, find_result->second);
}

}
}
