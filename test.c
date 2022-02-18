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

int main(int argc, char *argv[])
{
  int datac = 10;
  long *datav = malloc(datac * sizeof(long));

  printf("datac: %d\n", datac);
  printf("datav: [ ");
  for (int i = 0; i < datac; ++i) {
    datav[i] = rand() % 10;
    printf("%ld ", datav[i]);
  }
  printf("]\n");

  uset_t *s = uset_create(datac, (void **)datav);

  printf("every element > 0? %d\n", uset_every(s, gtz));
  printf("every element is odd? %d\n", uset_every(s, odd));
  printf("some elements < 0? %d\n", uset_some(s, ltz));
  printf("some elements are odd? %d\n", uset_some(s, odd));

  printf("show hex:\n");
  uset_foreach(s, show_hex);

  void **entries = uset_entries(s);
  int entries_sz = s->size;
  printf("entries: [ ");
  for (int i = 0; i < entries_sz; ++i)
    printf("%ld ", (long)entries[i]);
  printf("]\n");

  printf("deleting odd elements:\n");
  for (int i = 0; i < entries_sz; ++i) {
    if (odd(entries[i])) {
      printf("deleting %ld: %d\n", (long)entries[i],
             uset_delete(s, entries[i]));
    }
  }
  printf("some elements are odd? %d\n", uset_some(s, odd));

  printf("show hex:\n");
  uset_foreach(s, show_hex);

  uset_clear(s);
  uset_free(s);
  free(entries);

  return 0;
}
