liczba=1
nazwa = zadanie

zad$(liczba): $(nazwa)$(liczba).o
	gcc -o $(nazwa)$(liczba) $(nazwa)$(liczba).o
zad$(liczba).o: $(nazwa)$(liczba).c
	gcc -Wall -c $(nazwa)$(liczba).c -o $(nazwa)$(liczba).o
clean:
	rm $(nazwa)$(liczba).c $(nazwa)$(liczba).o $(nazwa)$(liczba)