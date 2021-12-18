#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <time.h>
#include <string.h>

int main(void) {
    
    int result = 0;
    
    const char * queue_name = "/test-mqueue";

    //Create queue or open existing one
    mqd_t queue_fd = mq_open(queue_name, O_RDWR | O_CREAT, 0600, NULL);
    if(queue_fd == -1) {
        perror("mq_open");
        return 1;
    }

    //Send 2 test messages
    if(mq_send(queue_fd, "hello1", strlen("hello1"), 0) < 0) {
        perror("mq_send1");
        mq_close(queue_fd);
        mq_unlink(queue_name);        
        return 25;
    }
    if(mq_send(queue_fd, "hello2", strlen("hello2"), 0) < 0) {
        perror("mq_send2");
        mq_close(queue_fd);
        mq_unlink(queue_name);
        return 25;
    }

    //Get and show queue info
    struct mq_attr m_info;
    if(mq_getattr(queue_fd, &m_info) == -1) {
        perror("mq_getattr");
        mq_close(queue_fd);
        mq_unlink(queue_name);
        return 2;
    }
    
    printf("Flags : %ld\n", m_info.mq_flags); //only 0 or O_NONBLOCK can appear
    printf("Max. # of messages on queue : %ld\n", m_info.mq_maxmsg);
    printf("Max. message size (bytes) : %ld\n", m_info.mq_msgsize);
    printf("# of messages currently in queue : %ld\n", m_info.mq_curmsgs); //2

    //Cleanup
    if(mq_close(queue_fd) == -1) {
        perror("mq_close");
        result = 3;
    }
    if(mq_unlink(queue_name) == -1) {
        perror("mq_unlink");
        result = 4;
    }
    
    return result;
}