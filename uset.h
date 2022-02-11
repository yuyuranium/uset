#ifndef _USET_H
#define _USET_H

#define USET_R 'R'
#define USET_B 'B'

typedef struct _ele {
  void *data;
  char color;
  struct _ele *left, *right, *parent;
} _ele_t;

typedef struct uset {
  _ele_t *_root, *_nil;
  int size;
} uset_t;

uset_t *uset_create(int datac, void **datav);

void uset_clear(uset_t *s);

void uset_free(uset_t *s);

uset_t *uset_add(uset_t *s, void *data);

int uset_delete(uset_t *s, void *data);

int uset_has(uset_t *s, const void *data);

void **uset_entries(uset_t *s);

void uset_foreach(uset_t *s, void (callback)(void **));

int uset_every(uset_t *s, int (cmp)(void *));

int uset_some(uset_t *s, int (cmp)(void *));

#endif
