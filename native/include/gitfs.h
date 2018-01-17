#ifndef _G_KID_GITFS_
#define _G_KID_GITFS_

#include "define.h"
#include "rbtree.h"
#include "util.h"

#define HEX(v) ((v) < 10 ? '0' + (v) : (v) - 10 + 'a')

int __access_file_readable (const char* filepath);
int __access_file_writable (const char* filepath);
int __access_file_executable (const char* filepath);
int __access_file_exist (const char* filepath);
int __access_chmod (const char* path, unsigned int mode);
int __access_chown (const char* path, unsigned int owner, unsigned int group);

struct __gitpack_index {
    int nth;
    size_t off;
    size_t len;
    void *sign;
};

struct __gitpack {
    char *sign;
    int count;

    char *idx_path;
    char *pack_path;

    struct rdt *rd_tree;
    struct __gitpack_index *indexes;

    struct __gitpack *prev;
    struct __gitpack *next;
};

struct __gitpack_collection {
    struct __gitpack *head;
    struct __gitpack *tail;
};


// 仓库结构体
struct gitrepo {
    char *path;                             // 仓库路径
    char *name;                             // 仓库名称
    struct __gitpack_collection *packes;    // 压缩包集合
    struct gitrepo *prev;                  // 下一个仓库
    struct gitrepo *next;                  // 上一个仓库
};

// 仓库市场结构体
struct gitmarket {
    char *path;                 //仓库市场路径

    struct gitrepo *head;      // 仓库队列头部指针
    struct gitrepo *tail;      // 仓库队列尾部指针
    struct gitrepo *cursor;    //仓库游标
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
G_KID_EXTERN struct gitmarket *get_gitmarket (const char *basepath);

// 析构仓库市场
// param <market>: 仓库市场指针
G_KID_EXTERN void gitmarket_dtor (struct gitmarket *market);

// 重置仓库市场游标
// param <market>: 仓库市场指针
G_KID_EXTERN void gitmarket_reset (struct gitmarket *market);

// 仓库市场是否存在下一个仓库
// param <market>: 仓库市场指针
// return: 返回是否存在下一个仓库
G_KID_EXTERN int gitmarket_hasnext (struct gitmarket *market);

// 获取仓库市场下一个仓库
// param <market>: 仓库市场指针
// return: 返回下一个仓库指针
G_KID_EXTERN struct gitrepo *gitmarket_next (struct gitmarket *market);

// 获取仓库路径
// param <repo>: 仓库指针
// return: 仓库路径
G_KID_EXTERN char *gitrepo_get_path (struct gitrepo *repo);

// 获取仓库名称
// param <repo>: 仓库指针
// return: 仓库名称
G_KID_EXTERN char *gitrepo_get_name (struct gitrepo *repo);

struct gitbranch {
    char *name;
    char *lastcommit_sign;

    struct gitbranch *prev;
    struct gitbranch *next;
};

struct gitbranches {
    struct gitbranch *head;
    struct gitbranch *tail;
    struct gitbranch *cursor;
};

// 从仓库中获取分支集合
// param <repo>: 仓库指针
// return: 分支集合
G_KID_EXTERN struct gitbranches *gitrepo_get_branches (struct gitrepo *repo);

// 析构分支集合
// param <branches>: 分支集合指针
G_KID_EXTERN void gitbranches_dtor (struct gitbranches *branches);


// 获取分支名称
// param <branch>: 分支指针
// return: 分支名称
G_KID_EXTERN char *gitbranch_get_name (struct gitbranch *branch);
// 获取分支最后提交的 commit 签名
// param <branch>: 分支指针
// return: commit 签名
G_KID_EXTERN char *gitbranch_get_lastcommit_sign (struct gitbranch *branch);


// 重置分支集合游标
// param <branches>: 分支集合
G_KID_EXTERN void gitbranches_reset (struct gitbranches *branches);

// 分支集合是否具有下一个分支
// param <branches>: 分支集合
// return: 分支集合是否具有下一分支
G_KID_EXTERN int gitbranches_hasnext (struct gitbranches *branches);

// 获取下一个分支
// param <branches>: 分支集合
// return: 下一个分支
G_KID_EXTERN struct gitbranch *gitbranches_next (struct gitbranches *branches);

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


struct __gitpack_file {
    int fd;
    size_t len;
};

struct __gitpack_item {
    struct __bytes bytes;
    unsigned char type;

    void *base_sign;
    size_t negative_off;
    size_t off;
    size_t origin_len;
};

struct __gitpack_item *__gitpack_ofsdelta_patch (struct gitrepo *repo, struct __gitpack_file *packfile, struct __gitpack_item packitem);
struct __gitpack_item *__gitpack_refdelta_patch (struct gitrepo *repo, struct __gitpack_item packitem);

struct __bytes *__gitpack_delta_patch (struct __bytes base, struct __gitpack_item delta);

struct gitobj *__gitpack_getobj__charstring (struct gitrepo *repo, const char *sign);
struct gitobj *__gitpack_getobj__bytestring (struct gitrepo *repo, const void *sign);
struct __gitpack_collection *__gitpack_get_collection (struct gitrepo *repo);
void __gitpack_collection_dtor (struct __gitpack_collection *collection);

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
struct gitobj_commitpatent {
    char *sign; // 上游节点签名
    struct gitobj_commitpatent *prev;
    struct gitobj_commitpatent *next;
};

// git commit 结构体
struct gitobj_commit {
    struct gitobj_commitpatent *parent_head; //上游commit节点列表头部
    struct gitobj_commitpatent *parent_tail; //上游commit节点列表尾部
    struct gitobj_commitpatent *parent_cursor;

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
G_KID_EXTERN struct gitobj *gitrepo_get_gitobj (struct gitrepo *repo, const char* signture);

// 析构git object
// param <obj>: git object
G_KID_EXTERN void gitobj_dtor (struct gitobj *obj);

// 获取当前git object 的类型
// param <obj>: git object
// return: 0-Unknow | 1-blob | 2-commit | 3-tree
G_KID_EXTERN enum gitobj_type gitobj_get_type (struct gitobj *obj);


// 通过 git object 获取 blob
// param <obj>: git object
// return: blob object
G_KID_EXTERN struct gitobj_blob *gitobj_get_blob (struct gitobj *obj);

// 通过 git object 获取 commit
// param <obj>: git object
// return: commit object
G_KID_EXTERN struct gitobj_commit *gitobj_get_commit (struct gitobj *obj);

// 通过 git object 获取 tree
// param <obj>: git object
// return: tree object
G_KID_EXTERN struct gitobj_tree *gitobj_get_tree (struct gitobj *obj);


struct gitobj_blob *__transfer_blob (struct __bytes bytes);
struct gitobj *__packitem_transfer_blob (struct __gitpack_item item);
void __gitobj_blob_dtor (struct gitobj_blob *obj);

struct gitobj_commit *__transfer_commit (struct __bytes bytes);
struct gitobj *__packitem_transfer_commit (struct __gitpack_item item);
void __gitobj_commit_dtor (struct gitobj_commit *obj);

struct gitobj_tree *__transfer_tree (struct __bytes bytes);
struct gitobj *__packitem_transfer_tree (struct __gitpack_item item);
void __gitobj_tree_dtor (struct gitobj_tree *obj);


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
G_KID_EXTERN char *gitobj_commit_get_treesign (struct gitobj_commit *commit_obj);

// 获取 commit 的作者信息
// param <commit_obj>: commit object
// return: 作者信息
G_KID_EXTERN struct gitperson *gitobj_commit_get_author (struct gitobj_commit *commit_obj);

// 获取 commit 的提交者信息
// param <commit_obj>: commit object
// return: 提交者信息
G_KID_EXTERN struct gitperson *gitobj_commit_get_committer (struct gitobj_commit *commit_obj);

// 获取 commit 的提交消息
// param <commit_obj>: commit object
// return: 提交消息
G_KID_EXTERN char *gitobj_commit_get_message (struct gitobj_commit *commit_obj);


// 重置 commit 的上游 commit 签名列表的游标
// param <commit_obj>: commit object
G_KID_EXTERN void gitobj_commitparents_reset (struct gitobj_commit *commit_obj);

// 将 commit 的上游 commit 签名列表是否存在下一个元素
// param <commit_obj>: commit object
// return: 是否存在下一个元素
G_KID_EXTERN int gitobj_commitparents_hasnext (struct gitobj_commit *commit_obj);

// 获取当前 commit 的上游 commit 签名列表游标所指向的上游 commit 签名结构体
// param <commit_obj>: commit object
// return: 上游 commit 签名结构体
G_KID_EXTERN struct gitobj_commitpatent *gitobj_commitparents_next (struct gitobj_commit *commit_obj);

// 获取上游 commit 签名结构体中的签名
// param <commit_parent_obj>: 上游 commit 签名结构体
// return: 上游 commit 签名
G_KID_EXTERN char *gitobj_commitpatent_get_sign (struct gitobj_commitpatent *commitparent_obj);


// 通过 person log 获取名称
// param <person_log>: person log
// return: 名称
G_KID_EXTERN char *gitperson_get_name (struct gitperson *person_log);

// 通过 person log 获取 mail
// param <person_log>: person log
// return: mail
G_KID_EXTERN char *gitperson_get_mail (struct gitperson *person_log);

// 通过 person log 获取时间戳
// param <person_log>: person log
// return: 时间戳
G_KID_EXTERN unsigned long gitperson_get_timestamp (struct gitperson *person_log);

// 通过 person log 获取时区
// param <person_log>: person log
// return 时区
G_KID_EXTERN char *gitperson_get_timezone (struct gitperson *person_log);


// 重置 tree 游标
// param <tree_obj>: tree object
G_KID_EXTERN void gitobj_tree_reset (struct gitobj_tree *tree_obj);

// tree 游标是否存在下一个 item
// param <tree_obj>: tree object
// return: tree 游标是否存在下一个 item
G_KID_EXTERN int gitobj_tree_hasnext (struct gitobj_tree *tree_obj);

// 获取当前 tree 游标所指向的 item
// param <tree_obj>: tree object
// return: item
G_KID_EXTERN struct gitobj_treeitem *gitobj_tree_next (struct gitobj_tree *tree_obj);


// 获取 tree item 中的签名
// param <tree_item_obj>: tree item object
// return: item 的签名
G_KID_EXTERN char *gitobj_treeitem_get_sign (struct gitobj_treeitem *tree_item_obj);

// 获取 tree item 中的名称
// param <tree_item_obj>: tree item object
// return: item 的名称
G_KID_EXTERN char *gitobj_treeitem_get_name (struct gitobj_treeitem *tree_item_obj);

// 获取 tree item 中的类型
// param <tree_item_obj>: tree item object
// return: item 的类型
G_KID_EXTERN enum gitobj_type gitobj_treeitem_get_type (struct gitobj_treeitem *tree_item_obj);
#endif