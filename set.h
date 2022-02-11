#ifndef _SET_H
#define _SET_H

#define R 'R'
#define B 'B'

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

set_t *make_set(int datac, void **datav);

void set_clear(set_t *s);

set_t *set_add(set_t *s, void *data);

int set_delete(set_t *s, void *data);

int set_has(set_t *s, const void *data);

void **set_entires(set_t *s, int *ret_sz);

void set_for_each(set_t *s, void (callback)(void *));

int set_every(set_t *s, int (cmp)(void *));

int set_some(set_t *s, int (cmp)(void *));

#endif
