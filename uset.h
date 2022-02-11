#ifndef _USET_H
#define _USET_H

#define USET_R 'R'
#define USET_B 'B'

typedef char color_t;

typedef struct ele {
  void *data;
  color_t color;
  struct ele *left, *right, *parent;
} ele_t;

typedef struct set {
  ele_t *root, *nil;
  int size;
} set_t;

set_t *uset_create(int datac, void **datav);

void uset_clear(set_t *s);

set_t *uset_add(set_t *s, void *data);

int uset_delete(set_t *s, void *data);

int uset_has(set_t *s, const void *data);

void **uset_entires(set_t *s, int *ret_sz);

void uset_foreach(set_t *s, void (callback)(void *));

int uset_every(set_t *s, int (cmp)(void *));

int uset_some(set_t *s, int (cmp)(void *));

#endif
