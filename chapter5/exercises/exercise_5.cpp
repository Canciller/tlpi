#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if(argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    int fd = open(argv[1], O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    if(fd == -1)
        errExit("open");

    int dfd = dup(fd);
    if(dfd == -1)
        errExit("dup");

    printf("fd - file offset: %lld\n", lseek(fd, 0, SEEK_CUR));
    printf("dfd - file offset: %lld\n", lseek(dfd, 0, SEEK_CUR));

    const char *str = "buffer\n";
    ssize_t status = write(fd, str, strlen(str));
    if(status == -1)
        errExit("write");

    printf("fd - written %zu bytes\n", status);

    printf("fd - file offset: %lld\n", lseek(fd, 0, SEEK_CUR));
    printf("dfd - file offset: %lld\n", lseek(dfd, 0, SEEK_CUR));

    status = write(dfd, str, strlen(str));
    if(status == -1)
        errExit("write");

    printf("dfd - written %zu bytes\n", status);

    printf("fd - file offset: %lld\n", lseek(fd, 0, SEEK_CUR));
    printf("dfd - file offset: %lld\n", lseek(dfd, 0, SEEK_CUR));

    int ffd = fcntl(fd, F_GETFL);
    if(ffd == -1)
        errExit("fcntl");

    int fdfd = fcntl(dfd, F_GETFL);
    if(ffd == -1)
        errExit("fcntl");

    printf("fd - flags: %d\n", ffd);
    printf("dfd - flags: %d\n", fdfd);

    return EXIT_FAILURE;
}
