#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

ssize_t readv(int fd, const iovec *iov, int iovcnt)
{
    if(!iov) {
        errno = EINVAL;
        return -1;
    }

    if(iovcnt <= 0) {
        errno = EINVAL;
        return -1;
    }

    printf("readv\n");

    size_t totalLength = 0;
    for(int i = 0; i < iovcnt; ++i) {
        printf("iov[%d].iov_len = %zu\n", i, iov[i].iov_len);
        totalLength += iov[i].iov_len;
    }

    char *buf = (char *)malloc(totalLength);
    if(!buf) return -1;
    memset(buf, 0, totalLength);
    printf("allocated %zu bytes of memory\n", totalLength);

    ssize_t readStatus = read(fd, buf, totalLength);
    if(readStatus == -1) {
        free(buf);
        return -1;
    }

    size_t current = 0;
    for(int i = 0; i < iovcnt; ++i) {
        memcpy(iov[i].iov_base, buf + current, iov[i].iov_len);
        current += iov[i].iov_len;
    }

    free(buf);

    return readStatus;
}

ssize_t writev(int fd, const iovec *iov, int iovcnt)
{
    if(!iov) {
        errno = EINVAL;
        return -1;
    }

    if(iovcnt <= 0) {
        errno = EINVAL;
        return -1;
    }

    printf("writev\n");

    size_t totalLength = 0;
    for(int i = 0; i < iovcnt; ++i) {
        printf("iov[%d].iov_len = %zu\n", i, iov[i].iov_len);
        totalLength += iov[i].iov_len;
    }

    char *buf = (char *)malloc(totalLength);
    if(!buf) return -1;
    memset(buf, 0, totalLength);
    printf("allocated %zu bytes of memory\n", totalLength);

    size_t current = 0;
    for(int i = 0; i < iovcnt; ++i) {
        memcpy(buf + current, iov[i].iov_base, iov[i].iov_len);
        current += iov[i].iov_len;
    }

    ssize_t writeStatus = write(fd, buf, totalLength);
    if(writeStatus == -1) {
        free(buf);
        return -1;
    }

    free(buf);

    return writeStatus;
}

void errExit(const char *str)
{
    fprintf(stderr, "Error: %s: %s\n", strerror(errno), str);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char tmp[] = "/tmp/file_XXXXXX";
    int fd = mkstemp(tmp);
    if(fd == -1)
        errExit("mkstemp");
    unlink(tmp);

    iovec iov[3] = {};

    ssize_t total = 0;

    char first[]  = "first",
         second[] = "second",
         third[]  = "third";

    iov[0].iov_base = first;
    iov[0].iov_len = sizeof(first);
    total += iov[0].iov_len;

    iov[1].iov_base = second;
    iov[1].iov_len = sizeof(second);
    total += iov[1].iov_len;

    iov[2].iov_base = third;
    iov[2].iov_len = sizeof(third);
    total += iov[2].iov_len;

    // writev
    ssize_t numWritten = writev(fd, iov, 3);
    if(numWritten == -1)
        errExit("writev");

    if(numWritten != total) {
        fprintf(stderr, "Error: couldn't write whole buffers\n");
        return EXIT_FAILURE;
    }

    printf("bytes written: %ld\n", numWritten);

    // lseek start of file
    off_t offset = lseek(fd, 0, SEEK_SET);
    if(offset == -1)
        errExit("lseek");

    // readv
    ssize_t numRead = readv(fd, iov, 3);
    if(numRead == -1)
        errExit("readv");

    printf("bytes read: %ld\n", total);
#define LEN(i) iov[i].iov_len
#define STR(i) (char*) iov[i].iov_base
    printf("%.*s %.*s %.*s\n", LEN(0), STR(0), LEN(1), STR(1), LEN(2), STR(2));

    if(close(fd) == -1)
        errExit("close");

    return EXIT_SUCCESS;
}
