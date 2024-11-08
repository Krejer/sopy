sop-grep: sop-grep.o
	gcc -o sop-grep sop-grep.o
sop-grep.o: sop-grep.c
	gcc -c sop-grep.c -o sop-grep.o -Wall -Werror -pedantic -g
.PHONY: clean
clean:
	rm -f sop-grep.o sop-grep
