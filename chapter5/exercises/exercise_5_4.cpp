#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int dup(int oldfd)
{
    return fcntl(oldfd, F_DUPFD, 0);
}

int dup2(int oldfd, int newfd)
{
    int status = fcntl(oldfd, F_GETFL);
    if(status == -1) {
        errno = EBADF;
        return -1;
    }

    if(oldfd == newfd)
        return newfd;

    close(newfd); // close errors are silently ignored.

    return fcntl(oldfd, F_DUPFD, newfd);
}

void errExit(const char *str)
{
    fprintf(stderr, "Error: %s: %s\n", strerror(errno), str);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if(close(0) == -1)
        errExit("close");

    printf("stdin closed\n");

    int newfd1 = dup(1);
    if(newfd1 == -1)
        errExit("dup");

    printf("stdout: 1, newfd1: %d\n", newfd1);

    const char *str1 = "write stdout\n";
    ssize_t status = write(1, str1, strlen(str1));
    if(status == -1)
        errExit("write");

    const char *str2 = "write: newfd1\n";
    status = write(newfd1, str2, strlen(str2));
    if(status == -1)
        errExit("write");

    int newfd2 = dup2(1, newfd1);
    if(newfd2 == -1)
        errExit("dup2");

    printf("stdout: 1, newfd2: %d\n", newfd2);

    status = write(1, str1, strlen(str1));
    if(status == -1)
        errExit("write");

    const char *str3 = "write: newfd2\n";
    status = write(newfd2, str3, strlen(str3));
    if(status == -1)
        errExit("write");

    return EXIT_SUCCESS;
}
