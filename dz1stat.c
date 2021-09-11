#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

int main(int argc, char *argv[]) {
    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(1);
    }

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(1);
    }

    printf("File:                     %s\n", argv[1]);
    printf("Size:                     %lld bytes\n", (long long) sb.st_size); 
    printf("Blocks:                   %lld\n", (long long) sb.st_blocks);
    printf("IO Block:                 %ld bytes\n", (long) sb.st_blksize);

    printf("File type:                ");
    switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:  printf("block device\n");            break;
        case S_IFCHR:  printf("character device\n");        break;
        case S_IFDIR:  printf("directory\n");               break;
        case S_IFIFO:  printf("FIFO/pipe\n");               break;
        case S_IFLNK:  printf("symlink\n");                 break;
        case S_IFREG:  printf("regular file\n");            break;
        case S_IFSOCK: printf("socket\n");                  break;
        default:       printf("unknown?\n");                break;
    }

    printf("ID of containing device:  %ld/%ld\n", (long) major(sb.st_dev), (long) minor(sb.st_dev));

    printf("Inode:                    %ld\n", (long) sb.st_ino);

    //printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);

    printf("Links:                    %ld\n", (long) sb.st_nlink);
    printf("Access:                   %lo (octal)  Uid:%ld  Gid:%ld  \n", (unsigned long) sb.st_mode, (long) sb.st_uid, (long) sb.st_gid);
    printf("Access:                   %s", ctime(&sb.st_atime));
    printf("Modified:                 %s", ctime(&sb.st_mtime));
    printf("Change:                   %s", ctime(&sb.st_ctime));


    exit(0);
}



