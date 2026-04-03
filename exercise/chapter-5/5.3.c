/**
 * This exercise is designed to demonstrate why the atomicity guaranteed by opening
a file with the O_APPEND flag is necessary. Write a program that takes up to three
command-line arguments:
$ atomic_append filename num-bytes [x]
This file should open the specified filename (creating it if necessary) and append
num-bytes bytes to the file by using write() to write a byte at a time. By default, the
program should open the file with the O_APPEND flag, but if a third command-line
argument (x) is supplied, then the O_APPEND flag should be omitted, and instead the 
program should perform an lseek(fd, 0, SEEK_END) call before each write(). Run
two instances of this program at the same time without the x argument to write
1 million bytes to the same file:
$ atomic_append f1 1000000 & atomic_append f1 1000000
Repeat the same steps, writing to a different file, but this time specifying the x
argument:
$ atomic_append f2 1000000 x & atomic_append f2 1000000 x

List the sizes of the files f1 and f2 using ls –l and explain the difference.

Answer:
when i use ls -la, this is my result:
lvtminh@lvtminh:~/Documents/linux system programming$ ls -la exercise/chapter-5/
total 3216
drwxrwxr-x 2 lvtminh lvtminh    4096 Apr  3 21:43 .
drwxrwxr-x 5 lvtminh lvtminh    4096 Apr  3 20:44 ..
-rw-rw-r-- 1 lvtminh lvtminh     524 Apr  3 21:07 5.1.md
-rwxrwxr-x 1 lvtminh lvtminh   22320 Apr  3 21:00 5.2
-rw-rw-r-- 1 lvtminh lvtminh    1071 Apr  3 21:04 5.2.c
-rw-rw-r-- 1 lvtminh lvtminh      60 Apr  3 21:00 5.2.test.txt
-rw-rw-r-- 1 lvtminh lvtminh    2265 Apr  3 21:38 5.3.c
-rw-rw-r-- 1 lvtminh lvtminh       0 Apr  3 20:45 5.4.c
-rwxrwxr-x 1 lvtminh lvtminh   26720 Apr  3 21:40 atomic_append
-rw------- 1 lvtminh lvtminh 2000000 Apr  3 21:42 f1.txt
-rw------- 1 lvtminh lvtminh 1211719 Apr  3 21:43 f2.txt

The first difference is the file size of f1.txt and f2.txt, because the f1.txt is written
in atomic way, so the seek to the end and write in one operation and other process will be blocked
until it finish. But when we does not use the flag O_APPEND, two processes can write "x" into one place
(in there is the same offset, which is not expected). It's the kind of race condition.
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc < 3 || strcmp("--help", argv[1]) == 0)
    {
        usageErr("%s file num-bytes [x]\n"
                 "        'x' means use lseek() instead of O_APPEND\n",
                 argv[0]);
    }

    int flags = O_CREAT | O_RDWR;
    mode_t fileMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IRGRP;

    bool useLseek = argc > 3;
    if (!useLseek)
    {
        flags |= O_APPEND;
    }

    int fd = open(argv[1], flags, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        errExit("open error");
    }

    int numBytes = getInt(argv[2], 0, "num-bytes");

    for (int start = 0; start < numBytes; start++)
    {
        // if enable useLseek, we will lseek to the end of file each time,
        // then write one byte to it, otherwise, we just need to write one byte at
        // one time
        if (useLseek)
        {
            if (lseek(fd, 0, SEEK_END) == -1)
                errExit("lseek error");
        }

        if (write(fd, "x", 1) == -1)
        {
            errExit("write error");
        }
    }

    if (close(fd) == -1) {
        errExit("close error");
    }
    printf("%ld done\n", (long)getpid());
    exit(EXIT_SUCCESS);
}