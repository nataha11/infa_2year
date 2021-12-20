#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <poll.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 4096

static void handle_events(int fd, const char * path) {

   char buf[BUFSIZE];
   
    /* Loop while events can be read from inotify file descriptor. */
    for (;;) {
        errno = 0;
        // Read some events.
        ssize_t len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(1);
        }

        /* If the nonblocking read() found no events to read, then
          it returns -1 with errno set to EAGAIN. */
        if (len <= 0)
            break;

        /* Loop over all events in the buffer */
        const struct inotify_event * event;
        for (char * ptr = buf; ptr < buf + len;
                ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *)ptr;

            /* Print what was created */ 
            if (event->mask & IN_CREATE)
                printf("Created: %s/%s", path, event->len ? event->name : "");
           
            if (event->mask & IN_ISDIR) {
                printf("[directory]\n");
            } else {
                printf("[file]\n");
            }           
        }
    }
}

int main(int argc, const char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s path\n", argv[0]);
        return 1;
    }

    printf("Press ENTER key to terminate\n"); 
    /* Create the file descriptor for accessing the inotify API */
    int fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        return 1;
    }

    int wd = inotify_add_watch(fd, argv[1], IN_CREATE | IN_MOVE); //File/directory created in watched directory
    if (wd == -1) {
        fprintf(stderr, "Cannot watch '%s': %s\n", argv[1], strerror(errno));
        return 1; 
   }
    /* Prepare for polling */
    nfds_t nfds = 2;

    /* Console input */
    struct pollfd fds[2];
    
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Inotify input */
    fds[1].fd = fd;
    fds[1].events = POLLIN;

    /* Wait for events and/or terminal input */
    printf("Listening for events in %s\n", argv[1]);
    
    char buf;
    while (1) {
        int poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR) //interrupted by a signal
                continue;
            perror("poll");
            return 1;
        }
        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                /* Console input is available. Empty stdin and quit */
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }
            if (fds[1].revents & POLLIN) {
                /* Inotify events are available */
                handle_events(fd, argv[1]);
            }
        }
    }

    printf("Listening for events stopped.\n");

    if(close(fd) == -1) {
        perror("close");
        return 1;
    }
    
    return 0;
}      

