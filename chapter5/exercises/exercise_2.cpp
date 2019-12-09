#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if(argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    int fd = open(argv[1], O_WRONLY | O_APPEND);
    if(fd == -1)
        errExit("open");

    off_t off = lseek(fd, 0, SEEK_SET);
    if(off == -1)
        errExit("lseek");

    const char *str = "APPEND ";
    ssize_t status = write(fd, str, strlen(str));
    if(status == -1)
        errExit("write");

    if(close(fd) == -1)
        errExit("close");

    return EXIT_SUCCESS;
}
