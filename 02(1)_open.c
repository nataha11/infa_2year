#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

ssize_t writeall(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    const uint8_t *buf_addr = buf;
    while (bytes_written < count) {
        ssize_t res = write(fd, buf_addr + bytes_written, count - bytes_written);
        if (res < 0) {
            return res;
        }
        bytes_written += (size_t)res;
    }
    return (ssize_t)bytes_written;
}

int main(int argc, char *argv[]) {

    int result = 0;
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s path text\n", argv[0]);
        return 1;
    }
    //0644 - r/w for user, read-only for group and others
    int fd = open (argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0) {
        perror("Failed to open file for writing");
        return 2;
    }

    if (writeall(fd, argv[2], strlen(argv[2])) < 0) {
        perror ("Failed to write");
        result = 3;
    }

    if (close(fd) < 0) {
        perror("Failure during close");
        result = 4;
    }

    return result;
}
