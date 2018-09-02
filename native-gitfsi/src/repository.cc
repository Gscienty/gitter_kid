#include "repository.h"
#include "pack.h"
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <algorithm>
#include <string.h>

namespace gitter_kid {
namespace fsi {

repository::repository(std::string path)
    : _path(path) {}

const std::string &repository::path() {
    return this->_path;
}

void repository::initialize_packs() {
    std::stringstream path_builder;
    std::string packs_path(this->_path.size() + 14, 0);
    path_builder.rdbuf()->pubsetbuf(const_cast<char *>(packs_path.data()),
                                    packs_path.size());

    path_builder.write(this->_path.data(), this->_path.size());
    path_builder.write("/objects/pack/", 14);

    DIR *dir = opendir(packs_path.data());
    dirent *ent;

    while ((ent = readdir(dir))) {
        if (ent->d_type != DT_DIR) {
            if (strcmp(ent->d_name + 45, ".idx") != 0) { continue; }
            std::string pack_sign(ent->d_name + 5, ent->d_name + 45);

            this->_packs.push_back(pack(this->_path, pack_sign));
            this->_packs.back().idx_init();
        }
    }

    closedir(dir);
    free(ent);
}

std::string repository::looseobj_path(sign_t sign) {
    std::stringstream obj_path_builder;

    std::string path(this->_path.size() +
                     1 + 7 + 1 + 2 + 1 + 38, 0);
    obj_path_builder.rdbuf()
        ->pubsetbuf(const_cast<char *>(path.data()),
                    path.size());

    obj_path_builder.write(this->_path.data(),
                           this->_path.size());
    obj_path_builder.write("/objects/", 9);
    obj_path_builder.write(sign.str().data(), 2);
    obj_path_builder.put('/');
    obj_path_builder.write(sign.str().data() + 2,
                           38);

    return path;
}

object repository::get(sign_t sign) {
    // determine whether the sign corresponds to loose object
    std::string may_looseobj_path = this->looseobj_path(sign);

    if (access(may_looseobj_path.c_str(), F_OK) != -1) {
        std::ifstream loose_file(may_looseobj_path, std::ios::binary);
        loose_file.seekg(0, std::ios::end);
        std::basic_string<byte> file_content(loose_file.tellg(), 0);
        loose_file.seekg(0, std::ios::beg);

        loose_file.read(reinterpret_cast<char *>(const_cast<unsigned char *>(file_content.data())),
                        file_content.size());

        return object(file_content);
    }

    std::vector<pack>::iterator findpack_ret =
        std::find_if(this->_packs.begin(), this->_packs.end(),
                     [&] (pack &_pack) -> bool { 
                     return _pack.sign_index().find(sign) != _pack.sign_index().end();
                     });

    if (findpack_ret == this->_packs.end()) {
        return object();
    }

    return (*findpack_ret).get(this->_packs, sign);
}

}
}
