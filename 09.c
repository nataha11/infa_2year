#include <stdio.h>
#include <sys/statvfs.h>
#include <sys/types.h>

int main(int argc, char const *argv[]) {

    if(argc != 2) {
        printf("Usage: %s pathname\n", argv[0]);
        return 1;
    }

    struct statvfs sv;

    if(statvfs(argv[1], &sv) == -1) {
        perror("statvfs");
        return 2;
    }

    printf("Total (bytes): %lu\n", sv.f_blocks * (unsigned int)sv.f_frsize);
    printf("Free (bytes): %lu\n", sv.f_bfree * (unsigned int)sv.f_frsize);
    printf("Free for unprivileged users (bytes): %lu\n", sv.f_bavail * (unsigned int)sv.f_frsize);
    printf("Used (bytes): %lu\n", (sv.f_blocks - sv.f_bfree) * (unsigned int)sv.f_frsize);

    return 0;
}