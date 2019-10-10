# Simple Coherent makefile for echo driver

Driver.o: echo.c
	$(CC) $(CFLAGS) -o Driver.o -c echo.c

install:
	cp Driver.o /etc/conf/echo

clean:
	rm -f Driver.o
