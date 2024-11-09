#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 20

int main(int argc, char **argv)
{
    int i;
    for (i = 0; i < argc; i++)
        printf("%s\n", argv[i]);
    return EXIT_SUCCESS;
}
// cat dane.txt |tr "\n" "\0"| xargs -0 ./prog4 - tr zamienia \n na \0, xargs uzywa /0 jako separatora argumentow
// xargs ./prog4 < dane.txt - zamienia tak jakby wejscie na nowe linie,argumenty