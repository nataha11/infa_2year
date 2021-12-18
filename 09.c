#include <stdio.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <stdint.h>

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
    //f_frsize  -- the size in bytes of the minimum unit of allocation on this file system. (f_bsize in struct statfs.)
    //f_bsize -- the preferred length of I/O requests for files on this file system. (f_iosize in struct statfs.)
    //On linux, f_frsize is rarely (if ever) different than f_bsize.

    printf("Total (bytes): %lu\n", sv.f_blocks * (uintmax_t)sv.f_frsize);
    printf("Free (bytes): %lu\n", sv.f_bfree * (uintmax_t)sv.f_frsize);
    printf("Free for unprivileged users (bytes): %lu\n", sv.f_bavail * (uintmax_t)sv.f_frsize);
    printf("Used (bytes): %lu\n", (sv.f_blocks - sv.f_bfree) * (uintmax_t)sv.f_frsize);

    return 0;
}