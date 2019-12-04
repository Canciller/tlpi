#include <sys/stat.h>
#include <fcntl.h> // file control optional
#include "tlpi_hdr.h"

#define SIZE 1024

int main()
{
    int fs = open("dir", O_RDONLY | O_DIRECTORY);
    if(fs == -1)
        errExit("open");

    char buf[SIZE];

    ssize_t numRead;
    while((numRead = read(fs, buf, SIZE)) > 0)
        if(write(1, buf, numRead) != numRead)
            fatal("write");

    if(numRead == -1)
        errExit("read");

    return EXIT_SUCCESS;
}
