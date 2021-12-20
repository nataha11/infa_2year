#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
        proc_info("child try");
        pause();//waiting for signal(s)
        return 0;
    }

    //this code is executed in parent process only
    proc_info("parent");
    int status;
    
    while(1) {
        pid_t res = waitpid(child_id, &status, WUNTRACED | WCONTINUED); 
        //waitpid returns the process ID of the terminated\resumed\stopped child
        if (res == -1) {
            perror("waitpid");
            return 2;
        }

        if (WIFEXITED(status)) {
            printf("child with PID = %d has exited normally with code %d\n", res, WEXITSTATUS(status));
            break;
        } else if (WIFSIGNALED(status)) {//child terminated
            if (WCOREDUMP(status))
                printf("child with PID = %d  produced a core dump\n", res);            
            printf("child with PID = %d has been killed by signal %d\n", res, WTERMSIG(status));
            break;
        } else if (WIFSTOPPED(status)) {
            printf("child with PID = %d has been stopped by signal %d\n", res, WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("child with PID = %d continued\n", res);
        } else {
            printf("Something weird has happened\n");
            break;
        }

    }

    return 0;
}

