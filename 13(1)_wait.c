#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void proc_info(const char * procname) {
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

int main(void) {
    pid_t child_id = fork();
    if (child_id < 0) {
        perror("fork");
        return 1;
    }

    if (child_id == 0) {
        //this code is executed in child process only
        proc_info("child try 1");
        sleep(1);

        //comment out to kill child by signal
        kill(getpid(), SIGTERM);
        proc_info("child try 2");
        return 0;
    }

    //this code is executed in parent process only
    proc_info("parent");
    int status;
    pid_t res = wait(&status); // wait returns the process ID of the terminated child
    if(res == -1) {
        perror("wait failed");
        return 2;
    }

    if(WIFEXITED(status)) {
        printf("child with PID = %d has exited normally with code %d\n", res, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        if(WCOREDUMP(status))
            printf("child with PID = %d  produced a core dump\n", res);
        else 
            printf("child with PID = %d has been killed by signal %d\n", res, WTERMSIG(status));
    } else {
        printf("child with PID = %d has exited with termination status %d\n", res, status);
    }

    return 0;
}

