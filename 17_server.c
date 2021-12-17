#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {

    int result = 0;

    if(argc != 2) {
        printf("Usage: %s /queue_name\n", argv[0]);
        return 1;
    }

    //Create new queue with name arvg[1]
    const char * queue_name = argv[1];
    mqd_t queue_fd = mq_open(queue_name, O_RDWR | O_CREAT, 0600, NULL);
    if(queue_fd == -1) {
        perror("mq_open");
        return 2;
    }
    
    //Get queue info
    struct mq_attr m_info;
    if(mq_getattr(queue_fd, &m_info) == -1) {
        perror("mq_getattr");
        mq_close(queue_fd);
        mq_unlink(queue_name);
        return 3;
    }
    unsigned int mssize = (unsigned int)m_info.mq_msgsize;
    
    char * buf = malloc(mssize);
    
    int i = 0;
    while(i < 4) { //while(1), 4 just for test
        if(mq_receive(queue_fd, buf, mssize, NULL) == -1) {
            perror("mq_receive");
            result = 4;
            break;
        }
        printf("%s", buf); //valgrind hates this printf
        i++;
    }

    //Cleanup
    free(buf);
    if(mq_close(queue_fd) == -1) {
        perror("mq_close");
        result = 5;
    }
    if(mq_unlink(queue_name) == -1) {
        perror("mq_unlink");
        result = 6;
    }
    
    return result;
}
