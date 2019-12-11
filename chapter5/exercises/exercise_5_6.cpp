#include <fcntl.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if(argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    int fd1 = open(argv[1], O_TRUNC | O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    int fd2 = dup(fd1);
    int fd3 = open(argv[1], O_RDWR);

    write(fd1, "Hello,", 6);
    write(fd2, "world", 6);
    lseek(fd2, 0, SEEK_SET);
    write(fd1, "HELLO,", 6);
    write(fd3, "Gidday", 6);


    char buff[65] = {};
    lseek(fd1, 0, SEEK_SET);
    ssize_t total = read(fd1, buff, 64);
    buff[total] = '\0';

    const char *prediction = "Giddayworld";
    printf("%s\n", strcmp(prediction, buff) == 0 ? "true" : "false");

    return EXIT_SUCCESS;
}
