#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ; // zdeefiniowanie, ze korzystam z listy wszystkich zmiennych

int main(int argc, char **argv)
{
    int index = 0;
    // Dodawanie zmiennych do środowiska:
    // export TVAR1='qwert' - trwale, jak kiedys znowu uruchomie prog7 to ta zmienna tam bedzie
    // TVAR2="122345" ./prog7 - jednorazowo
    while (environ[index])
        printf("%s\n", environ[index++]);
    return EXIT_SUCCESS;
}