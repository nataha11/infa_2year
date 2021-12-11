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
    printf("Total data blocks in filesystem: %lu\n", stf.f_blocks);
    printf("Number of free blocks: %lu\n", stf.f_bfree);
    printf("Number of free blocks for unprivileged users: %lu\n", stf.f_bavail);
    printf("Number of free blocks used: %lu\n", stf.f_blocks - stf.f_bfree);

    return 0;
}