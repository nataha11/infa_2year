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
    //get system page size
    size_t page_size = (size_t)sysconf(_SC_PAGE_SIZE);
    //create named shared memory or open existng one
    int fd = shm_open("/test-mem-area", O_RDWR | O_CREAT, 0600);
    if(fd < 0) {
        perror("shm_open");
        return 1;
    }
    
    if(ftruncate(fd, page_size) < 0) {
        perror("ftruncate");
        close(fd);
        return 1;
    }

    void * buf = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(buf == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    shm_unlink("/test-mem-area");
    close(fd);

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