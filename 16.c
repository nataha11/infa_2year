#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <time.h>
#include <string.h>

int main(void) {
    const char * queue_name = "/test-mqueue";
    //create queue or open existing one
    mqd_t queue_fd = mq_open(queue_name, O_RDWR | O_CREAT, 0600, NULL);
    if(queue_fd == (mqd_t)-1) {
        perror("mq_open");
        return 1;
    }

    //send test message
    if(mq_send(queue_fd, "hello", strlen("hello"), 0) < 0) {
        perror("failed to send message");
        return 1;
    }

    //get and show queue info
    struct mq_attr m_info;
    mq_getattr(queue_fd, &m_info);
    printf("Flags : %ld\n", m_info.mq_flags);
    printf("Max. # of messages on queue : %ld\n", m_info.mq_maxmsg);
    printf("Max. message size (bytes) : %ld\n", m_info.mq_msgsize);
    printf("# of messages currently in queue : %ld\n", m_info.mq_curmsgs);

    //Cleanup
    mq_close(queue_fd);
    mq_unlink(queue_name);

    return 0;
}

/*
    struct mq_attr {
        long mq_flags;
        long mq_maxmsg;
        long mq_msgsize;
        long mq_curmsgs;
    };*/