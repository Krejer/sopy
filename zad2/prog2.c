#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXL 22

#define ERR(source) (perror(source), \
fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
exit(EXIT_FAILURE))
int main()
{
   char name[MAXL];
   int liczba = MAXL - 1;
   scanf("%21s", name); // 21 a nie 22 bo w C kazdy string konczy sie znakiem \0, scanf czyta tylko jedną linie
   if (strlen(name) > MAXL - 2) ERR("Name too long"); // ./prog2 2> /dev/null - przeniesie do dev/nulla ewentualny error
   printf("Hello %s\n", name); // printf bo printujemy na konsole, jak do pliku jakiegos to fprintf
   //return EXIT_SUCCESS;
}

// fgets czyta ze strumienia, char *fgets(char *str, int n, FILE *stream);, gdzie str to nasza tablica n elementowa, a File nasz plik(chyba)