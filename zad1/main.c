#include <stdio.h>
#include <stdlib.h>


int main(void)
{
   printf("Hello world\n");

   return EXIT_SUCCESS;
}
// ogolnie proces tworzenia binarki to kompilowanie i linking - wrzucenie wszystkiego do jednego wora, zeby stworzyc executable file
// gcc -o main main.c - tworzy binarny plik main, ktory nastepnie wywolujemy ./main
// gcc -c -o main.o main.c // -c nie linkuje pliku, utworzy się plik main.o - plik obiektowy
// następnie gcc -o main main.o z tego co rozumiem zlinkuje wszystko do pliku main, ktory jest binarny
// jakieś parametry, ktore mozna dodac do gcc:
// -Wall - włącza opcjonalne warningi, polepsza jakosc kodu i sprawia, ze moge mniej bledow zrobic - MUSZE STOSOWAC
// -pedantic - wlacza rygorystyczne sprawdzanie kodu ze standardem 
// -Werror - wszystkie warningi zamienia w errory xd
// -g - Daje informacje o debuggowaniu
// -ansi - specyfikuje standard, do ktorego kod powinien sie odwolywac                                  
// -OX - optymalizuje kod, X oznacza level optymalizacji - 3 najwyzszy, 0 defaultowy - najnizszy
// do linkera - -lLibrary, Library to nazwa biblioteki, z ktorej korzystam w danym programie
// jak mam wiecej niz jeden plik .c to po prostu tworze sobie iles razy plik .o a potem wywoluje jednorazowo gcc -o main *.o
// make mi przetrzymuje caly proces tworzenia binarki, jesli chodzi o clean to, zeby on wykonal sie zawsze(nawet jesli se stworzylem plik clean)
// to musze dopisac te .PHONY clean..., - przed rm znaczy, ze nawet jak rm nie usunie pliku to nie wywali errora i i tak sie reszta wykona
// troche jak w c# jak nie chce powtarzac ktore pliki gdzie tworze i usuwam to se daje ${FILES}, gdzie FILES = main.o hello.o
// Przekierowanie wyjścia do pliku tekstowego - ./main > plik.txt, cat plik.txt go wyświetli 