#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int copy_reg(char const *argv[], int bufsize);
int copy_symlink(char const *argv[], int bufsiz);

int main(int argc, char const *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s src_file dest_file\n", argv[0]);
        return 1;
    }

    struct stat sb;
    if (lstat (argv[1], &sb) == -1) {
        perror("lstat failed");
        return 2;
    }

    int filetype = (sb.st_mode) & S_IFMT;

    if (filetype == S_IFREG) {
        if(copy_reg(argv, sb.st_size) < 0) {
            perror("copy_reg");
            return 3;
        }

    } else if (filetype == S_IFDIR) {
        fprintf(stderr, "Can't copy directory yet\n");
        return 4; 

    } else if (filetype == S_IFIFO) {
        if(mkfifo (argv[2], sb.st_mode) == -1) {
            perror("mkfifo failed");
            return 5;
        }

    } else if (filetype == S_IFCHR) {
        if(mknod (argv[2], 'c', sb.st_rdev) == -1) {
            perror("mknod(character device) failed");
            return 6;
        }

    } else if (filetype == S_IFBLK) {
        if(mknod (argv[2], 'b', sb.st_rdev) == -1) {
            perror("mknod(block device) failed");
            return 7;
        }

    } else if (filetype == S_IFSOCK) {
        fprintf(stderr, "Can't copy sockets yet\n");
        return 8; 

    } else if (filetype == S_IFLNK) {
        if(copy_symlink(argv, sb.st_size) < 0) {
            perror("copy_symlink failed");
            return 9;
        }

    } else {
        fprintf(stderr, "Unknown filetype\n");
        return 10;
    }

    int fd3 = open(argv[2], O_RDWR);
    if (fd3 < 0) {
        perror("Failed to open copied file");
        return 11;
    }    

    //set permissions
    if(fchmod(fd3, sb.st_mode & ALLPERMS) == -1) {
        perror("fchmod failed");
        return 12;
    }

    //set timestamps
    struct timespec array_time[2] = {sb.st_atim, sb.st_mtim};
    if(futimens(fd3, array_time) == -1) {
        perror("futimens failed");
        return 13;
    }

    //set uid, gid
    if(fchown(fd3, sb.st_uid, sb.st_gid) == -1) {
        perror("fchown failed");
        return 14;
    }    

    if (close(fd3) < 0) {
        perror("Closing copied file failed");
        return 15;
    }    

    return 0;
}

int copy_reg(char const *argv[], int bufsize) {

    int fd1 = open (argv[1], O_RDONLY);
    if (fd1 < 0) {
        //perror("Failed to open src file");
        return -1;
    }

    int fd2 = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd2 < 0) {
        //perror("Failed to open or create dest file");
        return -2;
    }    

    char buf[bufsize]; 
    ssize_t nbytes, nbytes_w;    

    off_t offset_r = 0, offset_w = 0;
    while ((nbytes = pread(fd1, buf, bufsize, offset_r)) > 0) {
        offset_r += nbytes;
        while (nbytes) {
            nbytes_w = pwrite(fd2, buf, nbytes, offset_w);
            if (nbytes_w == -1) {
                //perror("pwrite failed");
                return -3;
            }
            nbytes -= nbytes_w;
            offset_w += nbytes_w;
        }
    }
    if (nbytes == -1) {
        //perror("pread failed");
        return -4;
    }

    if (close(fd1) < 0) {
        //perror("Closing src file failed");
        return -5;
    }

    if (close(fd2) < 0) {
        //perror("Closing dest file failed");
        return -6;
    }
    return 0;
}

int copy_symlink(char const *argv[], int bufsiz) {

    /* Add 1 to the link size, so that we can determine whether the buffer returned by readlink() was truncated. */
    bufsiz += 1;

    char * buf = malloc(bufsiz);
    if (buf == NULL) {
        //perror("malloc failed");
        return -1;
    }

    ssize_t nbytes = readlink(argv[1], buf, bufsiz);
    if (nbytes == -1) {
        //perror("readlink failed");
        return -2;
    }

    //printf("'%s' points to '%.*s'\n", argv[1], (int) nbytes, buf);

    /* If the return value was equal to the buffer size, then the link target was larger than expected 
    (perhaps because the target was changed between the call to lstat() and the call to readlink()). 
    Warn the user that the returned target may have been truncated. */

    if (nbytes == bufsiz)
       printf("(Returned buffer may have been truncated)\n");

    if(symlink(buf, argv[2]) == -1) {
        //perror("symlink falied");
        return -3;
    } 

    free(buf);
    return 0;
}












