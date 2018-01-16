#ifndef _G_KID_REPOSITORY_
#define _G_KID_REPOSITORY_

#include "define.h"
#include "rbtree.h"

#define HEX(v) ((v) < 10 ? '0' + (v) : (v) - 10 + 'a')

int __access_file_readable (const char* filepath);
int __access_file_writable (const char* filepath);
int __access_file_executable (const char* filepath);
int __access_file_exist (const char* filepath);
int __access_chmod (const char* path, unsigned int mode);
int __access_chown (const char* path, unsigned int owner, unsigned int group);


// 仓库市场结构体
struct git_market {
    char *path;                 //仓库市场路径

    struct git_repo *head;      // 仓库队列头部指针
    struct git_repo *tail;      // 仓库队列尾部指针
    struct git_repo *cursor;    //仓库游标
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


// 通过指定的路径构造仓库市场
// param <basepath>: 指定的路径
// return: 返回仓库市场指针
G_KID_EXTERN struct git_market *git_market_build (const char *basepath);

// 析构仓库市场
// param <market>: 仓库市场指针
G_KID_EXTERN void git_market_dispose (struct git_market *market);

// 将仓库市场的游标移动指向到下一个仓库
// param <market>: 仓库市场指针
// return: 成功移动到下一个仓库时，返回0，否则返回-1
G_KID_EXTERN int git_market_cursor_movenext (struct git_market *market);

// 获取仓库市场的游标当前指向的仓库
// param <market>: 仓库市场指针
// return: 获取当前仓库，如果不存在则返回为NULL
G_KID_EXTERN struct git_repo *git_market_cursor_current (const struct git_market *market);

// 重置仓库市场的游标
// param <market>: 仓库市场指针
G_KID_EXTERN void git_market_cursor_reset (struct git_market *market);

// 获取仓库路径
// param <repo>: 仓库指针
// return: 仓库路径
G_KID_EXTERN char *git_repo_path (struct git_repo *repo);

// 获取仓库名称
// param <repo>: 仓库指针
// return: 仓库名称
G_KID_EXTERN char *git_repo_name (struct git_repo *repo);

struct git_branch {
    char *name;
    char *last_commit_sign;

    struct git_branch *prev;
    struct git_branch *next;
};

struct git_branches {
    struct git_branch *head;
    struct git_branch *tail;
    struct git_branch *cursor;
};

G_KID_EXTERN struct git_branches *git_branches_get (struct git_repo *repo);
G_KID_EXTERN void git_branches_dispose (struct git_branches *branches);
G_KID_EXTERN void git_branches_reset (struct git_branches *branches);
G_KID_EXTERN int git_branches_movenext (struct git_branches *branches);
G_KID_EXTERN struct git_branch *git_branches_get_current (struct git_branches *branches);
G_KID_EXTERN char *git_branch_get_name (struct git_branch *branch);
G_KID_EXTERN char *git_branch_get_last_commit_sign (struct git_branch *branch);


struct __buf {
    unsigned char *buf;
    int len;
};

struct __buf *__inflate (struct __buf *zip_buffer, int inflated_buffer_len);

// git object 类型
enum gitobj_type {
    GIT_OBJ_TYPE_UNKNOW,    // 未知类型
    GIT_OBJ_TYPE_BLOB,      // blob
    GIT_OBJ_TYPE_COMMIT,    // commit
    GIT_OBJ_TYPE_TREE,      // tree
    GIT_OBJ_TYPE_TAG,       // tag
    GIT_OBJ_TYPE_OFS_DELTA, // offset delta
    GIT_OBJ_TYPE_REF_DELTA  // reference delta
};

// git object 结构体
struct gitobj {
    void *buf;
    char *path;
    char *sign;
    enum gitobj_type type;
    size_t size;
    char *body;
    void *ptr;
};


struct __gitpack {
    char *sign;
    int count;

    char *idx_path;
    char *pack_path;

    struct rdt *rd_tree;

    struct __gitpack *prev;
    struct __gitpack *next;
};

struct __gitpack_item {
    struct __buf buf;
    unsigned char type;

    void *base_sign;
    size_t negative_off;
    size_t off;
    size_t origin_len;
};

struct __gitpack_collection {
    struct __gitpack *head;
    struct __gitpack *tail;
};


// 仓库结构体
struct git_repo {
    char *path;                             // 仓库路径
    char *name;                             // 仓库名称
    struct __gitpack_collection *packes;    // 压缩包集合
    struct git_repo *prev;                  // 下一个仓库
    struct git_repo *next;                  // 上一个仓库
};

struct __buf *__gitpack_delta_patch (struct __gitpack_item base, struct __gitpack_item delta);

struct gitobj *__gitpack_getobj__charstring (struct git_repo *repo, const char *sign);
struct gitobj *__gitpack_getobj__bytestring (struct git_repo *repo, const void *sign);
struct __gitpack_collection *__gitpack_collection_get (struct git_repo *repo);

// blob 结构体
struct gitobj_blob {
    void *content; // blob 文件内容
    int len;    // blob 文件内容长度
};

// person log 结构体
struct gitperson {
    char *name;               // person log 姓名
    char *mail;               // person log 邮箱
    unsigned long timestamp;  // person log 时间戳
    char *timezone;           // person log 时区
};

// git commit 上游节点结构体
struct gitobj_commit_patent {
    char *sign; // 上游节点签名
    struct gitobj_commit_patent *prev;
    struct gitobj_commit_patent *next;
};

// git commit 结构体
struct gitobj_commit {
    struct gitobj_commit_patent *parent_head; //上游commit节点列表头部
    struct gitobj_commit_patent *parent_tail; //上游commit节点列表尾部
    struct gitobj_commit_patent *parent_cursor;

    char *tree_sign; // commit关联tree的签名
    char *message;   // commit 消息部分
    struct gitperson *author; // commit 作者信息
    struct gitperson *committer; //commit 提交者信息
};

// tree 成员结构体
struct gitobj_treeitem {
    char *sign; // 成员签名
    char *name; // 成员名称
    int type;   // 成员类型 (blob | tree)

    struct gitobj_treeitem *prev;
    struct gitobj_treeitem *next;
};

// tree 结构体
struct gitobj_tree {
    struct gitobj_treeitem *head;
    struct gitobj_treeitem *tail;
    struct gitobj_treeitem *cursor;
};

// 获取仓库内的git object
// param <repo>: 仓库指针
// param <signture>: git object 签名
// return: git object
G_KID_EXTERN struct gitobj *get_gitobj (struct git_repo *repo, const char* signture);

// 析构git object
// param <obj>: git object
G_KID_EXTERN void dispose_gitobj (struct gitobj *obj);

// 获取当前git object 的类型
// param <obj>: git object
// return: 0-Unknow | 1-blob | 2-commit | 3-tree
G_KID_EXTERN enum gitobj_type get_gitobj_type (struct gitobj *obj);


// 通过 git object 获取 blob
// param <obj>: git object
// return: blob object
G_KID_EXTERN struct gitobj_blob *get_gitobj_blob (struct gitobj *obj);

// 通过 git object 获取 commit
// param <obj>: git object
// return: commit object
G_KID_EXTERN struct gitobj_commit *get_gitobj_commit (struct gitobj *obj);

// 通过 git object 获取 tree
// param <obj>: git object
// return: tree object
G_KID_EXTERN struct gitobj_tree *get_gitobj_tree (struct gitobj *obj);


struct gitobj_blob *__git_obj_transfer_blob (char *body, size_t size);
struct gitobj *__gitpack_item_transfer_blob (struct __gitpack_item item);
void __gitobj_blob_dispose (struct gitobj_blob *obj);
struct gitobj_commit *__git_obj_transfer_commit (char *body, size_t size);
struct gitobj *__gitpack_item_transfer_commit (struct __gitpack_item item);
void __gitobj_commit_dispose (struct gitobj_commit *obj);
struct gitobj_tree *__git_obj_transfer_tree (char *body, size_t size);
struct gitobj *__gitpack_item_transfer_tree (struct __gitpack_item item);
void __gitobj_tree_dispose (struct gitobj_tree *obj);


// 获取 blob 内容的长度
// param <blob_obj>: blob object
// return: blob 内容的长度
G_KID_EXTERN int gitobj_blob_length (struct gitobj_blob *blob_obj);

// 获取 blob 内容
// param <blob_obj>: blob object
// return: blob 内容
G_KID_EXTERN void *gitobj_blob_content (struct gitobj_blob *blob_obj);


// 获取与 commit 关联的 tree 的签名
// param <commit_obj>: commit object
// return: tree 的签名
G_KID_EXTERN char *gitobj_commit_treesign (struct gitobj_commit *commit_obj);

// 获取 commit 的作者信息
// param <commit_obj>: commit object
// return: 作者信息
G_KID_EXTERN struct gitperson *gitobj_commit_author (struct gitobj_commit *commit_obj);

// 获取 commit 的提交者信息
// param <commit_obj>: commit object
// return: 提交者信息
G_KID_EXTERN struct gitperson *gitobj_commit_committer (struct gitobj_commit *commit_obj);

// 获取 commit 的提交消息
// param <commit_obj>: commit object
// return: 提交消息
G_KID_EXTERN char *gitobj_commit_message (struct gitobj_commit *commit_obj);

// 重置 commit 的上游 commit 签名列表的游标
// param <commit_obj>: commit object
G_KID_EXTERN void gitobj_commit_patent_reset (struct gitobj_commit *commit_obj);

// 将 commit 的上游 commit 签名列表的游标移动到下一签名
// param <commit_obj>: commit object
// return: 成功移动到下一个签名时，返回0，否则返回-1
G_KID_EXTERN int gitobj_commit_patent_movenext (struct gitobj_commit *commit_obj);

// 获取当前 commit 的上游 commit 签名列表游标所指向的上游 commit 签名结构体
// param <commit_obj>: commit object
// return: 上游 commit 签名结构体
G_KID_EXTERN struct gitobj_commit_patent *gitobj_commit_patent_current (struct gitobj_commit *commit_obj);

// 获取上游 commit 签名结构体中的签名
// param <commit_parent_obj>: 上游 commit 签名结构体
// return: 上游 commit 签名
G_KID_EXTERN char *gitobj_commit_patent_sign (struct gitobj_commit_patent *commit_parent_obj);


// 通过 person log 获取名称
// param <person_log>: person log
// return: 名称
G_KID_EXTERN char *gitperson_name (struct gitperson *person_log);

// 通过 person log 获取 mail
// param <person_log>: person log
// return: mail
G_KID_EXTERN char *gitperson_mail (struct gitperson *person_log);

// 通过 person log 获取时间戳
// param <person_log>: person log
// return: 时间戳
G_KID_EXTERN unsigned long gitperson_timestamp (struct gitperson *person_log);

// 通过 person log 获取时区
// param <person_log>: person log
// return 时区
G_KID_EXTERN char *git_person_timezone (struct gitperson *person_log);


// 重置 tree 游标
// param <tree_obj>: tree object
G_KID_EXTERN void gitobj_tree_reset (struct gitobj_tree *tree_obj);

// 移动 tree 游标指向下一个item
// param <tree_obj>: tree object
// return: 成功移动到下一个item时，返回0，否则返回-1
G_KID_EXTERN int gitobj_tree_movenext (struct gitobj_tree *tree_obj);

// 获取当前 tree 游标所指向的 item
// param <tree_obj>: tree object
// return: item
G_KID_EXTERN struct gitobj_treeitem *gitobj_tree_current (struct gitobj_tree *tree_obj);


// 获取 tree item 中的签名
// param <tree_item_obj>: tree item object
// return: item 的签名
G_KID_EXTERN char *gitobj_treeitem_sign (struct gitobj_treeitem *tree_item_obj);

// 获取 tree item 中的名称
// param <tree_item_obj>: tree item object
// return: item 的名称
G_KID_EXTERN char *gitobj_treeitem_name (struct gitobj_treeitem *tree_item_obj);

// 获取 tree item 中的类型
// param <tree_item_obj>: tree item object
// return: item 的类型
G_KID_EXTERN enum gitobj_type gitobj_treeitem_type (struct gitobj_treeitem *tree_item_obj);
#endif