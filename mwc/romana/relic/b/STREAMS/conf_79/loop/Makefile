# Simple Coherent makefile for loop driver

Driver.o : loop.c
	$(CC) $(CFLAGS) -o Driver.o -c loop.c

install :
	cp Space.c Driver.o /etc/conf/loop

clean:
	rm -f Driver.o
