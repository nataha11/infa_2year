#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

char dtype_letter(unsigned d_type) {
    switch(d_type) {
        case DT_BLK:  return 'b';
        case DT_CHR:  return 'c';
        case DT_DIR:  return 'd';
        case DT_FIFO: return 'p';
        case DT_LNK:  return 'l';
        case DT_REG:  return '-';
        case DT_SOCK: return 's';
        default :     return '?';
    }
}

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
    int result = 0;

    if(argc > 2) {
        printf("Too many arguments\n");
        printf("Usage: %s [path]\n", argv[0]);
        return 1;
    }

    DIR * dir_fd;
    if(argc == 2) {
        if(chdir(argv[1]) == -1) {
            perror("chdir");
            return 2;
        }
    } 
    dir_fd = opendir(".");
    if (dir_fd == NULL) {     
        perror("opendir");
        return 3;
    }
    
    errno = 0;
    struct dirent * entry;
    while((entry = readdir(dir_fd)) != NULL) {
        char type = dtype_letter(entry->d_type);
        if (type == '?') {
            struct stat sb;
            if(lstat(entry->d_name, &sb) == -1) {
                perror ("lstat failed");
                result = 4;
                break;
            } else {
                type = stattype(sb.st_mode);
            }
        }
        printf("%c %s\n", type, entry->d_name);     
    }
    
    if(errno != 0) {
        perror("readdir");
        result = 5;
    }

    if(closedir(dir_fd) == -1) {
        perror("closedir");
        result = 6;
    }

    return result;
}

