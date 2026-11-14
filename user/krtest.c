#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  char *a, *b, *c;

  printf("K&R test\n");

  a = kr_malloc(20);
  vmprint();
  b = kr_malloc(100);
  vmprint();
  c = kr_malloc(8000);
  vmprint();

  if (!a || !b || !c) {
    printf("krtest: malloc failed\n");
    exit(1);
  }

  a[0] = 1;
  a[19] = 2;
  b[0] = 3;
  b[99] = 4;
  c[0] = 5;
  c[7999] = 6;

  kr_free(a);
  vmprint();
  kr_free(b);
  vmprint();
  kr_free(c);
  vmprint();

  printf("krtest: OK\n");
  exit(0);
}
