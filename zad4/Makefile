CC=gcc
CFLAGS=-Wall -fsanitize=address,undefined
LDFLAGS=-fsanitize=address,undefined
liczba=9

prog$(liczba): prog$(liczba).o
	$(CC) $(LDFLAGS) -o prog$(liczba) prog$(liczba).o
prog$(liczba).o: prog$(liczba).c
	$(CC) $(CFLAGS) -c prog$(liczba).c -o prog$(liczba).o
clean:
	rm prog$(liczba).o prog$(liczba)