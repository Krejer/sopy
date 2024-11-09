liczba=
nazwa=testowy1_niskopoziomowy

$(nazwa)$(liczba): $(nazwa)$(liczba).o
	gcc -o $(nazwa)$(liczba) $(nazwa)$(liczba).o
$(nazwa)$(liczba).o: $(nazwa)$(liczba).c
	gcc -Wall -c $(nazwa)$(liczba).c -o $(nazwa)$(liczba).o
clean:
	rm $(nazwa)$(liczba).c $(nazwa)$(liczba).o $(nazwa)$(liczba)