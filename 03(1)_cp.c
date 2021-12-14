#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int main(int argc, char const *argv[]) {
    int result = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s src dest\n", argv[0]);
        return 1;
    }

    struct stat sb;
    if (lstat (argv[1], &sb) == -1) {
        perror("lstat failed");
        return 2;
    }

    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        perror("Not a regular file");
        return 3;
    }

    int fd1 = open (argv[1], O_RDONLY);
    if (fd1 < 0) {
        perror("Failed to open src file");
        return 4;
    }
    //0644 - r/w for user, read-only for group and others
    int fd2 = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd2 < 0) {
        perror("Failed to open or create dest file");
        close(fd1);
        return 5;
    }

    char buf[sb.st_size];//файл может не влезть, копировать в цикле блоками (чтение + цикл записи writeall)

    if (read(fd1, &buf, sb.st_size) == -1) {
        perror("Failed to read src file");
        return 6;
    }

    if (dprintf(fd2, "%s", buf) != sb.st_size) {/////////////
        perror ("dprintf failed");
        result = 7;
    }

    if (close(fd1) < 0) {
        perror("Closing src file failed");
        result = 8;
    }

    if (close(fd2) < 0) {
        perror("Closing dest file failed");
        result = 9;
    }

    return result;
}