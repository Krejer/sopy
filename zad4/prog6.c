#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // musi być do getopt

void usage(char *pname)
{
    fprintf(stderr, "USAGE:%s ([-t x] -n Name ... )\n", pname); // fprintf najpierw pobiera gdzie ma przeslac dalej output
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int c, i;
    int x = 1; // tu nizej musze podac argc(tablica) i argv - liczba elementow
    while ((c = getopt(argc, argv, "t:n:")) != -1) // to oznacza, ze po t musze dawac argument, i po n tez, bo jest :, jakby był :: to jest opcjonalny argument
        switch (c)
        {
            case 't':
                x = atoi(optarg);
                break;
            case 'n':
                for (i = 0; i < x; i++)
                    printf("Hello %s\n", optarg);
                break;
            case '?': // invalid or an unknown option
            default:
                usage(argv[0]);
        }
    if (argc > optind)
        usage(argv[0]);
    return EXIT_SUCCESS;
}