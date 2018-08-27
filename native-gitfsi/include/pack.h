#ifndef _GIT_FSI_PACK_
#define _GIT_FSI_PACK_

#include "repository.h"
#include "sign.h"
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
public:
    pack(repository &repo, std::string sign);

    void idx_init();

    std::string &pack_path();
    std::string &idx_path();

    std::map<sign_t, __pack_idx_s> &sign_index();
    std::vector<__pack_idx_s> &off_index();
};

}
}

#endif
