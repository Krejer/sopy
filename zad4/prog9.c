#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

extern char **environ;

void usage(char *pname)
{
    fprintf(stderr, "USAGE:%s [VARN_NAME VARN_VALUE] ... \n", pname);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc % 2 != 1)
        usage(argv[0]);

    for (int i = argc - 1; i > 0; i -= 2)
    {
        if (setenv(argv[i - 1], argv[i], 1)) // to ogolnie zmienia nazwe argv[i - 1] na argv[i], 1 sprawia, ze mozna overridowac, jakby bylo zero to jesli istnieje juz argv[i - 1] wywali blad
        {
            if (EINVAL == errno) // errno przechowuje wartosc ostatniego bledu, a einval odpowiada za "Invalid argument", zmienne nie mogą mieć = w nazwie
                ERR("setenv - variable name contains '='");
            ERR("setenv"); // to sie wykona, bo dla ifa jakakolwiek wartosc niezerowa oznacza true, a setenv wtedy zwroci -1(w przypadku bledu)
        }
    }

    int index = 0;
    while (environ[index])
        printf("%s\n", environ[index++]);
    return EXIT_SUCCESS;
}