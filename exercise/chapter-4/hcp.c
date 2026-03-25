/*
 * usage: ./hcp <old_file> <new_file>
 * should support -h for user want to use this command
 * purpose: used to copy regular file that contains
 * holes (sequence bytes of null)
*/
#ifndef BUF_SIZE
#define BUF_SIZE 512
#endif

#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>
#include<getopt.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]){  
    int opt;
    
    while (1) {
        opt = getopt(argc, argv, "h");
        if (opt == -1)
        {
            break;
        }

        if (opt != 'h')
        {
            errExit("Invalid option. We support only for 'h' option");
        }
    }

    // check number of argument -> should equal 3
    if (argc != 3)
    {
        usageErr("%s [-h] <old file> <new file>", argv[0]);
    }

    // first, open old file, if not exist, exit error
    int fdInput = open(argv[1], O_RDONLY);
    if (fdInput == -1)
    {
        errExit("open old file failed: %s", strerror(errno));
    }

    int fdOut;
    int flags = O_CREAT | O_TRUNC | O_WRONLY;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
    // then open new file, if not exist, create new one, otherwise, we
    // override the old content
    fdOut = open(argv[2], flags, mode);

    char buf[BUF_SIZE];
    int numRead;

    // read file and check that buffer contains any hole, if it contains hole,
    // we should use lseek() to skip that buffer
    while ((numRead = read(fdInput, buf, BUF_SIZE)) > 0) {
        int start = 0, curr = 0, isValid = 1;
       
        while (curr < numRead) {
            if (buf[curr] == '\0' && isValid == 0) {
                curr++;
                continue;
            }

            if (buf[curr] != '\0' && isValid == 1) {
                curr++;
                continue;
            }

            if (buf[curr] == '\0' && isValid == 1) {
                // should write to fdOut from start -> curr, then mark isValid = 0,
                // mark start = curr
                isValid = 0;
                int length = curr - start;
                char tmp[length];
                
                for (int i = 0; i < length; i++) {
                    tmp[i] = buf[start + i];
                }
                
                ssize_t result = write(fdOut, tmp, length);
                if (result == -1 || result != length) {
                    errExit("write error");
                }

                start = curr;
                curr++;
                continue;
            }

            // case buf[curr] != '\0' && isValid == 0
            // should lseek the current file
            if (lseek(fdOut, curr-start, SEEK_CUR) == -1){
                errExit("lseek error: %s", strerror(errno));
            }
            isValid = 1;
            start = curr;
            curr++;
        }
        
        if (isValid == 1) {
            int length = curr - start;
            char tmp[length];

            for (int i = 0; i < length; i++)
            {
                tmp[i] = buf[start + i];
            }

            ssize_t result = write(fdOut, tmp, length);
            if (result == -1 || result != length)
            {
                errExit("write error");
            }
        }
        else
        {
            if (lseek(fdOut, curr - start, SEEK_CUR) == -1)
            {
                errExit("lseek error: %s", strerror(errno));
            }
        }
    }

    if (numRead == -1)
    {
        errExit("read error: %s", strerror(errno));
    }

    // close all file descriptors
    if (close(fdInput) == -1)
    {
        errMsg("close file %s failed: %s", argv[1], strerror(errno));
    }

    if (close(fdOut) == -1)
    {
        errMsg("close file %s failed: %s", argv[2], strerror(errno));
    }

    return 0;
}