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

void plus1(void **datap)
{
  long *p = (long *)datap;
  (*p)++;
}

void show_hex(void **datap)
{
  long *p = (long *)datap;
  printf("0x%lx\n", *p);
}

int main(int argc, char *argv[])
{
  int datac = 10;
  long *datav = malloc(datac * sizeof(long));

  printf("datav: ");
  for (int i = 0; i < datac; ++i) {
    datav[i] = rand() % 100;
    printf("%ld ", datav[i]);
  }
  printf("\n");

  uset_t *s = uset_create(datac, (void **)datav);

  printf("every element > 0? %d\n", uset_every(s, gtz));
  printf("every element is odd? %d\n", uset_every(s, odd));
  printf("some elements < 0? %d\n", uset_some(s, ltz));
  printf("some elements are odd? %d\n", uset_some(s, odd));

  uset_foreach(s, plus1);
  printf("Plus1 to each element\n");
  printf("Show hex\n");
  uset_foreach(s, show_hex);

  void **entries = uset_entries(s);
  printf("entries: ");
  for (int i = 0; i < s->size; ++i)
    printf("%ld ", (long)entries[i]);
  printf("\n");

  uset_clear(s);

  return 0;
}
