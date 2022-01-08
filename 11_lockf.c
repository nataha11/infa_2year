#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int execution(FILE * fs, int fd) {
    //block file, read value, clear file, write ++value, unblock file
    
    rewind(fs);//set the file position indicator to the beginning of the file so that
               //lockf(, , 0) will block the whole file (starting from the beginning)
    if(lockf(fd, F_LOCK, 0) == -1) {//exclusive lock also for fscanf not to read invalid value
        perror("lockf");
        return 1;
    }
    printf("Running\n");
    int current_count;
    errno = 0;
    int fsc_res = fscanf(fs, "%d", &current_count);
    printf("fsc_res = %d\n", fsc_res);

    if (fsc_res == EOF && errno == 0) {
            current_count = 0;//no executions before
    } else if (fsc_res == 1) {//current_count = number of executions
        //clear file
        if (ftruncate(fd, 0) == -1) {
            perror("ftruncate");
            return 1;
        }        
        //set the file position indicator to the beginning of the file
        rewind(fs);
    } else {//fsc_res == 0 (matching error) or (fsc_res = EOF) && (errno != 0) (read error)
        perror("fscanf");
        return 1;
    }
    
    printf("current_count = %d\n", current_count);
    printf("++current_count = %d\n", ++current_count);
    if(fprintf(fs, "%d", current_count) < 0) {
        perror("fprintf");
        return 1;
    }
    //fprintf leaves offset at the beginning of the file, no rewind(fs) needed
    if(lockf(fd, F_ULOCK, 0) == -1) {
        perror("lockf");
        return 1;
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    
    if (argc > 2) {
        printf("Usage: %s [pathname]\n", argv[0]);
    }
    const char * pathname = (argc == 2) ? argv[1] : "11counter.txt";
    
    //0644 - r/w for user, read-only for group and others
    int fd = open(pathname, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    FILE * fs = fdopen(fd, "r+");//read/write
    if (fs == NULL) {
        perror("fdopen");
        close(fd);
        return 1;
    }

    if(execution(fs, fd) != 0) {
        fprintf(stderr, "execution failed\n");
        fclose(fs);
        return 1;
    }

    if(fclose(fs) == EOF) {
        perror("fclose");
        return 1;
    }

    return 0;
}


