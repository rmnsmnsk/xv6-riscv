#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
    int p[2];
    int status;

    pipe(p);
    
    if(fork() == 0) {
      close(0);
      dup(p[0]);

      close(p[0]);
      close(p[1]);
      char* argvs[] = {"/wc", 0};
      exec("/wc", argvs);
    }
    
    close(p[0]);

    for (int i = 1; i < argc; i++) {
        write(p[1], argv[i], strlen(argv[i]));
        write(p[1], "\n", 1);
    }

    close(p[1]);

    wait(&status);
    
    exit(status);
}
