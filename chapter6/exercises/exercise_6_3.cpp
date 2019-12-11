#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

int unsetenv(const char* name)
{
    extern char **environ;

    if(!environ)
        return 0;

    if(name == NULL || strlen(name) == 0 || strchr(name, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }

    size_t len = strlen(name);

    for(char **start = environ; *start != NULL;) {
        if(strncmp(*start, name, len) == 0) {
            //printf("[unsetenv] found \"%s\"\n", *start);

            for(char **begin = start; *begin != NULL; ++begin) {
                //printf("[unsetenv] *%p (\"%s\") = *%p (\"%s\")\n",
                        //begin, *begin, begin + 1, *(begin + 1));

                *begin = *(begin + 1);
            }
        } else {
            ++start;
        }
    }

    return 0;
}

int setenv(const char *name, const char *value, int overwrite)
{
    if(name == NULL || strlen(name) == 0 || strchr(name, '=') != NULL || value == NULL) {
        errno = EINVAL;
        return -1;
    }

    if(getenv(name) && overwrite == 0) {
        //printf("[setenv] found \"%s\"; no overwrite is set, doing nothing\n", name);
        return 0;
    }

    unsetenv(name);

    size_t nameLen = strlen(name),
           valueLen = strlen(value);

    size_t len = nameLen + 1 + valueLen; // =

    char *buffer = (char *) malloc(len + 1); // \0
    if(!buffer) {
        errno = ENOMEM;
        return -1;
    }

    memcpy(buffer, name, nameLen);
    buffer[nameLen] = '=';
    memcpy(buffer + nameLen + 1, value, valueLen);
    buffer[len] = '\0';

    if(putenv(buffer) != 0) {
        free(buffer);
        return -1;
    }

    //printf("[setenv] %p; allocated %zu bytes of memory; buffer=\"%s\"\n", buffer, len, buffer);

    return 0;
}


int main(int argc, char *argv[])
{
   clearenv();

   if(setenv("ERROR=", "error", 0) == -1) // error
   printf("ERROR [%s] setenv\n", strerror(errno));

   char test1[] = "TEST1=test1_original";
   if(putenv(test1) != 0)
       perror("putenv");

   setenv("TEST1", "test1", 0);
   setenv("TEST2", "test2", 0);
   setenv("TEST3", "test3", 0);
   setenv("TEST4", "test4", 0);
   setenv("TEST5", "test5", 0);
   setenv("TEST6", "test6", 0);

   setenv("TEST2", "test2_overwrite", 1);
   unsetenv("NAME");
   setenv("TEST7", "test7", 0);
   unsetenv("TEST3");

   system("printenv | grep ^TEST");

   return EXIT_SUCCESS;
}
