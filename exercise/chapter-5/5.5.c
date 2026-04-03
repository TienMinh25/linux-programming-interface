/**
 * Write a program to verify that duplicated file descriptors share a file offset value
and open file status flags.
 */
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {
    int fd = open("5.5.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit("open error");
    }
    
    char *buf = "this file will be used to duplicate file descriptor";

    if (write(fd, buf, strlen(buf)) == -1) {
        errExit("write error");
    }
    
    int dupFd = dup(fd);
    if (dupFd == -1) {
        errExit("dup error");
    }
    
    // use fcntl to get file status flags
    int flags1 = fcntl(fd, F_GETFL);
    if (flags1 == -1) {
        errExit("fcntl file descriptor 1 error");
    }

    int flags2 = fcntl(dupFd, F_GETFL);
    if (flags2 == -1)
    {
        errExit("fcntl file descriptor 2 error");
    }
    
    if (flags1 == flags2) {
        printf("file descriptor 1 and file descriptor dup has the same status flags\n");
    } else {
        printf("file descriptor 1 and file descriptor dup does not have the same status flags\n");
    }

    // use lseek to get current file offset of fd and dupFd, then compare them
    off_t fdOffset = lseek(fd, 0, SEEK_CUR);
    if (fdOffset == -1)
    {
        errExit("lseek file descriptor fd error");
    }

    off_t fdDupOffset = lseek(fd, 0, SEEK_CUR);
    if (fdDupOffset == -1)
    {
        errExit("lseek file descriptor fdDup error");
    }

    if (fdOffset != fdDupOffset)
    {
        printf("offset of file descriptor 1 != offset of file descriptor dup\n");
    }
    else
    {
        printf("offset of file descriptor 1 == offset of file descriptor dup\n");
    }
    
    if (close(fd) == -1 || close(dupFd) == -1) {
        errExit("close fd or fd dup error");
    }

    exit(EXIT_SUCCESS);
}