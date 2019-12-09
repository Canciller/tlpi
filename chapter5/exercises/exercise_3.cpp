#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if(argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s filename num-bytes [x]\n", argv[0]);

    bool x = argc > 3;
    size_t bytes = atoi(argv[2]);

    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    if(!x) flags |= O_APPEND;

    int fd = open(argv[1], flags, S_IRUSR | S_IWUSR);
    if(fd == -1)
        errExit("open");

    if(x && lseek(fd, 0, SEEK_END) == -1)
        errExit("lseek");

    for(size_t i = 0; i < bytes; ++i) {
        ssize_t status = write(fd, "x", 1);
        if(status == -1)
            errExit("write");
    }

    if(close(fd) == -1)
        errExit("close");

    printf("%zu bytes written %s append\n",
            bytes, x ? "without" : "with");

    return EXIT_SUCCESS;
}
