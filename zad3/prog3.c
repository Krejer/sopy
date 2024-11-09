#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 20

int main(int argc, char **argv)
{
    //FILE * plik;
    //plik = fopen("dane.txt", "r"); // otwiera ten plik dane.txt
    char name[MAX_LINE + 2]; // 22 bo musi sie zmiescic znak nowej lini i konczacy znak 0
    while (fgets(name, MAX_LINE + 2, stdin) != NULL) // fgets czyta ze strumienia, char *fgets(char *str, int n, FILE *stream);, gdzie str to nasza tablica n elementowa, a File nasz plik(chyba)
    // wazne - fgets zwraca NULL dopiero po osiagnieciu konca strumienia, 22 nie 22 bo musi sie zmiescic znak nowej lini i \0
        printf("Hello %s", name); // fgets pobiera \n wiec nie musimy dodawac sami
    return EXIT_SUCCESS; 
}

// mozna tez przeczytac dane z dane.txt w inny sposob, wystarczy przekierowac ./prog3 < dane.txt, lub cat dane.tct | ./prog3