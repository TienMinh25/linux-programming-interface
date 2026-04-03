/**
 * Write a program that opens an existing file for writing with the O_APPEND flag, and
then seeks to the beginning of the file before writing some data. Where does the
data appear in the file? Why?

 * Answer:
 The data will be appeared in last file. Because when we open file with file status flag O_APPEND,
 before each write, it will seek to last offset then write the data.
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#include <string.h>

int main() {
    int fd = open("./exercise/chapter-5/5.2.test.txt", O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        errExit("open error");
    }

    char *buf = "this is append text";

    if (write(fd, buf, strlen(buf)) != strlen(buf)) {
        errExit("partial data is written");
    }

    off_t res = lseek(fd, 0, SEEK_SET);

    if (res == -1) {
        errExit("lseek error");
    }

    buf = "this is next append test";

    if (write(fd, buf, strlen(buf)) != strlen(buf)) {
        errExit("partial data is written");
    }
    
    if (close(fd) == -1) {
        errExit("close exit");
    }

    return 0;
}