#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 20

int main(int argc, char **argv)
{
    int x, i;
    char *env = getenv("TIMES"); // pobieram wartosc, ktora zmienna srodowiskowa TIMES przechowuje
    if (env) // trzeba sprawdzic czy istnieje, jesli nie to zwraca null ptr
        x = atoi(env);
    else
        x = 1;
    char name[MAX_LINE + 2];
    while (fgets(name, MAX_LINE + 2, stdin) != NULL)
        for (i = 0; i < x; i++)
            printf("Hello %s", name);
        printf("\n");
    if (putenv("RESULT=Done") != 0) // putenv zmienia wartosc, w tym przypadku RESULT na Done, lub dodaje do srodowiska, jesli sie uda zwraca 0, wpp niezero
    {
        fprintf(stderr, "putenv failed");
        return EXIT_FAILURE;
    }
    printf("%s\n", getenv("RESULT"));
    if (system("env|grep RESULT") != 0)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}