#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#ifndef MAX_FILES
#define MAX_FILES 128
#endif

#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<fcntl.h>
#include<sys/stat.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    int opt;
    Boolean isAppend = FALSE;
    int fd, flags, numFiles = 0;
    int fds[MAX_FILES];
    ssize_t numRead;

    while (1) {
        opt = getopt(argc, argv, "ah");
        if (opt == -1)
        {
            break;
        }

        switch (opt)
        {
        case 'a':
            isAppend = TRUE;
            break;
        case 'h':
            usageErr("%s [-a] <file1> <file2> ... <fileN>\n", argv[0]);
            break;
        case '?':
            usageErr("%s [-a] <file1> <file2> ... <fileN>\n", argv[0]);
            break;
        }
    }
    
    // usage like: ./tee -a (this is not valid)
    if (optind >= argc) {
        usageErr("%s [-a] <file1> <file2> ... <fileN>\n", argv[0]);
    }

    // flags
    flags = O_CREAT | O_WRONLY;
    // mode
    mode_t mode = S_IRUSR | S_IWUSR; // rw-------
    
    if (isAppend == TRUE) {
        flags |= O_APPEND;
    } else {
        flags |= O_TRUNC;
    }
    
    // open all files which are passed to command ./tee
    for (int i = optind; i < argc; i++) {
        printf("file: %s\n", argv[i]);
        fds[i - optind] = fd = open(argv[i], flags, mode);
        
        if (fd == -1) {
            errExit("open error");
        }
        
        numFiles++;
    }
    
    char buf[BUF_SIZE];

    // read from stdin, then write to stdout and files by using fd
    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, numRead) != numRead) {
            errExit("write stdout error");
        }
        
        for (int i = 0; i < numFiles; i++) {
            if (write(fds[i], buf, numRead) != numRead) {
                errExit("write file error");
            }
        }
    }
    
    if (numRead == -1) {
        errExit("read stdin error");
    }

    // close all file descriptor of files
    for (int i = 0; i < numFiles; i++) {
        if (close(fds[i]) == -1) {
            errExit("close erorr");
        }
    }

    return 0;
}