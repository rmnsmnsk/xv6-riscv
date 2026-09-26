#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    int myPipe[2];

    pipe(myPipe);

    int pid = fork();

    if (pid == 0) {
        close(myPipe[1]);

        char buff[100];

        int n = read(myPipe[0], buff, 100);

        while (n != 0) {
            write(1, buff, n);
            n = read(myPipe[0], buff, 100);
        }

        close(myPipe[0]);
    }


    close(myPipe[0]);

    for (int i = 1; i < argc; i++) {
        write(myPipe[1], argv[i], strlen(argv[i]));
        write(myPipe[1], "\n", 1);
    }
    close(myPipe[1]);

    int status;
    wait(&status);
    exit(status);
}