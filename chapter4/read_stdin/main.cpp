#include <sys/stat.h>
#include <fcntl.h> // file control optional
#include "tlpi_hdr.h"

#define MAX_READ 20

int main()
{
    char buf[MAX_READ + 1];

    int numRead = read(STDIN_FILENO, buf, MAX_READ);
    if(numRead == -1)
        errExit("read");

    buf[numRead] = '\0';
    printf("%s\n", buf);

    return 0;
}
