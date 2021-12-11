#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    int result = 0;

    if (argc != 3) {
        printf("Usage: %s filename string\n", argv[0]);
        return 1;
    }   
    //0644 - r/w for user, read-only for group and others
    int fd = open(argv[1], O_RDWR|O_CREAT|O_APPEND, 0644);
    if (fd < 0) {
        perror("File can't be opened or created");
        return 2;
    }   

    if (dprintf(fd, "%s", argv[2]) != (int)strlen(argv[2])) {
        perror("dprintf failed");
        result = 3;
    }

    if (close(fd) == -1) {
        perror("Failed to close");
        result = 4;
    }

    return result;
} 