#include <sys/stat.h>
#include <fcntl.h>
#include <cctype>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')

static void usageError(char *progName, const char *msg = nullptr, int opt = 0)
{
    if(msg != nullptr && opt != 0)
        fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
    fprintf(stderr, "Usage: %s [-a] path\n", progName);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int opt, afnd = 0;
    char *path = nullptr;

    while((opt = getopt(argc, argv, ":a")) != -1) {
        switch(opt) {
            case 'a': ++afnd; break;
            case ':': usageError(argv[0], "Missing argument", optopt);
            case '?': usageError(argv[0], "Unrecognized option", optopt);
            default: fatal("Unexpected case in switch()");
        }
    }

    if(optind < argc) {
        path = argv[optind];
    } else {
        usageError(argv[0]);
    }

    int flags = O_WRONLY | O_CREAT;
    flags |= afnd != 0 ? O_APPEND : O_TRUNC;

    int fd = open(path, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fd == -1)
        errExit("opening file \"%s\"", path);

    char buf[BUF_SIZE] = {};

    ssize_t numRead;
    while((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        if(write(fd, buf, numRead) != numRead)
            fatal("couldn't write whole buffer");
        if(write(STDOUT_FILENO, buf, numRead) != numRead)
            fatal("couldn't write whole buffer");
    }
    if(numRead == -1)
        errExit("read");

    if(close(fd) == -1)
        errExit("close");

    return 0;
}
