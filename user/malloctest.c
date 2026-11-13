#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  void *a;
  void *b;
  void *c;

  a = malloc(20);
  vmprint();
  b = malloc(100);
  vmprint();
  c = malloc(8000);
  vmprint();

  free(a);
  vmprint();
  free(b);
  vmprint();
  free(c);
  vmprint();

  exit(0);
}
