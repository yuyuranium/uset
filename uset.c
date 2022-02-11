#include <stdio.h>
#include <stdlib.h>
#include "uset.h"

static ele_t *make_ele(void *data, color_t color,
                       ele_t *left, ele_t *right, ele_t *parent)
{
  ele_t *e;
  if (!(e = malloc(sizeof(ele_t))))
    return NULL;

  e->data = data;
  e->color = color;
  e->left = left;
  e->right = right;
  e->parent = parent;
  return e;
}

static void left_rotate(set_t *s, ele_t *e)
{
  ele_t *y = e->right;
  e->right = y->left;
  if (y->left != s->nil)
    y->left->parent = e;
  y->parent = e->parent;
  if (e->parent == s->nil)
    s->root = y;
  else if (e == e->parent->left)
    e->parent->left = y;
  else 
    e->parent->right = y;
  y->left = e;
  e->parent = y;
}

static void right_rotate(set_t *s, ele_t *e)
{
  ele_t *x = e->left;
  e->left = x->right;
  if (x->right != s->nil)
    x->right->parent = e;
  x->parent = e->parent;
  if (e->parent == s->nil)
    s->root = x;
  else if (e == e->parent->left)
    e->parent->left = x;
  else 
    e->parent->right = x;
  x->right = e;
  e->parent = x;
}

static void insert_fixup(set_t *s, ele_t *e)
{
  while (e->parent->color == USET_R) {
    if (e->parent == e->parent->parent->left) {
      ele_t *uncle = e->parent->parent->right;
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
      ele_t *uncle = e->parent->parent->left;
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
  s->root->color = USET_B;
}

set_t *uset_add(set_t *s, void *data)
{
  ele_t *parent = s->nil;
  ele_t **indirect = &(s->root);
  while (*indirect != s->nil) {
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

  ele_t *e = make_ele(data, USET_R, s->nil, s->nil, parent);
  *indirect = e;
  insert_fixup(s, e);
  s->size++;
  return s;
}

set_t *uset_create(int datac, void **datav)
{
  set_t *s;
  if (!(s = malloc(sizeof(set_t))))
    return NULL;

  ele_t *nil = make_ele(NULL, USET_B, NULL, NULL, NULL); 
  s->nil = nil;
  s->root = nil;
  s->size = 0;

  while (--datac >= 0)
    uset_add(s, datav[datac]);

  return s;
}

void uset_clear(set_t *s)
{

}

int uset_has(set_t *s, const void *data)
{
  ele_t *e = s->root;
  while (e != s->nil && e->data != data)
    e = (data < e->data)? e->left : e->right;

  return e != s->nil;
}