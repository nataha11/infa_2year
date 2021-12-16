#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 65

volatile int g_last_signal;

void proc_info(void) {
    printf("PID %d, PPID %d, PGID %d, SID %d, UID %d, GID %d\n", getpid(), getppid(), getpgrp(), getsid(0), getuid(), getgid());
}

void sig_handler(int signum) {
    g_last_signal = signum;
}

int main(void) {

    proc_info();
    
    for(int i = 0; i < N; i++) {
        struct sigaction sig_act;
        memset(&sig_act, 0, sizeof(sig_act));
        sig_act.sa_handler = sig_handler;
        sigaction(i, &sig_act, NULL);
    }

    while(1) {
        pause();
        printf("last_signal = %d (%s)\n", g_last_signal, strsignal(g_last_signal));
    }

    return 0;
}
