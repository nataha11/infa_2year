#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <time.h>
#include <string.h>

int main(int argc, char const *argv[]) {

    if(argc != 3) {
        printf("Usage: path queue_fd message\n");
        return 1;
    }

    int result = 0;

    //open existing queue
    mqd_t queue_fd = mq_open(argv[1], O_WRONLY);
    if(queue_fd == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    //send test message
    if(mq_send(queue_fd, argv[2], strlen(argv[2]), 0) < 0) {
        perror("failed to send another message");
        result = 1;
    }

    //Cleanup
    mq_close(queue_fd);
    return result;
}
