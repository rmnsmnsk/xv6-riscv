#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char *hex = "0123456789abcdef";
  int n = atoi(argv[1]);
  int fd = open(argv[2], O_RDONLY);
  uchar *buf = malloc(n);
  int readn = read(fd, buf, n);
  int i;

  if (readn < 0) {
    printf("Read error\n");
    exit(1);
  }

  for (i = 0; i < readn; i++) {
    if (i > 0)
      printf(" ");

    printf("%c%c", hex[buf[i] >> 4], hex[buf[i] & 15]);
  }

  if (readn > 0)
    printf("\n");

  exit(0);
}
