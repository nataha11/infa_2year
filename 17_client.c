#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {

    int result = 0;

    if(argc != 2) {
        printf("Usage: %s /queue_name\n", argv[0]);
        return 1;
    }

    //Open existing queue
    mqd_t queue_fd = mq_open(argv[1], O_WRONLY); //Open the queue to send messages only
    if(queue_fd == -1) {
        perror("mq_open");
        if (errno == ENOENT) {
            printf("Queue %s does not exist\n", argv[1]);
        }
        return 2;
    }
    
    //Get queue info
    struct mq_attr m_info;
    if(mq_getattr(queue_fd, &m_info) == -1) {
        perror("mq_getattr");
        mq_close(queue_fd);
        return 3;
    }
    unsigned int mssize = (unsigned int)m_info.mq_msgsize; //8192 default
    char * buf = malloc(mssize);

    while(1) {
              
        size_t bytes_read = (size_t)read(STDIN_FILENO, buf, mssize);
        if((int)bytes_read == -1) {
            perror("read");
            result = 4;
            break;            
        }
        if(mq_send(queue_fd, buf, bytes_read, 0) < 0) {
            perror("failed to send message");
            result = 5;
            break;
        }
//todo: on receiving a signal (for instance ctrl C) break
    }

    //Cleanup
    free(buf);
    if(mq_close(queue_fd) == -1) {
        perror("mq_close");
        result = 6;
    }
    
    return result;
}
