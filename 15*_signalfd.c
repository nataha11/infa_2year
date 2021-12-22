#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <string.h>

void proc_info(void) {
    printf("PID %d, PPID %d, PGID %d, SID %d, UID %d, GID %d\n", getpid(), getppid(), getpgrp(), getsid(0), getuid(), getgid());
}

int main(void) {
    
    int result = 0;
    
    proc_info();
    
    sigset_t mask;
    if(sigfillset(&mask) == -1) {//add all signals
        perror("sigfillset");
        return 1;
    }

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
       perror("sigprocmask");
       return 1;
    }

    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1) {
       perror("signalfd");
       return 1;
    }
    
    struct signalfd_siginfo fdsi;
    
    ssize_t siginfo_size = sizeof(struct signalfd_siginfo);

    while(1) {
        ssize_t bytes_read = read(sfd, &fdsi, (size_t)siginfo_size);
        if (bytes_read != siginfo_size) {
            if(bytes_read == -1) {
                perror("read");
                result = 3;
            }
            break;
        }
        printf("%s\n", strsignal((int)fdsi.ssi_signo));
        printf("Received signal %d (%s)\n", fdsi.ssi_signo, strsignal((int)fdsi.ssi_signo));
    }
    
    if(close(sfd) == -1) {
        perror("close");
        result = 1;
    }
    return result;
}
