#include <pwd.h>
#include <shadow.h>
#include "tlpi_hdr.h"

int main()
{
    long lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if(lnmax == -1)
        lnmax = 256;

    passwd *pwd = getpwuid(getuid());
    if(pwd == NULL)
        fatal("couldn't get password record");

    spwd *sp = getspnam(pwd->pw_name);
    if(sp == NULL && errno == EACCES)
        fatal("no permission to read shadow password file");

    if(sp != NULL)
        pwd->pw_passwd = sp->sp_pwdp;

    char *password = getpass("Password: "),
         *encrypted = crypt(password, pwd->pw_passwd);

    for(char *p = password; *p != '\0';)
        *p++ = '\0';

    if(encrypted == NULL)
        errExit("crypt");

    if(strcmp(encrypted, pwd->pw_passwd) != 0) {
        fprintf(stderr, "Incorrect password\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", pwd->pw_uid);

    return EXIT_SUCCESS;
}
