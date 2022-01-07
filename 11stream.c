#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

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
    FILE * fs = fdopen(fd, "r+");
    if (fs == NULL) {
        perror("fdopen");
        close(fd);
        return 1;
    }
    int current_count;
    errno = 0;
    int fsc_res = fscanf(fs, "%d", &current_count);
    printf("fsc_res = %d\n", fsc_res);
    if (fsc_res == EOF) {
        if(errno == 0)
            fprintf(fs, "%d", 1);//first execution, write 1
        else {
            perror("fscanf");
            fclose(fs);
            return 1;
        } 
    } else {
        //not first execution
        printf("current_count = %d\n", current_count);
        
        if (ftruncate(fd, 0) == -1) {
            perror("ftruncate");
            fclose(fs);
            return 1;
        }
        current_count++;
        printf("current_count++ = %d\n", current_count);
        if(fprintf(fs, "%d", current_count) < 0) {
            perror("fprintf");
            fclose(fs);
            return 1;
        }
    }    
    
    if(fclose(fs) == -1) {
        perror("fclose");
        return 1;
    }
    
    return 0;
}


