#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char **argv)
{
    bool append = argc < 2;

    char buf[8];
    if(append) strcpy(buf, "append ");
    else strcpy(buf, "write ");

    int flags = O_CREAT | O_WRONLY;
    if(append) flags |= O_APPEND;

    int fd = open("append.txt", flags, S_IRUSR | S_IWUSR);
    if(fd == -1)
        return EXIT_FAILURE;

    int size = append ? 7 : 6;
    if(write(fd, buf, size) != size)
        return EXIT_FAILURE;

    if(close(fd) == -1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
