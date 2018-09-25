#include "repository.h"
#include "pack.h"
#include "tree.h"
#include <algorithm>
#include <vector>
#include <iostream>

int main() {
    using namespace gitter_kid::fsi;
    repository repo("/root/gitter_kid/.git");
    repo.initialize_packs();

    object obj = repo.get(sign_t("7c077eb4df6e3a9bdcdab9b2aaa2d460e81a32ba"));


    std::cout << obj.type() << std::endl;
    /* pack p(repo.path(), "1d434796cf17b47275df59b817f504f428a22d0e"); */
    /* p.idx_init(); */

    /* std::vector<pack> packes; */
    /* packes.push_back(p); */

    /* object obj = p.get(packes, sign_t("7c077eb4df6e3a9bdcdab9b2aaa2d460e81a32ba")); */
    
    std::for_each(obj.get<tree>().items().begin(),
                  obj.get<tree>().items().end(),
                  [] (tree_item &item) -> void {
                        std::cout << item.name() << ' ' << item.sign().str() << ' ' << item.type() << std::endl;
                  });

    return 0;
 }
