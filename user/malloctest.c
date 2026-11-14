#include "kernel/types.h"
#include "user/user.h"

struct node {
  int value;
  struct node *next;
};

int
main(void)
{
  struct node *a, *b, *c, *d;
  char *big;
  char *heap_before = sbrk(0);
  char *heap_after_three;
  uint64 freed_address;

  printf("SFL test\n");
  malloc_debug(1);

  a = malloc(sizeof(*a));
  vmprint();
  b = malloc(sizeof(*b));
  vmprint();
  c = malloc(sizeof(*c));
  vmprint();

  if (!a || !b || !c) {
    printf("malloctest: malloc failed\n");
    exit(1);
  }

  a->value = 10;
  b->value = 20;
  c->value = 30;
  a->next = b;
  b->next = c;
  c->next = 0;

  if (a->value != 10 || a->next->value != 20 ||
      a->next->next->value != 30) {
    printf("malloctest: list is broken\n");
    exit(1);
  }

  heap_after_three = sbrk(0);
  freed_address = (uint64)b;
  free(b);
  vmprint();

  d = malloc(sizeof(*d));
  vmprint();

  if (!d || (uint64)d != freed_address || sbrk(0) != heap_after_three) {
    printf("malloctest: free block was not reused\n");
    exit(1);
  }

  d->value = 40;
  d->next = c;
  a->next = d;

  big = malloc(8000);
  vmprint();
  if (!big) {
    printf("malloctest: large malloc failed\n");
    exit(1);
  }
  big[0] = 1;
  big[7999] = 2;

  free(big);
  vmprint();
  free(c);
  vmprint();
  free(d);
  vmprint();
  free(a);
  vmprint();

  if (sbrk(0) != heap_before) {
    printf("malloctest: heap was not returned\n");
    exit(1);
  }

  printf("malloctest: OK\n");
  exit(0);
}
