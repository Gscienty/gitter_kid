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

    dispose_passwd (db);

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
        } while (group_member_move_next (mem) == 0);

        dispose_group_member (mem);

    } while (move_group_cursor_next (db) == 0);

    dispose_group (db);

    return 0;
}

int test_market_init () {
    struct git_market *market = git_market_build("/home/ant");

    git_market_cursor_reset (market);

    do {
        struct git_repo *repo = git_market_cursor_current (market);
        
        if (strcmp (git_repo_name (repo), "repo") == 0) {
            struct git_obj *obj = git_obj_get (repo, "ce013625030ba8dba906f756967f9e9ca394464a");
        }
    } while (git_market_cursor_move_next (market) == 0);
    return 0;
}

int test_create_account () {
    struct db *db = build_passwd_handle ();
    printf ("open_passwd: %d\n", open_passwd (db));
    printf ("%d\n", create_account (db, "test", "/home/test", "/bin/sh", 1000));
    return 0;
}


int test_branches () {
    struct git_market *market = git_market_build("/home/ant");

    git_market_cursor_reset (market);

    do {
        struct git_repo *repo = git_market_cursor_current (market);
        
        if (strcmp (git_repo_name (repo), "repo") == 0) {
            git_branches_get (repo);
            break;
        }
    } while (git_market_cursor_move_next (market) == 0);
    return 0;
}

int main() {
    // test_enum_passwd ();
    // test_enum_group ();
    // test_user_create_account ();
    // test_market_init ();
    // test_create_account ();
    test_branches ();
    return 0;
}