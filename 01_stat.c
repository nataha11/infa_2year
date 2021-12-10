#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BL_SIZE 512

struct filetype {
    const char * typename;
    char ctype;
};

#define MAKE_FILETYPE(description, c) (const struct filetype) {(description), (c)}
struct filetype get_file_type(mode_t mode);

char * read_symlink(const char * symlink_path, size_t size_hint);

#define MODESTR_SIZE sizeof("?rwxrwxrwx")
void format_mode(char * buf, mode_t mode);

int main(int argc, char *argv[]) {
    
    struct stat sb;
    const char * pathname = argv[1];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(1);
    }

    if (lstat(pathname, &sb) == -1) {
        perror("lstat");
        exit(1);
    }

    //show pathname and link target
    if(S_ISLNK(sb.st_mode)) {
        char * symlink_dst = read_symlink(pathname, sb.st_size);
        printf("Symlink:\t\t%s -> %s\n", pathname, symlink_dst ? symlink_dst : "?");
        free(symlink_dst);
    } else {
        printf("File:                     %s\n", pathname);
       
    }

    printf("Size:                     %ju bytes\n", (uintmax_t) sb.st_size); 
    printf("Blocks:                   %ju (%ju bytes)\n", (uintmax_t) sb.st_blocks, (uintmax_t) sb.st_blocks * BL_SIZE);
    printf("IO Block:                 %ju bytes\n", (uintmax_t) sb.st_blksize);
    printf("File type:                %s\n", get_file_type(sb.st_mode).typename);
    printf("Device ID:                %lxh/%ldd\n", (long) sb.st_dev, (long) sb.st_dev);
    printf("Inode:                    %ju\n", (uintmax_t) sb.st_ino);
    printf("Links:                    %ju\n", (uintmax_t) sb.st_nlink);

    //get filetype and permissions
    char mode_str[MODESTR_SIZE];
    format_mode(mode_str, sb.st_mode);

    printf("Access:                   (0o%04lo/%s)  UID:%jd  GID:%jd  \n", (unsigned long) (sb.st_mode & ALLPERMS), mode_str, (intmax_t) sb.st_uid, (intmax_t) sb.st_gid);
    printf("Access (UTC):              %s", asctime(gmtime(&sb.st_atime)));
    printf("Modify (UTC):              %s", asctime(gmtime(&sb.st_mtime)));
    printf("Change (UTC):              %s", asctime(gmtime(&sb.st_ctime)));
//наносекунды, локальное смещение strftime
    exit(0);
}

struct filetype get_file_type(mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:  return MAKE_FILETYPE("block device", 'b');
        case S_IFCHR:  return MAKE_FILETYPE("character device", 'c');
        case S_IFDIR:  return MAKE_FILETYPE("directory", 'd');
        case S_IFIFO:  return MAKE_FILETYPE("FIFO/pipe", 'p');
        case S_IFLNK:  return MAKE_FILETYPE("symbolic link", 'l');
        case S_IFREG:  return MAKE_FILETYPE("regular file", '-');
        case S_IFSOCK: return MAKE_FILETYPE("socket", 's');
        default:       return MAKE_FILETYPE("unknown", '?');
    }
}

char * read_symlink(const char * symlink_path, size_t size_hint) {
        size_t buf_size = size_hint ? size_hint : PATH_MAX;
        char * buf = malloc(buf_size + 1);// +1 for '\0'
        if (buf == NULL) {
            perror("malloc failed");
            return NULL;
        }

        ssize_t nbytes = readlink(symlink_path, buf, buf_size);
        if (nbytes == -1) {
            perror("readlink failed");
            return NULL;
        }

        buf[nbytes] = '\0';
        return buf;
}

void format_mode(char * buf, mode_t mode) {
    char * p = buf;

    // fill buf with info for basic mode bits
    *p++ = get_file_type(mode).ctype;
    for(int i  = 8; i >= 0; i++) {
        *p++ = ((mode & (1 << i)) ? "xwr"[i % 3] : '-');
    }
    
    *p = '\0';//terminate string

    //fix buf chars according to SUID/SGID/sticky bits
    //0123456789
    //?rwxrwxrwx
    if(mode & S_ISUID) {
        buf[3] = 's';
    }
    if(mode & S_ISGID) {
        buf[6] = 's';
    }
    if(mode & S_ISUID) {
        buf[9] = S_ISDIR(mode) ? 't' : 'T';
    }        
}




