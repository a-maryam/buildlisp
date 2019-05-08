#makefile
all: parsing parsing.o mpc.o
parsing: parsing.o mpc.o   
	gcc -std=c99 -o parsing  parsing.o mpc.o -lm -ledit
parsing.o: parsing.c
	gcc -c -std=c99 parsing.c
mpc.o: mpc.c mpc.h
	gcc -c -std=c99 mpc.c
clean:
	rm *.o parsing
