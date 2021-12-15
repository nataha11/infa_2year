#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

    #define STACK_SIZE (1024 * 1024)    /* Stack size for cloned child */

void proc_info(const char * procname) {
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

static int childFunc(void * arg) {
    proc_info(arg);

    sleep (20);
    return 0;
}

int main(void) {


    char *stack;                    /* Start of stack buffer */
    char *stackTop;                 /* End of stack buffer */

    
    /* Allocate memory to be used for the stack of the child */

    stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        return 3;
    }

    stackTop = stack + STACK_SIZE;  /* Assume stack grows downward */   
    char * argument = malloc(sizeof("arggs child"));
    strcpy(argument, "arggs child");
    pid_t child_id = clone(childFunc, stackTop, CLONE_PARENT | SIGCHLD, argument);

    if (child_id == -1) {
        perror("clone failed");
        return 1;
    }


    //this code is executed in parent process only
    proc_info("parent");
    pid_t res = wait(NULL); // wait returns the process ID of the terminated child
    if(res == -1) {
        perror("wait failed");
        return 2;
    }
    return 0;
}