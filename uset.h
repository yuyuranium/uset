#ifndef _USET_H
#define _USET_H

#define USET_RED   1
#define USET_BLACK 0

struct uset_ele {
    void *data;
    unsigned char color;
    struct uset_ele *left, *right, *parent;
};

typedef struct uset {
    struct uset_ele *_root, *_nil;
    int size;
} uset_t;

uset_t *uset_create(int datac, void **datav);

void uset_clear(uset_t *s);

void uset_free(uset_t *s);

uset_t *uset_add(uset_t *s, void *data);

int uset_delete(uset_t *s, void *data);

int uset_has(uset_t *s, const void *data);

void **uset_entries(uset_t *s);

void uset_foreach(uset_t *s, void (callback)(void *data));

int uset_every(uset_t *s, int (cmp)(void *data));

int uset_some(uset_t *s, int (cmp)(void *data));

#endif
