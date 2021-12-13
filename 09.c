#include <stdio.h>
#include <sys/statfs.h>
#include <sys/types.h>

int main(int argc, char const *argv[]) {

    if(argc != 2) {
        printf("Usage: %s pathname\n", argv[0]);
        return 1;
    }

    struct statfs stf;

    if(statfs(argv[1], &stf) == -1) {
        perror("statfs");
        return 2;
    }
    printf("Block size: %lu\n", stf.f_bsize);
    printf("Total (bytes): %lu\n", stf.f_blocks * (unsigned int)stf.f_bsize);
    printf("Free (bytes): %lu\n", stf.f_bfree * (unsigned int)stf.f_bsize);
    printf("Free for unprivileged users (bytes): %lu\n", stf.f_bavail * (unsigned int)stf.f_bsize);
    printf("Used (bytes): %lu\n", (stf.f_blocks - stf.f_bfree) * (unsigned int)stf.f_bsize);

    return 0;
}