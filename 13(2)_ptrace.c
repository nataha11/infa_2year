#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <assert.h>

void proc_info(const char * procname) {
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

int main(void) {
    pid_t child_id = fork();
    if (child_id < 0) {
        perror("fork");
        return 1;
    }
    int status;

    if (child_id == 0) {
        //this code is executed in child process only
        proc_info("child try");
        pid_t tracee = getppid();
        printf("child: parent pid is %d\n", tracee);
        sleep(1); // give parent time to set readme
        if(0 != ptrace(PTRACE_SEIZE, tracee)) {
            perror("ptrace");
            return 1;
        }
        if(tracee != waitpid(tracee, &status, 0))
        return 0;
    }
    
    //this code is executed in parent process only
    proc_info("parent");   
        printf("parent: child pid is %d\n", child_id);
        assert(child_id == wait(&status));
        printf("parent: child terminated?\n");
        assert(0 == status);
        return 0;
}

