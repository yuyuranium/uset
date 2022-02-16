#include <stdio.h>
#include <stdlib.h>
#include "uset.h"

int gtz(void *data)
{
  return (long)data > 0;
}

int ltz(void *data)
{
  return (long)data < 0;
}

int odd(void *data)
{
  return (long)data & 1;
}

void show_hex(void *data)
{
  long p = (long)data;
  printf("0x%lx\n", p);
}

void level_order(uset_t *s)
{
  struct uset_ele **q = malloc(32 * sizeof(struct uset_ele *));
  int tail = -1, head = -1;
  int cnt = 0, flag = 1;
  q[++tail] = s->_root;
  while (head != tail) {
    struct uset_ele *e = q[++head];
    if (e != s->_nil) {
      printf("[%c%ld] ", e->color, (long)e->data);
      q[++tail] = e->left;
      q[++tail] = e->right;
    } else {
      printf("[ ] ");
    }
    cnt++;
    if (cnt == flag) {
      printf("\n");
      flag <<= 1;
      cnt = 0;
    }
  }
  printf("\n");
  free(q);
}

int main(int argc, char *argv[])
{
  int datac = 10;
  long *datav = malloc(datac * sizeof(long));

  printf("datav: ");
  for (int i = 0; i < datac; ++i) {
    datav[i] = i;
    printf("%ld ", datav[i]);
  }
  printf("\n");

  uset_t *s = uset_create(datac, (void **)datav);

  printf("every element > 0? %d\n", uset_every(s, gtz));
  printf("every element is odd? %d\n", uset_every(s, odd));
  printf("some elements < 0? %d\n", uset_some(s, ltz));
  printf("some elements are odd? %d\n", uset_some(s, odd));

  printf("Plus1 to each element\n");
  printf("Show hex\n");
  uset_foreach(s, show_hex);
  for (long i = 0; i < datac; ++i) {
    if (odd((void *)i)) {
      printf("Deleting %ld, success: %d\n", i, uset_delete(s, (void *) i));
      level_order(s);
    }
  }
  printf("some elements are odd? %d\n", uset_some(s, odd));
  uset_add(uset_add(uset_add(s, (void *)10), (void *)9), (void *)8);
  printf("%d\n", uset_delete(s, (void *)3));

  void **entries = uset_entries(s);
  printf("entries: ");
  for (int i = 0; i < s->size; ++i)
    printf("%ld ", (long)entries[i]);
  printf("\n");
  level_order(s);

  uset_clear(s);

  return 0;
}
