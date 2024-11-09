main2: main2.o
	gcc -o main2 main2.o
main2.o: main2.c
	gcc -Wall -c main2.c -o main2.o
clean:
	rm main2.c main2.o main2



# jakbym chcial budowac tylko server, to po prostu make server, jak oba to make all
# tu jest Makefile dla 3 plikow:
#main: main.o hello.o\
	gcc -o main *.o\
main.o: main.c hello.\
	gcc -c main.c -o main.o\
hello.o: hello.c hello.h\
	gcc -c hello.c -o hello.o\
clean:\
	-rm main.o hello.o main