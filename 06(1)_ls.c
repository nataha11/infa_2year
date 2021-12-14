#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

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

    if(argc > 1) {
        printf("Too many arguments\n");
        printf("Usage: %s\n", argv[0]);
        return 1;
    }

    DIR * dir_fd = opendir(".");
    if (dir_fd == NULL) {
        perror("opendir");
        return 2;
    }

    struct dirent * entry;
    while((entry = readdir(dir_fd)) != NULL) {
        struct stat sb;
        if(lstat(entry->d_name, &sb) == -1) {
            perror ("lstat");
            closedir(dir_fd);
            return 3;
        }
        char type = stattype(sb.st_mode); 
        printf("%c %s\n", type, entry->d_name);     
    }

    if(closedir(dir_fd) == -1) {
        perror("closedir");
        return 4;
    }

    return 0;
}

