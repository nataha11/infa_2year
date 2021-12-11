#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <grp.h>
#include <pwd.h>

void proc_info(void) {

    printf("PID %d, PPID %d\n", getpid(), getppid());
    printf("\tPGID %d, SID %d\n", getpgrp(), getsid(0));    
    printf("\tUID %d(%s), GID %d(%s)\n", getuid(), getpwuid(getuid())->pw_name, getgid(), getgrgid(getgid())->gr_name);    
    printf("\tEUID %d(%s), EGID %d(%s)\n", geteuid(), getpwuid(geteuid())->pw_name, getegid(), getgrgid(getegid())->gr_name);

    //get supplementary groups
    long ngroups_max;//65536
    if((ngroups_max = sysconf(_SC_NGROUPS_MAX)) == -1) {
        perror("sysconf");
        return;        
    }
    gid_t list[ngroups_max];
    int num_groups;
    if ((num_groups = getgroups((int)ngroups_max + 1, list)) == -1) {
        perror("getgroups");
        return;
    }
    //вопрос: supplementary groups у процесса это то же самое, что группы юзера-владельца процесса?
    printf("Supplementary groups(%d): ", num_groups);
    for (int i = 0; i < num_groups; i++) {
        printf("%d(%s) ", list[i], getgrgid(list[i])->gr_name);
    }
    printf("\n");     
}

int main(void) {
    proc_info();
    return 0;
}

//а man su тут зачем?