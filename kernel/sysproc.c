#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

static void
line_print(int level)
{
  for (int i = 0; i < level; i++)
    printk(".........");
}

static void
flags_print(uint64 page)
{
  char flags[8];

  flags[0] = (page & PTE_R) ? 'R' : '_';
  flags[1] = (page & PTE_W) ? 'W' : '_';
  flags[2] = (page & PTE_X) ? 'X' : '_';
  flags[3] = (page & PTE_U) ? 'U' : '_';
  flags[4] = (page & PTE_G) ? 'G' : '_';
  flags[5] = (page & PTE_A) ? 'A' : '_';
  flags[6] = (page & PTE_D) ? 'D' : '_';
  flags[7] = '\0';

  printk("%s", flags);
}

static void
vmprint_walk(pagetable_t pagetable, int level)
{
  for (int i = 0; i < 512; i++) {
    uint64 page = pagetable[i];
    
    if (page & PTE_V) { // страница используется, а не пустая
      line_print(level);
      printk("0x%lx -> %p ", (uint64)i, (void *)PTE2PA(page));
      flags_print(page);
      printk("\n");
      
      if ((page & (PTE_R | PTE_W | PTE_X)) == 0) // ссылка на таблицу следующего уровня, а не данные
        vmprint_walk((pagetable_t)PTE2PA(page), level + 1);
    }
  }
}

uint64
sys_vmprint(void) 
{
  struct proc *p = myproc();
  if (p == 0)
    return -1;

  printk("PAGETABLE %p\n", (void *)p->pagetable);
  
  vmprint_walk(p->pagetable, 0);

  return 0;  
}
