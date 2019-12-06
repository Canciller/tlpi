#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main(int argc, char **argv)
{
    if(argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file {r<length>}|R<length>|w<string>|s<offset>\n", argv[0]);

    int fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd == -1)
        errExit("open", argv[1]);

    for(int i = 2; i < argc; ++i) {
        char *value = &argv[i][1];
        switch(argv[i][0]) {
            case 'r':
            case 'R': {
                size_t length = getLong(value, GN_ANY_BASE, argv[i]);

                char *buf = (char*) malloc(length);
                if(!buf)
                    errExit("malloc");

                ssize_t numRead = read(fd, buf, length);
                if(numRead == -1)
                    errExit("read");

                if(numRead == 0) {
                    printf("%s: end-of-file\n", argv[1]);
                } else {
                    printf("%s: ", argv[i]);
                    for(size_t j = 0; j < numRead; ++j) {
                        if(argv[i][0] == 'r')
                            printf("%c", isprint((unsigned char) buf[j]) ? buf[j] : '?');
                        else
                            printf("%02x ", (unsigned char) buf[j]);
                    }
                    printf("\n");
                }

                free(buf);
                break;
            }
            case 'w': {
                size_t len = strlen(value);
                ssize_t numWrite = write(fd, value, len);
                if(numWrite == -1)
                    errExit("write");
                if(numWrite != len)
                    fatal("couldn't write whole string");

                printf("%s: wrote %ld bytes\n", argv[i], (long) numWrite);
                break;
            }
            case 's': {
                long offset = getLong(value, GN_ANY_BASE, argv[i]);
                if(lseek(fd, offset, SEEK_SET) == -1)
                    errExit("lseek");
                printf("%s: seek succeeded\n", argv[i]);
                break;
            }
            default:
                cmdLineErr("Argument must start with [rRws]: %s\n", argv[i]);
                break;
        }
    }

    if(close(fd) == -1)
        errExit("close", argv[1]);

    return 0;
}
