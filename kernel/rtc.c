#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "defs.h"

#define RTC_LOW  0x00
#define RTC_HIGH 0x04

void rtcinit(void) {
    devsw[RTC].read = rtcread;
    devsw[RTC].write = rtcwrite;
}

int
rtcread(int user_dst, uint64 dst, int n, int minor)
{
    if (n != 8) 
        return -1;

    uint32 low = *(volatile uint32*)(RTC0 + RTC_LOW);
    uint32 high = *(volatile uint32*)(RTC0 + RTC_HIGH);

    uint64 time = ((uint64)high << 32) | low;

    if (either_copyout(user_dst, dst, &time, sizeof(time)) < 0)
        return -1;

    return 8;
}

int
rtcwrite(int user_src, uint64 src, int n, int minor)
{
    return -1;
}