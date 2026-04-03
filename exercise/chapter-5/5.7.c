/**
 * Implement readv() and writev() using read(), write(), and suitable functions from the
malloc package
 * readv(2) and writev(2) perform scather-gather IO on UNIX systems. They allow for
 * reading or writing multiple buffers to a file, while guarateeing atomicity, i.e.,
 * the bytes written or read will be consecutive.
 *
 * Usage:
 *    $ ./readv_writev
 *    # Print performed steps to ensure implementation is correct.
 */
#define _DEFAULT_SOURCE // macro test for mkstemp() func

#define STR_SIZE 20
#include <stdlib.h>
#include "tlpi_hdr.h"
#include <sys/uio.h>
#include <fcntl.h>

static char filePath[] = "tmpXXXXXX";

ssize_t _readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t _writev(int fd, const struct iovec *iov, int iovcnt);

int main()
{
    ssize_t bytesRequired = 0;
    struct iovec iovInput[3];
    struct iovec iovOutput[3];

    int fd = mkstemp(filePath);
    if (fd == -1)
    {
        errExit("mkstemp");
    }
    printf("template file name is: %s\n", filePath);
    unlink(filePath);
    // prepare data to write
    char code = 'x';                       // first buffer
    int nCode = 200;                       // second buffer
    char words[STR_SIZE] = "readv writev"; // third buffer

    // first buffer
    iovInput[0].iov_base = &code;
    iovInput[0].iov_len = sizeof(char);
    bytesRequired += iovInput[0].iov_len;

    // second buffer
    iovInput[1].iov_base = &nCode;
    iovInput[1].iov_len = sizeof(int);
    bytesRequired += iovInput[1].iov_len;

    // third buffer
    iovInput[2].iov_base = words;
    iovInput[2].iov_len = sizeof(words);
    bytesRequired += iovInput[2].iov_len;

    // writev to tmp file
    ssize_t numWritten = _writev(fd, iovInput, 3);
    if (numWritten == -1)
    {
        errExit("_writev error");
    }

    if (numWritten != bytesRequired)
    {
        errExit("_writev: failed test, numWritten should equal bytesRequired");
    }

    // setup to read
    char outCode;
    int outNCode;
    char outWords[STR_SIZE];

    // first buffer
    iovOutput[0].iov_base = &outCode;
    iovOutput[0].iov_len = sizeof(char);

    // second buffer
    iovOutput[1].iov_base = &outNCode;
    iovOutput[1].iov_len = sizeof(int);

    // third buffer
    iovOutput[2].iov_base = outWords;
    iovOutput[2].iov_len = sizeof(outWords);

    // we must seek to the start of file to read
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        errExit("lseek error");
    }

    // readv from tmp file
    ssize_t numRead = _readv(fd, iovOutput, 3);
    if (numRead == -1)
    {
        errExit("_readv error");
    }

    if (numRead != bytesRequired)
    {
        errExit("_readv: failed test, numRead should equal bytesRequired");
    }

    printf("Scatter-Gather I/O finish.\nRead data: code = %c, nCode = %d, words = %s\n", outCode, outNCode, outWords);

    // close file
    if (close(fd) == -1)
    {
        errExit("close error");
    }

    exit(EXIT_SUCCESS);
}

ssize_t _readv(int fd, const struct iovec *iov, int iovcnt)
{
    if (fcntl(fd, F_GETFL) == -1)
    {
        return (long)-1;
    }

    size_t bufSize = 0;

    for (int i = 0; i < iovcnt; i++)
    {
        bufSize += iov[i].iov_len;
    }

    void *buf = malloc(bufSize);
    if (buf == NULL)
    {
        errExit("malloc exit");
    }

    ssize_t numRead = read(fd, buf, bufSize);
    if (numRead == -1)
    {
        return -1;
    }

    ssize_t start = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        memcpy(iov[i].iov_base, buf + start, iov[i].iov_len);
        start += iov[i].iov_len;
    }

    free(buf);
    return numRead;
}

ssize_t _writev(int fd, const struct iovec *iov, int iovcnt)
{
    if (fcntl(fd, F_GETFL) == -1)
    {
        return (long)-1;
    }

    size_t bufSize = 0;

    for (int i = 0; i < iovcnt; i++)
    {
        bufSize += iov[i].iov_len;
    }

    void *buf = malloc(bufSize);
    if (buf == NULL)
    {
        errExit("malloc error");
    }

    ssize_t start = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        memcpy(buf + start, iov[i].iov_base, iov[i].iov_len);
        start += iov[i].iov_len;
    }

    ssize_t numWritten = write(fd, buf, bufSize);

    free(buf);
    return numWritten;
}