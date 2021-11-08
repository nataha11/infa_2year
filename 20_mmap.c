#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>

int main(void) {

    void * buf = mmap(NULL, (size_t)sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE| MAP_ANONYMOUS, -1, 0);
    //кусок памяти, который просим, не ассоциирован ни с каким файлом (fd = -1);
    //MAP_PRIVATE или MAP_SHARED
    if(buf == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    char *x = buf;
    strcpy(x, "Hello");
    printf("A: %s\n", x);

    pid_t child_id = fork();
    if(child_id < 0) {
        perror("fork");
        return 1;
    }

    if(child_id == 0) {
        //this code is executed in child process only
        printf("B: %s\n", x);
        strcpy(x,"Some child process was here");
        printf("C: %s\n", x);
        return 0;
    }
    //wait for child prosess termination
    wait(NULL);
    printf("D: %s\n", x);
    return 0;
}