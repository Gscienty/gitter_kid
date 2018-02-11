#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "passwd.h"

int __passwd_open_store () {
    if (access (__passwd_path, F_OK) != 0) {
        return -1;
    }
    int fd = open (__passwd_path, O_RDONLY, S_IRUSR);
    if (fd == -1) {
        return -1;
    }
    
    return fd;
}

struct __bytes *__read_content (int fd) {    
    size_t data_length = lseek (fd, 0, SEEK_END);
    lseek (fd, 0, SEEK_SET);

    struct __bytes *ret = __bytes_ctor (data_length + 1);
    ((char *)ret->buf)[data_length] = 0;
    read (fd, ret->buf, data_length);

    return ret;
}

struct __passwd_record *__parse (const char *line) {
    char *fields[7] = { 0 };
    register char *ch = (char *) line;
    int field_count = 0;
    const char *limit = line + strlen (line);
    for (; ch && ch < limit; field_count++) {
        fields[field_count] = ch;
        
        char *field_split_position = strchr (ch, ':');
        if (field_split_position == NULL) {
            ch = 0;
        }
        else {
            *field_split_position = 0;
            ch = field_split_position + 1;
        }
    }

    if (field_count < 4) {
        return NULL;
    }

    struct __passwd_record *ret = (struct __passwd_record *) malloc (sizeof (*ret));
    if (ret == NULL) {
        return NULL;
    }

    ret->username = strdup (fields[0]);
    ret->password = strdup (fields[1]);

    char *stop_position;
    ret->user_id = strtol (fields[2], &stop_position, 10);
    ret->group_id = strtol (fields[3], &stop_position, 10);
    ret->description = 4 < field_count ? strdup (fields[4]) : NULL;
    ret->home = 5 < field_count ? strdup (fields[5]) : NULL;
    ret->shell = 6 < field_count ? strdup (fields[6]) : NULL;

    return ret;
}

struct __bytes *__serialize (struct __passwd_record record) {
    size_t count_size = 5;

    count_size += record.username == NULL ? 0 : strlen (record.username);
    count_size += record.password == NULL ? 0 : strlen (record.password);
    count_size += 16;// user_id & group_id
    count_size += record.description == NULL ? 0 : strlen (record.description);
    count_size += record.home == NULL ? 0 : strlen (record.home);
    count_size += record.shell == NULL ? 0 : strlen (record.shell);
    struct __bytes *ret = __bytes_ctor (count_size);
    snprintf (ret->buf, ret->len, "%s:%s:%d:%d:%s:%s:%s\n",
        record.username == NULL ? "" : record.username,
        record.password == NULL ? "" : record.password,
        record.user_id,
        record.group_id,
        record.description == NULL ? "" : record.description,
        record.home == NULL ? "" : record.home,
        record.shell == NULL ? "" : record.shell
    );

    return ret;
}

void passwd_save_collection (struct __passwd_collection *collection) {
    if (collection == NULL) {
        return;
    }
    passwd_collection_reset (collection);
    while (passwd_collection_has_next (collection)) {
        struct __passwd_item *item = passwd_collection_next (collection);

        struct __bytes *line = __serialize (item->record);
        printf ("%s", line->buf);
        __bytes_dtor (line);
    }
}

struct __passwd_item *__passwd_item_wrap (struct __passwd_record record) {
    struct __passwd_item *ret = (struct __passwd_item *) malloc (sizeof (*ret));
    if (ret == NULL) {
        return NULL;
    }

    ret->record = record;
    ret->modify_flag = 0;
    ret->prev = NULL;
    ret->next = NULL;

    return ret;
}

struct __passwd_collection *passwd_get_collection () {
    int fd = __passwd_open_store ();
    if (fd == -1) {
        return NULL;
    }
    struct __passwd_collection *ret = (struct __passwd_collection *) malloc (sizeof (ret));
    if (ret == NULL) {
        return NULL;
    }
    ret->head = ret->tail = ret->cursor = NULL;

    struct __bytes *content = __read_content (fd);

    register char *ch = content->buf;
    const char *limit = content->buf + content->len;
    while (ch && ch < limit) {
        char *line_end_ptr = strchr (ch, '\n');
        if (line_end_ptr != NULL) {
            *line_end_ptr = 0;
        }

        struct __passwd_record *record = __parse (ch);
        if (record != NULL) {
            struct __passwd_item *item = __passwd_item_wrap (*record);

            if (ret->head == NULL) {
                ret->head = item;
            }
            if (ret->tail != NULL) {
                ret->tail->next = item;
            }
            item->prev = ret->tail;
            ret->tail = item;

            free (record);
        }
        
        if (line_end_ptr == NULL) {
            ch = 0;
        }
        else {
            ch = line_end_ptr + 1;
        }
    }

    __bytes_dtor (content);
    close (fd);

    return ret;
}

void passwd_collection_dtor (struct __passwd_collection *collection) {
    if (collection == NULL) {
        return;
    }
    while (collection->head != NULL) {
        struct __passwd_record record = collection->head->record;

        if (record.username != NULL) free (record.username);
        if (record.password != NULL) free (record.password);
        if (record.description != NULL) free (record.description);
        if (record.home != NULL) free (record.home);
        if (record.shell != NULL) free (record.shell);

        struct __passwd_item *next = collection->head->next;
        free (collection->head);
        collection->head = next;
    }

    free (collection);
}

void passwd_collection_reset (struct __passwd_collection *collection) {
    if (collection == NULL) {
        return;
    }
    collection->cursor = NULL;
}

int passwd_collection_has_next (struct __passwd_collection *collection) {
    if (collection == NULL) {
        return 0;
    }

    return (collection->cursor == NULL ? collection->head : collection->cursor->next) != NULL;
}

struct __passwd_item *passwd_collection_next (struct __passwd_collection *collection) {
    if (collection == NULL) {
        return NULL;
    }

    collection->cursor = (collection->cursor == NULL ? collection->head : collection->cursor->next);
    return collection->cursor;
}

struct __passwd_item *passwd_collection_append (struct __passwd_collection *collection) {
    if (collection == NULL) {
        return NULL;
    }

    struct __passwd_item *ret = (struct __passwd_item *) malloc (sizeof (*ret));

    ret->modify_flag = 1;
    ret->record.username = ret->record.password = ret->record.description = ret->record.home = ret->record.shell = NULL;
    ret->record.user_id = ret->record.group_id = 0;

    ret->prev = collection->tail;
    ret->next = NULL;

    if (collection->head == NULL) {
        collection->head = ret;
    }

    if (collection->tail != NULL) {
        collection->tail->next = ret;
    }
    collection->tail = ret;

    return ret;
}


char *passwd_item_get_username (struct __passwd_item *item) {
    if (item == NULL) {
        return NULL;
    }
    return item->record.username;
}

char *passwd_item_get_password (struct __passwd_item *item) {
    if (item == NULL) {
        return NULL;
    }
    return item->record.password;
}

unsigned int passwd_item_get_user_id (struct __passwd_item *item) {
    if (item == NULL) {
        return 0;
    }
    return item->record.user_id;
}

unsigned int passwd_item_get_group_id (struct __passwd_item *item) {
    if (item == NULL) {
        return 0;
    }
    return item->record.group_id;
}

char *passwd_item_get_description (struct __passwd_item *item) {
    if (item == NULL) {
        return NULL;
    }
    return item->record.description;
}

char *passwd_item_get_home (struct __passwd_item *item) {
    if (item == NULL) {
        return NULL;
    }
    return item->record.home;
}

char *passwd_item_get_shell (struct __passwd_item *item) {
    if (item == NULL) {
        return NULL;
    }
    return item->record.shell;
}

void passwd_item_set_username (struct __passwd_item *item, const char *username) {
    if (item == NULL) {
        return;
    }
    if (item->record.username != NULL) free (item->record.username);
    item->record.username = username == NULL ? NULL : strdup (username);
    item->modify_flag = 1;
}

void passwd_item_set_password (struct __passwd_item *item, const char *password) {
    if (item == NULL) {
        return;
    }
    if (item->record.password != NULL) free (item->record.password);
    item->record.password = password == NULL ? NULL : strdup (password);
    item->modify_flag = 1;
}

void passwd_item_set_user_id (struct __passwd_item *item, unsigned int user_id) {
    if (item == NULL) {
        return;
    }
    item->record.user_id = user_id;
    item->modify_flag = 1;
}

void passwd_item_set_group_id (struct __passwd_item *item, unsigned int group_id) {
    if (item == NULL) {
        return;
    }
    item->record.group_id = group_id;
    item->modify_flag = 1;
}

void passwd_item_set_description (struct __passwd_item *item, const char *description) {
    if (item == NULL) {
        return;
    }
    if (item->record.description != NULL) free (item->record.description);
    item->record.description = description == NULL ? NULL : strdup (description);
    item->modify_flag = 1;
}

void passwd_item_set_home (struct __passwd_item *item, const char *home) {
    if (item == NULL) {
        return;
    }
    if (item->record.home != NULL) free (item->record.home);
    item->record.home = home == NULL ? NULL : strdup (home);
    item->modify_flag = 1;
}

void passwd_item_set_shell (struct __passwd_item *item, const char *shell) {
    if (item == NULL) {
        return; 
    }
    if (item->record.shell != NULL) free (item->record.shell);
    item->record.shell = shell == NULL ? NULL : strdup (shell);
    item->modify_flag = 1;
}