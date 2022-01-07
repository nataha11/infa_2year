#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int execution(const char * owner, FILE * fs, int fd) {
    //block file, read value, clear file, write ++value, unblock file
    
    rewind(fs);
    if(lockf(fd, F_LOCK, 0) == -1) {//exclusive lock also for read not to read invalid value
        perror("lockf");
        return 1;
    }
    printf("%s works\n", owner);
    int current_count;
    errno = 0;
    int fsc_res = fscanf(fs, "%d", &current_count);

    if (fsc_res == EOF && errno == 0) {
        fprintf(fs, "%d", 1);//first execution, write 1
    } else if (fsc_res == 1) {
        //not first execution
        printf("%s current_count = %d\n", owner, current_count);
        //clear file
        if (ftruncate(fd, 0) == -1) {
            perror("ftruncate");
            return 1;
        }
        //set the file position indicator to the beginning of the file.
        rewind(fs);
        printf("%s ++current_count = %d\n", owner, ++current_count);
        if(fprintf(fs, "%-d", current_count) < 0) {
            perror("fprintf");
            return 1;
        }
    } else {
        perror("fscanf");
        return 1;
    }
    
    rewind(fs);
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
    const char * pathname = (argc == 2 ? argv[1] : "11counter.txt");
    
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
    
    pid_t child_id = fork();
    
    if (child_id < 0) {
        perror("fork");
        fclose(fs);
        return 1;
    }

    if (child_id == 0) {
        //this code is executed in child process only
        if(execution("child", fs, fd) != 0) {
            fprintf(stderr, "child execution failed\n");
            fclose(fs);
            return 1;
        }

        return 0;
    }
    
    //this code is executed in parent process only

    pid_t res = wait(NULL); 
    if (res == -1) {
        perror("wait");
        fclose(fs);
        return 1;
    }

    if(execution("parent", fs, fd) != 0) {
        fprintf(stderr, "parent execution failed\n");
        fclose(fs);
        return 1;
    }

    if(fclose(fs) == EOF) {
        perror("fclose");
        return 1;
    }

    return 0;
}


