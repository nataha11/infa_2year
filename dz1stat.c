#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <string.h>

struct filetype {
    char * typename;
    char ctype;
};

struct filetype fill_struct(struct filetype * f, char * typnam, char type);
struct filetype get_file_type(int mode);
char * access_rights(int rmode);

int main(int argc, char *argv[]) {
    
    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(1);
    }

    if (lstat(argv[1], &sb) == -1) {        //почему lstat а не stat?
        perror("lstat");
        exit(1);
    }

    struct filetype filtyp = get_file_type(sb.st_mode);

    printf("File:                     %s\n", argv[1]);
    printf("Size:                     %lld bytes\n", (long long) sb.st_size); 
    printf("Blocks:                   %lld\n", (long long) sb.st_blocks);
    printf("IO Block:                 %ld bytes\n", (long) sb.st_blksize);
    printf("File type:                %s\n", filtyp.typename);
    printf("Device ID:                %lxh/%ldd\n", (long) sb.st_dev, (long) sb.st_dev);
    printf("Inode:                    %ld\n", (long) sb.st_ino);
    printf("Links:                    %ld\n", (long) sb.st_nlink);
    printf("Access:                   (%04lo/%c%s)  Uid:%ld  Gid:%ld  \n", (unsigned long) sb.st_mode & ALLPERMS, filtyp.ctype, access_rights(sb.st_mode & ALLPERMS), (long) sb.st_uid, (long) sb.st_gid);
    printf("Access:                   %s", ctime(&sb.st_atime));
    printf("Modified:                 %s", ctime(&sb.st_mtime));
    printf("Change:                   %s", ctime(&sb.st_ctime));

    exit(0);
}







struct filetype fill_struct(struct filetype * f, char * typnam, char type) {
    f->typename = typnam;
    f->ctype = type;
    return *f;
}

struct filetype get_file_type(int mode) {
    struct filetype f;
    switch (mode & S_IFMT) {
        case S_IFBLK:  {return fill_struct(&f, "block device", ' '); }  
        case S_IFCHR:  {return fill_struct(&f, "character device", ' '); }
        case S_IFDIR:  {return fill_struct(&f, "directory", 'd'); }       
        case S_IFIFO:  {return fill_struct(&f, "FIFO/pipe", ' '); }        
        case S_IFLNK:  {return fill_struct(&f, "symlink", ' '); }          
        case S_IFREG:  {return fill_struct(&f, "regular file", '-'); }   
        case S_IFSOCK: {return fill_struct(&f, "socket", ' '); }          
        default:       {return fill_struct(&f, "unknown?", ' '); }       
    }
}

char * access_rights(int rmode) {
    int n = 9;
    char * rights = malloc((n + 1) * sizeof(char));
    for (int i = n - 1; i >= 0; i--) {
        char ch = (rmode & (1 << i)) ? "xwr"[i % 3] : '-';
        rights[(n - 1) - i] = ch;
    }

    rights[n] = '\0';
    return rights;
}



