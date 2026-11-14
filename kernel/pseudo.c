#include "types.h"
#include "spinlock.h"
#include "file.h"
#include "defs.h"

static struct {
  struct spinlock lock;
  uint64 count;
} nullstat;

static int
pseudoread(int user_dst, uint64 dst, int n, int minor)
{
  char zeros[64] = {0};
  uint64 count;
  int done;
  int part;

  if (n < 0)
    return -1;

  switch (minor) {
  case DEV_NULL:
    return 0;

  case DEV_ZERO:
    done = 0;

    while (done < n) {
      part = n - done;

      if (part > (int)sizeof(zeros))
        part = sizeof(zeros);

      if (either_copyout(user_dst,
                         dst + done,
                         zeros,
                         part) < 0)
        return -1;

      done += part;
    }

    return n;

  case DEV_NULLSTAT:
    // Счётчик имеет размер ровно 8 байт.
    if (n != (int)sizeof(uint64))
      return -1;

    acquire(&nullstat.lock);
    count = nullstat.count;
    release(&nullstat.lock);

    if (either_copyout(user_dst,
                       dst,
                       &count,
                       sizeof(count)) < 0)
      return -1;

    return sizeof(count);

  default:
    // Такого minor у этого драйвера нет.
    return -1;
  }
}

static int
pseudowrite(int user_src, uint64 src, int n, int minor)
{
  // Эти устройства не используют содержимое записываемого буфера.
  (void)user_src;
  (void)src;

  if (n < 0)
    return -1;

  switch (minor) {
  case DEV_NULL:
    // Данные отброшены, но запись считается успешной.
    return n;

  case DEV_ZERO:
    // Запись в zero запрещена.
    return -1;

  case DEV_NULLSTAT:
    // Данные отбрасываем, но считаем их количество.
    acquire(&nullstat.lock);
    nullstat.count += n;
    release(&nullstat.lock);

    return n;

  default:
    return -1;
  }
}

void
pseudoinit(void)
{
  initlock(&nullstat.lock, "nullstat");
  nullstat.count = 0;

  devsw[PSEUDO].read = pseudoread;
  devsw[PSEUDO].write = pseudowrite;
}
