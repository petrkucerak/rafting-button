CFLAGS= --std=c99 -pipe -Wall -O3 -pedantic
CFLAGS+= -ggdb3 # debug memmory leaks
all:	main

main.o:	main.c main.h
	$(CC) $(CFLAGS) -c $< -o $@

main:	main.o
	$(CC) $(CFLAGS) main.o -o $@
 
clean:	
	rm -f *.o
	rm -f main