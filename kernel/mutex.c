#include "file.h"
#include "sleeplock.h"

int mutexalloc(struct file **f) {
    *f = filealloc();
    if (f == 0) {
        printk("Error with filealloc");
        return -1;
    }

    struct sleeplock *sl = (struct sleeplock*)kalloc();
    if (sl == 0) {
        fileclose(*f);
        printk("Error with kalloc");
        return -1;
    }

    initsleeplock(sl, "sleep lock");

    (*f)->type = FD_MUTEX;
    (*f)->readable = 0;
    (*f)->writable = 0;
    (*f)->mutex = sl;
    printk("mutex was allocated");
    return 0;
}

void mutexclose(struct sleeplock *sl) {
    kfree(sl);
    sl = 0;
    printk("mutex was closed\n");
}
