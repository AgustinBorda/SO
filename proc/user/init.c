#include "user.h"

int main(void)
{
    int pid = fork();

    if (pid == 0) {
        /* in child process */
        print("Hello from child\n");
    } else if (pid > 0) {
        print("Hello from init: waiting for child finish.\n");
        wait();
        print("Child finished!\n");
        print("Parent init exiting...\n");
    } else {
        print("fork() failed!\n");
    }
    exit();
}
