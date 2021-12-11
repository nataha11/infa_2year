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
        proc_info("child try 1");
        pause();//waiting for signal(s)
        return 0;//any value as exit code
    }

    //this code is executed in parent process only
    proc_info("parent");
    int status;
    do {
        pid_t res = waitpid(child_id, &status, WUNTRACED | WCONTINUED); 
        //waitpid returns the process ID of the terminated\resumed\stopped child
        if(res == -1) {
            perror("waitpid");
            return 2;
        }
//todo: rewrite the following code
        if(WIFEXITED(status)) {
            printf("child with PID = %d has exited normally with code %d\n", res, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            if(WCOREDUMP(status))
                printf("child with PID = %d  produced a core dump\n", res);
            else 
                printf("child with PID = %d has been killed by signal %d\n", res, WTERMSIG(status));
        } else if (WIFSTOPPED(status))
            printf("child with PID = %d has been stopped by signal %d\n", res, WSTOPSIG(status));
        else if (WIFCONTINUED(status))
            printf("child with PID = %d continued\n", res);

    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return 0;
}

