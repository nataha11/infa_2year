#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 65

volatile int g_last_signal;

void proc_info(void) {
    printf("PID %d, PPID %d, UID %d, GID %d, PGID %d, SID %d\n", getpid(), getppid(), getuid(), getgid(), getpgrp(), getsid(0));
    printf("EUID %d, EGID %d\n", geteuid(), getegid());
}

void sig_handler(int signum) {
    g_last_signal = signum;
}

int main(void) {

    proc_info();
    
    for(int i = 0; i < N; i++) {
        signal(i, sig_handler);
    }

    while(1) {
        pause();
        printf("last_signal = %d (%s)\n", g_last_signal, strsignal(g_last_signal));
    }

    return 0;
}
