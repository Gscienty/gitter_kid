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

int test_pack () {
    struct gitmarket *market = get_gitmarket ("/home/ant");
    gitmarket_reset (market);

    while (gitmarket_hasnext (market)) {
        struct gitrepo *repo = gitmarket_next (market);
        if (strcmp (gitrepo_get_name (repo), "gitterRepo") == 0) {
            gitrepo_get_gitobj (repo, "997cc5e85aff5cc35e317cd993f9979c858b17fa");
            gitrepo_get_gitobj (repo, "3ff1e8785454b32f7dcadf3335bbd786ffa46d9c");
            gitrepo_get_gitobj (repo, "c49860eeec92d4477a57a7c58135a9b69b360f32");
            gitrepo_get_gitobj (repo, "582a103bd9ed07676ebda0020ffe8ce97c0159e3");
            gitrepo_get_gitobj (repo, "1dbadf22f5d1f0b534071dfbc16d47dda1941be1");
            gitrepo_get_gitobj (repo, "f516d431df61f028aaa377c72bd42facc65ed1dc");
            gitrepo_get_gitobj (repo, "7e9c8b75c9d5157b4d8bcc3e90ad8ea53a943863");
            gitrepo_get_gitobj (repo, "7e9c8b75c9d5157b4d8bcc3e90ad8ea53a943863");
            gitrepo_get_gitobj (repo, "cae768c7cec368a47f82ae09f6579ca274a57d73");
            gitrepo_get_gitobj (repo, "ba5897fa37124e65d71424036163cc1f70b959bd");
            gitrepo_get_gitobj (repo, "1a8c597789f3376f72d59e726b45873fb982afad");
            gitrepo_get_gitobj (repo, "d3043090193f43eb5330b83f17c3e667ce9a3a40");
            gitrepo_get_gitobj (repo, "7b55b69c2a5b77e36b10377bc5d313e1091ea099");
            gitrepo_get_gitobj (repo, "cf395c1d48a5832d1a62041863d05e05c2045681");
            gitrepo_get_gitobj (repo, "3edb19b739762ea5fb1fa677e68e0b045e5748c2");
        }
    }
    return 0;
}

int main() {
    // test_enum_passwd ();
    // test_enum_group ();
    // test_user_create_account ();
    // test_market_init ();
    // test_create_account ();
    // test_branches ();
    // test_commits_parent ();
    test_pack ();
    return 0;
}