#include "gitfs.h"
#include <malloc.h>

struct __deflate_param *__git_refdelta_get (struct git_repo *repo, unsigned char *ptr, size_t deflate_len, size_t origin_len) {
    unsigned char *base_sign = (void *) malloc (41);
    int i;
    for (i = 0; i < 20; i++) {
        base_sign[i << 1] = HEX ((*ptr & 0xF0) >> 4);
        base_sign[(i << 1) + 1] = HEX ((*ptr & 0x0F));
        ptr++;
        deflate_len--;
    }
    base_sign[40] = 0;

    struct git_obj *base_obj = git_obj_get (repo, base_sign);

    struct __deflate_param deflate_obj = { ptr, deflate_len };
}

struct git_obj *__git_ofsdelta_get (pack, packitem_meta, ptr, origin_len)
    struct __gitpack *pack;
    struct rdt_node *packitem_meta;
    unsigned char *ptr;
    size_t origin_len;
{
    int offset = *ptr & 0x7F;
    int nbytes = 1;
    while (*ptr & 0x80) {
        ptr++;
        nbytes++;
        offset = (offset << 7) + (*ptr & 0x7F);
    }
    if (nbytes >= 2) offset += (1 << (7 * (nbytes - 1)));

    printf ("%d\n", packitem_meta->off - offset);

    return NULL;
}