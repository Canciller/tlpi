#include <unistd.h>
#include <pwd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

passwd *getpwnam(const char *name)
{
    if(name == NULL || *name == '\0')
        return NULL;

    setpwent();

    passwd *pwd;
    while((pwd = getpwent()) != NULL) {
        if(strcmp(name, pwd->pw_name) == 0) {
            setpwent();
            endpwent();
            return pwd;
        }
    }

    setpwent();
    endpwent();

    return NULL;
}

int main(int argc, char **argv)
{
    passwd *pwd = getpwnam("canciller");
    if(pwd == NULL)
        return EXIT_FAILURE;

    printf("%-10s %5ld\n", pwd->pw_name, (long) pwd->pw_uid);

    pwd = getpwnam("root");
    if(pwd == NULL)
        return EXIT_FAILURE;

    printf("%-10s %5ld\n", pwd->pw_name, (long) pwd->pw_uid);

    return EXIT_SUCCESS;
}
