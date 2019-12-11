#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if(argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname offset\n", argv[0]);

    int fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd == -1)
        errExit("open");

    off_t off = lseek(fd, atoll(argv[2]), SEEK_SET);
    if(off == -1)
        errExit("lseek");

    const char *str = "string ";
    ssize_t status = write(fd, str, strlen(str));
    if(status == -1)
        errExit("write");

    if(close(fd) == -1)
        errExit("close");

    return EXIT_SUCCESS;
}
