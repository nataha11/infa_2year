#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void proc_info(void) {
    printf("PID %d, PPID %d, UID %d, GID %d, PGID %d, SID %d\n", getpid(), getppid(), getuid(), getgid(), getpgrp(), getsid(0));
    printf("EUID %d, EGID %d\n", geteuid(), getegid());
}

int main(void) {
    proc_info();
    return 0;
}
