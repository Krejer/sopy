liczba=5

zad$(liczba): zad$(liczba).o
	gcc -o zad$(liczba) zad$(liczba).o
zad$(liczba).o: zad$(liczba).c
	gcc -Wall -c zad$(liczba).c -o zad$(liczba).o
clean:
	rm zad$(liczba).c zad$(liczba).o zad$(liczba)