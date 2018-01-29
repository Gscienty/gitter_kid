#include "user.h"
#include "gitfs.h"
#include <stdio.h>
#include <string.h>

int test_enum_passwd () {
    struct db *db = build_passwd_handle ();
    open_passwd (db);

    reset_passwd_cursor (db);

    do {
        struct passwd *pw = get_current_passwd (db);
        printf ("%d:%s:%d:%s:%s:%s\n",
            get_passwd_uid (pw),
            get_passwd_username (pw),
            get_passwd_gid (pw),
            get_passwd_gecos (pw),
            get_passwd_dir (pw),
            get_passwd_shell (pw)
        );
    } while (move_passwd_cursor_next (db) == 0);

    dtor_passwd (db);

    return 0;
}

int test_enum_group () {
    struct db *db = build_group_handle ();
    open_group (db);

    reset_group_cursor (db);
    do {
        struct group *grp = get_current_group (db);
        printf ("%s:%s:%d\n",
            get_group_name (grp),
            get_group_passwd (grp),
            get_group_gid (grp)
        );
        
        struct group_member *mem = get_group_member_cursor (grp);
        reset_group_member_cursor (mem);
        do {
            printf ("\t\t%s\t%d\n", get_current_group_member_name (mem), get_group_member_count (mem));
        } while (group_member_movenext (mem) == 0);

        dtor_group_member (mem);

    } while (move_group_cursor_next (db) == 0);

    dtor_group (db);

    return 0;
}

int test_market_init () {
    struct gitmarket *market = get_gitmarket("/home/ant");

    gitmarket_reset (market);

    while (gitmarket_hasnext (market)) {
        struct gitrepo *repo = gitmarket_next (market);
        
        if (strcmp (gitrepo_get_name (repo), "repo") == 0) {
            struct gitobj *obj = gitrepo_get_gitobj (repo, "ce013625030ba8dba906f756967f9e9ca394464a");
        }
    }
    return 0;
}

int test_create_account () {
    struct db *db = build_passwd_handle ();
    printf ("open_passwd: %d\n", open_passwd (db));
    printf ("%d\n", create_account (db, "test", "/home/test", "/bin/sh", 1000));
    return 0;
}


int test_branches () {
    struct gitmarket *market = get_gitmarket ("/home/ant");

    gitmarket_reset (market);

    while (gitmarket_hasnext (market)) {
        struct gitrepo *repo = gitmarket_next (market);
        
        if (strcmp (gitrepo_get_name (repo), "gitterRepo") == 0) {
            struct gitbranches *branches = gitrepo_get_branches (repo);
            gitbranches_reset (branches);

            while (gitbranches_hasnext (branches)) {
                struct gitbranch *branch = gitbranches_next (branches);
                printf ("%s\t%s\n", 
                    gitbranch_get_name (branch),
                    gitbranch_get_lastcommit_sign (branch)
                );
            }
            break;
        }
    }
    return 0;
}

int test_commits_parent () {
    //659cba9dc3f15adc2ba51f0fb66b92ef7127186d
    struct gitmarket *market = get_gitmarket ("/home/ant");

    gitmarket_reset (market);

    while (gitmarket_hasnext (market)) {
        struct gitrepo *repo = gitmarket_next (market);
        
        if (strcmp (gitrepo_get_name (repo), "gitterRepo") == 0) {
            struct gitobj *obj = gitrepo_get_gitobj (repo, "54415a12532e7e77dfc4037962e870abe72e0300");
            struct gitobj_commit *commit = gitobj_get_commit (obj);

            gitobj_commitparents_reset (commit);
            while (gitobj_commitparents_hasnext (commit)) {
                struct gitobj_commitpatent *parent = gitobj_commitparents_next (commit);
                printf ("%s\n", gitobj_commitpatent_get_sign (parent));
            }

            break;
        }
    }
    return 0;
}

struct gitrepo *__test_pack_getgitrepo(struct gitmarket *market) {
    gitmarket_reset (market);

    while (gitmarket_hasnext (market)) {
        struct gitrepo *repo = gitmarket_next (market);
        if (strcmp (gitrepo_get_name (repo), "gitterRepo") == 0) {
            return repo;
        }
    }

    return NULL;
}

int test_pack () {
    int i = 0;
    for (i = 0; i < 65536; i++) {
        struct gitmarket *market = get_gitmarket ("/home/ant");
        if (market == NULL) return 0;

        struct gitrepo *repo = __test_pack_getgitrepo (market);
        struct gitobj *obj = gitrepo_get_gitobj (repo, "e02af234195f9bc4219aa705d9e7625cd0b54283");
        printf ("%d\n", (int) obj);
        gitobj_dtor (obj);
        obj = gitrepo_get_gitobj (repo, "3ff1e8785454b32f7dcadf3335bbd786ffa46d9c");
        printf ("%d\n", (int) obj);

        gitmarket_dtor (market);
    }
    return 0;
}

void test_tree () {

    int i;
    for (i = 0; i < 65536; i++) {
        struct gitmarket *market = get_gitmarket ("/home/ant");
        gitmarket_reset (market);
        while (gitmarket_hasnext (market)) {
            struct gitrepo *repo = gitmarket_next (market);

            if (strcmp (gitrepo_get_name (repo), "gitterRepo") == 0) {
                struct gitbranches *branches = gitrepo_get_branches(repo);
                gitbranches_reset (branches);
                while (gitbranches_hasnext (branches)) {
                    struct gitbranch *branch = gitbranches_next (branches);

                    struct gitobj *commit_obj = gitrepo_get_gitobj (repo, gitbranch_get_lastcommit_sign (branch));
                    struct gitobj *tree_obj = gitrepo_get_gitobj (repo, gitobj_commit_get_treesign (gitobj_get_commit (commit_obj)));

                    struct gitobj_tree *trees = gitobj_get_tree (tree_obj);

                    gitobj_tree_reset (trees);
                    while (gitobj_tree_hasnext (trees)) {
                        struct gitobj_treeitem *item = gitobj_tree_next (trees);

                        if (strcmp (gitobj_treeitem_get_name (item), "front") == 0) {
                            struct gitobj *front_trees_obj = gitrepo_get_gitobj (repo, gitobj_treeitem_get_sign (item));

                            gitobj_tree_reset (gitobj_get_tree (front_trees_obj));
                            while (gitobj_tree_hasnext (gitobj_get_tree (front_trees_obj))) {
                                struct gitobj_treeitem *front_item = gitobj_tree_next (gitobj_get_tree (front_trees_obj));

                                if (strcmp (gitobj_treeitem_get_name (front_item), "scripts") == 0) {
                                    struct gitobj *scripts_trees_obj = gitrepo_get_gitobj (repo, gitobj_treeitem_get_sign (front_item));

                                    gitobj_tree_reset (gitobj_get_tree (scripts_trees_obj));
                                    while (gitobj_tree_hasnext (gitobj_get_tree (scripts_trees_obj))) {
                                        struct gitobj_treeitem *scripts_item = gitobj_tree_next (gitobj_get_tree (scripts_trees_obj));

                                        printf ("%s, %s\n", gitobj_treeitem_get_name (scripts_item), gitobj_treeitem_get_sign (scripts_item));
                                    }
                                    gitobj_dtor (scripts_trees_obj);
                                }
                            }

                            gitobj_dtor (front_trees_obj);
                        }
                    }

                    gitobj_dtor (commit_obj);
                    gitobj_dtor (tree_obj);
                }
                gitbranches_dtor (branches);
            }
        }

        gitmarket_dtor (market);
    }
}

int main() {
    // test_enum_passwd ();
    // test_enum_group ();
    // test_user_create_account ();
    // test_market_init ();
    // test_create_account ();
    // test_branches ();
    // test_commits_parent ();
    // test_pack ();
    test_tree ();
    return 0;
}