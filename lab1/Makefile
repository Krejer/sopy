# liczba=
# nazwa=testowy2

# $(nazwa)$(liczba): $(nazwa)$(liczba).o
# 	gcc -o $(nazwa)$(liczba) $(nazwa)$(liczba).o
# $(nazwa)$(liczba).o: $(nazwa)$(liczba).c
# 	gcc -Wall -c $(nazwa)$(liczba).c -o $(nazwa)$(liczba).o
# clean:
# 	rm $(nazwa)$(liczba).c $(nazwa)$(liczba).o $(nazwa)$(liczba)

CC=gcc
C_FLAGS=-Wall -g
L_FLAGS=-fsanitize=address,undefined

TARGET=sop-dika
FILES=${TARGET}.o

.PHONY: clean all

${TARGET} : ${FILES}
	${CC} ${L_FLAGS} -o ${TARGET} ${FILES}

${TARGET}.o: ${TARGET}.c
	${CC} ${C_FLAGS} -o ${TARGET}.o -c ${TARGET}.c

all: ${TARGET}

clean:
	rm -f ${FILES} ${TARGET}
