#define _DEFAULT_SOURCE
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>


/*char dtype_letter(unsigned d_type) {
    switch(d_type) {
        case DT_BLK :  return 'b';
        case DT_CHR :  return 'c';
        case DT_DIR :  return 'd';
        case DT_FIFO : return 'p';
        case DT_LNK :  return 'l';
        case DT_REG :  return '-';
        case DT_SOCK : return 's';
        default :      return '?';
    }
}*/

char stattype(unsigned mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:  return 'b';
        case S_IFCHR:  return 'c';
        case S_IFDIR:  return 'd';
        case S_IFIFO:  return 'p';
        case S_IFLNK:  return 'l';   
        case S_IFREG:  return '-';
        case S_IFSOCK: return 's';
        default:       return '?';
    }
}

int main(int argc, char const *argv[]) {

    if(argc > 2) {
        printf("Too many arguments\n");
        printf("Usage: %s [path]\n", argv[0]);
        return 1;
    }

    DIR * dir_fd;
    if(argc == 2) {
        if(chdir(argv[1]) == -1) {
            perror("chdir");
            return 1;
        }
    } 
    dir_fd = opendir(".");

    if (dir_fd == NULL) {     
        perror("opendir failed");
        return 2;
    }

    struct dirent * entry;
    while((entry = readdir(dir_fd)) != NULL) {
            struct stat sb;
            if(lstat(entry->d_name, &sb) == -1) {//pathname
                printf("entry->d_name = %s\n", entry->d_name);
                perror ("lstat failed");
                return 3;

            }
            //get rights
            int rmode = sb.st_mode & ALLPERMS;
            int n = sizeof("rwxrwxrwx");
            char rights[n + 1];
            rights[0] = stattype(sb.st_mode);
            for(int i = 0; i < n - 1; i++) {
                rights[n - 1 - i] = ((rmode & (1 << i))? "xwr"[i % 3] : '-');
            }
            rights[n] = '\0';
                 
            
            printf("%s %s\n", rights, entry->d_name);		
    }

    if(closedir(dir_fd) == -1) {
        perror("closedir failed");
        return 4;
    }    	
	return 0;
}

