#define _GNU_SOURCE
#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>


struct linux_dirent {
    long            d_ino;
    off_t           d_off;
    unsigned short  d_reclen;
    char            d_name[];
};

#define BUF_SIZE    1024

#define handle_error(msg)\
        do{ perror(msg); exit(EXIT_FAILURE); } while(0)

const char * str_dtype(char type);

int main(int argc, char const *argv[]) {

    int fd, nread, bpos;
    char d_type;
    char buf[BUF_SIZE];
    struct linux_dirent* d;

    fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);

    if(fd == -1) {
        handle_error("open");
    }

    for( ; ; ) {

        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);

        if(nread == -1) {
            handle_error("getdents");
        }

        if(nread == 0) {
            break;
        }

        printf("--------------- nread=%d ---------------\n", nread);
        printf("inode#            \tfile type  d_reclen  d_off      d_name\n");

        for(bpos = 0; bpos < nread; ) {

            d = (struct linux_dirent*)(buf + bpos);
            printf("%8ld \t", d->d_ino);

            d_type = *(buf + bpos + d->d_reclen - 1);
            printf("%-8s\t", str_dtype(d_type));

            printf("%d %6lld \t%s\n", d->d_reclen, (long long)d->d_off, d->d_name);

            bpos += d->d_reclen;
        }
    }

    return 0;
}

const char * str_dtype(char type) {

    switch(type) {
        case DT_REG: return "regular";
        case DT_DIR: return "directory";
        case DT_FIFO: return "FIFO";
        case DT_SOCK: return "socket";
        case DT_LNK: return "symlink";
        case DT_BLK: return "block dev";
        case DT_CHR: return "char dev";
        default: return "unknown type";
    }
}

