#include <stdio.h>
#include <stdlib.h>
#include "uset.h"

static struct uset_ele *make_ele(void *data, char color, struct uset_ele *left,
                                 struct uset_ele *right,
                                 struct uset_ele *parent)
{
  struct uset_ele *e;
  if (!(e = malloc(sizeof(struct uset_ele))))
    return NULL;

  e->data = data;
  e->color = color;
  e->left = left;
  e->right = right;
  e->parent = parent;
  return e;
}

static void left_rotate(uset_t *s, struct uset_ele *e)
{
  struct uset_ele *y = e->right;
  e->right = y->left;
  if (y->left != s->_nil)
    y->left->parent = e;
  y->parent = e->parent;
  if (e->parent == s->_nil)
    s->_root = y;
  else if (e == e->parent->left)
    e->parent->left = y;
  else 
    e->parent->right = y;
  y->left = e;
  e->parent = y;
}

static void right_rotate(uset_t *s, struct uset_ele *e)
{
  struct uset_ele *x = e->left;
  e->left = x->right;
  if (x->right != s->_nil)
    x->right->parent = e;
  x->parent = e->parent;
  if (e->parent == s->_nil)
    s->_root = x;
  else if (e == e->parent->left)
    e->parent->left = x;
  else 
    e->parent->right = x;
  x->right = e;
  e->parent = x;
}

static void insert_fixup(uset_t *s, struct uset_ele *e)
{
  while (e->parent->color == USET_R) {
    if (e->parent == e->parent->parent->left) {
      struct uset_ele *uncle = e->parent->parent->right;
      if (uncle->color == USET_R) {
        e->parent->color = USET_B;
        uncle->color = USET_B;
        e->parent->parent->color = USET_R;
        e = e->parent->parent;
      } else {
        if (e == e->parent->right) {
          e = e->parent;
          left_rotate(s, e);
        }
        e->parent->color = USET_B;
        e->parent->parent->color = USET_R;
        right_rotate(s, e->parent->parent);
      }
    } else {
      struct uset_ele *uncle = e->parent->parent->left;
      if (uncle->color == USET_R) {
        e->parent->color = USET_B;
        uncle->color = USET_B;
        e->parent->parent->color = USET_R;
        e = e->parent->parent;
      } else {
        if (e == e->parent->left) {
          e = e->parent;
          right_rotate(s, e);
        }
        e->parent->color = USET_B;
        e->parent->parent->color = USET_R;
        left_rotate(s, e->parent->parent);
      }
    }
  }
  s->_root->color = USET_B;
}

static void postorder_free(uset_t *s, struct uset_ele *e)
{
  if (e != s->_nil) {
    postorder_free(s, e->left);
    postorder_free(s, e->right);
    free(e);
  }
}

static void inorder_append(uset_t *s, struct uset_ele *e, void **entries,
                           int *pp)
{
  if (e != s->_nil) {
    inorder_append(s, e->left, entries, pp);
    entries[(*pp)++] = e->data;
    inorder_append(s, e->right, entries, pp);
  }
}

static int preorder_every(uset_t *s, struct uset_ele *e, int (cmp)(void *))
{
  if (e != s->_nil)
    return cmp(e->data) && preorder_every(s, e->left, cmp) &&
        preorder_every(s, e->right, cmp);
  else
    return 1;
}

static int preorder_some(uset_t *s, struct uset_ele *e, int (cmp)(void *))
{
  if (e != s->_nil)
    return cmp(e->data) || preorder_some(s, e->left, cmp) ||
        preorder_some(s, e->right, cmp);
  else
    return 0;
}

static void inorder_apply(uset_t *s, struct uset_ele *e,
                          void (callback)(void **datap))
{
  if (e != s->_nil) {
    inorder_apply(s, e->left, callback);
    callback(&(e->data));
    inorder_apply(s, e->right, callback);
  }
}

uset_t *uset_add(uset_t *s, void *data)
{
  struct uset_ele *parent = s->_nil;
  struct uset_ele **indirect = &(s->_root);
  while (*indirect != s->_nil) {
    void *pivot = (*indirect)->data;
    parent = *indirect;
    if (data < pivot)
      indirect = &((*indirect)->left);
    else if (data > pivot)
      indirect = &((*indirect)->right);
    else
      /* Duplicated data */
      return s;
  }

  struct uset_ele *e = make_ele(data, USET_R, s->_nil, s->_nil, parent);
  *indirect = e;
  insert_fixup(s, e);
  s->size++;
  return s;
}

uset_t *uset_create(int datac, void **datav)
{
  uset_t *s;
  if (!(s = malloc(sizeof(uset_t))))
    return NULL;

  struct uset_ele *nil = make_ele(NULL, USET_B, NULL, NULL, NULL); 
  s->_nil = nil;
  s->_root = nil;
  s->size = 0;

  while (--datac >= 0)
    uset_add(s, datav[datac]);

  return s;
}

void uset_clear(uset_t *s)
{
  postorder_free(s, s->_root);
  s->_root = s->_nil;
  s->size = 0;
}

void uset_free(uset_t *s)
{
  uset_clear(s);
  free(s->_nil);
  free(s);
}

int uset_has(uset_t *s, const void *data)
{
  struct uset_ele *e = s->_root;
  while (e != s->_nil && e->data != data)
    e = (data < e->data)? e->left : e->right;

  return e != s->_nil;
}

void **uset_entries(uset_t *s)
{
  int p = 0;
  void **entries;
  if (!(entries = malloc(s->size * sizeof(void *))))
    return NULL;

  inorder_append(s, s->_root, entries, &p);
  return entries;
}

void uset_foreach(uset_t *s, void (*callback)(void **datap))
{
  inorder_apply(s, s->_root, callback);
}

int uset_every(uset_t *s, int (cmp)(void *data))
{
  return preorder_every(s, s->_root, cmp);
}

int uset_some(uset_t *s, int (cmp)(void *data))
{
  return preorder_some(s, s->_root, cmp);
}
