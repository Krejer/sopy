#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void usage(char* argv[])
{
    printf("%s pattern\n", argv[0]);
    printf("pattern - string pattern to search at standard input\n");
    printf("n - (optional) displaying line numbers\n");
    exit(EXIT_FAILURE);
}

#define max_line 999;

int main(int argc, char* argv[])
{
    if(argc < 2 || argc > 3) usage(argv);
    size_t line_len = max_line;
    char * line = (char *)malloc(sizeof(char)*line_len);
    char * pattern = argv[argc - 1];
    int numerowanie = 0;
    
    int c = 0;
    while((c = getopt(argc, argv, "n")) != -1)
    {
        switch(c)
        {
            case'n':
                numerowanie = 1;
                break;
            case '?':
            default:
                usage(argv);
        }
    }
    int linia = 1;
    while (getline(&line, &line_len, stdin) != -1)  // man 3p getdelim
    {
        if(strstr(line, pattern))
        {
            if(numerowanie == 1)
            {
                printf("%d:%s", linia, line);
            }
            else
            {
                printf("%s", line);
            }
        } 
        linia++;
    }
    if (line)
        free(line);

    return EXIT_SUCCESS;
}
