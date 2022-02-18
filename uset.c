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
  while (e->parent->color == USET_RED) {
    if (e->parent == e->parent->parent->left) {
      struct uset_ele *uncle = e->parent->parent->right;
      if (uncle->color == USET_RED) {
        e->parent->color = USET_BLACK;
        uncle->color = USET_BLACK;
        e->parent->parent->color = USET_RED;
        e = e->parent->parent;
      } else {
        if (e == e->parent->right) {
          e = e->parent;
          left_rotate(s, e);
        }
        e->parent->color = USET_BLACK;
        e->parent->parent->color = USET_RED;
        right_rotate(s, e->parent->parent);
      }
    } else {
      struct uset_ele *uncle = e->parent->parent->left;
      if (uncle->color == USET_RED) {
        e->parent->color = USET_BLACK;
        uncle->color = USET_BLACK;
        e->parent->parent->color = USET_RED;
        e = e->parent->parent;
      } else {
        if (e == e->parent->left) {
          e = e->parent;
          right_rotate(s, e);
        }
        e->parent->color = USET_BLACK;
        e->parent->parent->color = USET_RED;
        left_rotate(s, e->parent->parent);
      }
    }
  }
  s->_root->color = USET_BLACK;
}

static struct uset_ele *minimum(uset_t *s, struct uset_ele *e)
{
  while (e->left != s->_nil)
    e = e->left;
  return e;
}

static void transplant(uset_t *s, struct uset_ele *u, struct uset_ele *v)
{
  if (u->parent == s->_nil)
    s->_root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

static void delete_fixup(uset_t *s, struct uset_ele *x)
{
  while (x != s->_root && x->color == USET_BLACK) {
    struct uset_ele *w;
    if (x == x->parent->left) {
      w = x->parent->right;
      if (w->color == USET_RED) {
        w->color = USET_BLACK;
        x->parent->color = USET_RED;
        left_rotate(s, x->parent);
        w = x->parent->right;
      }

      if (w->left->color == USET_BLACK && w->right->color == USET_BLACK) {
        w->color = USET_RED;
        x = x->parent;
      } else {
        if (w->right->color == USET_BLACK) {
          w->left->color = USET_BLACK;
          w->color = USET_RED;
          right_rotate(s, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = USET_BLACK;
        w->right->color = USET_BLACK;
        left_rotate(s, x->parent);
        x = s->_root;
      }
    } else {
      w = x->parent->left;
      if (w->color == USET_RED) {
        w->color = USET_BLACK;
        x->parent->color = USET_RED;
        right_rotate(s, x->parent);
        w = x->parent->left;
      }

      if (w->right->color == USET_BLACK && w->left->color == USET_BLACK) {
        w->color = USET_RED;
        x = x->parent;
      } else {
        if (w->left->color == USET_BLACK) {
          w->right->color = USET_BLACK;
          w->color = USET_RED;
          left_rotate(s, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = USET_BLACK;
        w->left->color = USET_BLACK;
        right_rotate(s, x->parent);
        x = s->_root;
      }
    }
  }
  x->color = USET_BLACK;
}

static struct uset_ele *search(uset_t *s, const void *data) {
  struct uset_ele *e = s->_root;
  while (e != s->_nil && e->data != data)
    e = (e->data < data)? e->right : e->left;

  return e;
}

static void postorder_free_all(uset_t *s, struct uset_ele *e)
{
  if (e != s->_nil) {
    postorder_free_all(s, e->left);
    postorder_free_all(s, e->right);
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
                          void (callback)(void *data))
{
  if (e != s->_nil) {
    inorder_apply(s, e->left, callback);
    callback(e->data);
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

  struct uset_ele *e = make_ele(data, USET_RED, s->_nil, s->_nil, parent);
  *indirect = e;
  insert_fixup(s, e);
  s->size++;
  return s;
}

int uset_delete(uset_t *s, void *data)
{
  struct uset_ele *z = search(s, data); 
  if (z == s->_nil) 
    return 0;

  struct uset_ele *y = z, *x;
  char y_orig_color = y->color;

  if (z->left == s->_nil) {
    x = z->right;
    transplant(s, z, z->right);
  } else if (z->right == s->_nil) {
    x = z->left;
    transplant(s, z, z->left);
  } else {
    y = minimum(s, z->right);
    y_orig_color = y->color;
    x = y->right;
    if (y->parent == z) {
      x->parent = y;
    } else {
      transplant(s, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    transplant(s, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (y_orig_color == USET_BLACK)
    delete_fixup(s, x);

  free(z);
  s->size--;
  return 1;
}

uset_t *uset_create(int datac, void **datav)
{
  uset_t *s;
  if (!(s = malloc(sizeof(uset_t))))
    return NULL;

  struct uset_ele *nil = make_ele(NULL, USET_BLACK, NULL, NULL, NULL); 
  s->_nil = nil;
  s->_root = nil;
  s->size = 0;

  while (--datac >= 0)
    uset_add(s, datav[datac]);

  return s;
}

void uset_clear(uset_t *s)
{
  postorder_free_all(s, s->_root);
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
  return search(s, data) != s->_nil;
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

void uset_foreach(uset_t *s, void (*callback)(void *data))
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
