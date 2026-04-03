/**
 * Implement dup() and dup2() using fcntl() and, where necessary, close(). (You may
ignore the fact that dup2() and fcntl() return different errno values for some error
cases.) For dup2(), remember to handle the special case where oldfd equals newfd. In
this case, you should check whether oldfd is valid, which can be done by, for example,
checking if fcntl(oldfd, F_GETFL) succeeds. If oldfd is not valid, then the function
should return –1 with errno set to EBADF.
 */
#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>

int _dup(int fd);
int _dup2(int fd1, int fd2);

int main() {
    // test _dup
    if (_dup(5) == -1) {
        printf("_dup: pass test file descriptor is not valid\n");
    }

    int dupStdout = _dup(STDOUT_FILENO);
    if (dupStdout == 3) {
        printf("_dup: pass test, case file descriptor is valid and assign lowest-numbered available (3)\n");
    }
    
    char *buf = "This is message written by duplicate file descriptor stdout\n";
    if (write(dupStdout, buf, strlen(buf)) == -1) {
        printf("_dup: failed test when dup stdout is not written full text\n");
        exit(EXIT_FAILURE);
    }

    // test _dup2
    if (_dup2(5, 5) == -1)
    {
        printf("_dup2: pass test file descriptor input is not valid\n");
    }

    if (_dup2(1, 6) == -1)
    {
        printf("_dup2: pass test file descriptor specified is not valid\n");
    }
    
    if (_dup2(STDOUT_FILENO, STDOUT_FILENO) == STDOUT_FILENO) {
        printf("_dup2: pass test do nothing when specify the same file descriptor\n");
    }

    char *buf2 = "Message written by standard output\n";
    if (write(STDOUT_FILENO, buf2, strlen(buf2)) == -1)
    {
        printf("_dup2: failed test when stdout is not written full text\n");
        exit(EXIT_FAILURE);
    }

    // create new file descriptor on one file
    int fd = open("tmp", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit("open file tmp is not success!");
    }
    
    int dupStdout2 = _dup2(STDOUT_FILENO, fd);
    if (dupStdout2 == -1) {
        errExit("_dup2: failed test because cannot duplicate file descriptor\n");
    }

    buf = "This is message written by dup stdout 2\n";
    if (dupStdout2 == 4 && write(dupStdout2, buf, strlen(buf)) != -1) {
        printf("_dup2: pass test when write to stdout\n");
    }
    
    if (close(dupStdout2) == -1 || close(dupStdout) == -1) {
        errExit("close error");
    }
    
    if (remove("tmp") == -1) {
        errExit("remove file tmp error");
    }

    exit(EXIT_SUCCESS);
}

int _dup(int fd)
{
    if (fcntl(fd, F_GETFL) == -1)
    {
        return -1;
    }

    return fcntl(fd, F_DUPFD);
}

int _dup2(int fd1, int fd2)
{
    if (fcntl(fd1, F_GETFL) == -1)
    {
        return -1;
    }

    if (fcntl(fd2, F_GETFL) == -1)
    {
        return -1;
    }

    // case fd1 == fd2, do nothing, return the fd1
    if (fd1 == fd2)
    {
        return fd1;
    }

    if (close(fd2) == -1)
    {
        return -1;
    }

    return fcntl(fd1, F_DUPFD, fd2);
}
