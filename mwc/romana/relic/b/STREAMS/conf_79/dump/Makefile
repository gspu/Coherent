# Simple Coherent makefile for dump driver

Driver.o: dump.c
	$(CC) $(CFLAGS) -D__VOID__=char -o Driver.o -c dump.c

install:
	cp Space.c Driver.o /etc/conf/dump

clean:
	rm -f Driver.o
