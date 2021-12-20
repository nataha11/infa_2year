#define _GNU_SOURCE
#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

struct linux_dirent64 {
    ino64_t        d_ino;    /* 64-bit inode number */
    off64_t        d_off;    /* 64-bit offset to next structure */
    unsigned short d_reclen; /* Size of this dirent */
    unsigned char  d_type;   /* File type */
    char           d_name[]; /* Filename (null-terminated) */
};

#define BUF_SIZE    1024

#define handle_error(msg)\
        do{ perror(msg); exit(1); } while(0)

const char * str_dtype(unsigned char type) {

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

int main(int argc, char const *argv[]) {

    int fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);

    if(fd == -1) {
        handle_error("open");
    }
    char buf[BUF_SIZE];
    for( ; ; ) {

        long nread = syscall(SYS_getdents64, fd, buf, BUF_SIZE);

        if(nread == -1) {
            handle_error("getdents64");
        }

        if(nread == 0) {
            break;
        }

        printf("--------------- nread=%ld ---------------\n", nread);
        printf("inode#            \tfile type  d_reclen  d_off      d_name\n");
        
        for(int bpos = 0; bpos < nread; ) {

            struct linux_dirent64 * d = (struct linux_dirent64*)(buf + bpos);
            printf("%8lu \t", d->d_ino);

            char d_type = *(buf + bpos + d->d_reclen - 1);
            printf("%-8s\t", str_dtype(d_type));//On kernels up to and including  2.6.3,  attempting  to  access this field always provides the value 0 (DT_UNKNOWN) -- default in str_type.

            printf("%d %6lld \t%s\n", d->d_reclen, (long long)d->d_off, d->d_name);

            bpos += d->d_reclen;
        }
    }

    return 0;
}



