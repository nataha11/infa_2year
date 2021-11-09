#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s src dest\n", argv[0]);
        return 1;
    }

	struct stat sb;
    if (lstat (argv[1], &sb) == -1) {
        perror("lstat failed");
        return 2;
    }

    if (((sb.st_mode) & (S_IFMT)) != S_IFREG) {
        perror("Not a regular file");
        return 3;
    }

    int fd1 = open (argv[1], O_RDONLY);
    if (fd1 < 0) {
        perror("Failed to open src file");
        return 4;
    }

    int fd2 = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd2 < 0) {
        perror("Failed to open or create dest file");
        return 5;
    }
    char buf[sb.st_size];
    ssize_t nbytes, nbytes_w;    

	off_t offset_r = 0, offset_w = 0;
	while ((nbytes = pread(fd1, buf, sb.st_size, offset_r)) > 0) {
		offset_r += nbytes;
		while (nbytes) {
			nbytes_w = pwrite(fd2, buf, nbytes, offset_w);
			if (nbytes_w == -1) {
				perror("pwrite failed");
				return 6;
			}
			nbytes -= nbytes_w;
			offset_w += nbytes_w;
		}
	}
	if (nbytes == -1) {
		perror("pread failed");
		return 7;
	}

    if (close(fd1) < 0) {
        perror("Closing src file failed");
        return 8;
    }

    if (close(fd2) < 0) {
        perror("Closing dest file failed");
        return 9;
    }
        return 0;
}
