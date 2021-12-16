#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>

void proc_info(void) {

    printf("PID %d, PPID %d\n", getpid(), getppid());
    printf("\tPGID %d, SID %d\n", getpgrp(), getsid(0));
    
    uid_t uid = getuid();
    gid_t gid = getgid();
    //чтобы не было разыменования нулевого указателя
    char * unknown = malloc(sizeof("?"));
    strcpy(unknown, "?");
    struct passwd * pwuid = getpwuid(uid);
    char * pwname = pwuid ? pwuid->pw_name : unknown;
    struct group * grid = getgrgid(gid);
    char * grname = grid ? grid->gr_name : unknown;

    printf("\tUID %d(%s), GID %d(%s)\n", uid, pwname, gid, grname);

    uid_t euid = geteuid();
    gid_t egid = getegid();
    //чтобы не было разыменования нулевого указателя
    struct passwd * epwuid = getpwuid(euid);
    char * epwname = epwuid ? epwuid->pw_name : unknown;
    struct group * egrid = getgrgid(egid);
    char * egrname = egrid ? egrid->gr_name : unknown;

    printf("\tEUID %d(%s), EGID %d(%s)\n", euid, epwname, egid, egrname);

    //get supplementary groups
    // If size is 0, the total number of supplementary group IDs for the process is returned.
    int ngroups = getgroups(0, NULL);

    gid_t list[ngroups];
    int num_groups;
    if ((num_groups = getgroups(ngroups + 1, list)) == -1) {
        perror("getgroups");
        return;
    }

    printf("Supplementary groups(%d): ", num_groups);
    for (int i = 0; i < num_groups; i++) {
        //чтобы не было разыменования нулевого указателя
        struct group * ngrid = getgrgid(list[i]);
        char * grname = ngrid ? ngrid->gr_name : unknown;
        printf("%d(%s) ", list[i], grname);
    }
    printf("\n");
    free(unknown);
}

int main(void) {
    proc_info();
    return 0;
}