#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct filetype {
    const char * typename;
    char ctype;
};

struct filetype fill_struct(struct filetype * f, const char * typnam, char type);
struct filetype get_file_type(int mode);
char * access_rights(int rmode);

int main(int argc, char *argv[]) {
    
    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(1);
    }

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(1);
    }

    struct filetype filtyp = get_file_type(sb.st_mode);
    char * filename;

    if(filtyp.ctype == 'l') {
        //if symlink, add " -> target" to linkname
        char * buf = malloc(sb.st_size + 1);
        if (buf == NULL) {
            perror("malloc failed");
            return -1;
        }

        ssize_t nbytes = readlink(argv[1], buf, sb.st_size);
        if (nbytes == -1) {
            perror("readlink failed");
            return -2;
        }
        filename = malloc(sizeof(argv[1]) + sizeof(" -> ") + sizeof(buf) + 3);
        strcat(filename, argv[1]);
        strcat(filename, " -> ");
        strcat(filename, buf);
        free(buf);
    } else
        filename = argv[1];


    //get rights
    int rmode = sb.st_mode & ALLPERMS;
    int n = sizeof("rwxrwxrwx");
    char rights[n + 1];
    rights[0] = filtyp.ctype;
    for(int i = 0; i < n - 1; i++) {
        rights[n - 1 - i] = ((rmode & (1 << i))? "xwr"[i % 3] : '-');
    }
    rights[n] = '\0';


    printf("File:                     %s\n", filename);
    printf("Size:                     %lld bytes\n", (long long) sb.st_size); 
    printf("Blocks:                   %lld\n", (long long) sb.st_blocks);
    printf("IO Block:                 %ld bytes\n", (long) sb.st_blksize);
    printf("File type:                %s\n", filtyp.typename);
    printf("Device ID:                %lxh/%ldd\n", (long) sb.st_dev, (long) sb.st_dev);
    printf("Inode:                    %ld\n", (long) sb.st_ino);
    printf("Links:                    %ld\n", (long) sb.st_nlink);
    printf("Access:                   (%04lo/%s)  Uid:%ld  Gid:%ld  \n", (unsigned long) sb.st_mode & ALLPERMS, rights, (long) sb.st_uid, (long) sb.st_gid);
    printf("Access:                   %s", ctime(&sb.st_atime));
    printf("Modified:                 %s", ctime(&sb.st_mtime));
    printf("Change:                   %s", ctime(&sb.st_ctime));

    exit(0);
}

struct filetype fill_struct(struct filetype * f, const char * typnam, char type) {
    f->typename = typnam;
    f->ctype = type;
    return * f;
}

struct filetype get_file_type(int mode) {
    struct filetype f;
    switch (mode & S_IFMT) {
        case S_IFBLK:  {return fill_struct(&f, "block device", 'b'); }
        case S_IFCHR:  {return fill_struct(&f, "character device", 'c'); }
        case S_IFDIR:  {return fill_struct(&f, "directory", 'd'); }
        case S_IFIFO:  {return fill_struct(&f, "FIFO/pipe", 'p'); }
        case S_IFLNK:  {return fill_struct(&f, "symbolic link", 'l'); }     
        case S_IFREG:  {return fill_struct(&f, "regular file", '-'); }
        case S_IFSOCK: {return fill_struct(&f, "socket", 's'); } 
        default:       {return fill_struct(&f, "unknown", '?'); }
    }
}




