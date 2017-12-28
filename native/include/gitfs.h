#ifndef _G_KID_REPOSITORY_
#define _G_KID_REPOSITORY_

#include "define.h"

int __access_file_readable (const char* filepath);
int __access_file_writable (const char* filepath);
int __access_file_executable (const char* filepath);
int __access_file_exist (const char* filepath);
int __access_chmod (const char* path, unsigned int mode);
int __access_chown (const char* path, unsigned int owner, unsigned int group);

struct git_repo {
    char *path;
    char *name;
    struct git_repo *prev;
    struct git_repo *next;
};

struct git_market {
    char *path;

    struct git_repo *head;
    struct git_repo *tail;
    struct git_repo *cursor;
};

#define USER_READABLE    0400
#define USER_WRITABLE    0200
#define USER_EXECUTABLE  0100

#define GROUP_READABLE      0040
#define GROUP_WRITABLE      0020
#define GROUP_EXECUTABLE    0010

#define OTHER_READABLE      0004
#define OTHER_WRITABLE      0002
#define OTHER_EXECUTABLE    0001

int __repository_init (const char* path, const char* description);
int __repository_remove (const char* path);

G_KID_EXTERN struct git_market *git_market_build (const char *basepath);
G_KID_EXTERN void git_market_dispose (struct git_market *market);
G_KID_EXTERN int git_market_cursor_move_next (struct git_market *market);
G_KID_EXTERN struct git_repo *git_market_cursor_current (const struct git_market *market);
G_KID_EXTERN void git_market_cursor_reset (struct git_market *market);
G_KID_EXTERN char *git_repo_path (struct git_repo *repo);
G_KID_EXTERN char *git_repo_name (struct git_repo *repo);

enum git_obj_type {
    GIT_OBJ_TYPE_UNKNOW,
    GIT_OBJ_TYPE_BLOB,
    GIT_OBJ_TYPE_COMMIT,
    GIT_OBJ_TYPE_TREE
};

struct git_obj {
    void *buf;
    char *path;
    char *sign;
    enum git_obj_type type;
    int size;
    char *body;
    void *ptr;
};

struct git_obj_blob {
    void *content;
    int length;
};

struct git_person_log {
    char *name;
    char *mail;
    unsigned long timestamp;
    char *timezone;
};

struct git_obj_commit_parent {
    char *sign;
    struct git_obj_commit_parent *prev;
    struct git_obj_commit_parent *next;
};

struct git_obj_commit {
    struct git_obj_commit_parent *parent_head;
    struct git_obj_commit_parent *parent_tail;
    struct git_obj_commit_parent *parent_cursor;

    char *tree_sign;
    char *message;
    struct git_person_log *author;
    struct git_person_log *committer;
};

struct git_obj_tree_item {
    char *sign;
    char *name;
    int type;

    struct git_obj_tree_item *prev;
    struct git_obj_tree_item *next;
};

struct git_obj_tree {
    struct git_obj_tree_item *head;
    struct git_obj_tree_item *tail;
    struct git_obj_tree_item *cursor;
};

G_KID_EXTERN struct git_obj *git_obj_get (struct git_repo *repo, const char* signture);
G_KID_EXTERN void git_obj_dispose (struct git_repo *repo);
G_KID_EXTERN struct git_obj_blob *git_obj_get_blob (struct git_obj *obj);
void __git_obj_blob_dispose (struct git_obj_blob *obj);
G_KID_EXTERN struct git_obj_commit *git_obj_get_commit (struct git_obj *obj);
void __git_obj_commit_dispose (struct git_obj_commit *obj);
G_KID_EXTERN struct git_obj_tree *git_obj_get_tree (struct git_obj *obj);
void __git_obj_tree_dispose (struct git_obj_tree *obj);

G_KID_EXTERN int git_obj_blob_length (struct git_obj_blob *blob_obj);
G_KID_EXTERN void *git_obj_blob_content (struct git_obj_blob *blob_obj);

G_KID_EXTERN char *git_obj_commit_tree_sign (struct git_obj_commit *commit_obj);
G_KID_EXTERN struct git_obj_tree *git_obj_commit_tree (struct git_obj_commit *commit_obj);
G_KID_EXTERN struct git_person_log *git_obj_commit_author (struct git_obj_commit *commit_obj);
G_KID_EXTERN struct git_person_log *git_obj_commit_committer (struct git_obj_commit *commit_obj);
G_KID_EXTERN char *git_obj_commit_message (struct git_obj_commit *commit_obj);
G_KID_EXTERN void git_obj_commit_parent_reset (struct git_obj_commit *commit_obj);
G_KID_EXTERN int git_obj_commit_parent_move_next (struct git_obj_commit *commit_obj);
G_KID_EXTERN struct git_obj_commit_parent *git_obj_commit_parent_current (struct git_obj_commit *commit_obj);

G_KID_EXTERN char *git_obj_commit_parent_sign (struct git_obj_commit_parent *commit_parent_obj);
G_KID_EXTERN struct git_obj_commit *git_obj_commit_parent_commit (struct git_obj_commit_parent *commit_parent_obj);

G_KID_EXTERN char *git_person_log_name (struct git_person_log *person_log);
G_KID_EXTERN char *git_person_log_mail (struct git_person_log *person_log);
G_KID_EXTERN unsigned long git_person_log_timestamp (struct git_person_log *person_log);
G_KID_EXTERN char *git_person_timezone (struct git_person_log *person_log);

G_KID_EXTERN void git_obj_tree_reset (struct git_obj_tree *tree_obj);
G_KID_EXTERN int git_obj_tree_move_next (struct git_obj_tree *tree_obj);
G_KID_EXTERN struct git_obj_tree_item *git_obj_tree_current (struct git_obj_tree *tree_obj);

G_KID_EXTERN char *git_obj_tree_item_sign (struct git_obj_tree_item *tree_item_obj);
G_KID_EXTERN char *git_obj_tree_item_name (struct git_obj_tree_item *tree_item_obj);
G_KID_EXTERN int git_obj_tree_item_type (struct git_obj_tree_item *tree_item_obj);
#endif